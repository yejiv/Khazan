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

CCamera* CCamera_Manager::Find_Camera(_uint iLevelIndex, _wstring strCameraTag)
{
	for (auto& pCamera : m_pCameras[iLevelIndex])
	{
		if (pCamera->Get_CameraTag() == strCameraTag)
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

_float3 CCamera_Manager::Get_ActiveCameraPos()
{
	if (m_pActiveCamera == nullptr)
		return _float3(0.f, 0.f, 0.f);

	CTransform* pTransform = dynamic_cast<CTransform*>(m_pActiveCamera->Get_Component(TEXT("Com_Transform")));

	_float3 vPos{};
	XMStoreFloat3(&vPos, pTransform->Get_State(STATE::POSITION));

	return vPos;
}

_float4 CCamera_Manager::Get_ActiveCameraLook()
{
	if (m_pActiveCamera == nullptr)
		return _float4(0.f, 0.f, 0.f, 0.f);

	CTransform* pTransform = dynamic_cast<CTransform*>(m_pActiveCamera->Get_Component(TEXT("Com_Transform")));

	_float4 vLook{};
	XMStoreFloat4(&vLook, pTransform->Get_State(STATE::LOOK));

	return vLook;
}

void CCamera_Manager::ActiveCamera_Shaking(_float fPower, _float fDuration)
{
	if (m_pActiveCamera == nullptr)
		return;

	m_pActiveCamera->Shaking_Start(fPower, fDuration);
}

void CCamera_Manager::ActiveCamera_PushFOVModifier(const FOVModifier& tNewModifier)
{
	if (m_pActiveCamera == nullptr)
		return;

	m_pActiveCamera->Push_FOVModifier(tNewModifier);
}

void CCamera_Manager::ActiveCamera_KillFov(const _wstring& strID)
{
	if(m_pActiveCamera == nullptr)
		return;

	m_pActiveCamera->Kill_FOVModifier(strID);
}

void CCamera_Manager::Save_Json(_uint iLevelIndex, _wstring strCameraTag, nlohmann::ordered_json& pOutData)
{
	CCamera* pCamera = Find_Camera(iLevelIndex, strCameraTag);
	CCamera::CAMERA_DESC CameraDesc = pCamera->Get_CameraDesc();

	nlohmann::ordered_json Data;

	Data["Name"] = WStringToAnsi(CameraDesc.strCameraTag);
	Data["Eye"]["x"] = CameraDesc.vEye.x;
	Data["Eye"]["y"] = CameraDesc.vEye.y;
	Data["Eye"]["z"] = CameraDesc.vEye.z;
	Data["Eye"]["w"] = CameraDesc.vEye.w;
	Data["At"]["x"] = CameraDesc.vAt.x;
	Data["At"]["y"] = CameraDesc.vAt.y;
	Data["At"]["z"] = CameraDesc.vAt.z;
	Data["At"]["w"] = CameraDesc.vAt.w;
	Data["Fovy"] = CameraDesc.fFovy;
	Data["Near"] = CameraDesc.fNear;
	Data["Far"] = CameraDesc.fFar;
	Data["SpeedPerSec"] = CameraDesc.fSpeedPerSec;
	Data["RotationPerSec"] = CameraDesc.fRotationPerSec;
	Data["MouseSensor"] = CameraDesc.fMouseSensor;
	Data["CameraType"] = CameraDesc.iCameraType;

	map<_wstring, vector<CAMERA_KEYFRAME>>* Animations = pCamera->Get_AllAnimations();

	for (auto Animation : *Animations)
	{
		nlohmann::ordered_json AnimationData;
		AnimationData["Name"] = Animation.first.c_str();
		for (auto Ani : Animation.second)
		{
			nlohmann::ordered_json AniData;
			AniData["Translation"]["x"] = Ani.vTranslation.x;
			AniData["Translation"]["y"] = Ani.vTranslation.y;
			AniData["Translation"]["z"] = Ani.vTranslation.z;
			AniData["LookAt"]["x"] = Ani.vLookAt.x;
			AniData["LookAt"]["y"] = Ani.vLookAt.y;
			AniData["LookAt"]["z"] = Ani.vLookAt.z;
			AniData["LookAt"]["w"] = Ani.vLookAt.w;
			AniData["Speed"] = Ani.fSpeed;
			AniData["TrackPosition"] = Ani.fTrackPosition;

			AnimationData["Animations"].push_back(AniData);
		}
		Data["Animation"].push_back(AnimationData);
	}

	map<_wstring, vector<CAMERA_EVENT_DATA>>* Events = pCamera->Get_AllEvents();

	for (auto Event : *Events)
	{
		nlohmann::ordered_json EventData;
		EventData["Name"] = Event.first.c_str();
		for (auto Eve : Event.second)
		{
			nlohmann::ordered_json EveData;
			EveData["EventType"] = Eve.iEventType;
			EveData["EventKey"] = Eve.strEventKey.c_str();
			EveData["isComplete"] = Eve.isComplete;
			EveData["TrackPosition"] = Eve.fTrackPosition;

			EventData["Events"].push_back(EveData);
		}
		Data["Event"].push_back(EventData);
	}
	pOutData = Data;
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
