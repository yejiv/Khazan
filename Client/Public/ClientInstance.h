#pragma once
#include "Client_Defines.h"
#include "Base.h"
#include "DB_Manager.h"
#include "PlayerData_Manager.h"
#include "Player_Manager.h"

using CONTROL_BUTTON = CPlayer_Manager::CONTROL_BUTTON;
using SPEARSKILL = CPlayerData_Manager::SPEARSKILL;
//using GSWORDSKILL = CPlayerData_Manager::GSWORDSKILL;

NS_BEGIN(Engine)
class CUIObject;
class CCamera;
NS_END

NS_BEGIN(Client)

class CClientInstance final : public CBase
{
	DECLARE_SINGLETON(CClientInstance)

private:
	CClientInstance();
	virtual ~CClientInstance() = default;

#pragma region CLIENT
public:
	HRESULT Initialize(ID3D11Device** ppDevice, ID3D11DeviceContext** ppContext);
	void Update(_float fTimeDelta);

	LEVEL Get_CurrLevel() { return m_eCurrLevel; }
	void Set_CurrLevel(LEVEL eLevel) { m_eCurrLevel = eLevel; }

	LEVEL Get_PrevLevel() { return m_ePrevLevel; };
	void Set_PrevLevel(LEVEL eLevel) { m_ePrevLevel = eLevel; } ;

	void Release_Client();
#pragma endregion

#pragma region DB_MANGER
	HRESULT						Load_Data(DATATYPE eType, const _tchar* pFilePath);
	template <typename T>
	const T* Get_Data(_uint iID) const;

	template <typename T>
	const T* Get_DataTalbe() const;
#pragma endregion

#pragma region UI_MANGER
	//Fade 관련
	void						Fade_In(function<void()> FadeEvent = nullptr);
	void						Fade_Out(function<void()> FadeEvent = nullptr);
	_bool						Fade_End();

	//Event 관련
	HRESULT						Add_UIEvent(const _wstring& strLayerTag, const _wstring& strEventTag, std::function<void()> Event);
	HRESULT						Add_UIParamEvent(const _wstring& strLayerTag, const _wstring& strEventTag, std::function<void(void*)> Event);
	function<void()>			Pop_UIEvent(const _wstring& strLayerTag, const _wstring& strEventTag);
	function<void(void*)>		Pop_UIParamEvent(const _wstring& strLayerTag, const _wstring& strEventTag);
	HRESULT						Erase_UIEventLayer(const _wstring& strLayerTag);

	//UI JSON 로드
	HRESULT						Load_UIData(_uint iLayerLevelID, const _wstring& strLayerTag, _uint iPrototypeLevelID, const _tchar* pTextureFilePath);
	CUIObject*					Load_UIObject(_uint iPrototypeLevelID, const _tchar* pFilePath);
	
	_int						UIType_StringToEnum(string szUIType);

	//UI 랜더 그룹에 추가
	HRESULT						Add_UIRender(UI_RENDER_TYPE eRender, class CUIObject* pUIObject);

	//UI 관련 함수
	HRESULT						UI_UpdateSwitch(const _wstring& szRootUIName, void* pArg = nullptr);
	class CUIObject*			Get_RootUI(const _wstring& szRootUIName);
	HRESULT						Add_RootUI(const _wstring& szRootUIName, CUIObject* pUIObject);
	//ETC
	_float4						Get_AtlasUV(const string pFrameName, _uint iTextureIndex);
#pragma endregion

#pragma region Player_Mager
    PLAYER_DATA&                Get_ptrPlayerData();
	const PLAYER_DATA&			Get_PlayerData();
	void						Add_SkillExp(_float fExp);
	_bool						Add_SkillPoint(_int iPoint);
    void                        BindSkillToButton(CONTROL_BUTTON eButton, _uint iSkill);
    void                        UnBindSkillToButton(CONTROL_BUTTON eButton);
    _uint                       Get_ButtonSkill(CONTROL_BUTTON eButton);
    void                        Set_UsedSkill(_uint iSkill, _bool isUsed);
    _bool                       Is_UsedSkill(_uint iSkill);
    
    void                        Set_PlayerInput(_bool isInput) { m_isPlayerInput = isInput; }
    inline _bool                Get_PlayerInput() const { return m_isPlayerInput; }

#pragma endregion

#pragma region CAMERA_MANAGER
	HRESULT Add_Camera(_uint iLevelIndex, class CCamera* pCamera);
	void Change_Camera(_uint iLevelIndex, _uint iCameraType);
	void Change_Camera(_uint iLevelIndex, _wstring strCameraTag);
	vector<class CCamera*> Get_pCameras(_uint iNumLevel);
	class CCamera* Get_ActiveCamera();
	_float3 Get_ActiveCameraPos();
	_float4 Get_ActiveCameraLook();

	class CCamera* Find_Camera(_uint iLevelIndex, CAMERATYPE eCameraType);
	class CCamera* Find_Camera(_uint iLevelIndex, _wstring strCameraTag);

	void ActiveCamera_Shaking(_float fPower, _float fDuration);
	void ActiveCamera_PushFOVModifier(const FOVModifier& tNewModifier);
	void ActiveCamera_KillFov(const _wstring& strID);

	void Save_Json_Camera(_uint iLevelIndex, _wstring strCameraTag, nlohmann::ordered_json& pOutData);
	void Save_Json_Animation(_uint iLevelIndex, _wstring strCameraTag, nlohmann::ordered_json& pOutData);
	void Clear_CameraManager(_uint iLevelIndex);
#pragma endregion

#pragma region PlayerData
public:
	_bool   Check_SpearSkill(_uint skill);		// Check if the skill exists
	_bool   Check_GSwordSkill(_uint skill);
	void    AllUnlock_SpearSkill();				// Unlock all skills
	void    AllUnlock_GswordSkill( );
	void    Unlock_SpearSkill(_uint skill);		// Unlock a single skill
	void    Unlock_GswordSkill(_uint skill) ;
	void    Alllock_SpearSkill();				// Lock all skills
	void    Alllock_GswordSkill();
	void    lock_SpearSkill(_uint skill);		// Lock a single skill
	void    lock_GswordSkill(_uint skill);
#pragma endregion



#ifdef _DEBUG
#pragma region CAMERA_CONTROLLER
	void CameraTool_Clear();
#pragma region CAMERA_MANAGER
#endif



private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	LEVEL				m_ePrevLevel = { LEVEL::TITLE };
	LEVEL				m_eCurrLevel = { LEVEL::TITLE };
	
	class CUI_Manager*		m_pUI_Manager = { nullptr };
	class CPlayer_Manager*	m_pPlayer_Manager = { nullptr };
	CDB_Manager*			m_pDB_Manager = { nullptr };
	class CCamera_Manager*	m_pCamera_Manager = { nullptr };
	 CPlayerData_Manager* m_pPlayerData_Manager = { nullptr };
	
     _bool                  m_isPlayerInput = { true };
#ifdef _DEBUG
	class CDebug_Manager* m_pDebug_Manager = { nullptr };
	class CCamera_Controller* m_pCamera_Controller = { nullptr };
	class CShader_Controller* m_pShader_Controller = { nullptr };
#endif
public:
	virtual void Free() override;
};

template<typename T>
inline const T* CClientInstance::Get_Data(_uint iID) const
{
	return m_pDB_Manager->Get_Data<T>(iID);;
}

template<typename T>
inline const T* CClientInstance::Get_DataTalbe() const
{
	return m_pDB_Manager->Get_DataTalbe<T>();;
}

NS_END