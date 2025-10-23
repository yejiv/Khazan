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
	m_Cascade.iNumCascades = 4;

	m_Cascade.Splits.resize(m_Cascade.iNumCascades);
	m_Cascade.LightViewMatrices.resize(m_Cascade.iNumCascades);
	m_Cascade.LightProjMatrices.resize(m_Cascade.iNumCascades);
	m_ShadowDSVs.resize(m_Cascade.iNumCascades);

	// 이후 Directional Light 추가 될 시 갱신 해주기
	m_vLightDir = { 1.f, -1.f, 1.f, 0.f };

	// log, linear mix 수치
	m_fLamda = 0.5f;

	// 이후 카메라 매니저 추가 시 실제 카메라 Near, Far Get으로 가져오기 / Camera Create -> Shadow Create
	m_fCameraNear = 0.1f;
	m_fCameraFar = 1000.f;

	if (FAILED(Ready_Cascade_Shadow_Resources()))
		return E_FAIL;

	for (_uint i = 1; i <= m_Cascade.iNumCascades; ++i)
	{
		_float fSplitIndex = static_cast<_float>(i) / static_cast<_float>(m_Cascade.iNumCascades);
		_float fLinear = m_fCameraNear + (m_fCameraFar - m_fCameraNear) * fSplitIndex;
		_float fLog = m_fCameraNear * powf(m_fCameraFar / m_fCameraNear, fSplitIndex);
		m_Cascade.Splits[i - 1] = Lerp(fLinear, fLog, m_fLamda);
	}

    return S_OK;
}

void CShadow::Update()
{
	// 캐스케이드 코너 카메라 절두체 가져와서 비율로 계산
	const _float4* pWorldPoints = m_pGameInstance->Get_WorldPoints();

	for (_uint i = 0; i < m_Cascade.iNumCascades; ++i)
	{
		_float fCascadeNear = (i == 0) ? m_fCameraNear : m_Cascade.Splits[i - 1];
		_float fCascadeFar = m_Cascade.Splits[i];

		_float fNearRatio = (fCascadeNear - m_fCameraNear) / (m_fCameraFar - m_fCameraNear);
		_float fFarRatio = (fCascadeFar - m_fCameraNear) / (m_fCameraFar - m_fCameraNear);

		array<_float4, 8> FustumCorners = {};

		for (_uint j = 0; j < 4; ++j)
		{
			FustumCorners[j] = Lerp(pWorldPoints[j], pWorldPoints[j + 4], fNearRatio);
			FustumCorners[j + 4] = Lerp(pWorldPoints[j], pWorldPoints[j + 4], fFarRatio);
			FustumCorners[j].w = 1.f;
			FustumCorners[j + 4].w = 1.f;
		}

		// ===== 뷰 행렬 구하기 =====

		// 1. 각 절두체의 중심점 구하기
		_vector vCenter = {};
		
		for (_uint j = 0; j < 8; ++j)
			vCenter += XMLoadFloat4(&FustumCorners[j]);
		vCenter /= 8.f;

		// 2. 중점과 코너 사이의 길이가 가장 큰 길이를 구하고 그 길이를 반지름으로 사용
		_float fRadius = {};

		for (_uint j = 0; j < 8; ++j)
		{
			_float fDistance = XMVectorGetX(XMVector3Length(XMLoadFloat4(&FustumCorners[j]) - vCenter));
			fRadius = max(fRadius, fDistance);
		}

		// 반올림 후 나눠서 소수점을 0.0625..단위로 맞춤 -> 그림자 정밀도에서 사용
		fRadius = ceil(fRadius * 16.f) / 16.f;

		// 3. 반지름 구하기
		_vector vMaxExtents = XMVectorSet(fRadius, fRadius, fRadius, 1.f);
		_vector vMinExtents = -vMaxExtents;

		// 4. 광원 기준 뷰 위치 만들기 == Eye
		_vector vShadowCamPos = vCenter - XMVector3Normalize(XMLoadFloat4(&m_vLightDir)) * fRadius;

		_matrix LightViewMatrix = XMMatrixLookAtLH(vShadowCamPos, vCenter, XMVectorSet(0.f, 1.f, 0.f, 0.f));

		XMStoreFloat4x4(&m_Cascade.LightViewMatrices[i], LightViewMatrix);

		_float3 vMinPoint{ FLT_MAX, FLT_MAX, FLT_MAX }, vMaxPoint{ -FLT_MAX, -FLT_MAX, -FLT_MAX };

		for (_uint j = 0; j < 8; ++j)
		{
			XMStoreFloat4(&FustumCorners[j], XMVector3TransformCoord(XMLoadFloat4(&FustumCorners[j]), LightViewMatrix));
		
			vMinPoint.x = min(vMinPoint.x, FustumCorners[j].x);
			vMinPoint.y = min(vMinPoint.y, FustumCorners[j].y);
			vMinPoint.z = min(vMinPoint.z, FustumCorners[j].z);

			vMaxPoint.x = max(vMaxPoint.x, FustumCorners[j].x);
			vMaxPoint.y = max(vMaxPoint.y, FustumCorners[j].y);
			vMaxPoint.z = max(vMaxPoint.z, FustumCorners[j].z);
		}

		// ===== 투영 행렬 구하기 =====
		_matrix LightProjMatrix = XMMatrixOrthographicOffCenterLH
		(
			vMinPoint.x,
			vMaxPoint.x,
			vMinPoint.y,
			vMaxPoint.y,
			vMinPoint.z,
			vMaxPoint.z
		);

		XMStoreFloat4x4(&m_Cascade.LightProjMatrices[i], LightProjMatrix);
	}
}

HRESULT CShadow::Bind_ShadowDSV(_uint iIndex)
{
	if (iIndex >= m_Cascade.iNumCascades)
		return E_FAIL;

	m_pContext->OMSetRenderTargets(0, nullptr, m_ShadowDSVs[iIndex]);
	m_pContext->ClearDepthStencilView(m_ShadowDSVs[iIndex], D3D11_CLEAR_DEPTH, 1.f, 0);

	return S_OK;
}

HRESULT CShadow::Bind_ShadowSRVArray(CShader* pShader, const _char* pConstantName)
{
	return pShader->Bind_SRV(pConstantName, m_pShadowSRVArray);
}

void CShadow::Set_Splits(const _float* pSplits)
{
	memcpy(m_Cascade.Splits.data(), pSplits, sizeof(_float) * m_Cascade.iNumCascades);
}

const _float4x4* CShadow::Get_CurrentLightViewMatrix() const
{
	return &m_Cascade.LightViewMatrices[m_iCurrentCascade];
}

const _float4x4* CShadow::Get_CurrentLightProjMatrix() const
{
	return &m_Cascade.LightProjMatrices[m_iCurrentCascade];
}

void CShadow::Set_Lamda(_float fLamda)
{
	m_fLamda = fLamda;

	// 스플릿 재계산
	for (_uint i = 1; i <= m_Cascade.iNumCascades; ++i)
	{
		_float fSplitIndex = static_cast<_float>(i) / static_cast<_float>(m_Cascade.iNumCascades);
		_float fLinear = m_fCameraNear + (m_fCameraFar - m_fCameraNear) * fSplitIndex;
		_float fLog = m_fCameraNear * powf(m_fCameraFar / m_fCameraNear, fSplitIndex);
		m_Cascade.Splits[i - 1] = Lerp(fLinear, fLog, m_fLamda);
	}
}

HRESULT CShadow::Ready_Cascade_Shadow_Resources()
{
	ID3D11Texture2D* pDepthStencilTexture = { nullptr };

	D3D11_TEXTURE2D_DESC Desc{};
	Desc.Width = g_iMaxWidth;
	Desc.Height = g_iMaxHeight;
	Desc.MipLevels = 1;
	Desc.ArraySize = m_Cascade.iNumCascades;
	Desc.Format = DXGI_FORMAT_R32_TYPELESS;
	Desc.SampleDesc.Count = 1;
	Desc.SampleDesc.Quality = 0;
	Desc.Usage = D3D11_USAGE_DEFAULT;
	Desc.CPUAccessFlags = 0;
	Desc.MiscFlags = 0;
	Desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;

	if (FAILED(m_pDevice->CreateTexture2D(&Desc, nullptr, &pDepthStencilTexture)))
		return E_FAIL;

	for (_uint i = 0; i < m_Cascade.iNumCascades; ++i)
	{
		D3D11_DEPTH_STENCIL_VIEW_DESC DSVDesc{};
		DSVDesc.Format = DXGI_FORMAT_D32_FLOAT;
		DSVDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
		DSVDesc.Texture2DArray.MipSlice = 0;
		DSVDesc.Texture2DArray.FirstArraySlice = i;
		DSVDesc.Texture2DArray.ArraySize = 1;
		if (FAILED(m_pDevice->CreateDepthStencilView(pDepthStencilTexture, &DSVDesc, &m_ShadowDSVs[i])))
			return E_FAIL;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc{};
	SRVDesc.Format = DXGI_FORMAT_R32_FLOAT;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	SRVDesc.Texture2DArray.MostDetailedMip = 0;
	SRVDesc.Texture2DArray.MipLevels = 1;
	SRVDesc.Texture2DArray.FirstArraySlice = 0;
	SRVDesc.Texture2DArray.ArraySize = m_Cascade.iNumCascades;
	if (FAILED(m_pDevice->CreateShaderResourceView(pDepthStencilTexture, &SRVDesc, &m_pShadowSRVArray)))
		return E_FAIL;

	Safe_Release(pDepthStencilTexture);

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

	for (auto& pDSV : m_ShadowDSVs)
		Safe_Release(pDSV);

	Safe_Release(m_pShadowSRVArray);

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
}
