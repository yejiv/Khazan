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
	_float fNear = 0.1f;
	_float fFar = 1000.f; // 나중에 파이프 라인으로 옮겨야 할 듯
	_float fRatio = 0.5f; // 따로 선언

	m_iNumCascade = 3; // 이것도 인자로 받기?

	m_Splits.resize(m_iNumCascade);
	for (_uint i = 1; i <= m_iNumCascade; ++i)
	{
		_float fSplitIndex = static_cast<_float>(i) / static_cast<_float>(m_iNumCascade);
		_float fLinear = fNear + (fFar - fNear) * fSplitIndex;
		_float fLog = fNear * powf(fFar / fNear, fSplitIndex);
		m_Splits[i - 1] = Lerp(fLinear, fLog, fRatio);
	}

	// 뷰 스페이스 절두체 -> 월드로 변환하기
	// 캐스케이드 개수만큼 반복문
	// 인덱스 0번이면 Near = 0.1 아니면 스플릿의 i - 1번 == 0번 캐스케이드는 0.1이 Near, 1번 캐스케이드는 0번의 Far가 Near
	// Far는 그냥 인덱스가 의미 Splits[0] == far 
	// 이걸 카메라 뷰 공간에서 8점 계산 각각 Near Far 계산해서 이 포인트를 뷰역행렬 곱해서 월드로 변환한 뒤 저장
	
	m_FustumCorners.resize(m_iNumCascade);
	for (_uint i = 0; i < m_iNumCascade; ++i)
	{
		_float fFrustumNear = (i == 0) ? fNear : m_Splits[i - 1];
		_float fFrustumFar = m_Splits[i];

		array<_float4, 8> Points =
		{
			_float4(-1.f, 1.f, fFrustumNear, 1.f),
			_float4(1.f, 1.f, fFrustumNear, 1.f),
			_float4(1.f, -1.f, fFrustumNear, 1.f),
			_float4(-1.f, -1.f, fFrustumNear, 1.f),

			_float4(-1.f, 1.f, fFrustumFar, 1.f),
			_float4(1.f, 1.f, fFrustumFar, 1.f),
			_float4(1.f, -1.f, fFrustumFar, 1.f),
			_float4(-1.f, -1.f, fFrustumFar, 1.f)
		};

		_matrix		ViewMatrixInverse = m_pGameInstance->Get_Transform_Matrix_Inverse(D3DTS::VIEW);
		_matrix		ProjMatrixInverse = m_pGameInstance->Get_Transform_Matrix_Inverse(D3DTS::PROJ);

		for (_uint i = 0; i < 8; ++i)
		{
			_vector vPoint = XMLoadFloat4(&Points[i]);
			vPoint = XMVector3TransformCoord(vPoint, ProjMatrixInverse);
			vPoint = XMVector3TransformCoord(vPoint, ViewMatrixInverse);
			XMStoreFloat4(&Points[i], vPoint);
		}

		m_FustumCorners[i] = Points;

		int a = 10;
	}

	// XMMatrixOrthographicOffCenterLH -> LightView 기준으로 x, y, z 최소 / 최대 값을 구함, 직교 투영으로 그림자맵 범위 설정
	// 카메라가 보는 절두체 공간을 광원 기준에서 평면으로 투영하기 위함

	// 포인트와 광원 기준의 View 행렬 곱하기
	// 포인트의 최대 최소 구하기
	// 구한 최대 최소 값을 XMMatrixOrthographicOffCenterLH 인자로 넣어 투영 행렬 만들고
	// 기존 뷰 행렬과 구한 투영 행렬을 곱해서 배열에 저장하기

	m_LightViewProjMatrix.resize(m_iNumCascade);

	_matrix LightViewMatrix = XMLoadFloat4x4(&m_Matrices[ENUM_CLASS(D3DTS::VIEW)]);

	for (_uint i = 0; i < m_iNumCascade; ++i)
	{
		_float3 vMinPoint{ -FLT_MAX, -FLT_MAX, -FLT_MAX }, vMaxPoint{ FLT_MAX, FLT_MAX, FLT_MAX };

		for (auto& vPoint : m_FustumCorners[i])
		{
			// 광원 공간에서 AABB 구하기
			XMStoreFloat4(&vPoint, XMVector3TransformCoord(XMLoadFloat4(&vPoint), LightViewMatrix));

			vMinPoint.x = min(vMinPoint.x, vPoint.x);
			vMinPoint.y = min(vMinPoint.y, vPoint.y);
			vMinPoint.z = min(vMinPoint.z, vPoint.z);

			vMaxPoint.x = max(vMaxPoint.x, vPoint.x);
			vMaxPoint.y = max(vMaxPoint.y, vPoint.y);
			vMaxPoint.z = max(vMaxPoint.z, vPoint.z);
		}

		_matrix LightProjMatrix = XMMatrixOrthographicOffCenterLH(
			vMinPoint.x,
			vMaxPoint.x,
			vMinPoint.y,
			vMaxPoint.y,
			vMinPoint.z,
			vMaxPoint.z);

		XMStoreFloat4x4(&m_LightViewProjMatrix[i], LightViewMatrix * LightProjMatrix);
	}

	return S_OK;
}

HRESULT CShadow::Bind_LightViewProjMatrix(CShader* pShader, _uint iIndex)
{
	return pShader->Bind_Matrix("g_LightViewProjMatrices", &m_LightViewProjMatrix[iIndex]);
}

const _float4x4* CShadow::Get_CurrentLightViewProjMatrix() const
{
	return &m_LightViewProjMatrix[m_iCurrentCascade];
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
