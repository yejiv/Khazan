#include "Camera_Manager.h"
#include "GameInstance.h"
#include "Camera.h"


CCamera_Manager::CCamera_Manager()
	: m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}


HRESULT CCamera_Manager::Initialize(_uint iNumLevels)
{
	m_pCameras = new vector<CCamera*>[iNumLevels];

	m_iNumLevels = iNumLevels;

	return S_OK;
}

HRESULT CCamera_Manager::Add_Camera(_uint iLevelIndex, CCamera* pCamera)
{
	m_pCameras[iLevelIndex].push_back(pCamera);

	return S_OK;
}

void CCamera_Manager::Change_Camera(_uint iLevelIndex, _uint iCameraType)
{
	for (auto& pCamera : m_pCameras[iLevelIndex])
	{
		if (pCamera->Get_CameraType() == iCameraType)
		{
			pCamera->Set_IsActive(true);
			m_pActiveCamera = pCamera;
		}
		else
			pCamera->Set_IsActive(false);
	}
}

void CCamera_Manager::Change_Camera(_uint iLevelIndex, _wstring strCameraTag)
{
	for (auto& pCamera : m_pCameras[iLevelIndex])
	{
		if (pCamera->Get_CameraTag() == strCameraTag)
		{
			pCamera->Set_IsActive(true);
			m_pActiveCamera = pCamera;
		}
		else
			pCamera->Set_IsActive(false);
	}
}

CCamera* CCamera_Manager::Find_Camera(_uint iLevelIndex, _uint iCameraType)
{
	for (auto& pCamera : m_pCameras[iLevelIndex])
	{
		if (pCamera->Get_CameraType() == iCameraType)
			return pCamera;
	}

	return nullptr;
}

void CCamera_Manager::Clear(_uint iLevelIndex)
{
	if (iLevelIndex >= m_iNumLevels)
		return;

	m_pCameras[iLevelIndex].clear();
}

CCamera_Manager* CCamera_Manager::Create(_uint iNumLevels)
{
	CCamera_Manager* pInstance = new CCamera_Manager();

	if (FAILED(pInstance->Initialize(iNumLevels)))
	{
		MSG_BOX(TEXT("Failed to Created : CCamera_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CCamera_Manager::Free()
{
	__super::Free();

	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		m_pCameras[i].clear();
	}

	Safe_Release(m_pGameInstance);

	Safe_Delete_Array(m_pCameras);

}
