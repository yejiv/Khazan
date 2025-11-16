#pragma once
#include "Client_Defines.h"
#include "Creature.h"

NS_BEGIN(Engine)
class CNavigation;
class CCharacterVirtual;
class CRigidBody; 
class CCollider;
NS_END

NS_BEGIN(Client)

using DIR = DIRECTION_INFO;

class CKhazan_Spear final:  public CCreature
{
public:

	enum PLAYER_STATUS : _uint
	{
		BAREHAND = 1 << 0,
		SPEAR = 1 << 1,
		RESERVED = 1 << 2,
		CHARGING_SPRINT = 1 << 3,
		BACK_DODGE = 1 << 4,
		ROTATION = 1 << 5,
		CHARGING_STRONG_ATTACK = 1 << 6,
		AGAIN_REQUEST = 1 << 7, //스페이스바 유연하게 사용하도록 스프린트 제어
		LOCKON = 1 << 8,  // 락 온
        READY_ASSAULT = 1<< 9, // 강습 스킬 준비 
        INJURED  = 1 << 10,  //하인마흐에서 걸을 때 

        /* 가드 */
        GUARD   = 1 << 11,
        GUARD_SUCCESS = 1 << 12,
        JUST_GUARD = 1 << 13,
        GUARD_ROTATION = 1 << 14,
        GUARD_ROTATION_REQUEST = 1 << 15,

	};
	enum PLAYER_CAMERA_DIR {
		PC_FRONT, PC_FRONT_RIGHT, PC_RIGHT, PC_BACK_RIGHT,
		PC_BACK, PC_BACK_LEFT, PC_LEFT, PC_FRONT_LEFT
	};

private:
	CKhazan_Spear(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CKhazan_Spear(const CKhazan_Spear& Prototype);
	virtual ~CKhazan_Spear() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize_Clone(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

public:
    virtual void Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal) override;
    virtual void Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal) override;
    virtual void Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer) override;
    virtual void Take_Damage(_float fDamage, HITREACTION eHitreaction, CGameObject* pGameObject);

	const _float4x4* Get_BoneSpearFXMatrixPtr() { return &m_pSpearFX_WorldMatrix; }
	_matrix Get_BoneSpearFXMatrix() { return XMLoadFloat4x4(&m_pSpearFX_WorldMatrix); }

    void	Set_Camera(class CCamera_Compre* pCamera);
    void    Set_Position(_float4 vPos);

private:
	class CBody_Khazan_Spear*			    m_pBody = { nullptr };
    class CSpear_Khazan_Spear*              m_pSpear = { nullptr };
    class CLantern_Khazan_Spear*			m_pLantern = { nullptr };
	class CKhazan_Spear_Anim_Move*		    m_pAnimMove = { nullptr };
	class CKhazan_Spear_Anim_Attack*	    m_pAnimAttack = { nullptr };
    class CKhazan_Spear_Anim_Guard*         m_pAnimGuard = { nullptr };
    class CKhazan_Spear_Anim_Interaction*	m_pAnimInteraction = { nullptr };
    class CKhazan_Spaer_Anim_Damaged*       m_pAnimDamaged = { nullptr };

	class CCamera_Compre*				    m_pCamera = { nullptr };
    class CClientInstance*                  m_pClientInstance = { nullptr };

	//kHAZAN_ANIM_INFO			m_eCurAnimInfo = {}; //후보지에서 선택된 애님인포 
	//vector<kHAZAN_ANIM_INFO>	m_AnimCandidates; // 매 프레임 후보 리스트 적립

	/* state */
    PLAYER_DATA*                m_pPlayerData;
	_uint						m_iStatus = {};
	_uint						m_iCurMainState = {};
	_uint						m_iPrevMainState = {};
	_uint						m_iCurSubState = {};
	_uint						m_iPrevSubState = {};
	_uint						m_iCycle;
	_uint						m_iPrevCycle;
	DIR							m_eDir = {};		//플레이어의 로컬 방향  dir(애니메이션 선택용)
	_uint						m_ePrevDir = {};
    _uint                       m_eHitReaction = {}; //몬스터한테 가할 넉백이나 저스트가드 내용 담기 

	_uint						m_iCurAnimIndex = {};
	_uint						m_iReserveAnimIndex = {};

	/* info */
    _float4x4*                  m_LHandSocket_Matrix;  //왼손 소켓 
    _float4x4*                  m_LanternSocket_Matrix;         //랜턴 소켓
    _float4x4*                  m_BackPack_Matrix;              //창 소켓 
	_float4x4*					m_pWeaponR_Matrix = { nullptr };
	_float4x4*					m_pSpearFX_Matrix = { nullptr };
	_float4x4					m_pSpearFX_WorldMatrix = {};
	_matrix						m_SpearOffset_Matrix = {};
	_bool						m_isEnableControl = { true };

	//vector<_float2>				m_vCoolTime;

	/* Move*/
	DIR							m_eWorldDir = {}; // 카메라 기준 월드 방향 
	_float						m_fRotateTime[2] = { 0.f,0.15f };
	_vector						m_vRotateStart;
	_float						m_fSprintTime = { 0.f };
	//_float						m_fDodgeTime = { 0.f };
	uint						m_iStopMoveIndexTable[9];	/* 스탑 애니메이션일 때 움직임 x  */

	/* Attack  */
	_float						m_fChargingStrongTime = { 0.f };
    _uint                       m_iCurSkillIndex = {};
    DIR                         m_eHitNormalDir = {};       //맞은 방향  저장
    DIR                         m_eHitStrongDir = {};       //맞은 방향  저장

    /* Guard */
    _vector                     m_vGuardRotationStart = {};
    _vector                     m_vGuardRotationEnd = {};
    _float4                     m_vGuardRotationTarget = {}; // 가드 시 충돌지점으로 회전
    _float2                     m_fGuardRotationTime = { 0.f, 0.15f };


	/* const */
	const	_float				m_fMinSprintTime = { 0.15f };
	//const	_float				m_fMinDodgeTime = { 0.2f };

	/* Move Speed */
     _float				m_fInjuredSpeed = { 1.15f };
     _float				m_fWalkSpeed = { 2.6f };
	 _float				m_fRunSpeed = { 5.5f };
	 _float				m_fSprintSpeed = {11.5f };

	/*  Attack */
	const _float				m_fChargingStrongIntervalTime = { 0.25f };


private:
	void			Update_State(_float fTimeDelta);
    void            InjuredMove_Input(_float fTimeDelta);
	void			Move_Input(_float fTimeDelta);
    _bool			Skill_Input(_float fTimeDelta);
    _bool			Attack_Input(_float fTimeDelta);
	_bool			Guard_Input(_float fTimeDelta);
    _bool           Interaction_Input(_float fTimeDelta);
	void			Change_MoveIdle(_float fTimeDelta);
	void			ExecuteAnimationExit();
	void			Apply_PlayerMovement(_float fTimeDelta);
	void			Check_KeyInput_Direction(_float fTimeDelta);
    DIRECTION_INFO  Calculate_LockOnDirection(_float fTimeDelta);
    void            Update_PlayerDate();
    void            LockOn_Rotation(_float fTimeDelta);
    void            Setting_Guard_Rotation( );
    void            Guard_Rotation(_float fTimeDelta);
    void            Update_LockOn( );   //카메라 락온과 동기화
    void            Update_Die(_float fTimeDelta);
    void            Clear_Injured();

    void            Get_HitReaction( _float3 vContactPoint);

private:
	HRESULT			Ready_Components();
	HRESULT			Ready_PartObjects();
	HRESULT			Ready_Collision();
    HRESULT			Ready_AnimationStateMachine();
    //HRESULT			Ready_PlayerData();

    void            Clear_Step0();  // 이전 상태 빼고 다 클리어
    void            Clear_Step1();  //이전 상태 , lockOn, dir,worldDir 빼고 다 클리어
    void            Clear_Step2();  // 무빙 관련 클리어
    void            Clear_Step3();  // 가드 빼고 클리어



private:
	inline void		Add_Status(_uint i) { m_iStatus |= i; }
	inline void		Remove_Status(_uint i) { m_iStatus &= ~i; }
	inline _bool	Has_Status(_uint i) { return (m_iStatus & i) != 0; }
    inline void     Toggle_Status(_uint i) { m_iStatus ^= i; }
	//inline void		Clear_Status( ) { m_iStatus = 0; }

	inline void		Add_State(_uint i) { m_iCurMainState |= i; }
	inline void		Toggle_State(_uint i) { m_iCurMainState ^= i; }
	inline void		Remove_State(_uint i) { m_iCurMainState &= ~i; }
	inline _bool	Has_State(_uint i) { return (m_iCurMainState & i) != 0; }
	inline _bool	Has_AllStates(_uint i) { return (m_iCurMainState & i) == i; }
	inline _bool	Has_States();	//하나의 상태라도 있는지 없는지
	inline void		Clear_State() { m_iCurMainState = 0; }

	inline void		Add_SubState(_uint i) { m_iCurSubState |= i; }
	inline void		Toggle_SubState(_uint i) { m_iCurSubState ^= i; }
	inline void		Remove_SubState(_uint i) { m_iCurSubState &= ~i; }
	inline _bool	Has_SubState(_uint i) { return (m_iCurSubState & i) != 0; }
	inline _bool	Has_AllSubStates(_uint i) { return (m_iCurSubState & i) == i; }
	inline _bool	Has_SubStates();	//하나의 상태라도 있는지 없는지
	inline void		Clear_SubState() { m_iCurSubState = 0; }

	inline void		Add_CycleState(_uint i) { m_iCycle |= i; }
	inline void		Remove_CycleState(_uint i) { m_iCycle &= ~i; }
	inline _bool	Has_CycleState(_uint i) { return (m_iCycle & i) != 0; }
	inline void		Clear_CycleState() { m_iCycle = 0; }
	inline void		AllClear_CycleState() { m_iCycle = 0;	}

	_uint			ConvertCameraToPlayerDir( PLAYER_CAMERA_DIR playerCamDir);

#pragma region 상호 작용 맵 오브젝트 이벤트
private:
	EventInteractType			m_EventInteract = {};				// 트리거 접촉 여부, 이벤트 발생 여부, 상호 작용 타입, 상호 작용 타입들의 구조체
	_float						m_fEventTimeAcc = { 0.f };
	_ushort						m_sNextItem = { 0 };

    _float4                     m_vStartPos_Event = {};
    _float                      m_fLerpTime_Event = { 0.f };
    _bool                       m_isInteractEventSetting = { false };

private:
	void						Subscribe_Events();
	void						Event_Interact_Object(_float fTimeDelta);
	void						BladeNexus_Event(_float fTimeDelta);
	void						Chest_Event(_float fTimeDelta);
	void						TombStone_Event(_float fTimeDelta);

private:
    void                        Lerp_Position_ByInteractEvent(_float4 vTargetPos, _float4 vStartPos, _float fDuration, _float fTimeDelta, _bool& isDone);
#pragma endregion

#ifdef _DEBUG
	void			Debug_Widget();
	void			Debug_Widget_States();      // 상태 디버깅
    void Debug_Widget_Combat();
	void			Debug_Widget_Animation();   // 애니메이션 디버깅
	void			Debug_Widget_Movement();    // 이동 디버깅

    void Debug_Widget_Guard();

    std::string GetDirectionString(DIRECTION_INFO dir);

    std::string GetHitReactionString();

	const char*		GetStateName(_uint state);
	const char*		GetSubStateName(_uint subState);
	const char*		GetCycleName(_uint cycle);
	const char*		GetStatusName(_uint status);
	std::string		GetDirectionString();
#endif // _DEBUG


public:
	static CKhazan_Spear* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;


};

NS_END
