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

	if (FAILED(Ready_Cascade()))
		return E_FAIL;

    return S_OK;
}

void CShadow::Update()
{
	_float fNear = 0.1f;
	_float fFar = 1000.f; // 나중에 파이프 라인으로 옮겨야 할 듯
	_float fRatio = 0.5f; // 따로 선언
	_vector vLightDir = { 1.f, -1.f, 1.f, 0.f };

	for (_uint i = 1; i <= m_iNumCascade; ++i)
	{
		_float fSplitIndex = static_cast<_float>(i) / static_cast<_float>(m_iNumCascade);
		_float fLinear = fNear + (fFar - fNear) * fSplitIndex;
		_float fLog = fNear * powf(fFar / fNear, fSplitIndex);
		m_Splits[i - 1] = Lerp(fLinear, fLog, fRatio);
	}

	// 캐스케이드 코너 카메라 절두체 가져와서 비율로 계산
	const _float4* pPoints = m_pGameInstance->Get_WorldPoints();

	for (_uint i = 0; i < m_iNumCascade; ++i)
	{
		_float fCascadeNear = (i == 0) ? fNear : m_Splits[i - 1];
		_float fCascadeFar = m_Splits[i];

		_float fNearRatio = (fCascadeNear - fNear) / (fFar - fNear);
		_float fFarRatio = (fCascadeFar - fNear) / (fFar - fNear);

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
		_vector vShadowCamPos = vCenter + XMVector3Normalize(vLightDir) * XMVectorGetZ(vMinExtents);

		_matrix LightViewMatrix = XMMatrixLookAtLH(vShadowCamPos, vCenter, XMVectorSet(0.f, 1.f, 0.f, 0.f));

		XMStoreFloat4x4(&m_LightViewMatrix[i], LightViewMatrix);

		for (_uint j = 0; j < 8; ++j)
		{
			XMStoreFloat4(&m_FustumCorners[i][j], XMVector3TransformCoord(XMLoadFloat4(&m_FustumCorners[i][j]), LightViewMatrix));
		}

		_float3 vMinPoint{ FLT_MAX, FLT_MAX, FLT_MAX }, vMaxPoint{ -FLT_MAX, -FLT_MAX, -FLT_MAX };

		for (auto& vLightPoint : m_FustumCorners[i])
		{
			vMinPoint.x = min(vMinPoint.x, vLightPoint.x);
			vMinPoint.y = min(vMinPoint.y, vLightPoint.y);
			vMinPoint.z = min(vMinPoint.z, vLightPoint.z);

			vMaxPoint.x = max(vMaxPoint.x, vLightPoint.x);
			vMaxPoint.y = max(vMaxPoint.y, vLightPoint.y);
			vMaxPoint.z = max(vMaxPoint.z, vLightPoint.z);
		}

		// ===== 투영 행렬 구하기 =====
		//	_float4 vCascadeExtents = {};
		//	XMStoreFloat4(&vCascadeExtents, vMaxExtents - vMinExtents);
		//	
		//	_matrix LightProjMatrix = XMMatrixOrthographicOffCenterLH
		//	(
		//		XMVectorGetX(vMinExtents),
		//		XMVectorGetX(vMaxExtents),
		//		XMVectorGetY(vMinExtents),
		//		XMVectorGetY(vMaxExtents),
		//		0.1f,
		//		vCascadeExtents.z
		//	);

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

		int a = 10;
	}

	//for (_uint i = 0; i < m_iNumCascade; ++i)
	//{
	//	_float fFrustumNear = (i == 0) ? fNear : m_Splits[i - 1];
	//	_float fFrustumFar = m_Splits[i];

	//	array<_float4, 8> Points =
	//	{
	//		_float4(-1.f, 1.f, fFrustumNear, 1.f),
	//		_float4(1.f, 1.f, fFrustumNear, 1.f),
	//		_float4(1.f, -1.f, fFrustumNear, 1.f),
	//		_float4(-1.f, -1.f, fFrustumNear, 1.f),

	//		_float4(-1.f, 1.f, fFrustumFar, 1.f),
	//		_float4(1.f, 1.f, fFrustumFar, 1.f),
	//		_float4(1.f, -1.f, fFrustumFar, 1.f),
	//		_float4(-1.f, -1.f, fFrustumFar, 1.f)
	//	};

	//	_matrix		ViewMatrixInverse = m_pGameInstance->Get_Transform_Matrix_Inverse(D3DTS::VIEW);
	//	_matrix		ProjMatrixInverse = m_pGameInstance->Get_Transform_Matrix_Inverse(D3DTS::PROJ);

	//	for (_uint j = 0; j < 8; ++j)
	//	{
	//		_vector vPoint = XMLoadFloat4(&Points[j]);
	//		vPoint = XMVector3TransformCoord(vPoint, ProjMatrixInverse);
	//		vPoint = XMVector3TransformCoord(vPoint, ViewMatrixInverse);
	//		XMStoreFloat4(&Points[j], vPoint);
	//	}

	//	m_FustumCorners[i] = Points;

	//	int a = 10;
	//}

	//for (_uint i = 0; i < m_iNumCascade; ++i)
	//{
	//	_float3 vMinPoint{ FLT_MAX, FLT_MAX, FLT_MAX }, vMaxPoint{ -FLT_MAX, -FLT_MAX, -FLT_MAX };

	//	for (auto& vWorldPoint : m_FustumCorners[i])
	//	{
	//		vMinPoint.x = min(vMinPoint.x, vWorldPoint.x);
	//		vMinPoint.y = min(vMinPoint.y, vWorldPoint.y);
	//		vMinPoint.z = min(vMinPoint.z, vWorldPoint.z);

	//		vMaxPoint.x = max(vMaxPoint.x, vWorldPoint.x);
	//		vMaxPoint.y = max(vMaxPoint.y, vWorldPoint.y);
	//		vMaxPoint.z = max(vMaxPoint.z, vWorldPoint.z);
	//	}

	//	// LightDir -> 나중에 실제 Directional Light 가져오도록 수정
	//	_vector vLightDir = { 1.f, -1.f, 1.f, 0.f };

	//	// Light View 행렬 구하기
	//	_vector vCenter = XMVectorSetW(0.5f * XMLoadFloat3(&vMinPoint) + XMLoadFloat3(&vMaxPoint), 1.f);
	//	
	//	// Extents
	//	_vector vExtents = XMVectorSetW(0.5f * XMLoadFloat3(&vMaxPoint) - XMLoadFloat3(&vMinPoint), 1.f);
	//	_float fRadius = XMVectorGetX(XMVector3Length(vExtents));

	//	// Eye
	//	_vector vEye = vCenter - vLightDir * (fRadius + 1.f);

	//	_matrix LightViewMatrix = XMMatrixLookAtLH(vEye, vCenter, XMVectorSet(0.f, 1.f, 0.f, 0.f));
	//	XMStoreFloat4x4(&m_LightViewMatrix[i], LightViewMatrix);

	//	_float4 vLightViewPoint = {};
	//	vMinPoint = { FLT_MAX, FLT_MAX, FLT_MAX };
	//	vMaxPoint = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

	//	for (auto& vWorldPoint : m_FustumCorners[i])
	//	{
	//		XMStoreFloat4(&vLightViewPoint, XMVector3TransformCoord(XMLoadFloat4(&vWorldPoint), LightViewMatrix));

	//		vMinPoint.x = min(vMinPoint.x, vLightViewPoint.x);
	//		vMinPoint.y = min(vMinPoint.y, vLightViewPoint.y);
	//		vMinPoint.z = min(vMinPoint.z, vLightViewPoint.z);

	//		vMaxPoint.x = max(vMaxPoint.x, vLightViewPoint.x);
	//		vMaxPoint.y = max(vMaxPoint.y, vLightViewPoint.y);
	//		vMaxPoint.z = max(vMaxPoint.z, vLightViewPoint.z);
	//	}

	//	// Light Proj 행렬 구하기
	//	_matrix LightProjMatrix = XMMatrixOrthographicOffCenterLH(
	//		vMinPoint.x,
	//		vMaxPoint.x,
	//		vMinPoint.y,
	//		vMaxPoint.y,
	//		vMinPoint.z,
	//		vMaxPoint.z);

	//	XMStoreFloat4x4(&m_LightProjMatrix[i], LightProjMatrix);

	//	int a = 10;
	//}
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

HRESULT CShadow::Ready_Cascade()
{
	m_iNumCascade = 3; // 이것도 인자로 받기?

	m_Splits.resize(m_iNumCascade);

	//	_float fNear = 0.1f;
	//	_float fFar = 1000.f; // 나중에 파이프 라인으로 옮겨야 할 듯
	//	_float fRatio = 0.5f; // 따로 선언
	//	for (_uint i = 1; i <= m_iNumCascade; ++i)
	//	{
	//		_float fSplitIndex = static_cast<_float>(i) / static_cast<_float>(m_iNumCascade);
	//		_float fLinear = fNear + (fFar - fNear) * fSplitIndex;
	//		_float fLog = fNear * powf(fFar / fNear, fSplitIndex);
	//		m_Splits[i - 1] = Lerp(fLinear, fLog, fRatio);
	//	}

	// 뷰 스페이스 절두체 -> 월드로 변환하기
	// 캐스케이드 개수만큼 반복문
	// 인덱스 0번이면 Near = 0.1 아니면 스플릿의 i - 1번 == 0번 캐스케이드는 0.1이 Near, 1번 캐스케이드는 0번의 Far가 Near
	// Far는 그냥 인덱스가 의미 Splits[0] == far 
	// 이걸 카메라 뷰 공간에서 8점 계산 각각 Near Far 계산해서 이 포인트를 뷰역행렬 곱해서 월드로 변환한 뒤 저장
	
	m_FustumCorners.resize(m_iNumCascade);
	//	for (_uint i = 0; i < m_iNumCascade; ++i)
	//	{
	//		_float fFrustumNear = (i == 0) ? fNear : m_Splits[i - 1];
	//		_float fFrustumFar = m_Splits[i];
	//	
	//		array<_float4, 8> Points =
	//		{
	//			_float4(-1.f, 1.f, fFrustumNear, 1.f),
	//			_float4(1.f, 1.f, fFrustumNear, 1.f),
	//			_float4(1.f, -1.f, fFrustumNear, 1.f),
	//			_float4(-1.f, -1.f, fFrustumNear, 1.f),
	//	
	//			_float4(-1.f, 1.f, fFrustumFar, 1.f),
	//			_float4(1.f, 1.f, fFrustumFar, 1.f),
	//			_float4(1.f, -1.f, fFrustumFar, 1.f),
	//			_float4(-1.f, -1.f, fFrustumFar, 1.f)
	//		};
	//	
	//		_matrix		ViewMatrixInverse = m_pGameInstance->Get_Transform_Matrix_Inverse(D3DTS::VIEW);
	//		_matrix		ProjMatrixInverse = m_pGameInstance->Get_Transform_Matrix_Inverse(D3DTS::PROJ);
	//	
	//		for (_uint i = 0; i < 8; ++i)
	//		{
	//			_vector vPoint = XMLoadFloat4(&Points[i]);
	//			vPoint = XMVector3TransformCoord(vPoint, ProjMatrixInverse);
	//			vPoint = XMVector3TransformCoord(vPoint, ViewMatrixInverse);
	//			XMStoreFloat4(&Points[i], vPoint);
	//		}
	//	
	//		m_FustumCorners[i] = Points;
	//	
	//		int a = 10;
	//	}

	// XMMatrixOrthographicOffCenterLH -> LightView 기준으로 x, y, z 최소 / 최대 값을 구함, 직교 투영으로 그림자맵 범위 설정
	// 카메라가 보는 절두체 공간을 광원 기준에서 평면으로 투영하기 위함

	// 포인트와 광원 기준의 View 행렬 곱하기
	// 포인트의 최대 최소 구하기
	// 구한 최대 최소 값을 XMMatrixOrthographicOffCenterLH 인자로 넣어 투영 행렬 만들고
	// 기존 뷰 행렬과 구한 투영 행렬을 곱해서 배열에 저장하기

	m_LightViewMatrix.resize(m_iNumCascade);
	m_LightProjMatrix.resize(m_iNumCascade);

	//	_matrix LightViewMatrix = XMLoadFloat4x4(&m_Matrices[ENUM_CLASS(D3DTS::VIEW)]);
	//	
	//	for (_uint i = 0; i < m_iNumCascade; ++i)
	//	{
	//		// 여기가 잘못됐나봐
	//	
	//		_float3 vMinPoint{ -FLT_MAX, -FLT_MAX, -FLT_MAX }, vMaxPoint{ FLT_MAX, FLT_MAX, FLT_MAX };
	//	
	//		for (auto& vPoint : m_FustumCorners[i])
	//		{
	//			// 광원 공간에서 AABB 구하기
	//			XMStoreFloat4(&vPoint, XMVector3TransformCoord(XMLoadFloat4(&vPoint), LightViewMatrix));
	//	
	//			vMinPoint.x = min(vMinPoint.x, vPoint.x);
	//			vMinPoint.y = min(vMinPoint.y, vPoint.y);
	//			vMinPoint.z = min(vMinPoint.z, vPoint.z);
	//	
	//			vMaxPoint.x = max(vMaxPoint.x, vPoint.x);
	//			vMaxPoint.y = max(vMaxPoint.y, vPoint.y);
	//			vMaxPoint.z = max(vMaxPoint.z, vPoint.z);
	//		}
	//	
	//		_matrix LightProjMatrix = XMMatrixOrthographicOffCenterLH(
	//			vMinPoint.x,
	//			vMaxPoint.x,
	//			vMinPoint.y,
	//			vMaxPoint.y,
	//			vMinPoint.z,
	//			vMaxPoint.z);
	//	
	//		XMStoreFloat4x4(&m_LightViewProjMatrix[i], LightViewMatrix * LightProjMatrix);
	//	}

	return S_OK;
}

void CShadow::Set_CurrentCascade(_uint iIndex)
{
	m_iCurrentCascade = iIndex;

	int a = 10;
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
		MSG_BOX(TEXT("Failed to Created : CShadow"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CShadow::Free()
{
    __super::Free();

	Safe_Release(m_pGameInstance);
}
