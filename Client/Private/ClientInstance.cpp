#include "ClientInstance.h"
#include "GameInstance.h"

#include "UI_Manager.h"
#include "DB_Manager.h"
#include "Camera_Manager.h"
#include "Camera.h"

#include "PlayerData_Manager.h"
#include "Player_Manager.h"
#include "Interact_Manager.h"
#include "Sequence_Data_Manager.h"
#include "Sequence_Interface.h"
#ifdef _DEBUG
#include "Debug_Manager.h"
#include "Camera_Controller.h"
#include "Shader_Controller.h"
#endif


IMPLEMENT_SINGLETON(CClientInstance)

CClientInstance::CClientInstance()
{

}

#pragma region Client

HRESULT CClientInstance::Initialize(ID3D11Device** ppDevice, ID3D11DeviceContext** ppContext)
{
	m_pDevice = *ppDevice;
	m_pContext = *ppContext;

	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

	m_pUI_Manager = CUI_Manager::Create(m_pDevice, m_pContext);
	m_pDB_Manager = CDB_Manager::Create();

	m_pCamera_Manager = Client::CCamera_Manager::Create(ENUM_CLASS(LEVEL::END));
	if (nullptr == m_pCamera_Manager)
		return E_FAIL;

	m_pPlayerData_Manager = CPlayerData_Manager::Create();
	if (nullptr == m_pPlayerData_Manager)
		return E_FAIL;

	m_pPlayer_Manager = CPlayer_Manager::Create();
	if (nullptr == m_pPlayer_Manager)
		return E_FAIL;

    m_pInteract_Manager = CInteract_Manager::Create();
    if (nullptr == m_pInteract_Manager)
        return E_FAIL;

    m_pSeq_Data_Manager = CSequence_Data_Manager::Create();
    if (nullptr == m_pSeq_Data_Manager)
        return E_FAIL;

#ifdef _DEBUG
	m_pDebug_Manager = CDebug_Manager::Create();
	if (m_pDebug_Manager == nullptr)
		return E_FAIL;

	m_pCamera_Controller = CCamera_Controller::Create();
	if (m_pCamera_Controller == nullptr)
		return E_FAIL;

	m_pShader_Controller = CShader_Controller::Create();
	if (m_pShader_Controller == nullptr)
		return E_FAIL;
#endif

	return S_OK;
}

void CClientInstance::Update(_float fTimeDelta)
{
	m_pUI_Manager->UIObjectToRenderer();

#ifdef _DEBUG
	m_pDebug_Manager->Update(fTimeDelta);
#endif
}

HRESULT CClientInstance::Load_Data(DATATYPE eType, const _tchar* pFilePath)
{
	return m_pDB_Manager->Load_Data(eType, pFilePath);
}

#pragma region UI_MANGER
_bool CClientInstance::Item_Exist_ID(_uint iID)
{
    return m_pDB_Manager->Exist_ID(iID);
}
void CClientInstance::Fade_In(function<void()> FadeEvent, _float fSpeed)
{
	m_pUI_Manager->Fade_In(FadeEvent, fSpeed);
}
void CClientInstance::Fade_Out(function<void()> FadeEvent, _float fSpeed)
{
	m_pUI_Manager->Fade_Out(FadeEvent, fSpeed);
}
_bool CClientInstance::Fade_End()
{
	return 	m_pUI_Manager->Fade_End();
}
HRESULT CClientInstance::Add_UIEvent(const _wstring& strLayerTag, const _wstring& strEventTag, std::function<void()> Event)
{
	return m_pUI_Manager->Add_Event(strLayerTag, strEventTag, Event);
}

HRESULT CClientInstance::Add_UIParamEvent(const _wstring& strLayerTag, const _wstring& strEventTag, std::function<void(void*)> Event)
{
	return m_pUI_Manager->Add_ParamEvent(strLayerTag, strEventTag, Event);
}

function<void()> CClientInstance::Pop_UIEvent(const _wstring& strLayerTag, const _wstring& strEventTag)
{
	return m_pUI_Manager->Pop_Event(strLayerTag, strEventTag);
}

function<void(void*)> CClientInstance::Pop_UIParamEvent(const _wstring& strLayerTag, const _wstring& strEventTag)
{
	return m_pUI_Manager->Pop_ParamEvent(strLayerTag, strEventTag);
}

HRESULT CClientInstance::Erase_UIEventLayer(const _wstring& strLayerTag)
{
	return m_pUI_Manager->Erase_EventLayer(strLayerTag);
}

HRESULT CClientInstance::Load_UIData(_uint iLayerLevelID, const _wstring& strLayerTag, _uint iPrototypeLevelID, const _tchar* pUIFilePath)
{
	return m_pUI_Manager->Load_UIData(iLayerLevelID, strLayerTag, iPrototypeLevelID, pUIFilePath);
}

CUIObject* CClientInstance::Load_UIObject(_uint iPrototypeLevelID, const _tchar* pFilePath)
{
	return m_pUI_Manager->Load_UIObject(iPrototypeLevelID, pFilePath);
}

_int CClientInstance::UIType_StringToEnum(string szUIType)
{
	return m_pUI_Manager->UIType_StringToEnum(szUIType);
}

HRESULT CClientInstance::Add_UIRender(UI_RENDER_TYPE eRender, CUIObject* pUIObject)
{
	return m_pUI_Manager->Add_UIRender(eRender, pUIObject);
}

HRESULT CClientInstance::Release_RootUI(const _wstring& szRootUIName)
{
    return m_pUI_Manager->Release_RootUI(szRootUIName);
}

HRESULT CClientInstance::UI_UpdateSwitch(const _wstring& szRootUIName, void* pArg)
{
	return m_pUI_Manager->UI_UpdateSwitch(szRootUIName, pArg);
}

CUIObject* CClientInstance::Get_RootUI(const _wstring& szRootUIName)
{
	return m_pUI_Manager->Get_RootUI(szRootUIName);
}

HRESULT CClientInstance::Add_RootUI(const _wstring& szRootUIName, CUIObject* pUIObject)
{
	return m_pUI_Manager->Add_RootUI(szRootUIName, pUIObject);
}

_float4 CClientInstance::Get_AtlasUV(const string pFrameName, _uint iTextureIndex)
{
	return m_pUI_Manager->Get_AtlasUV(pFrameName, iTextureIndex);
}

#pragma endregion

#pragma region Player_Manager
PLAYER_DATA* CClientInstance::Get_pInitailizePlayerData()
{
    return m_pPlayer_Manager->Get_pInitailizePlayerData();
}
PLAYER_DATA& CClientInstance::Get_ptrPlayerData()
{
    return m_pPlayer_Manager->Get_ptrPlayerData();
}
const PLAYER_DATA& CClientInstance::Get_PlayerData()
{
	return m_pPlayer_Manager->Get_PlayerData();
}

void CClientInstance::Add_SkillExp(_float fExp)
{
	m_pPlayer_Manager->Add_SkillExp(fExp);
}
_bool CClientInstance::Add_SkillPoint(_int iPoint)
{
	return m_pPlayer_Manager->Add_SkillPoint(iPoint);
}
void CClientInstance::BindSkillToButton(CONTROL_BUTTON eButton, _uint iSkill)
{
    return m_pPlayer_Manager->BindSkillToButton(eButton, iSkill);
}
void CClientInstance::UnBindSkillToButton(CONTROL_BUTTON eButton)
{
    return m_pPlayer_Manager->UnBindSkillToButton(eButton);
}
_uint CClientInstance::Get_ButtonSkill(CONTROL_BUTTON eButton)
{
    return m_pPlayer_Manager->Get_ButtonSkill(eButton);
}
void CClientInstance::Set_UsedSkill(_uint iSkill, _bool isUsed)
{
    return m_pPlayer_Manager->Set_UsedSkill(iSkill, isUsed);
}
void CClientInstance::Set_UsedSkills(_uint iSkill, _bool isUsed)
{
    return m_pPlayer_Manager->Set_UsedSkills(iSkill, isUsed);
}
void CClientInstance::Set_UnUsedAllSkills()
{
    return m_pPlayer_Manager->Set_UnUsedAllSkills();
}
_bool CClientInstance::Is_UsedSkill(_uint iSkill)
{
    return m_pPlayer_Manager->Is_UsedSkill(iSkill);

}
#pragma endregion

#pragma region CAMERA_MANAGER

HRESULT CClientInstance::Add_Camera(_uint iLevelIndex, CCamera* pCamera)
{
	return m_pCamera_Manager->Add_Camera(iLevelIndex, pCamera);
}
void CClientInstance::Change_Camera(_uint iLevelIndex, _uint iCameraType)
{
	m_pCamera_Manager->Change_Camera(iLevelIndex, iCameraType);
}
void CClientInstance::Change_Camera(_uint iLevelIndex, _wstring strCameraTag)
{
	m_pCamera_Manager->Change_Camera(iLevelIndex, strCameraTag);
}
vector<CCamera*> CClientInstance::Get_pCameras(_uint iNumLevel)
{
	return m_pCamera_Manager->Get_pCameras(iNumLevel);
}
CCamera* CClientInstance::Get_ActiveCamera()
{
	return m_pCamera_Manager->Get_ActiveCamera();
}
_float3 CClientInstance::Get_ActiveCameraPos()
{
	return m_pCamera_Manager->Get_ActiveCameraPos();
}
_float4 CClientInstance::Get_ActiveCameraLook()
{
	return m_pCamera_Manager->Get_ActiveCameraLook();
}
CCamera* CClientInstance::Find_Camera(_uint iLevelIndex, CAMERATYPE eCameraType)
{
	return m_pCamera_Manager->Find_Camera(iLevelIndex, eCameraType);
}
CCamera* CClientInstance::Find_Camera(_uint iLevelIndex, _wstring strCameraTag)
{
	return m_pCamera_Manager->Find_Camera(iLevelIndex, strCameraTag);
}
void CClientInstance::ActiveCamera_Shaking(_float fPower, _float fDuration)
{
	m_pCamera_Manager->ActiveCamera_Shaking(fPower, fDuration);
}
void CClientInstance::ActiveCamera_PushFOVModifier(const FOVModifier& tNewModifier)
{
	m_pCamera_Manager->ActiveCamera_PushFOVModifier(tNewModifier);
}
void CClientInstance::ActiveCamera_KillFov(const _wstring& strID)
{
	m_pCamera_Manager->ActiveCamera_KillFov(strID);
}
void CClientInstance::Save_Json_Camera(_uint iLevelIndex, _wstring strCameraTag, nlohmann::ordered_json& pOutData)
{
	m_pCamera_Manager->Save_Json(iLevelIndex, strCameraTag, pOutData);
}
void CClientInstance::Save_Json_Animation(_uint iLevelIndex, _wstring strCameraTag, nlohmann::ordered_json& pOutData)
{
	m_pCamera_Manager->Save_Json_Animation(iLevelIndex, strCameraTag, pOutData);
}
void CClientInstance::Clear_CameraManager(_uint iLevelIndex)
{
	m_pCamera_Manager->Clear(iLevelIndex);
}
void CClientInstance::Camera_Set_ObjMatrix(_uint iLevelIndex, _wstring strCameraTag, _float4x4* ObjMatrix)
{
    m_pCamera_Manager->Set_ObjMatrix(iLevelIndex, strCameraTag, ObjMatrix);
}
void CClientInstance::Camera_Switch_CameraMode(CAMERATYPE eType)
{
    m_pCamera_Manager->Switch_CameraMode(eType);
}
void CClientInstance::Camera_Set_FixEnd()
{
    m_pCamera_Manager->Set_FixEnd();
}
void CClientInstance::Camera_Set_Animation(_wstring strAnimationTag)
{
    m_pCamera_Manager->Set_Animation(strAnimationTag);
}
HRESULT CClientInstance::Camera_Set_Animation_Json(string strAnimationTag)
{
    return m_pCamera_Manager->Set_Animation_Json(strAnimationTag);
}
void CClientInstance::Camera_Set_NpcTalk(_bool isNpcTalk, _float3 vTargetPos, _float3 vLookAt)
{
    m_pCamera_Manager->Set_NpcTalk(isNpcTalk, vTargetPos, vLookAt);
}
void CClientInstance::Camera_Force_AniEnd()
{
    m_pCamera_Manager->Force_AniEnd();
}
void CClientInstance::Start_ForceOrbit(CAMERA_FORCE_DIR eForceDir)
{
    m_pCamera_Manager->Start_ForceOrbit(eForceDir);
}
void CClientInstance::ActiveCamera_InteractMove()
{
    m_pCamera_Manager->ActiveCamera_InteractMove();
}
void CClientInstance::DeactivateCamera_InteractMove()
{
    m_pCamera_Manager->DeactivateCamera_InteractMove();
}
void CClientInstance::Yetuga_Holding_Start()
{
    m_pCamera_Manager->Yetuga_Holding_Start();
}
void CClientInstance::Yetuga_Holding_End()
{
    m_pCamera_Manager->Yetuga_Holding_End();
}
_bool CClientInstance::Check_Skill(_uint skill)
{
	return m_pPlayerData_Manager->Check_Skill(skill);
}

_bool CClientInstance::Check_Skills(_uint skill)
{
    return m_pPlayerData_Manager->Check_Skills(skill);
}

void CClientInstance::AllUnlock_Skill()
{
	m_pPlayerData_Manager->AllUnlock_Skill();
}
void CClientInstance::Unlock_Skill(_uint skill)
{
	m_pPlayerData_Manager->Unlock_Skill(skill);
}
void CClientInstance::Alllock_Skill()
{
	m_pPlayerData_Manager->Alllock_Skill();
}

void CClientInstance::Lock_Skill(_uint skill)
{
	m_pPlayerData_Manager->Lock_Skill(skill);
}

void CClientInstance::UsedSpear()
{
    m_pPlayerData_Manager->UsedSpear();
}

void CClientInstance::UsedGSword()
{
    m_pPlayerData_Manager->UsedGSword();
}

_bool CClientInstance::Is_CurrentSpear()
{
    return m_pPlayerData_Manager->Is_CurrentSpear();
}

_bool CClientInstance::Is_CurrentGSword()
{
    return m_pPlayerData_Manager->Is_CurrentGSword();
}

void CClientInstance::Set_ChangePlayerEquipmentCallBack(function<void(EQUIPMENTTYPE, const _wstring&)> callback)
{
    m_pPlayerData_Manager->Set_ChangePlayerEquipmentCallBack(callback);
}

void CClientInstance::Change_PlayerEquipment(EQUIPMENTTYPE eType, _uint iEquipmentIndex)
{
    m_pPlayerData_Manager->Change_PlayerEquipment(eType, iEquipmentIndex);
}

const CPlayerData_Manager::PLAYER_EQUIPMENT& CClientInstance::Get_PlayerEquipment() const
{
    return m_pPlayerData_Manager->Get_PlayerEquipment();
}

void CClientInstance::Set_PlayerEquipment(EQUIPMENTTYPE eType, _uint iEquipmentIndex)
{
    m_pPlayerData_Manager->Set_PlayerEquipment(eType, iEquipmentIndex);
}

const _wstring& CClientInstance::Get_EquipmentName(_uint iEquipmentIndex) const
{
    return m_pPlayerData_Manager->Get_EquipmentName(iEquipmentIndex);
}

#pragma endregion


#pragma region INTERACT_MANAGER
void CClientInstance::Add_BladeNexus(KHAZAN_MAP eMapName, INTER_BLADENEXUS_DESC* pDesc)
{
    return m_pInteract_Manager->Add_BladeNexus(eMapName, pDesc);
}
INTER_BLADENEXUS_DESC* CClientInstance::Find_BladeNexus(KHAZAN_MAP eMapName, _uint iID)
{
    return m_pInteract_Manager->Find_BladeNexus(eMapName, iID);
}
INTER_BLADENEXUS_DESC* CClientInstance::Find_BladeNexus(_uint iID)
{
    return m_pInteract_Manager->Find_BladeNexus(iID);
}
vector<INTER_BLADENEXUS_DESC*>* CClientInstance::Find_MapBladeNexus(KHAZAN_MAP eMapName)
{
    return m_pInteract_Manager->Find_MapBladeNexus(eMapName);
}
void CClientInstance::Unlock_BladeNexus(_uint iID)
{
    m_pInteract_Manager->Unlock_BladeNexus(iID);
}
#pragma endregion

#pragma region SEQ_DATA_MANAGER
HRESULT CClientInstance::Push_Sequence(_wstring strName, ISeqInstance* pSequence)
{
    return m_pSeq_Data_Manager->Push_Sequence(strName, pSequence);
}
ISeqInstance* CClientInstance::Find_Sequence(_wstring strName)
{
    return m_pSeq_Data_Manager->Find_Sequence(strName);
}
HRESULT CClientInstance::Remove_Sequence(_wstring strName)
{
    return m_pSeq_Data_Manager->Remove_Sequence(strName);
}
void CClientInstance::Seq_Clear()
{
    m_pSeq_Data_Manager->Clear();
}

#pragma endregion


#ifdef _DEBUG
#pragma region CAMERA_MANAGER

void CClientInstance::CameraTool_Clear()
{
	m_pCamera_Controller->CameraTool_Clear();
}

#pragma endregion
#endif


void CClientInstance::Release_Client()
{
	Safe_Release(m_pUI_Manager);
	Safe_Release(m_pDB_Manager);
	Safe_Release(m_pCamera_Manager);
	Safe_Release(m_pPlayerData_Manager);
	Safe_Release(m_pPlayer_Manager);
    Safe_Release(m_pInteract_Manager);
    Safe_Release(m_pSeq_Data_Manager);

#ifdef _DEBUG	
	Safe_Release(m_pDebug_Manager);
	Safe_Release(m_pCamera_Controller);
	Safe_Release(m_pShader_Controller);
#endif 

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}

void CClientInstance::Free()
{
	__super::Free();
	
	
}
