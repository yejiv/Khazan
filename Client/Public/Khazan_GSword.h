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

class CKhazan_GSword final : public CCreature
{
public:

    enum PLAYER_STATUS : _uint
    {
        BAREHAND = 1 << 0,
        SPEAR = 1 << 1,
        GSWORD = 1 << 2,
        RESERVED = 1 << 3,
        CHARGING_SPRINT = 1 << 4,
        BACK_DODGE = 1 << 5,
        ROTATION = 1 << 6,
        CHARGING_FAST_ATTACK = 1 << 7, // 차징 공격
        SPRINT_AGAIN_REQUEST = 1 << 8, //스페이스바 유연하게 사용하도록 스프린트 제어
        LOCKON = 1 << 9,  // 락 온
        CHARGING_STRONG_ATTACK = 1 << 10, // 차징 공격
        DODGING = 1 << 11,  //닷지할 때 

        /* 가드 */
        GUARD = 1 << 12,
        GUARD_SUCCESS = 1 << 13,
        JUST_GUARD = 1 << 14,
        GUARD_ROTATION = 1 << 15,
        GUARD_ROTATION_REQUEST = 1 << 16,

        /*  낙하 */
        FALLING = 1 << 17,
        PRE_LAND = 1 << 18, //착지 직전 
        FALLING_ATTACK = 1 << 19,

        /* 스태미나 */
        STAMINA_RECOVERY = 1 << 20,

        /* 브루탈 */
        BRUTAL_BEGIN = 1 << 21, //브루탈 탐지 범위 내에 옴
        BRUTAL_READY = 1 << 22, //브루탈공격 가능 범위 내에 옴
        BRUTAL_SUCCESS = 1 << 23,  //브루탈공격 함. 

        EARLY_DODGING = 1 << 24,  

        INTERACTION_STATUE = 1 << 25, //상호작용 조각상 (공격,가드,스킬키 입력 안받음)
        STAMINA_EXHAUSTION  = 1 << 26, 

        VIPER_GRAB = 1 << 27,

        /* 회전 */
        //TURN180 = 1 << 20,
        //TURN180_REQUESTED = 1 << 21, 
        //TURN180_COMPLETE = 1 << 22,
        //MOVE_AFTER_TURN = 1 << 23,
        //JUST_COMPLETED_TURN180 = 1 << 24,
        

        STATUS_CLEARS = RESERVED | CHARGING_SPRINT | BACK_DODGE | CHARGING_FAST_ATTACK | SPRINT_AGAIN_REQUEST | LOCKON | CHARGING_STRONG_ATTACK
        | GUARD | GUARD_SUCCESS | JUST_GUARD | GUARD_ROTATION_REQUEST
        | FALLING | FALLING_ATTACK | PRE_LAND  | DODGING | EARLY_DODGING
        | BRUTAL_BEGIN | BRUTAL_READY | BRUTAL_SUCCESS
        | INTERACTION_STATUE | STAMINA_EXHAUSTION | VIPER_GRAB

        /*| TURN180| TURN180_REQUESTED | TURN180_COMPLETE| MOVE_AFTER_TURN*/,

    };
    enum PLAYER_CAMERA_DIR {
        PC_FRONT, PC_FRONT_RIGHT, PC_RIGHT, PC_BACK_RIGHT,
        PC_BACK, PC_BACK_LEFT, PC_LEFT, PC_FRONT_LEFT
    };

private:
    CKhazan_GSword(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CKhazan_GSword(const CKhazan_GSword& Prototype);
    virtual ~CKhazan_GSword() = default;

public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize_Clone(void* pArg);
    virtual void Priority_Update(_float fTimeDelta);
    virtual void Update(_float fTimeDelta);
    virtual void Late_Update(_float fTimeDelta);
    virtual HRESULT Render();

public:
    virtual void Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc = nullptr) override;
    virtual void Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc = nullptr) override;
    virtual void Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, COLLISION_DESC* pMyDesc = nullptr) override;
    virtual void Take_Damage(_float fDamage, HITREACTION eHitreaction, CGameObject* pGameObject);

    //const _float4x4* Get_BoneSpearFXMatrixPtr() { return &m_pSpearFX_WorldMatrix; }
    //_matrix Get_BoneSpearFXMatrix() { return XMLoadFloat4x4(&m_pSpearFX_WorldMatrix); }

    void	Set_Camera(class CCamera_Compre* pCamera);
    void    Set_Position(_float4 vPos);


private:
    class CBody_Khazan_GS*              m_pBody = { nullptr };
    class CGSword_Khazan_GS*            m_pGSword = { nullptr };
    class CLantern_Khazan_GS*           m_pLantern = { nullptr };
    class CKhazan_GS_Anim_Move*         m_pAnimMove = { nullptr };
    class CKhazan_GS_Anim_Attack*       m_pAnimAttack = { nullptr };
    class CKhazan_GS_Anim_Guard*        m_pAnimGuard = { nullptr };
    class CKhazan_GS_Anim_Interaction*  m_pAnimInteraction = { nullptr };
    class CKhazan_GS_Anim_Damaged*      m_pAnimDamaged = { nullptr };
    class CKhazan_GS_Anim_Fall*         m_pAnimFall = { nullptr };

    class CCamera_Compre*               m_pCamera = { nullptr };
    class CClientInstance*              m_pClientInstance = { nullptr };

    /* FPS*/
    _float                      m_fTimeAcc = { 0.f };
    _uint                       m_iFpsCount = { 0 }; 
    _uint                       m_iFps = { 0 };

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
    _float4x4*                  m_GSwordBackPack_Matrix;              //대검 소켓 
    _float4x4*                  m_SpearBackPack_Matrix;              //대검 소켓 
    _float4x4*                  m_pWeaponR_Matrix = { nullptr };
    _float4x4*                  m_pGSword_Matrix = { nullptr };
   // _float4x4					m_pGSword_WorldMatrix = {};
   // _matrix						m_Offset_Matrix = {};
    _bool						m_isEnableControl = { true };


    /* Move*/
    DIR							m_eWorldDir = {}; // 카메라 기준 월드 방향 
    _float						m_fRotateTime[2] = { 0.f,0.15f };
    _vector						m_vRotateStart;
    _float						m_fSprintTime = { 0.f };
    uint						m_iStopMoveIndexTable[10];	/* 스탑 애니메이션일 때 움직임 x  */

    /* Attack  */
    _float						m_fChargingFastAttackTime = { 0.f };
    _float						m_fChargingStrongAttackTime = { 0.f };
    _uint                       m_iCurSkillIndex = {};
    DIR                         m_eHitNormalDir = {};               //맞은 방향  저장
    DIR                         m_eHitStrongDir = {};               //맞은 방향  저장

    /* Guard */
    _vector                     m_vGuardRotationStart = {};
    _vector                     m_vGuardRotationEnd = {};
    _float4                     m_vGuardRotationTarget = {}; // 가드 시 충돌지점으로 회전
    _float2                     m_fGuardRotationTime = { 0.f, 0.15f };

    /* Fall  */

    /* recovery */
    _float2                     m_fIntervalStaminaRecovery = { 0.f, 0.25f };
    _float2                     m_fWaitStaminaRecovery = { 0.f, 1.f };

    /* 180 turn */
    //_float2                     m_f180TurnTime = { 0.f, 0.f };      // x: 경과시간, y: 회전 소요시간
    //_vector                     m_v180TurnStartRot = {};            // 시작 회전
    //_vector                     m_v180TurnEndRot = {};              // 목표 회전
    //_uint                       m_iPrev180TurnSubState = {};
    //_float                      m_fTurn180CooldownTime = {};

    /* ====== const ======*/
    const	_float				m_fMinSprintTime = { 0.15f };

    /* Move Speed */
    _float				        m_fInjuredSpeed = { 1.15f };
    _float				        m_fWalkSpeed = { 2.6f };
    _float				        m_fRunSpeed = { 9.f };
    _float				        m_fSprintSpeed = { 15.4f };

    /*  Attack */
    const _float				m_fChargingStartIntervalTime = { 0.25f }; // 차징 시작 시간
    const _float                m_fBrutalAttackSearchMaxDistance = { 2.f };
    const _float                m_fBrutalAttackMaxDistance = { 1.f };

    /* Fall */
    const _float                m_fRayLength = { 8.f };
    const _float2               m_fRayZOffset = { 0.f, 0.5f };

    /* SnowEffect SpawnTime*/
private:
    _float                      m_EffectTimeDelta;

private:
    void			Update_Stats(_float fTimeDelta);
    void			Update_State(_float fTimeDelta);
    void            Update_LockOn();   //카메라 락온과 동기화

    /* Input */
    _bool           Fall_Input(_float fTimeDelta);
    void			Move_Input(_float fTimeDelta);
    _bool			Skill_Input(_float fTimeDelta);
    _bool			Attack_Input(_float fTimeDelta);
    _bool			Guard_Input(_float fTimeDelta);
    _bool           Interaction_Input(_float fTimeDelta);

    /* Animation  */
    void			Change_MoveIdle(_float fTimeDelta);
    void			ExecuteAnimationExit();
    _bool           ChangeGrabAnimation();

    /* Rotation, Direction */
    void			Apply_PlayerMovement(_float fTimeDelta);
    void			Check_KeyInput_Direction(_float fTimeDelta);
    DIRECTION_INFO  Calculate_LockOnDirection(_float fTimeDelta);
    void            LockOn_Rotation(_float fTimeDelta);
    void            Setting_Guard_Rotation();
    void            Guard_Rotation(_float fTimeDelta);
    void            Get_HitReaction(_float3 vContactPoint);
    //void            Check_Turn180(_float fTimeDelta);   // 180도 회전 체크
    //void            Start_Turn180Rotation();                             // 180도 회전 시작
    //void            Update_Turn180Rotation(_float fTimeDelta);           // 180도 회전 업데이트
    //void            Complete_Turn180();                                  // 180도 회전 완료 처리
    _uint			ConvertCameraToPlayerDir(PLAYER_CAMERA_DIR playerCamDir);

    /* state update */
    void            Update_Die(_float fTimeDelta);

    /* others,, */
    void            Check_IsInAir(_float fTimeDelta);
    void            Clear_Injured();

public:
    void            EnterStatuePuzzle(); //조각상 돌리기 모드 on
    void            ExitStatuePuzzle(); //조각상 돌리기 모드 off

private:
    HRESULT			Ready_Components();
    HRESULT			Ready_PartObjects();
    HRESULT			Ready_Collision();
    HRESULT			Ready_AnimationStateMachine();
    //HRESULT			Ready_PlayerData();/
    void            Ready_MotionTrailCallback();

    void            Clear_Step0();  // 이전 상태 빼고 다 클리어
    void            Clear_Step1();  //이전 상태 , lockOn, dir,worldDir 빼고 다 클리어
    void            Clear_Step2();  // 무빙 관련 클리어
    void            Clear_Step3();  // 가드 빼고 클리어

private:
    inline void		Add_Status(_uint i) { m_iStatus |= i; }
    inline void		Remove_Status(_uint i) { m_iStatus &= ~i; }
    inline _bool	Has_Status(_uint i) { return (m_iStatus & i) != 0; }
    inline _bool    Has_AllStatus(_uint i) { return (m_iStatus & i) == i; }
    inline void     Toggle_Status(_uint i) { m_iStatus ^= i; }
    //inline void		Clear_Status( ) { m_iStatus = 0; }

    inline void		Add_State(_uint i) { m_iCurMainState |= i; }
    inline void		Toggle_State(_uint i) { m_iCurMainState ^= i; }
    inline void		Remove_State(_uint i) { m_iCurMainState &= ~i; }
    inline _bool	Has_State(_uint i) { return (m_iCurMainState & i) != 0; }
    inline _bool	Has_AllStates(_uint i) { return (m_iCurMainState & i) == i; }
    inline _bool	Has_States() { return m_iCurMainState != 0; }	//하나의 상태라도 있는지 없는지
    inline void		Clear_State() { m_iCurMainState = 0; }

    inline void		Add_SubState(_uint i) { m_iCurSubState |= i; }
    inline void		Toggle_SubState(_uint i) { m_iCurSubState ^= i; }
    inline void		Remove_SubState(_uint i) { m_iCurSubState &= ~i; }
    inline _bool	Has_SubState(_uint i) { return (m_iCurSubState & i) != 0; }
    inline _bool	Has_AllSubStates(_uint i) { return (m_iCurSubState & i) == i; }
    inline _bool	Has_SubStates() { return m_iCurSubState != 0; }//하나의 상태라도 있는지 없는지
    inline void		Clear_SubState() { m_iCurSubState = 0; }

    inline void		Add_CycleState(_uint i) { m_iCycle |= i; }
    inline void		Remove_CycleState(_uint i) { m_iCycle &= ~i; }
    inline _bool	Has_CycleState(_uint i) { return (m_iCycle & i) != 0; }
    inline void		Clear_CycleState() { m_iCycle = 0; }
    inline void		AllClear_CycleState() { m_iCycle = 0; }


#pragma region 상호 작용 맵 오브젝트 이벤트
private:
    EventInteractType			m_EventInteract = {};				// 트리거 접촉 여부, 이벤트 발생 여부, 상호 작용 타입, 상호 작용 타입들의 구조체
    _float						m_fEventTimeAcc = { 0.f };
    _ushort						m_sNextItem = { 0 };

    _float4                     m_vStartPos_Event = {};
    _float                      m_fLerpTime_Event = { 0.f };
    _bool                       m_isInteractEventSetting = { false };

    _uint                       m_iInteractTypeEventID = { };
    _uint                       m_iObjectInteractEventID = { };


private:
    void						Subscribe_Events();
    void						Event_Interact_Object(_float fTimeDelta);
    /*  하인마스 + 엠바스 */
    void						BladeNexus_Event(_float fTimeDelta);
    void						Chest_Event(_float fTimeDelta);
    void						TombStone_Event(_float fTimeDelta);     //폐기

    /* 앰바스 */
    void						Lever_Event(_float fTimeDelta);
    void						Statue_Event(_float fTimeDelta); //조각상 퍼즐
    void						IronGate_Event(_float fTimeDelta); //레버 다음 철문 밀어서열기 (자물쇠 해제)
    void						UnLockGear_Event(_float fTimeDelta);  // ?? CA_P_Kazan_GearSwitch_001_Interation 이거 쓰면될 듯
    void						GiantGate_Event(_float fTimeDelta);  //거대한 철문 
    void						NPC_Event(_float fTimeDelta);
    void						Ladder_Event(_float fTimeDelta);

private:
    void                        Lerp_Position_ByInteractEvent(_float4 vTargetPos, _float4 vStartPos, _float fDuration, _float fTimeDelta, _bool& isDone);
#pragma endregion

#ifdef _DEBUG
    void			Debug_Widget();
    void			Debug_Widget_States();      // 상태 디버깅
    void            Debug_Widget_Combat();
    void			Debug_Widget_Animation();   // 애니메이션 디버깅
    void			Debug_Widget_Movement();    // 이동 디버깅

    void Debug_Widget_Guard();

    std::string GetDirectionString(DIRECTION_INFO dir);

    std::string GetHitReactionString();

#endif // _DEBUG


public:
    static CKhazan_GSword* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;


};

NS_END




#pragma region Try Later

//#pragma once
//
//#include "Client_Defines.h"
//#include "Creature.h"
//
//NS_BEGIN(Client)
//
//class CKhazan_GSword final : public CCreature
//{
//
//private:
//    CKhazan_GSword(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
//    CKhazan_GSword(const CKhazan_GSword& Prototype);
//    virtual ~CKhazan_GSword() = default;
//
//public:
//    virtual HRESULT Initialize_Prototype();
//    virtual HRESULT Initialize_Clone(void* pArg);
//    virtual void    Priority_Update(_float fTimeDelta);
//    virtual void    Update(_float fTimeDelta);
//    virtual void    Late_Update(_float fTimeDelta);
//    virtual HRESULT Render();
//
//    // Collision
//    virtual void    Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal) override;
//    virtual void    Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal) override;
//    virtual void    Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer) override;
//    virtual void    Take_Damage(_float fDamage, HITREACTION eHitreaction, CGameObject* pGameObject) override;
//
//    // Setters
//    void            Set_Camera(class CCamera_Compre* pCamera);
//    void            Set_Position(_float4 vPos);
//    void            Set_IsPlayerEnable(_bool isEnableControl) { m_isEnableControl = isEnableControl; }
//
//private:
//    //Core Modules
//    class CKhazan_GS_StateMachine*          m_pStateMachine = { nullptr };
//    class CKhazan_GS_InputHandler*          m_pInputHandler = { nullptr };
//    class CKhazan_GS_AnimationController*   m_pAnimController = { nullptr };
//    class CKhazan_GS_MovementController*    m_pMovementController = { nullptr };
//    class CKhazan_GS_CombatModule*          m_pCombatModule = { nullptr };
//
//    // Part Objects
//    class CBody_Khazan_GS*                  m_pBody = { nullptr };
//    class CGSword_Khazan_GS*                m_pWeapon = { nullptr };
//    class CLantern_Khazan_GS*               m_pLantern = { nullptr };
//
//    //External References
//    class CCamera_Compre*                   m_pCamera = { nullptr };
//    class CClientInstance*                  m_pClientInstance = { nullptr };
//
//    // Data
//    PLAYER_DATA*                            m_pPlayerData = { nullptr };
//
//    // Simple State
//    _bool                                   m_isEnableControl = { true };
//    _bool                                   m_isLockOn = { false };
//    _uint                                   m_iCurWeapon = { 0 };   // 현재 무기
//
//    //Core Update Flow
//    void            Process_Input(_float fTimeDelta);
//    void            Update_State(_float fTimeDelta);
//    void            Update_Animation(_float fTimeDelta);
//    void            Update_Movement(_float fTimeDelta);
//    void            Update_Combat(_float fTimeDelta);
//    void            Update_LockOn();        //카메라랑 동기화 
//
//
//    //Event Handling
//    void            Handle_StateChanged();
//    void            Handle_Death();
//
//private:
//    HRESULT         Ready_Components();
//    HRESULT         Ready_PartObjects();
//    HRESULT         Ready_Collision();
//    HRESULT         Ready_Modules();
//
//public:
//    static CKhazan_GSword*      Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
//    virtual CGameObject*        Clone(void* pArg) override;
//    virtual void                Free() override;
//};
//
//
//NS_END

#pragma endregion