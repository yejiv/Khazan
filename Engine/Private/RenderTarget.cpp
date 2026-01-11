#include "RenderTarget.h"

#include "Shader.h"
#include "VIBuffer_Rect.h"
#include "GameInstance.h"

CRenderTarget::CRenderTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
#ifdef _DEBUG
    , m_pGameInstance { CGameInstance::GetInstance() }
#endif
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
#ifdef _DEBUG
    Safe_AddRef(m_pGameInstance);
#endif
}

HRESULT CRenderTarget::Initialize(_uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4& vClearColor)
{
	D3D11_TEXTURE2D_DESC		TextureDesc{};

	TextureDesc.Width = iSizeX;
	TextureDesc.Height = iSizeY;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = ePixelFormat;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pTexture2D)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateRenderTargetView(m_pTexture2D, nullptr, &m_pRTV)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateShaderResourceView(m_pTexture2D, nullptr, &m_pSRV)))
		return E_FAIL;

	m_vClearColor = vClearColor;

#ifdef _DEBUG
    m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Engine_Shader_Font.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
    if (nullptr == m_pShader)
        return E_FAIL;
#endif

	return S_OK;
}

HRESULT CRenderTarget::Bind_ShaderResource(CShader* pShader, const _char* pConstantName)
{
	return pShader->Bind_SRV(pConstantName, m_pSRV);
}

void CRenderTarget::Clear()
{
	m_pContext->ClearRenderTargetView(m_pRTV, reinterpret_cast<_float*>(&m_vClearColor));
}

HRESULT CRenderTarget::Copy_Resource(ID3D11Texture2D* pDestTexture)
{
    // 복사를 받을 대상, 복사할 대상
	m_pContext->CopyResource(pDestTexture, m_pTexture2D);

	return S_OK;
}

#ifdef _DEBUG

HRESULT CRenderTarget::Ready_Debug(_float fX, _float fY, _float fSizeX, _float fSizeY)
{
	_uint       iNumViewports = { 1 };
	D3D11_VIEWPORT      ViewportDesc{};

	m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixScaling(fSizeX, fSizeY, 1.f));
	m_WorldMatrix._41 = fX - ViewportDesc.Width * 0.5f;
	m_WorldMatrix._42 = -fY + ViewportDesc.Height * 0.5f;

	return S_OK;
}

HRESULT CRenderTarget::Render(CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	if (FAILED(pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	if (FAILED(pShader->Bind_SRV("g_Texture", m_pSRV)))
		return E_FAIL;

	pShader->Begin(0);

	pVIBuffer->Bind_Resources();
	pVIBuffer->Render();

    // Font 출력
    m_pShader->Begin(0);

    // Outline
    m_pGameInstance->Draw_Text(TEXT("Blade_Medium_26"), m_strName, m_vLTPos.x + 3.f, m_vLTPos.y == 0.f ? m_vLTPos.y + 45.f - 2.f : m_vLTPos.y + 25.f - 2.f, _float4(1.f, 1.f, 1.f, 1.f));
    m_pGameInstance->Draw_Text(TEXT("Blade_Medium_26"), m_strName, m_vLTPos.x + 5.f, m_vLTPos.y == 0.f ? m_vLTPos.y + 45.f - 2.f : m_vLTPos.y + 25.f - 2.f, _float4(1.f, 1.f, 1.f, 1.f));
    m_pGameInstance->Draw_Text(TEXT("Blade_Medium_26"), m_strName, m_vLTPos.x + 7.f, m_vLTPos.y == 0.f ? m_vLTPos.y + 45.f - 2.f : m_vLTPos.y + 25.f - 2.f, _float4(1.f, 1.f, 1.f, 1.f));
    m_pGameInstance->Draw_Text(TEXT("Blade_Medium_26"), m_strName, m_vLTPos.x + 3.f, m_vLTPos.y == 0.f ? m_vLTPos.y + 45.f : m_vLTPos.y + 25.f, _float4(1.f, 1.f, 1.f, 1.f));
    m_pGameInstance->Draw_Text(TEXT("Blade_Medium_26"), m_strName, m_vLTPos.x + 7.f, m_vLTPos.y == 0.f ? m_vLTPos.y + 45.f : m_vLTPos.y + 25.f, _float4(1.f, 1.f, 1.f, 1.f));
    m_pGameInstance->Draw_Text(TEXT("Blade_Medium_26"), m_strName, m_vLTPos.x + 3.f, m_vLTPos.y == 0.f ? m_vLTPos.y + 45.f + 2.f : m_vLTPos.y + 25.f + 2.f, _float4(1.f, 1.f, 1.f, 1.f));
    m_pGameInstance->Draw_Text(TEXT("Blade_Medium_26"), m_strName, m_vLTPos.x + 5.f, m_vLTPos.y == 0.f ? m_vLTPos.y + 45.f + 2.f : m_vLTPos.y + 25.f + 2.f, _float4(1.f, 1.f, 1.f, 1.f));
    m_pGameInstance->Draw_Text(TEXT("Blade_Medium_26"), m_strName, m_vLTPos.x + 7.f, m_vLTPos.y == 0.f ? m_vLTPos.y + 45.f + 2.f : m_vLTPos.y + 25.f + 2.f, _float4(1.f, 1.f, 1.f, 1.f));

    // Fill
    m_pGameInstance->Draw_Text(TEXT("Blade_Medium_26"), m_strName, m_vLTPos.x + 5.f, m_vLTPos.y == 0.f ? m_vLTPos.y + 45.f : m_vLTPos.y + 25.f, _float4(0.f, 0.f, 0.f, 1.f));

	return S_OK;
}

#endif

CRenderTarget* CRenderTarget::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4& vClearColor)
{
	CRenderTarget* pInstance = new CRenderTarget(pDevice, pContext);

	if (FAILED(pInstance->Initialize(iSizeX, iSizeY, ePixelFormat, vClearColor)))
	{
		MSG_BOX(TEXT("Failed to Created : CRenderTarget"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRenderTarget::Free()
{
	__super::Free();

#ifdef _DEBUG
    Safe_Release(m_pShader);
    Safe_Release(m_pGameInstance);
#endif

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	Safe_Release(m_pSRV);
	Safe_Release(m_pRTV);
	Safe_Release(m_pTexture2D);
}
