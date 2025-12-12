#include "Camera_Manager.h"
#include "GameInstance.h"
#include "Camera.h"
#include "Camera_Compre.h"


static inline float Saturate(float x) { return Clamp(x, 0.f, 1.f); }


static inline float SmoothDampScalar(float current, float target, float& currentVel, float smoothTime, float dt)
{
    const float eps = 1e-4f;
    float omega = 2.0f / max(eps, smoothTime);
    float x = omega * dt;
    float expv = 1.0f / (1.0f + x + 0.48f * x * x + 0.235f * x * x * x);

    float change = current - target;
    float temp = (currentVel + omega * change) * dt;
    currentVel = (currentVel - omega * temp) * expv;
    float output = target + (change + temp) * expv;
    return output;
}

static inline float Clamp(float v, float lo, float hi) { return max(lo, min(v, hi)); }

static inline float WrapAngle(float a) {
    const float twoPi = XM_PI * 2.0f;
    while (a > XM_PI) a -= twoPi;
    while (a < -XM_PI) a += twoPi;
    return a;
}

static inline float DeltaAngle(float a, float b) {
    // shortest path from a to b in (-pi..pi]
    float d = WrapAngle(b - a);
    return d;
}

static inline float SmoothDampAngle(float current, float target, float& currentVel, float smoothTime, float dt)
{
    float delta = DeltaAngle(current, target);
    float out = SmoothDampScalar(0.f, delta, currentVel, smoothTime, dt);
    return WrapAngle(current + out);
}

static inline void BuildSafeBasis(_vector vLookIn, _vector& outRight, _vector& outUp, _vector& outLook)
{
    const _vector vWorldUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

    // Look 정규화 + 너무 짧으면 이전 값이나 fallback 사용해야 함
    _vector look = XMVector3Normalize(vLookIn);
    if (XMVectorGetX(XMVector3LengthSq(look)) < 1e-6f)
    {
        // 완전 비는 경우를 방지하기 위해 아주 기본 방향으로 fallback
        look = XMVectorSet(0.f, 0.f, 1.f, 0.f);
    }

    _vector right = XMVector3Cross(vWorldUp, look);
    float rightLenSq = XMVectorGetX(XMVector3LengthSq(right));

    if (rightLenSq < 1e-6f)
    {
        // look 이 거의 (0, ±1, 0) 이면, 임의의 축을 하나 선택해서 다시 만든다.
        // 예: x축 기반으로 새 basis 생성
        _vector arbitrary = XMVectorSet(1.f, 0.f, 0.f, 0.f);
        right = XMVector3Cross(arbitrary, look);
        rightLenSq = XMVectorGetX(XMVector3LengthSq(right));

        if (rightLenSq < 1e-6f)
        {
            // 그래도 0이면 마지막 fallback
            right = XMVectorSet(0.f, 0.f, 1.f, 0.f);
        }
    }

    right = XMVector3Normalize(right);
    _vector up = XMVector3Normalize(XMVector3Cross(look, right));

    outLook = look;
    outRight = right;
    outUp = up;
}

static inline _vector LoadFloat3(const _float3& v)
{
    return XMLoadFloat3(&v);
}

static inline void StoreFloat3(_float3& out, _vector v)
{
    XMStoreFloat3(&out, v);
}

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

    m_pActiveCamera = nullptr;

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

void CCamera_Manager::Play_FOVZoomSequence(
    const _wstring& strID,
    _float fZoomFOV,     // 줌인 목표 FOV (라디안)
    _float fInDuration,  // 줌 인 시간
    _float fHoldDuration,// 고정 시간
    _float fOutDuration, // 줌 아웃 시간
    _int   iPriority
)
{
    if (m_pActiveCamera == nullptr)
        return;

    m_pActiveCamera->Play_FOVZoomSequence(strID, fZoomFOV, fInDuration, fHoldDuration, fOutDuration, iPriority);
}

void CCamera_Manager::Start_FOVHoldZoom(
    const _wstring& strID,
    _float fZoomFOV,     // 줌인 목표 FOV (라디안)
    _float fInDuration,  // 줌 인 시간
    _int   iPriority 
)
{
    if (m_pActiveCamera == nullptr)
        return;

    m_pActiveCamera->Start_FOVHoldZoom(strID, fZoomFOV, fInDuration, iPriority);
}


// 홀드 해제 → 줌 아웃
void CCamera_Manager::Release_FOVHoldZoom(
    const _wstring& strID,
    _float fOutDuration  // 줌 아웃 시간
)
{
    if (m_pActiveCamera == nullptr)
        return;

    m_pActiveCamera->Release_FOVHoldZoom(strID, fOutDuration);
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

void CCamera_Manager::Set_NpcTalk(_bool isNpcTalk, _float3 vTargetPos, _float3 vLookAt)
{
    CCamera* pCamera = Get_ActiveCamera();
    if (pCamera == nullptr)
        return;
    
    CCamera_Compre* pCameraCompre = dynamic_cast<CCamera_Compre*>(pCamera);
    pCameraCompre->Set_NpcTalk(isNpcTalk, vTargetPos, vLookAt);
    
}

void CCamera_Manager::Play_SubShotOnce(const CAMERA_POSE& subShotPose, _float fInDur, _float fOutDur)
{
    CCamera* pCamera = Get_ActiveCamera();
    if (pCamera == nullptr)
        return;

    CCamera_Compre* pCameraCompre = dynamic_cast<CCamera_Compre*>(pCamera);
    pCameraCompre->Play_SubShotOnce(subShotPose, fInDur, fOutDur);
}

CAMERA_POSE CCamera_Manager::MakePose(const _float3& vPos, const _float3& vLookDir)
{
    CAMERA_POSE pose{};

    // pos / lookDir 를 XMVector로 변환
    _vector vPosWS = LoadFloat3(vPos);
    _vector vLook = LoadFloat3(vLookDir);

    // lookDir 로부터 right / up / look 안전하게 만들기
    _vector vR, vU, vL;
    BuildSafeBasis(vLook, vR, vU, vL);

    // CAMERAPOSE에 저장
    StoreFloat3(pose.vPos, vPosWS);
    StoreFloat3(pose.vRight, vR);
    StoreFloat3(pose.vUp, vU);
    StoreFloat3(pose.vLook, vL);

    return pose;
}

CAMERA_POSE CCamera_Manager::MakePose_FromTarget(const _float3& vPos, const _float3& vTargetPos)
{
    _vector vPosWS = LoadFloat3(vPos);
    _vector vTargetWS = LoadFloat3(vTargetPos);

    _vector vDir = XMVectorSubtract(vTargetWS, vPosWS); // target - pos

    CAMERA_POSE pose{};
    _vector vR, vU, vL;
    BuildSafeBasis(vDir, vR, vU, vL);

    StoreFloat3(pose.vPos, vPosWS);
    StoreFloat3(pose.vRight, vR);
    StoreFloat3(pose.vUp, vU);
    StoreFloat3(pose.vLook, vL);

    return pose;
}

void CCamera_Manager::ReturnToPreviousPose(_float fDuration)
{
    CCamera* pCamera = Get_ActiveCamera();
    if (pCamera == nullptr)
        return;

    CCamera_Compre* pCameraCompre = dynamic_cast<CCamera_Compre*>(pCamera);
    pCameraCompre->ReturnToPreviousPose(fDuration);
}

void CCamera_Manager::Force_AniEnd()
{
    CCamera* pCamera = Get_ActiveCamera();
    if (pCamera == nullptr)
        return;

    CCamera_Compre* pCameraCompre = dynamic_cast<CCamera_Compre*>(pCamera);
    pCameraCompre->Force_AniEnd();
}

void CCamera_Manager::Switch_CameraMode(CAMERATYPE eType)
{
    CCamera* pCamera = Get_ActiveCamera();
    CCamera_Compre* pCameraCompre = dynamic_cast<CCamera_Compre*>(pCamera);
    pCameraCompre->Switch_CameraMode(eType);

}

void CCamera_Manager::MouseOnOff(_bool isOn)
{
    CCamera* pCamera = Get_ActiveCamera();
    CCamera_Compre* pCameraCompre = dynamic_cast<CCamera_Compre*>(pCamera);
    pCameraCompre->MouseOnOff(isOn);

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
