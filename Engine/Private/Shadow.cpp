#include "Shadow.h"
#include "GameInstance.h"

CShadow::CShadow()
	: m_pGameInstance { CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

const _float4x4* CShadow::Get_Transform_Float4x4(D3DTS eTransformState) const
{
    return &m_Matrices[ENUM_CLASS(eTransformState)];
}

HRESULT CShadow::Initialize(_uint iWinSizeX, _uint iWinSizeY)
{
	m_fViewportWidth = iWinSizeX;
	m_fViewportHeight = iWinSizeY;

	m_iNumCascade = g_iNumCascades;

	m_Splits.resize(m_iNumCascade);
	m_FustumCorners.resize(m_iNumCascade);
	m_LightViewMatrix.resize(m_iNumCascade);
	m_LightProjMatrix.resize(m_iNumCascade);

	// 이후 Directional Light 추가 될 시 갱신 해주기
	m_vLightDir = { 1.f, -1.f, 1.f, 0.f };

	// log, linear mix 수치
	m_fLamda = 0.5f;

	// 이후 카메라 매니저 추가 시 실제 카메라 Near, Far Get으로 가져오기 / Camera Create -> Shadow Create
	m_fCameraNear = 0.1f;
	m_fCameraFar = 1000.f;

    return S_OK;
}

void CShadow::Update()
{
	for (_uint i = 1; i <= m_iNumCascade; ++i)
	{
		_float fSplitIndex = static_cast<_float>(i) / static_cast<_float>(m_iNumCascade);
		_float fLinear = m_fCameraNear + (m_fCameraFar - m_fCameraNear) * fSplitIndex;
		_float fLog = m_fCameraNear * powf(m_fCameraFar / m_fCameraNear, fSplitIndex);
		m_Splits[i - 1] = Lerp(fLinear, fLog, m_fLamda);
	}

	// 캐스케이드 코너 카메라 절두체 가져와서 비율로 계산
	const _float4* pPoints = m_pGameInstance->Get_WorldPoints();

	for (_uint i = 0; i < m_iNumCascade; ++i)
	{
		_float fCascadeNear = (i == 0) ? m_fCameraNear : m_Splits[i - 1];
		_float fCascadeFar = m_Splits[i];

		_float fNearRatio = (fCascadeNear - m_fCameraNear) / (m_fCameraFar - m_fCameraNear);
		_float fFarRatio = (fCascadeFar - m_fCameraNear) / (m_fCameraFar - m_fCameraNear);

		array<_float4, 8> Points = {};

		for (_uint j = 0; j < 4; ++j)
		{
			Points[j] = Lerp(pPoints[j], pPoints[j + 4], fNearRatio);
			Points[j + 4] = Lerp(pPoints[j], pPoints[j + 4], fFarRatio);
			Points[j].w = 1.f;
			Points[j + 4].w = 1.f;
		}

		m_FustumCorners[i] = Points;

		// ===== 뷰 행렬 구하기 =====

		// 1. 각 절두체의 중심점 구하기
		_vector vFustumCorner, vCenter = {};
		
		for (_uint j = 0; j < 8; ++j)
		{
			vFustumCorner = XMLoadFloat4(&m_FustumCorners[i][j]);
			vCenter += vFustumCorner;
		}
		vCenter /= 8.f;

		// 2. 중점과 코너 사이의 길이가 가장 큰 길이를 구하고 그 길이를 반지름으로 사용
		_float fRadius = {};
		for (_uint j = 0; j < 8; ++j)
		{
			_float fDistance = XMVectorGetX(XMVector3Length(XMLoadFloat4(&m_FustumCorners[i][j]) - vCenter));
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

		XMStoreFloat4x4(&m_LightViewMatrix[i], LightViewMatrix);

		_float3 vMinPoint{ FLT_MAX, FLT_MAX, FLT_MAX }, vMaxPoint{ -FLT_MAX, -FLT_MAX, -FLT_MAX };

		for (_uint j = 0; j < 8; ++j)
		{
			XMStoreFloat4(&m_FustumCorners[i][j], XMVector3TransformCoord(XMLoadFloat4(&m_FustumCorners[i][j]), LightViewMatrix));
		
			vMinPoint.x = min(vMinPoint.x, m_FustumCorners[i][j].x);
			vMinPoint.y = min(vMinPoint.y, m_FustumCorners[i][j].y);
			vMinPoint.z = min(vMinPoint.z, m_FustumCorners[i][j].z);

			vMaxPoint.x = max(vMaxPoint.x, m_FustumCorners[i][j].x);
			vMaxPoint.y = max(vMaxPoint.y, m_FustumCorners[i][j].y);
			vMaxPoint.z = max(vMaxPoint.z, m_FustumCorners[i][j].z);
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

		XMStoreFloat4x4(&m_LightProjMatrix[i], LightProjMatrix);
	}
}

HRESULT CShadow::Ready_ShadowLight(SHADOW_LIGHT_DESC LightDesc)
{
	m_ShadowLight = LightDesc;

	XMStoreFloat4x4(&m_Matrices[ENUM_CLASS(D3DTS::VIEW)],
		XMMatrixLookAtLH(XMLoadFloat4(&LightDesc.vEye), XMLoadFloat4(&LightDesc.vAt), XMVectorSet(0.f, 1.f, 0.f, 0.f)));

	XMStoreFloat4x4(&m_Matrices[ENUM_CLASS(D3DTS::PROJ)],
		XMMatrixPerspectiveFovLH(LightDesc.fFovy, m_fViewportWidth / m_fViewportHeight, LightDesc.fNear, LightDesc.fFar));

	return S_OK;
}

const _float4x4* CShadow::Get_CurrentLightViewMatrix() const
{
	return &m_LightViewMatrix[m_iCurrentCascade];
}

const _float4x4* CShadow::Get_CurrentLightProjMatrix() const
{
	return &m_LightProjMatrix[m_iCurrentCascade];
}

CShadow* CShadow::Create(_uint iWinSizeX, _uint iWinSizeY)
{
	CShadow* pInstance = new CShadow();

	if (FAILED(pInstance->Initialize(iWinSizeX, iWinSizeY)))
	{
		MSG_BOX(TEXT("Failed to Create : CShadow"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CShadow::Free()
{
    __super::Free();

	Safe_Release(m_pGameInstance);
}
