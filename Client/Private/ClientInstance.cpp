#include "ClientInstance.h"
#include "GameInstance.h"

#include "UI_Manager.h"
#include "DB_Manager.h"
#include "Camera_Manager.h"
#include "Camera.h"

#include "PlayerData_Manager.h"

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
void CClientInstance::Fade_In(function<void()> FadeEvent)
{
	m_pUI_Manager->Fade_In(FadeEvent);
}
void CClientInstance::Fade_Out(function<void()> FadeEvent)
{
	m_pUI_Manager->Fade_Out(FadeEvent);
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
_bool CClientInstance::Check_SpearSkill(_uint skill)
{
	return m_pPlayerData_Manager->Check_SpearSkill(skill);
}
_bool CClientInstance::Check_GSwordSkill(_uint skill)
{
	return false;
	//return m_pPlayerData_Manager->Check_GSwordSkill(skill);
}
void CClientInstance::AllUnlock_SpearSkill()
{
	m_pPlayerData_Manager->AllUnlock_SpearSkill();
}
void CClientInstance::AllUnlock_GswordSkill()
{
	//m_pPlayerData_Manager->AllUnlock_GswordSkill();
}
void CClientInstance::Unlock_SpearSkill(_uint skill)
{
	m_pPlayerData_Manager->Unlock_SpearSkill(skill);
}
void CClientInstance::Unlock_GswordSkill(_uint skill)
{
	//m_pPlayerData_Manager->Unlock_GswordSkill(skill);
}
void CClientInstance::Alllock_SpearSkill()
{
	m_pPlayerData_Manager->Alllock_SpearSkill();
}
void CClientInstance::Alllock_GswordSkill()
{
	//m_pPlayerData_Manager->Alllock_GswordSkill();
}
void CClientInstance::lock_SpearSkill(_uint skill)
{
	m_pPlayerData_Manager->lock_SpearSkill(skill);
}
void CClientInstance::lock_GswordSkill(_uint skill)
{
	//m_pPlayerData_Manager->lock_GswordSkill(skill);
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
