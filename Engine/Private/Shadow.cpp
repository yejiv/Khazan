#include "Shadow.h"
#include "GameInstance.h"

CShadow::CShadow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice { pDevice }
	, m_pContext { pContext }
	, m_pGameInstance { CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

HRESULT CShadow::Initialize()
{
	if (FAILED(Ready_ShaderResources()))
		return E_FAIL;

    m_Desc.fSplit = 35.f;

    m_fCameraNear = 0.1f;
    m_fCameraFar = 6000.f;

	m_Desc.vLightDir = { 1.f, -1.f, 1.f };
	m_Desc.fBias = 0.001f;
	m_Desc.fIntensity = 0.6f;

    m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Engine_Shader_Font.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
    if (nullptr == m_pShader)
        return E_FAIL;

    return S_OK;
}

void CShadow::Update(_float fTimeDelta)
{
    if (true == m_isTransition)
    {
        m_fTransTimeAcc += fTimeDelta;

        _float fRatio = m_fTransTimeAcc / m_fDuration;

        if (1.f <= fRatio)
        {
            fRatio = 1.f;
            m_isTransition = false;
        }

        m_Desc.fIntensity = Lerp(m_Desc.fIntensity, m_fTargetIntensity, fRatio);
    }

    m_pGameInstance->Get_Frustum_WorldPoints(m_vFrustumCorners);

    _float fNearRatio = (m_fCameraNear - m_fCameraNear) / (m_fCameraFar - m_fCameraNear);
    _float fFarRatio = (m_Desc.fSplit - m_fCameraNear) / (m_fCameraFar - m_fCameraNear);

    // 1. 코너 비율로 나누기
    for (_uint j = 0; j < 4; ++j)
    {
        _vector vNear = XMLoadFloat4(&m_vFrustumCorners[j]);
        _vector vFar = XMLoadFloat4(&m_vFrustumCorners[j + 4]);

        _vector vCornerNear = XMVectorLerp(vNear, vFar, fNearRatio);
        _vector vCornerFar = XMVectorLerp(vNear, vFar, fFarRatio);

        vCornerNear = XMVectorSetW(vCornerNear, 1.f);
        vCornerFar = XMVectorSetW(vCornerFar, 1.f);

        XMStoreFloat4(&m_vFrustumCorners[j], vCornerNear);
        XMStoreFloat4(&m_vFrustumCorners[j + 4], vCornerFar);
    }

    // 2. 각 코너의 중심 위치 찾기
    _vector vCenter = {};

    for (_uint j = 0; j < 8; ++j)
        vCenter += XMLoadFloat4(&m_vFrustumCorners[j]);
    vCenter /= 8.f;

    // 3. 중심점 -> 코너의 대각선 중 최대 길이 구하기
    _float fRadius = {};

    for (_uint j = 0; j < 8; ++j)
    {
        _float fDistance = XMVectorGetX(XMVector3Length(XMLoadFloat4(&m_vFrustumCorners[j]) - vCenter));
        fRadius = max(fRadius, fDistance);
    }

    fRadius = ceil(fRadius * 16.f) / 16.f;

    _vector vMaxExtents = XMVectorSet(fRadius, fRadius, fRadius, 1.f);
    _vector vMinExtents = -vMaxExtents;

    _vector vShadowCamPos = vCenter - XMVector3Normalize(XMVectorSetW(XMLoadFloat3(&m_Desc.vLightDir), 0.f)) * fRadius;

    _matrix LightViewMatrix = XMMatrixLookAtLH(vShadowCamPos, vCenter, XMVectorSet(0.f, 1.f, 0.f, 0.f));

    _vector vMinPoint = XMVectorSet(FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX);
    _vector vMaxPoint = XMVectorSet(-FLT_MAX, -FLT_MAX, -FLT_MAX, -FLT_MAX);

    for (_uint j = 0; j < 8; ++j)
    {
        _vector vCornerWorld = XMLoadFloat4(&m_vFrustumCorners[j]);
        _vector vCornerView = XMVector3TransformCoord(vCornerWorld, LightViewMatrix);

        vMinPoint = XMVectorMin(vMinPoint, vCornerView);
        vMaxPoint = XMVectorMax(vMaxPoint, vCornerView);
    }

    // 모든 루프가 끝난 후, 최종 Min/Max 레지스터 값을 스칼라로 추출
    _float3 vMin{}, vMax{};
    XMStoreFloat3(&vMin, vMinPoint);
    XMStoreFloat3(&vMax, vMaxPoint);

    _matrix LightProjMatrix = XMMatrixOrthographicOffCenterLH
    (
        vMin.x,
        vMax.x,
        vMin.y,
        vMax.y,
        vMin.z,
        vMax.z
    );

    XMStoreFloat4x4(&m_LightMatrices[ENUM_CLASS(D3DTS::VIEW)], LightViewMatrix);
    XMStoreFloat4x4(&m_LightMatrices[ENUM_CLASS(D3DTS::PROJ)], LightProjMatrix);
}

const _float4x4* CShadow::Get_ShadowLightMatrix(D3DTS eTransformState) const
{
    return &m_LightMatrices[ENUM_CLASS(eTransformState)];
}

void CShadow::Bind_ShadowDSV()
{
	m_pContext->OMSetRenderTargets(0, nullptr, m_pShadowDSV);
}

HRESULT CShadow::Bind_Shadow_ShaderResources(CShader* pShader)
{
	if (FAILED(pShader->Bind_Matrix("g_LightViewMatrix", &m_LightMatrices[ENUM_CLASS(D3DTS::VIEW)])))
		return E_FAIL;

	if (FAILED(pShader->Bind_Matrix("g_LightProjMatrix", &m_LightMatrices[ENUM_CLASS(D3DTS::PROJ)])))
		return E_FAIL;

	if (FAILED(pShader->Bind_SRV("g_ShadowTexture", m_pShadowSRV)))
		return E_FAIL;

	if (FAILED(pShader->Bind_RawValue("g_fShadowBias", &m_Desc.fBias, sizeof(_float))))
		return E_FAIL;

	if (FAILED(pShader->Bind_RawValue("g_fShadowIntensity", &m_Desc.fIntensity, sizeof(_float))))
		return E_FAIL;

    if (FAILED(pShader->Bind_RawValue("g_fSplitFar", &m_Desc.fSplit, sizeof(_float))))
        return E_FAIL;

	_float2 vShadowMapSize = _float2(g_iMaxWidth, g_iMaxHeight);
	if (FAILED(pShader->Bind_RawValue("g_vShadowMapSize", &vShadowMapSize, sizeof(_float2))))
		return E_FAIL;

	return S_OK;
}

void CShadow::Start_ShadowTransition(_float fDuration, _float fTargetIntensity)
{
    m_isTransition = true;
    m_fTransTimeAcc = 0.f;
    m_fDuration = fDuration;
    m_fTargetIntensity = fTargetIntensity;
}

void CShadow::Clear_DSV()
{
    m_pContext->ClearDepthStencilView(m_pShadowDSV, D3D11_CLEAR_DEPTH, 1.f, 0);	
}

//  #ifdef _DEBUG
HRESULT CShadow::Ready_Debug(_float fX, _float fY, _float fSizeX, _float fSizeY)
{
	_uint			iNumViewports = { 1 };
	D3D11_VIEWPORT  ViewportDesc{};

	m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

    XMStoreFloat4x4(&m_WorldMatrix, XMMatrixScaling(fSizeX, fSizeY, 1.f));
    m_WorldMatrix._41 = fX - ViewportDesc.Width * 0.5f;
    m_WorldMatrix._42 = -fY + ViewportDesc.Height * 0.5f;

    m_vLTPos = { fX - (fSizeX * 0.5f), fY - (fSizeY * 0.5f) };

	return S_OK;
}

HRESULT CShadow::Render(CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
    if (FAILED(pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
        return E_FAIL;

	if (FAILED(pShader->Bind_SRV("g_Texture", m_pShadowSRV)))
		return E_FAIL;

    pShader->Begin(0);

    pVIBuffer->Bind_Resources();
    pVIBuffer->Render();

    // Font 출력
    m_pShader->Begin(0);

    // Outline
    m_pGameInstance->Draw_Text(TEXT("Blade_Medium_20"), TEXT("RT_Shadow"), m_vLTPos.x + 3.f, m_vLTPos.y == 0.f ? m_vLTPos.y + 45.f - 2.f : m_vLTPos.y + 25.f - 2.f, _float4(1.f, 1.f, 1.f, 1.f));
    m_pGameInstance->Draw_Text(TEXT("Blade_Medium_20"), TEXT("RT_Shadow"), m_vLTPos.x + 5.f, m_vLTPos.y == 0.f ? m_vLTPos.y + 45.f - 2.f : m_vLTPos.y + 25.f - 2.f, _float4(1.f, 1.f, 1.f, 1.f));
    m_pGameInstance->Draw_Text(TEXT("Blade_Medium_20"), TEXT("RT_Shadow"), m_vLTPos.x + 7.f, m_vLTPos.y == 0.f ? m_vLTPos.y + 45.f - 2.f : m_vLTPos.y + 25.f - 2.f, _float4(1.f, 1.f, 1.f, 1.f));
    m_pGameInstance->Draw_Text(TEXT("Blade_Medium_20"), TEXT("RT_Shadow"), m_vLTPos.x + 3.f, m_vLTPos.y == 0.f ? m_vLTPos.y + 45.f : m_vLTPos.y + 25.f, _float4(1.f, 1.f, 1.f, 1.f));
    m_pGameInstance->Draw_Text(TEXT("Blade_Medium_20"), TEXT("RT_Shadow"), m_vLTPos.x + 7.f, m_vLTPos.y == 0.f ? m_vLTPos.y + 45.f : m_vLTPos.y + 25.f, _float4(1.f, 1.f, 1.f, 1.f));
    m_pGameInstance->Draw_Text(TEXT("Blade_Medium_20"), TEXT("RT_Shadow"), m_vLTPos.x + 3.f, m_vLTPos.y == 0.f ? m_vLTPos.y + 45.f + 2.f : m_vLTPos.y + 25.f + 2.f, _float4(1.f, 1.f, 1.f, 1.f));
    m_pGameInstance->Draw_Text(TEXT("Blade_Medium_20"), TEXT("RT_Shadow"), m_vLTPos.x + 5.f, m_vLTPos.y == 0.f ? m_vLTPos.y + 45.f + 2.f : m_vLTPos.y + 25.f + 2.f, _float4(1.f, 1.f, 1.f, 1.f));
    m_pGameInstance->Draw_Text(TEXT("Blade_Medium_20"), TEXT("RT_Shadow"), m_vLTPos.x + 7.f, m_vLTPos.y == 0.f ? m_vLTPos.y + 45.f + 2.f : m_vLTPos.y + 25.f + 2.f, _float4(1.f, 1.f, 1.f, 1.f));

    // Fill
    m_pGameInstance->Draw_Text(TEXT("Blade_Medium_20"), TEXT("RT_Shadow"), m_vLTPos.x + 5.f, m_vLTPos.y == 0.f ? m_vLTPos.y + 45.f : m_vLTPos.y + 25.f, _float4(0.f, 0.f, 0.f, 1.f));

	return S_OK;
}
//  #endif

HRESULT CShadow::Ready_ShaderResources()
{
    ID3D11Texture2D* pDepthStencilTexture = nullptr;

    D3D11_TEXTURE2D_DESC	TextureDesc;
    ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

    TextureDesc.Width = g_iMaxWidth;
    TextureDesc.Height = g_iMaxHeight;
    TextureDesc.MipLevels = 1;
    TextureDesc.ArraySize = 1;
    TextureDesc.Format = DXGI_FORMAT_R32_TYPELESS;
    TextureDesc.SampleDesc.Quality = 0;
    TextureDesc.SampleDesc.Count = 1;
    TextureDesc.Usage = D3D11_USAGE_DEFAULT;
    TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
    TextureDesc.CPUAccessFlags = 0;
    TextureDesc.MiscFlags = 0;

    if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &pDepthStencilTexture)))
        return E_FAIL;

    D3D11_DEPTH_STENCIL_VIEW_DESC DSVDesc{};
    DSVDesc.Format = DXGI_FORMAT_D32_FLOAT;
    DSVDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    DSVDesc.Texture2D.MipSlice = 0;

    if (FAILED(m_pDevice->CreateDepthStencilView(pDepthStencilTexture, &DSVDesc, &m_pShadowDSV)))
        return E_FAIL;

    D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc{};
    SRVDesc.Format = DXGI_FORMAT_R32_FLOAT;
    SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    SRVDesc.Texture2D.MostDetailedMip = 0;
    SRVDesc.Texture2D.MipLevels = 1;

    if (FAILED(m_pDevice->CreateShaderResourceView(pDepthStencilTexture, &SRVDesc, &m_pShadowSRV)))
        return E_FAIL;

    Safe_Release(pDepthStencilTexture);
    
    Clear_DSV();

	return S_OK;
}

CShadow* CShadow::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CShadow* pInstance = new CShadow(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Create : CShadow"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CShadow::Free()
{
    __super::Free();

    Safe_Release(m_pShader);

    Safe_Release(m_pShadowSRV);
    Safe_Release(m_pShadowDSV);

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
}
