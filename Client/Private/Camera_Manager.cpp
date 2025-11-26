#include "Camera_Manager.h"
#include "GameInstance.h"
#include "Camera.h"
#include "Camera_Compre.h"


CCamera_Manager::CCamera_Manager()
	: m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}


HRESULT CCamera_Manager::Initialize(_uint iNumLevels)
{
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

CCamera* CCamera_Manager::Find_Camera(_uint iLevelIndex, CAMERATYPE eCameraType)
{
	for (auto& pCamera : m_pCameras[iLevelIndex])
	{
		if (pCamera->Get_CameraType() == ENUM_CLASS(eCameraType))
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

void CCamera_Manager::Start_ForceOrbit(CAMERA_FORCE_DIR eForceDir)
{
    if (Get_ActiveCamera()->Get_CameraType() == ENUM_CLASS(CAMERATYPE::PLAYER))
    {
        CCamera_Compre* pCamera = dynamic_cast<CCamera_Compre*>(Get_ActiveCamera());

        pCamera->Start_ForceOrbit(eForceDir);
    }
}

void CCamera_Manager::ActiveCamera_InteractMove()
{
    if (Get_ActiveCamera()->Get_CameraType() == ENUM_CLASS(CAMERATYPE::PLAYER))
    {
        CCamera_Compre* pCamera = dynamic_cast<CCamera_Compre*>(Get_ActiveCamera());

        pCamera->Start_InteractFocus(CAMERA_FORCE_DIR::FRONT, 0.8f, 0.25f, true);
    }
}

void CCamera_Manager::DeactivateCamera_InteractMove()
{
    if (Get_ActiveCamera()->Get_CameraType() == ENUM_CLASS(CAMERATYPE::PLAYER))
    {
        CCamera_Compre* pCamera = dynamic_cast<CCamera_Compre*>(Get_ActiveCamera());

        pCamera->Exit_PostForceFrameRight();
    }
}

void CCamera_Manager::Yetuga_Holding_Start()
{
    if (Get_ActiveCamera()->Get_CameraType() == ENUM_CLASS(CAMERATYPE::PLAYER))
    {
        CCamera_Compre* pCamera = dynamic_cast<CCamera_Compre*>(Get_ActiveCamera());

        pCamera->Yetuga_Holding_Start();
    }
}
void CCamera_Manager::Yetuga_Holding_End()
{
    if (Get_ActiveCamera()->Get_CameraType() == ENUM_CLASS(CAMERATYPE::PLAYER))
    {
        CCamera_Compre* pCamera = dynamic_cast<CCamera_Compre*>(Get_ActiveCamera());

        pCamera->Yetuga_Holding_End();
    }
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

	map<_wstring, CAMERA_ANIMATION>* Animations = pCamera->Get_AllAnimations();

	for (auto Animation : *Animations)
	{
		nlohmann::ordered_json AnimationData;
		AnimationData["Name"] = Animation.first.c_str();
        AnimationData["isFix"] = Animation.second.isFix;
		for (auto Ani : Animation.second.KeyFrames)
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
			AniData["isCurPos"] = Ani.isCurPos;

			AnimationData["KeyFrame"].push_back(AniData);
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

void CCamera_Manager::Save_Json_Animation(_uint iLevelIndex, _wstring strCameraTag, nlohmann::ordered_json& pOutData)
{
	CCamera* pCamera = Find_Camera(iLevelIndex, strCameraTag);
	CCamera::CAMERA_DESC CameraDesc = pCamera->Get_CameraDesc();

    map<_wstring, CAMERA_ANIMATION>* Animations = pCamera->Get_AllAnimations();

	for (auto Animation : *Animations)
	{
		nlohmann::ordered_json AnimationData;        
		AnimationData["Name"] = Animation.first.c_str();
        AnimationData["isFix"] = Animation.second.isFix;     
		for (auto Ani : Animation.second.KeyFrames)
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
			AniData["isCurPos"] = Ani.isCurPos;

			AnimationData["KeyFrame"].push_back(AniData);
		}
		pOutData["Animation"].push_back(AnimationData);
	}
}

HRESULT CCamera_Manager::Set_Animation_Json(string strFilePath)
{

    string filePath = strFilePath;
    filePath += ".json";
    ifstream In(filePath);
    if (!In.is_open())
    {
        In.close();
        return E_FAIL;
    }
    else
    {
        nlohmann::json jsonData;
        In >> jsonData;

        map<_wstring, CAMERA_ANIMATION> Animations;
        for (auto Animation : jsonData["Animation"])
        {
            CAMERA_ANIMATION AnimationDesc;
            AnimationDesc.Name = AnsiToWString(Animation["Name"]);
            AnimationDesc.isFix = Animation["isFix"];
            vector<CAMERA_KEYFRAME> KeyFrames;
            for (auto Ani : Animation["KeyFrame"])
            {
                CAMERA_KEYFRAME KeyFrame{};
                KeyFrame.vTranslation.x = Ani["Translation"]["x"];
                KeyFrame.vTranslation.y = Ani["Translation"]["y"];
                KeyFrame.vTranslation.z = Ani["Translation"]["z"];
                KeyFrame.vLookAt.x = Ani["LookAt"]["x"];
                KeyFrame.vLookAt.y = Ani["LookAt"]["y"];
                KeyFrame.vLookAt.z = Ani["LookAt"]["z"];
                KeyFrame.vLookAt.w = Ani["LookAt"]["w"];
                KeyFrame.fSpeed = Ani["Speed"];
                KeyFrame.fTrackPosition = Ani["TrackPosition"];

                KeyFrame.isCurPos = Ani["isCurPos"];
                KeyFrames.push_back(KeyFrame);
            }
            AnimationDesc.KeyFrames = KeyFrames;
            Animations.emplace(AnsiToWString(Animation["Name"]), AnimationDesc);
        }

        m_pActiveCamera->Load_Animation(Animations);
        In.close();
    }
    return S_OK;
}

void CCamera_Manager::Set_ObjMatrix(_uint iLevelIndex, _wstring strCameraTag, _float4x4* ObjMatrix)
{
    CCamera* pCamera = Find_Camera(iLevelIndex, strCameraTag);
    if (pCamera->Get_CameraType() == ENUM_CLASS(CAMERATYPE::PLAYER))
    {
        CCamera_Compre* pCameraCompre = dynamic_cast<CCamera_Compre*>(pCamera);
        pCameraCompre->Set_ObjMatrix(ObjMatrix);
    }
}

void CCamera_Manager::Set_FixEnd()
{
    CCamera* pCamera = Get_ActiveCamera();
    pCamera->Set_IsAniFix(false);
}

void CCamera_Manager::Set_Animation(_wstring strAnimationTag)
{
    CCamera* pCamera = Get_ActiveCamera();
    pCamera->Set_Animation(strAnimationTag);
}

void CCamera_Manager::Set_NpcTalk(_bool isNpcTalk, const _float4x4* pSubObjMatrix, _float3 vNpcTalkOffset, _float4 vNpcTalkLookat)
{
    CCamera* pCamera = Get_ActiveCamera();
    if (pCamera == nullptr)
        return;
    
    CCamera_Compre* pCameraCompre = dynamic_cast<CCamera_Compre*>(pCamera);
    pCameraCompre->Set_NpcTalk(isNpcTalk, pSubObjMatrix, vNpcTalkOffset, vNpcTalkLookat);
    
}

void CCamera_Manager::Switch_CameraMode(CAMERATYPE eType)
{
    CCamera* pCamera = Get_ActiveCamera();
    CCamera_Compre* pCameraCompre = dynamic_cast<CCamera_Compre*>(pCamera);
    pCameraCompre->Switch_CameraMode(eType);

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

}
