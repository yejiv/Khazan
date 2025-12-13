#pragma once
#include "Client_Defines.h"
#include "Base.h"
#include "DB_Manager.h"
#include "PlayerData_Manager.h"
#include "Player_Manager.h"
#include "Sequence_Interface.h"


NS_BEGIN(Engine)
class CUIObject;
class CCamera;
class ISeqInstance;
NS_END

NS_BEGIN(Client)

using CONTROL_BUTTON = CPlayer_Manager::CONTROL_BUTTON;
using SPEARSKILL = CPlayerData_Manager::SPEARSKILL;
using GSWORDSKILL = CPlayerData_Manager::GSWORDSKILL;

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

    _bool   Item_Exist_ID(_uint iID);
#pragma endregion

#pragma region UI_MANGER
	//Fade 관련
	void						Fade_In(function<void()> FadeEvent = nullptr, _float fSpeed = 1.f);
	void						Fade_Out(function<void()> FadeEvent = nullptr, _float fSpeed = 1.f);
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
    HRESULT						Release_RootUI(const _wstring& szRootUIName);
	//UI 관련 함수
    void                        Set_UIAllRenderSet(_bool isRender);
	HRESULT						UI_UpdateSwitch(const _wstring& szRootUIName, void* pArg = nullptr);
	class CUIObject*			Get_RootUI(const _wstring& szRootUIName);
	HRESULT						Add_RootUI(const _wstring& szRootUIName, CUIObject* pUIObject);
	//ETC
	_float4						Get_AtlasUV(const string pFrameName, _uint iTextureIndex);
#pragma endregion

#pragma region Player_Manager
    PLAYER_DATA*                Get_pInitailizePlayerData();
    void                        Initialize_PlayerData();
    PLAYER_DATA&                Get_ptrPlayerData();
	const PLAYER_DATA&			Get_PlayerData();
	void						Add_SkillExp(_float fExp);
	_bool						Add_SkillPoint(_int iPoint);
    void                        BindSkillToButton(CONTROL_BUTTON eButton, _uint iSkill);
    void                        UnBindSkillToButton(CONTROL_BUTTON eButton);
    _uint                       Get_ButtonSkill(CONTROL_BUTTON eButton);
    void                        Set_UsedSkill(_uint iSkill, _bool isUsed);
    void                        Set_UsedSkills(_uint iSkill, _bool isUsed);
    void                        Set_UnUsedAllSkills();                      //Unlock all currently used skills
    _bool                       Is_UsedSkill(_uint iSkill);
    
    /* 플레이어 입력 막기 */
    void                        Set_PlayerInput(_bool isInput, _bool isIdle = false) { 
        m_isPlayerInput = isInput;
        if (isIdle && m_OnForcePlayerAnimationIdle)
            m_OnForcePlayerAnimationIdle(); 
    }
    void                        Force_PlayerIdleCallBack(function<void()> callback) { m_OnForcePlayerAnimationIdle = callback; }
    inline _bool                Get_PlayerInput() const { return m_isPlayerInput; }

#pragma endregion

#pragma region PlayerData
public:
    _bool                                           Check_Skill(_uint skill);		// Check if the skill exists
    _bool                                           Check_Skills(_uint skill);		// Check if the skills exists
	void                                            AllUnlock_Skill();				// Unlock all skills
	void                                            Unlock_Skill(_uint skill);		// Unlock a single skill
	void                                            Alllock_Skill();				// Lock all skills
	void                                            Lock_Skill(_uint skill);		// Lock a single skill
    void                                            UsedSpear();                    // Choose Spear 
    void                                            UsedGSword();                   // Choose GSword 
    _bool                                           Is_CurrentSpear();              // is picked Spear 
    _bool                                           Is_CurrentGSword();             // is picked GSword 
    void                                            Set_ChangePlayerWeaponEquipmentCallBack(function<void(EQUIPMENTTYPE, const _wstring&)> callback);
    void                                            Set_ChangePlayerArmorEquipmentCallBack(function<void(EQUIPMENTTYPE, const _wstring&)> callback);
    void                                            Change_PlayerEquipment(EQUIPMENTTYPE eType, _uint iEquipmentIndex);  // ui 장착버튼같은거 누를 시 사용해주세요 
    const CPlayerData_Manager::PLAYER_EQUIPMENT&    Get_PlayerEquipment() const;
    void                                            Set_PlayerEquipment(EQUIPMENTTYPE eType, _uint iEquipmentIndex);  // 테스트용 Setter 
    const _wstring&                                 Get_EquipmentName(_uint iEquipmentIndex) const;
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
    void Camera_Play_FOVZoomSequence(
        const _wstring& strID,
        _float fZoomFOV,     // 줌인 목표 FOV (라디안)
        _float fInDuration,  // 줌 인 시간
        _float fHoldDuration,// 고정 시간
        _float fOutDuration, // 줌 아웃 시간
        _int   iPriority = 0 // PRIORITY 모드 우선순위
    );
    void Camera_Start_FOVHoldZoom(
        const _wstring& strID,
        _float fZoomFOV,     // 줌인 목표 FOV (라디안)
        _float fInDuration,  // 줌 인 시간
        _int   iPriority = 0 // PRIORITY 우선순위
    );
    // 홀드 해제 → 줌 아웃
    void Camera_Release_FOVHoldZoom(
        const _wstring& strID,
        _float fOutDuration  // 줌 아웃 시간
    );

    void Start_ForceOrbit(CAMERA_FORCE_DIR eForceDir);
    void ActiveCamera_InteractMove();
    void DeactivateCamera_InteractMove();
    void Yetuga_Holding_Start();
    void Yetuga_Holding_End();

	void Save_Json_Camera(_uint iLevelIndex, _wstring strCameraTag, nlohmann::ordered_json& pOutData);
	void Save_Json_Animation(_uint iLevelIndex, _wstring strCameraTag, nlohmann::ordered_json& pOutData);
	void Clear_CameraManager(_uint iLevelIndex);


    void Camera_Set_ObjMatrix(_uint iLevelIndex, _wstring strCameraTag, _float4x4* ObjMatrix);
    void Camera_Switch_CameraMode(CAMERATYPE eType);

    void Camera_Set_FixEnd();
    void Camera_Set_Animation(_wstring strAnimationTag);
    HRESULT Camera_Set_Animation_Json(string strFilePath);

    void Camera_Set_NpcTalk(_bool isNpcTalk, _float3 vTargetPos = _float3(0.f, 0.f, 0.f), _float3 vLookAt = _float3(0.f, 0.f, 0.f));

    void Camera_SubShot(const CAMERA_POSE& subShotPose, _float fInDur, _float fOutDur);
    CAMERA_POSE Camera_MakePose(const _float3& vPos, const _float3& vLookDir);
    CAMERA_POSE Camera_MakePose_FromTarget(const _float3& vPos, const _float3& vTargetPos);
    void Camera_ReturnToPreviousPose(_float fDuration);

    void Camera_Force_AniEnd();
    void Camera_MouseOnOff(_bool isOn);
#pragma endregion

#pragma region INTERACT_MANAGER
    void Add_BladeNexus(KHAZAN_MAP eMapName, INTER_BLADENEXUS_DESC* pDesc);
    INTER_BLADENEXUS_DESC* Find_BladeNexus(KHAZAN_MAP eMapName, _uint iID);
    INTER_BLADENEXUS_DESC* Find_BladeNexus(_uint iID);
    vector<INTER_BLADENEXUS_DESC*>* Find_MapBladeNexus(KHAZAN_MAP eMapName);
    void Unlock_BladeNexus(_uint iID);
#pragma endregion

#pragma region SEQ_DATA_MANAGER
public:
    HRESULT Push_Sequence(_wstring strName, class ISeqInstance* pSequence);
    class ISeqInstance* Find_Sequence(_wstring strName);
    HRESULT Remove_Sequence(_wstring strName);
    void Seq_Clear();
#pragma endregion

#pragma region BGM_MANAGER
public:
    // 글로벌 볼륨 말고 BGM 로컬 볼륨 ( 고정 )
    _float Get_Volume_BGM();
    void Set_Volume_BGM(_float fVolume);

    // BGM 음소거
    void BGM_Mute();
    // BGM 음소거 해제
    void BGM_UnMute();

    // 글로벌 볼륨 말고 AMB 로컬 볼륨 ( 고정 )
    _float Get_Volume_AMB();
    void Set_Volume_AMB(_float fVolume);

    // AMB 음소거
    void AMB_Mute();
    // AMB 음소거 해제
    void AMB_UnMute();

    // BGM + AMB 음소거
    void All_Mute();
    // BGM + AMB 음소거 해제
    void All_UnMute();

    // 레벨 전환시 BGM_Mgr에 있는 Curr BGM Key 초기화
    void Clear_CurrentKey_BGM();

public:
    // SoundKey로 현재 진행중인게 있는지 검사 후 BGM 재생
    void PlayBGM(const _tchar* pSoundKey, _float fFadeTime = 1.f);
    // 기존 Key값을 교체하면서 크로스 페이드 하며 BGM 교체
    void ChangeBGM(const _tchar* pSoundKey, _float fFadeTime = 1.f);

    // SoundKey로 현재 진행중인게 있는지 검사 후 AMB 재생
    void PlayAMB(const _tchar* pSoundKey, _float fFadeTime = 1.f);
    // 기존 Key값을 교체하면서 크로스 페이드 하며 AMB 교체
    void ChangeAMB(const _tchar* pSoundKey, _float fFadeTime = 1.f, _bool isWav = true);


#pragma region 하인마흐 프리셋

    void BGM_HeinMach_Entry(_float fFadeTime = 1.f);
    void BGM_HeinMach_Dawn(_float fFadeTime = 1.f);
    void BGM_HeinMach_CutScene(_float fFadeTime = 1.f);
    void BGM_HeinMach_Cave(_float fFadeTime = 1.f);
    void BGM_HeinMach_Day(_float fFadeTime = 1.f);
    void BGM_HeinMach_Halberd(_float fFadeTime = 1.f);
    void BGM_HeinMach_Yetuga_CutScene(_float fFadeTime = 1.f);
    void BGM_HeinMach_Yetuga_1Phase(_float fFadeTime = 1.f);

#pragma endregion

#pragma region 엠바스 프리셋

    void BGM_Embars_Entry(_float fFadeTime = 1.f);
    void BGM_Embars_B1(_float fFadeTime = 1.f);
    void BGM_Embars_Club(_float fFadeTime = 1.f);
    void BGM_Embars_Club_Game(_float fFadeTime = 1.f);    
    void Embars_Club_Shuffle_0(_float fFadeTime = 1.f);
    void Embars_Club_Shuffle_1(_float fFadeTime = 1.f);
    void Embars_Club_Shuffle_2(_float fFadeTime = 1.f);
    void BGM_Embars_1F(_float fFadeTime = 1.f);
    void BGM_Embars_Elamein(_float fFadeTime = 1.f);

#pragma endregion

#pragma region 바이퍼 프리셋

    void BGM_Viper_Entry(_float fFadeTime = 1.f);
    void BGM_Viper_1PhaseCutScene(_float fFadeTime = 1.f);
    void BGM_Viper_1Phase(_float fFadeTime = 1.f);
    void BGM_Viper_2PhaseCutScene(_float fFadeTime = 1.f);
    void BGM_Viper_2Phase(_float fFadeTime = 1.f);
    void BGM_Viper_End(_float fFadeTime = 1.f);

#pragma endregion

public:
    // BGM 정지
    void BGM_Stop(_float fFadeTime = 1.f);

    // BGM 재시작 ( isFade 가 true 면 FadeIn )
    void BGM_Resume(_bool isFade = false, _float fFadeTime = 1.f);
    // BGM 정지 ( isFade 가 true 면 FadeOut )
    void BGM_Pause(_bool isFade = false, _float fFadeTime = 1.f);

    // AMB 정지
    void AMB_Stop(_float fFadeTime = 1.f);

    // AMB 재시작 ( isFade 가 true 면 FadeIn )
    void AMB_Resume(_bool isFade = false, _float fFadeTime = 1.f);
    // AMB 정지 ( isFade 가 true 면 FadeOut )
    void AMB_Pause(_bool isFade = false, _float fFadeTime = 1.f);
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
    class CInteract_Manager* m_pInteract_Manager = { nullptr };
    class CSequence_Data_Manager* m_pSeq_Data_Manager = { nullptr };
    class CBGM_Manager* m_pBGM_Manager = { nullptr };

     _bool                  m_isPlayerInput = { true };
     function<void()>       m_OnForcePlayerAnimationIdle;
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