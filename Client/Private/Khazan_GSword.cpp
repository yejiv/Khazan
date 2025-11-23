#include "Khazan_GSword.h"
#include "Body_Khazan_GS.h"
#include "GSword_Khazan_GS.h"

#include "ClientInstance.h"
#include "GameInstance.h"
#include "RigidBody.h"
#include "CharacterVirtual.h"

#include "Khazan_Spear_ASMachine.h"

#include "Khazan_GS_Anim_Move.h"
#include "Khazan_GS_Anim_Attack.h"
#include "Khazan_GS_Anim_Guard.h"
#include "Khazan_GS_Anim_Interaction.h"
#include "Khazan_GS_Anim_Damaged.h"
#include "Khazan_GS_Anim_Fall.h"
#include "Lantern_Khazan_GS.h"

#include "Camera_Compre.h"
#include "UI_HUD.h"
#include "Damage_Text.h"


#pragma region 이벤트 - 인벤토리
#include "UI_Inven.h"
#include <Monster.h>
#include <Target_BrutalAttack.h>
#include "UI_Talk_Daphrona.h"
#pragma endregion

using WEA = CKhazan_Spear_ASMachine::WEAPON;
using CAT = CKhazan_Spear_ASMachine::CATEGORY;
using ATT = CKhazan_Spear_ASMachine::ATTACK;
using SKI = CKhazan_Spear_ASMachine::SKILL;
using MOV = CKhazan_Spear_ASMachine::MOVE;
using MOV_S = CKhazan_Spear_ASMachine::MOVESUB;
using CYC = CKhazan_Spear_ASMachine::CYCLE;
using GUA = CKhazan_Spear_ASMachine::GUARD;
using GRO = CKhazan_Spear_ASMachine::GROGGY;
using INTE = CKhazan_Spear_ASMachine::INTERACT;
using WEA_C = CKhazan_Spear_ASMachine::WEAPONCHANGE;
using HOL = CKhazan_Spear_ASMachine::HOLD;
using DAM = CKhazan_Spear_ASMachine::DAMAGED;
using CONTROL_BUTTON = CPlayer_Manager::CONTROL_BUTTON;


CKhazan_GSword::CKhazan_GSword(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CCreature{ pDevice, pContext }
{
}

CKhazan_GSword::CKhazan_GSword(const CKhazan_GSword& Prototype)
    : CCreature{ Prototype }
    , m_pClientInstance{ CClientInstance::GetInstance() }
{
    Safe_AddRef(m_pClientInstance);
}

HRESULT CKhazan_GSword::Initialize_Prototype()
{
    return S_OK;

}

HRESULT CKhazan_GSword::Initialize_Clone(void* pArg)
{
    CREATURE_DESC desc{};

    desc.fAttack = 10.f;
    desc.fMaxHP = 100.f;
    desc.fMaxStamina = 100.f;
    desc.fMoveSpeed = 10.f;
    desc.fRotationPerSec = XMConvertToRadians(180.f);
    desc.fSpeedPerSec = 1.f;

    if (FAILED(__super::Initialize_Clone(&desc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_PartObjects()))
        return E_FAIL;

    if (FAILED(Ready_Collision()))
        return E_FAIL;

    if (FAILED(Ready_AnimationStateMachine()))
        return E_FAIL;


#pragma region 상호 작용 맵 오브젝트 이벤트
    Subscribe_Events();
#pragma endregion

#ifdef _DEBUG
    Debug_Widget();
#endif // _DEBUG

    m_eDir.Add_Flag(DIRECTION_INFO::NONE);

    m_iCurAnimIndex = m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_BareHands_Stand");
    m_pBody->Get_Model()->Set_Animation(m_iCurAnimIndex);
    Add_Status(BAREHAND);
    //Add_Status(GSWORD);

    m_iStopMoveIndexTable[0] = m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_BareHands_Walk_Stop_F_RF");
    m_iStopMoveIndexTable[1] = m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_GSword_Run_Stop_F_RF");
    m_iStopMoveIndexTable[2] = m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_BareHands_Walk_Stop_F_LF");
    m_iStopMoveIndexTable[3] = m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_GSword_Walk_Stop_F_LF");
    m_iStopMoveIndexTable[4] = m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_BareHands_Run_Stop_F_RF");
    m_iStopMoveIndexTable[5] = m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_GSword_Run_Stop_F_RF");
    m_iStopMoveIndexTable[6] = m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_BareHands_Run_Stop_F_LF");
    m_iStopMoveIndexTable[7] = m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_GSword_Run_Stop_F_LF");
    m_iStopMoveIndexTable[8] = m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_GSword_Sprint_Stop_F");


    /* 플레이어 데이터 연결  */
    m_pPlayerData = m_pClientInstance->Get_pInitailizePlayerData();

    m_pGSword->Set_Enble(true);

    m_strName = "Khazan";

    m_EffectTimeDelta = 0.f;


    return S_OK;

}

void CKhazan_GSword::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);

    if (m_pGameInstance->Key_Down(DIK_P))
    {
        m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(516.f, -11.f, 264.f, 1.f));
        m_pCharVirCom->Set_Position(XMVectorSet(516.f, -11.f, 264.f, 1.f));
    }


}

void CKhazan_GSword::Update(_float fTimeDelta)
{
    if (m_isEnableControl)
    {
        m_fTimeAcc += fTimeDelta;
        ++m_iFpsCount;
        if (m_fTimeAcc >= 1.f)
        {
            m_iFps = m_iFpsCount;
            m_iFpsCount = 0;
            m_fTimeAcc = 0.f;
        }

        if (m_pGameInstance->Key_Pressing(DIK_LSHIFT, fTimeDelta) && m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::LB))
        {
            _float3     vPickedPos{};
            _bool isPicked = m_pGameInstance->isPicked(&vPickedPos);
            if (true == isPicked)
            {
                m_pTransformCom->Set_State(Engine::STATE::POSITION, XMVectorSetW(XMLoadFloat3(&vPickedPos), 1.f));
                m_pCharVirCom->Set_Velocity(XMVectorSet(0.f, 0.f, 0.f, 1.f));
            }
        }

        Check_IsInAir(fTimeDelta);

        Update_Stats(fTimeDelta);

        m_pBody->Search_BrutalTarget(fTimeDelta);
        m_pBody->Check_BrutalAttack(fTimeDelta);

        Update_State(fTimeDelta);

    }

#pragma region 상호 작용 맵 오브젝트 이벤트
    Event_Interact_Object(fTimeDelta);
#pragma endregion

    if (m_pCharVirCom->Get_isGround())
    {
        m_vGravity = XMVectorSet(0.f, g_fGravity, 0.f, 0.f);
    }
    else {
        m_vGravity = XMVectorSet(0.f, g_fGravity * 2.f, 0.f, 0.f);
    }

    __super::Update(fTimeDelta);

   //XMStoreFloat4x4(&m_pGSword_WorldMatrix, m_Offset_Matrix * XMLoadFloat4x4(m_pGSword_Matrix) * m_pTransformCom->Get_WorldMatrix());


    //if (m_pGameInstance->Key_Pressing(DIK_RSHIFT, fTimeDelta) && m_pGameInstance->Key_Down(DIK_2))
    //{
    //    m_pBody->Get_Model()->Set_Animation(m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_Com_Lantern_On"));
    //}
    //if (m_pGameInstance->Key_Pressing(DIK_RSHIFT, fTimeDelta) && m_pGameInstance->Key_Down(DIK_3))
    //{
    //    m_pBody->Get_Model()->Set_Animation(m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_Com_Lantern_Off"));
    //}

    if (m_pGameInstance->Get_CurrentLevelID() == ENUM_CLASS(LEVEL::HEINMACH) && m_EventInteract.isInCave() == false)
    {
        m_EffectTimeDelta += fTimeDelta;
        if (m_EffectTimeDelta > 2.f)
        {
            m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Snow_Once"), m_pTransformCom->Get_State(STATE::POSITION));
            m_EffectTimeDelta = 0.f;
        }
    }

}

void CKhazan_GSword::Late_Update(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::DYNAMIC, this)))
        return;

    __super::Late_Update(fTimeDelta);
}

HRESULT CKhazan_GSword::Render()
{
    return S_OK;
}

void CKhazan_GSword::Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal)
{
    if (iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::MONSTERATTACK))
        Get_HitReaction(vContactPoint);
}

void CKhazan_GSword::Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal)
{

}

void CKhazan_GSword::Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer)
{

}

void CKhazan_GSword::Take_Damage(_float fDamage, HITREACTION eHitreaction, CGameObject* pGameObject)
{
    /* Just Guard 우선 처리*/
    if (Has_Status(JUST_GUARD))
    {
        Clear_Step3();
        m_pAnimGuard->Try_JustGuard(m_eHitNormalDir.iDirFlag);
        Remove_Status(JUST_GUARD);
        return;
    }


    /* 가드 중 강한넉백공격이 오면 성공모션 취하기 */
    if (m_pAnimGuard->Is_Guarding())
    {
        if (eHitreaction == HITREACTION::KNOCKBACK_STRONG) {
            Clear_Step3();
            m_pAnimGuard->Try_SuccessGuard(m_eHitNormalDir.iDirFlag);
        }

        return;
    }

    m_pPlayerData->fCulHp -= fDamage;

    /* 플레이어 죽었을 때 세팅하는 법  */
    if (m_pPlayerData->fCulHp <= 0.f)
    {
        /* 입력 막기 */
        m_pClientInstance->Set_PlayerInput(false);

        /* 상태 초기화 */
        Clear_CycleState();
        Clear_SubState();
        Clear_State();
        m_eDir.iDirFlag = 0;
        //m_eWorldDir.iDirFlag = 0;
        m_iStatus = 0;

        /* 상태 DIE로 재정비 */
        if (Has_Status(BAREHAND)) m_iStatus = BAREHAND;
        else if (Has_Status(SPEAR)) m_iStatus = SPEAR;
        else if (Has_Status(GSWORD)) m_iStatus = GSWORD;


        Add_State(CAT::M_DIE);
        m_iCurAnimIndex = m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_Die_F");
        m_pBody->Get_Model()->Set_Animation(m_iCurAnimIndex);

        /* UI */
        m_pGameInstance->Emit_Event< EVENT_ANNOUNCE_RESULT>(ENUM_CLASS(EVENT_TYPE::ANNOUNCE_OVER), {});
        return;
    }

    /* Damage UI font */
    CDamage_Text* pDamage = static_cast<CDamage_Text*>(m_pGameInstance->Pop_PoolObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Pool_Damage_Text")));
    if (pDamage != nullptr)
    {
        pDamage->Render_Damage(CDamage_Text::DAMAGE_TYPE::PLAYER, m_pTransformCom->Get_State(STATE::POSITION), fDamage, { 0.f, 5.f });
        m_pGameInstance->Push_PoolObject_ToLayer(m_pGameInstance->Get_CurrentLevelID(), TEXT("Layer_UI"), pDamage);
    }

    /*  Decal */
    _vector vDecalPos = m_pTransformCom->Get_State(STATE::POSITION);
    _float fOffset = 2.f;
    _float fPosX = XMVectorGetX(vDecalPos);
    _float fPosZ = XMVectorGetZ(vDecalPos);
    vDecalPos = XMVectorSetX(vDecalPos, m_pGameInstance->Rand(fPosX - fOffset, fPosX + fOffset));
    vDecalPos = XMVectorSetZ(vDecalPos, m_pGameInstance->Rand(fPosZ - fOffset, fPosZ + fOffset));
    DECAL_DESC Desc{};
    Desc.fLifeTime = 8.f;
    Desc.vFadeTime = _float2(0.2f, 0.2f);
    Desc.eType = static_cast<DECALTYPE>(m_pGameInstance->Rand(0.f, static_cast<_float>(DECALTYPE::END)));
    XMStoreFloat3(&Desc.vPosition, vDecalPos);
    Desc.vScale = _float3(
        m_pGameInstance->Rand(3.f, 5.f),
        2.f,
        m_pGameInstance->Rand(3.f, 5.f)
    );
    Desc.vColor = _float3(0.2745f, 0.08f, 0.08f);
    m_pGameInstance->Spawn_Decal(TEXT("Pool_Decal"), ENUM_CLASS(LEVEL::HEINMACH), TEXT("Layer_Decal"), Desc);

    switch (eHitreaction)
    {
    case Client::HITREACTION::NONE:

        break;
        //case Client::HITREACTION::GROGGY:

        //    break;
    case Client::HITREACTION::KNOCKBACK_WEAK:
        if (Has_State(CAT::M_ATTACK)) m_pAnimAttack->Exit();
        if (Has_State(CAT::M_SKILL))  m_pAnimAttack->Exit();
        if (Has_State(CAT::M_GUARD)) m_pAnimGuard->Exit();
        if (Has_State(CAT::M_MOVE)) m_pAnimMove->Exit();

        Clear_CycleState();
        Clear_SubState();
        Clear_State();
        Remove_Status(RESERVED | CHARGING_SPRINT | BACK_DODGE | CHARGING_STRONG_ATTACK | SPRINT_AGAIN_REQUEST | READY_ASSAULT);
        m_eDir.iDirFlag = 0;
        // m_eWorldDir.iDirFlag = 0;
        cout << "        KNOCKBACK_WEAK    " << endl;

        Add_State(CAT::M_DAMAGED);
        m_pAnimDamaged->Force_DamagedNormal(Has_Status(GSWORD), m_eHitNormalDir.iDirFlag);
        break;
    case Client::HITREACTION::KNOCKBACK_NORMAL:
        if (Has_State(CAT::M_ATTACK)) m_pAnimAttack->Exit();
        if (Has_State(CAT::M_SKILL))  m_pAnimAttack->Exit();
        if (Has_State(CAT::M_GUARD)) m_pAnimGuard->Exit();
        if (Has_State(CAT::M_MOVE)) m_pAnimMove->Exit();
        cout << "       KNOCKBACK_NORMAL     " << endl;

        Clear_CycleState();
        Clear_SubState();
        Clear_State();
        Remove_Status(RESERVED | CHARGING_SPRINT | BACK_DODGE | CHARGING_STRONG_ATTACK | SPRINT_AGAIN_REQUEST | READY_ASSAULT);
        m_eDir.iDirFlag = 0;
        // m_eWorldDir.iDirFlag = 0;

        Add_State(CAT::M_DAMAGED);
        m_pAnimDamaged->Force_DamagedNormal(Has_Status(GSWORD), m_eHitNormalDir.iDirFlag);
        break;
    case Client::HITREACTION::KNOCKBACK_STRONG:
        if (Has_State(CAT::M_ATTACK)) m_pAnimAttack->Exit();
        if (Has_State(CAT::M_SKILL))  m_pAnimAttack->Exit();
        if (Has_State(CAT::M_GUARD)) m_pAnimGuard->Exit();
        if (Has_State(CAT::M_MOVE)) m_pAnimMove->Exit();
        cout << "        KNOCKBACK_STRONG       " << endl;

        Clear_CycleState();
        Clear_SubState();
        Clear_State();
        Remove_Status(RESERVED | CHARGING_SPRINT | BACK_DODGE | CHARGING_STRONG_ATTACK | SPRINT_AGAIN_REQUEST | READY_ASSAULT);
        m_eDir.iDirFlag = 0;
        // m_eWorldDir.iDirFlag = 0;

        Add_State(CAT::M_DAMAGED);
        m_pAnimDamaged->Force_DamagedStrong(Has_Status(GSWORD), m_eHitStrongDir.iDirFlag);
        break;
    case Client::HITREACTION::PARRY:

        break;
    case Client::HITREACTION::GRAB:
        cout << " GRAB !!! " << endl;
        m_iCurAnimIndex = m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_DamageHold_Yetuga_RushGrab");
        break;

    }

}

void CKhazan_GSword::Set_Camera(CCamera_Compre* pCamera)
{
    m_pCamera = pCamera;
    Safe_AddRef(m_pCamera);
}

void CKhazan_GSword::Set_Position(_float4 vPos)
{
    m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat4(&vPos), 1.f));
}

void CKhazan_GSword::Update_Stats(_float fTimeDelta)
{
    /* idle, run, walk, 현재 스태미나가 닳아 있는 상태일 때*/
    if (!Has_States() || (Has_State(CAT::M_MOVE) && Has_SubState(MOV::MOVE_RUN | MOV::MOVE_WALK)) && m_pPlayerData->fCulStamina < m_pPlayerData->fMaxStamina)
    {
        if (!Has_Status(STAMINA_RECOVERY))
        {
            m_fWaitStaminaRecovery.x += fTimeDelta;
            if (m_fWaitStaminaRecovery.x >= m_fWaitStaminaRecovery.y)
            {
                m_fIntervalStaminaRecovery.x = 1.f;
                m_fWaitStaminaRecovery.x = 0.f;
                Add_Status(STAMINA_RECOVERY);
            }
        }
        else
        {
            m_fIntervalStaminaRecovery.x += fTimeDelta;

            if (m_fIntervalStaminaRecovery.x >= m_fIntervalStaminaRecovery.y)
            {
                m_fIntervalStaminaRecovery.x = 0.f;
                m_pPlayerData->fCulStamina += m_pPlayerData->fStaminaRegen;
                if (m_pPlayerData->fCulStamina > m_pPlayerData->fMaxStamina)
                    m_pPlayerData->fCulStamina = m_pPlayerData->fMaxStamina;
            }
        }
    }
    else if (Has_Status(STAMINA_RECOVERY))
    {
        m_fWaitStaminaRecovery.x = 0.f;
        Remove_Status(STAMINA_RECOVERY);
    }

}

void CKhazan_GSword::Update_State(_float fTimeDelta)
{
    if (Has_State(CAT::M_DIE))
        return;

    /* 이전 상태 저장*/
    m_iPrevMainState = m_iCurMainState;
    m_iPrevSubState = m_iCurSubState;
    m_ePrevDir = m_eDir.iDirFlag;
    m_iPrevCycle = m_iCycle;

    /* Fall 상태 최우선 체크 */
    if (Fall_Input(fTimeDelta))
        return;

    /* 대미지 상태 우선 체크 */
    _bool IsDamaged = m_pAnimDamaged->Is_Damaged();
    _bool IsGuarding = m_pAnimGuard->Is_Guarding();

    // 가드 중이면 대미지 무시
    if (IsGuarding && IsDamaged)
    {
        // 가드 성공 처리는 여기서
        m_pAnimDamaged->Clear_Damaged();
        IsDamaged = false;
    }

    // 가드중이 아니고  대미지 상태가 끝났을 때만 상태 제거
    if (!IsGuarding && !IsDamaged && (m_iPrevMainState & CAT::M_DAMAGED))
    {
        Clear_Step0();
        Remove_State(CAT::M_DAMAGED);
    }

    // 가드중이 아니고  대미지 받는 중이면 입력 및 다른 모든 처리 차단
    if (!IsGuarding && IsDamaged)
    {
        // 대미지 로직만 실행
        if (Has_State(CAT::M_DAMAGED))
        {
            _bool isEnter = (m_iCurMainState != m_iPrevMainState);
            if (isEnter) m_pAnimDamaged->Enter();
            m_pAnimDamaged->Continue(fTimeDelta);
        }
        return;  // 대미지 중에는 다른 모든 처리 차단
    }

    /* 키 입력 */
    if (m_pClientInstance->Get_PlayerInput())
    {

        /* 락온상태 체크  */
        Update_LockOn();

        /* 방향 결정 */
        Check_KeyInput_Direction(fTimeDelta);

        Interaction_Input(fTimeDelta);
        Guard_Input(fTimeDelta);
        Skill_Input(fTimeDelta);
        Attack_Input(fTimeDelta);

        // 공격 중일 때는 Move_Input을 완전히 차단
        if (!Has_State(CAT::M_ATTACK | CAT::M_GUARD | CAT::M_SKILL) && !m_pAnimAttack->Is_Attacking() && m_pAnimAttack)
            Move_Input(fTimeDelta);

        if (Has_State(CAT::M_ATTACK | CAT::M_SKILL))
        {
            if (Has_State(CAT::M_MOVE)) Clear_Step2();
            if (Has_Status(LOCKON)) LockOn_Rotation(fTimeDelta);
        }

        if (Has_Status(GUARD_ROTATION_REQUEST)) Setting_Guard_Rotation();
        if (Has_Status(GUARD_ROTATION)) Guard_Rotation(fTimeDelta);

    }

    /*  상태 전환 여부*/
    _bool isEnter = (m_iCurMainState != m_iPrevMainState) || (m_iCurSubState != m_iPrevSubState);
    _bool isContinue = (m_iCurMainState == m_iPrevMainState) && (m_iCurSubState == m_iPrevSubState);

    /* 공격 상태가 아닐 때 공격 콜리더 끄기  */
    if (!m_pAnimAttack->Is_Attacking() && !m_pAnimFall->Is_FallAttacking())
        m_pBody->Event_AttackTiming(false);


    /* (move , idle 애니메이션 재생 시도 */
    if (!Has_State(CAT::M_FALL | CAT::M_DAMAGED) && !m_pAnimDamaged->Is_Damaged())
        Change_MoveIdle(fTimeDelta);

    /* 실제 이동값 주기 */
    if (Has_State(CAT::M_MOVE | CAT::M_GUARD) &&
        !Has_State(CAT::M_ATTACK | CAT::M_SKILL | CAT::M_DAMAGED | CAT::M_INTERACT) &&
        !Has_SubState(MOV::MOVE_DODGE) &&
        !m_pAnimMove->IsDodge() &&
        !m_pAnimAttack->Is_Attacking())
    {
        Apply_PlayerMovement(fTimeDelta);
    }

    /* Exit 실행 */
    if (isEnter)
        ExecuteAnimationExit();

    /* 상태별 로직 실행 */
    if (Has_State(CAT::M_DIE)) {
        Update_Die(fTimeDelta);
    }
    else if (Has_State(CAT::ORDER2))
    {
        if (Has_State(CAT::M_FALL))
        {
            if (isEnter) m_pAnimFall->Enter();
            m_pAnimFall->Continue(fTimeDelta);

            if (!m_pAnimFall->Is_Falling())
            {
                Remove_State(CAT::M_FALL);
                // Clear_SubState();
            }
        }


        if (Has_State(CAT::M_GUARD))
        {
            if (isEnter) m_pAnimGuard->Enter();
            m_pAnimGuard->Continue(fTimeDelta);

            if (!m_pAnimGuard->Is_Guarding())
            {
                Remove_State(CAT::M_GUARD | CAT::M_MOVE);
                Clear_SubState();
            }
        }

        if (Has_State(CAT::M_DAMAGED))
        {
            if (isEnter) m_pAnimDamaged->Enter();
            m_pAnimDamaged->Continue(fTimeDelta);

            if (!m_pAnimDamaged->Is_Damaged())
            {
                Remove_State(CAT::M_DAMAGED);
                Clear_SubState();
            }
        }
    }
    else if (Has_State(CAT::M_SKILL))
    {
        if (isEnter) m_pAnimAttack->Enter();
        m_pAnimAttack->Continue(fTimeDelta);

        if (!m_pAnimAttack->Is_Skilling())
        {
            Remove_State(CAT::M_SKILL);
            Clear_SubState();
            m_pAnimAttack->Clear_Skill();

            if (m_eDir.iDirFlag > 0)
            {
                Clear_Step2();
                Add_State(CAT::M_MOVE);
                Add_SubState(MOV::MOVE_RUN);
                Add_CycleState(CYC::CYCLE_START);
            }
        }
    }

    else if (Has_State(CAT::ORDER5))
    {
        if (Has_State(CAT::M_ATTACK))
        {
            if (isEnter) m_pAnimAttack->Enter();
            m_pAnimAttack->Continue(fTimeDelta);

            if (!m_pAnimAttack->Is_Attacking())
            {
                Remove_State(CAT::M_ATTACK);
                Remove_Status(CHARGING_STRONG_ATTACK);
                Clear_SubState();

                if (m_eDir.iDirFlag > 0)
                {
                    Clear_Step2();
                    Add_State(CAT::M_MOVE);
                    Add_SubState(MOV::MOVE_RUN);
                    Add_CycleState(CYC::CYCLE_START);
                }
            }
        }

        if (!Has_State(CAT::M_ATTACK | CAT::M_SKILL) && Has_State(CAT::M_MOVE))
        {
            if (isEnter) m_pAnimMove->Enter();
            if (isEnter || isContinue) m_pAnimMove->Continue(fTimeDelta);


        }
    }
    else if (Has_State(CAT::M_INTERACT))
    {

    }
    else if (Has_State(CAT::M_WEAPON_CHANGE))
    {

    }
    else
    {

    }
}

_bool CKhazan_GSword::Fall_Input(_float fTimeDelta)
{
    if (Has_Status(FALLING | PRE_LAND | FALLING_ATTACK))
    {
        // 낙하 시작
        if (Has_Status(FALLING) && !Has_Status(PRE_LAND))
        {
            // 낙하공격 입력 체크 (낙하 중에만)
            if (m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::LB) &&
                !Has_Status(FALLING_ATTACK))
            {
                m_pAnimFall->Force_StartFallAttack();
                Add_Status(FALLING_ATTACK);
                Remove_Status(FALLING);
            }
            // 일반 낙하 애니메이션
            else if (!m_pAnimFall->Is_Falling() && !m_pAnimFall->Is_FallAttacking())
            {
                m_pAnimFall->Force_StartFall();
                Add_State(CAT::M_FALL);
            }
        }
        // 착지 직전
        else if (Has_Status(PRE_LAND))
        {
            if (Has_Status(FALLING_ATTACK))
            {
                m_pAnimFall->Force_AttackLanding();
            }
            else
            {
                m_pAnimFall->Force_Landing();
            }
        }

        // 낙하/착지 애니메이션 계속 실행
        m_pAnimFall->Continue(fTimeDelta);

        // 착지 애니메이션 완료 체크
        if (Has_Status(PRE_LAND) && m_pBody->Get_Model()->IsFinished())
        {
            Remove_Status(FALLING | FALLING_ATTACK | PRE_LAND);
            Remove_State(CAT::M_FALL);
            m_pAnimFall->Exit();
        }

        // 낙하 중에는 다른 입력 차단
        return true;
    }
    return false;
}

void CKhazan_GSword::Move_Input(_float fTimeDelta)
{
    /*  공격중 닷지는 예외 처리*/
    if (m_pGameInstance->Key_Down(DIK_SPACE) &&/* m_pBody->Is_SpearFullExtension() &&*/ Has_State(CAT::M_ATTACK | CAT::M_SKILL))  // zzzz
    {
        Remove_State(CAT::M_ATTACK | CAT::M_SKILL);
        Remove_Status(CHARGING_STRONG_ATTACK);
        Clear_SubState();
        Add_State(CAT::M_MOVE);
        Add_SubState(MOV::MOVE_DODGE);
    }

    // 공격/가드 중일 때는 완전히 리턴
    if (m_pAnimAttack->Is_Attacking() || m_pAnimGuard->Is_Guarding())
    {
        // 공격 중일 때는 Move 상태도 제거
        if (Has_State(CAT::M_MOVE))
        {
            Remove_State(CAT::M_MOVE);
            Clear_SubState();
            AllClear_CycleState();
        }
        return;
    }

    _bool isPrevMove = Has_State(CAT::M_MOVE);
    _bool isPrevDodge = Has_SubState(MOV::MOVE_DODGE);

    //Dodge 종료 체크
    if (isPrevDodge)
    {
        Remove_SubState(MOV::MOVE_DODGE);
        Remove_Status(CHARGING_SPRINT);

        // Dodge 애니메이션이 끝났는지 확인
        if (m_pAnimMove->IsStopMoveAnimantionFinished())
        {
            // 방향키만 눌려있으면 즉시 Run으로 전환
            if (!m_pGameInstance->Key_Pressing(DIK_SPACE, fTimeDelta) && m_eDir.iDirFlag > 0)
            {
                Add_State(CAT::M_MOVE);
                Add_SubState(MOV::MOVE_RUN);
                Add_CycleState(CYC::CYCLE_START);

                CKhazan_GS_Anim_Move::GS_MOVEINFO info;

                if (Has_Status(BAREHAND))info.iWeapon = BAREHAND;
                else if (Has_Status(SPEAR))info.iWeapon = SPEAR;
                else if (Has_Status(GSWORD))info.iWeapon = GSWORD;

                info.isLockOn = Has_Status(LOCKON);
                info.iState = m_iCurSubState;
                info.iCycle = m_iCycle;
                info.eDir = m_eDir;

                m_pAnimMove->Try_ChangeAnimation(info);
            }
            return;  //Dodge 종료 처리 후 바로 리턴
        }
        else
        {
            //Dodge 애니메이션 재생 중이면 입력 무시
            return;
        }
    }

    //Clear_State();
    if (Has_CycleState(CYC::CYCLE_END))
        Remove_State(CAT::M_MOVE);

    AllClear_CycleState();


    if (Has_Status(BACK_DODGE))
    {
        Remove_Status(BACK_DODGE);
        m_eDir.Delete_Flag(DIR::B);
    }


    /* 방향이 들어오면 Move On */
    if (m_eDir.iDirFlag) {
        Add_State(CAT::M_MOVE);
        if (isPrevMove)    Add_CycleState(CYC::CYCLE_LOOP);
        else Add_CycleState(CYC::CYCLE_START);
    }
    else {
        if (isPrevMove) {
            Add_CycleState(CYC::CYCLE_END);
        }
    }


    if (Has_State(CAT::M_MOVE))
    {
        Clear_SubState();
        _bool isSpaceHandled = false;

        /*  Sprint , Dodge */
        if (m_pGameInstance->Key_Down(DIK_SPACE))
        {
            /* 닷지 중복 방지*/
            if (isPrevDodge)
                return;

            m_fSprintTime = 0.f;
            Add_Status(CHARGING_SPRINT);
            Add_SubState(MOV::MOVE_DODGE);
            /* 백 닷지라면 */
            if (m_eDir.Check_Flag(DIR::B))
            {
                Add_CycleState(CYC::CYCLE_END);
                Add_Status(BACK_DODGE);
            }
            isSpaceHandled = true;

        }
        /* 스페이스 떼고 방향키를 누르고 있다는 예약이 걸려있을 때 다시 스페이스를 누를 경우*/
        else if (Has_State(SPRINT_AGAIN_REQUEST) && m_pGameInstance->Key_Pressing(DIK_SPACE, fTimeDelta))
        {
            Add_SubState(MOV::MOVE_SPRINT);
            Remove_SubState(MOV::MOVE_RUN);
            Remove_Status(SPRINT_AGAIN_REQUEST);
            isSpaceHandled = true;
        }
        else if (m_pGameInstance->Key_Pressing(DIK_SPACE, fTimeDelta, INPUT_TYPE::GAMEPLAY, &m_fSprintTime))
        {
            if (Has_Status(CHARGING_SPRINT))
            {

                // Dodge 애니메이션 진행도 체크
                _float trackPos = *m_pBody->Get_Model()->Get_CurTrackPosition();
                _float duration = m_pBody->Get_Model()->Get_CurDuration();
                if (m_pBody->Get_Model()->Check_MinAnimationTime() || (trackPos / duration) >= 0.3f)
                {
                    Add_SubState(MOV::MOVE_SPRINT);
                    Remove_Status(CHARGING_SPRINT);  // Sprint 전환 완료
                }
                else
                {
                    Add_SubState(MOV::MOVE_DODGE);  // 아직 Dodge 유지
                    isSpaceHandled = true;
                }
            }
            else
            {
                // 이미 Sprint 상태 - 유지
                Add_SubState(MOV::MOVE_SPRINT);
                isSpaceHandled = true;
            }
        }
        // Space를 뗌
        else if (m_pGameInstance->Key_Up(DIK_SPACE))
        {
            m_fSprintTime = 0.f;
            Remove_Status(CHARGING_SPRINT | SPRINT_AGAIN_REQUEST);

            if (m_eDir.iDirFlag > 0)
            {
                Remove_SubState(MOV::MOVE_SPRINT);
                Add_SubState(MOV::MOVE_RUN);
                Add_Status(SPRINT_AGAIN_REQUEST);
            }
            // Sprint 종료 신호 (다음 프레임에 END 애니메이션 재생)
            if (Has_SubState(MOV::MOVE_SPRINT))
            {
                Add_CycleState(CYC::CYCLE_END);
                Add_SubState(MOV::MOVE_SPRINT);
                isSpaceHandled = true;
            }
        }

        /* Walk,  Run */
        if (!isSpaceHandled && !Has_State(CAT::M_ATTACK | CAT::M_SKILL))
        {
            if (m_pGameInstance->Key_Pressing(DIK_LALT, fTimeDelta))  Add_SubState(MOV::MOVE_WALK);
            else
                Add_SubState(MOV::MOVE_RUN);

        }

    }
    else
    {
        // 뒤로 회피
        if (m_pGameInstance->Key_Down(DIK_SPACE))
        {
            /* 닷지 중복 방지*/
            if (isPrevDodge)
                return;

            Add_State(CAT::M_MOVE);
            Add_SubState(MOV::MOVE_DODGE);
            Add_CycleState(CYC::CYCLE_END);
            m_eDir.Clear_Flag();
            m_eDir.Add_Flag(DIR::B);  // 뒤로 회피
            m_fSprintTime = 0.f;
            Add_Status(BACK_DODGE);
            Remove_Status(SPRINT_AGAIN_REQUEST | CHARGING_SPRINT);
        }
        else
        {
            // 이동하지 않을 때는 SubState 초기화
            Clear_SubState();
            m_fSprintTime = 0.f;
            Remove_Status(CHARGING_SPRINT);
        }
    }

}

_bool CKhazan_GSword::Skill_Input(_float fTimeDelta)
{
    //// 스킬이 끝났는지 체크
    //if (Has_State(CAT::M_SKILL) && m_pAnimAttack && !m_pAnimAttack->Is_Skilling())
    //{
    //    m_pClientInstance->Set_UsedSkill(m_iCurSkillIndex, false);
    //    Clear_Step1();

    //    return false;
    //}

    ///* 투지, 스태미나  스탯 확인 */
    //if (m_pPlayerData->fCulDoggedness < 1.f && m_pPlayerData->fCulStamina < m_pPlayerData->fUsedStamina)
    //    return false;

    //if (m_pGameInstance->Key_Down(DIK_Q))
    //{
    //    m_iCurSkillIndex = m_pClientInstance->Get_ButtonSkill(CONTROL_BUTTON::Q);
    //    if (m_iCurSkillIndex == 0) return false;

    //    if (!m_pAnimAttack->Try_SkillAttack(m_iCurSkillIndex))
    //        m_pAnimAttack->Reserve_SkillAttack(m_iCurSkillIndex);
    //    Add_State(CAT::M_SKILL);
    //    return true;

    //}
    //if (m_pGameInstance->Key_Down(DIK_E))
    //{
    //    m_iCurSkillIndex = m_pClientInstance->Get_ButtonSkill(CONTROL_BUTTON::E);
    //    if (m_iCurSkillIndex == 0) return false;

    //    if (!m_pAnimAttack->Try_SkillAttack(m_iCurSkillIndex))
    //        m_pAnimAttack->Reserve_SkillAttack(m_iCurSkillIndex);
    //    Add_State(CAT::M_SKILL);
    //    return true;

    //}
    //if (m_pGameInstance->Key_Down(DIK_R))
    //{
    //    m_iCurSkillIndex = m_pClientInstance->Get_ButtonSkill(CONTROL_BUTTON::R);
    //    if (m_iCurSkillIndex == 0) return false;

    //    if (!m_pAnimAttack->Try_SkillAttack(m_iCurSkillIndex))
    //        m_pAnimAttack->Reserve_SkillAttack(m_iCurSkillIndex);
    //    Add_State(CAT::M_SKILL);
    //    return true;
    //}
    //if (m_pGameInstance->Key_Pressing(DIK_LCONTROL, fTimeDelta) && m_pGameInstance->Key_Down(DIK_F))
    //{
    //    m_iCurSkillIndex = m_pClientInstance->Get_ButtonSkill(CONTROL_BUTTON::CTRL_F);
    //    if (m_iCurSkillIndex == 0) return false;

    //    if (!m_pAnimAttack->Try_SkillAttack(m_iCurSkillIndex))
    //        m_pAnimAttack->Reserve_SkillAttack(m_iCurSkillIndex);
    //    Add_State(CAT::M_SKILL);
    //    return true;
    //}
    //if (m_pGameInstance->Key_Pressing(DIK_LCONTROL, fTimeDelta) && m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::LB))
    //{
    //    m_iCurSkillIndex = m_pClientInstance->Get_ButtonSkill(CONTROL_BUTTON::CTRL_LB);
    //    if (m_iCurSkillIndex == 0) return false;

    //    if (!m_pAnimAttack->Try_SkillAttack(m_iCurSkillIndex))
    //        m_pAnimAttack->Reserve_SkillAttack(m_iCurSkillIndex);
    //    Add_State(CAT::M_SKILL);
    //    return true;
    //}
    //if (m_pGameInstance->Key_Pressing(DIK_LCONTROL, fTimeDelta) && m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::RB))
    //{
    //    m_iCurSkillIndex = m_pClientInstance->Get_ButtonSkill(CONTROL_BUTTON::CTRL_RB);
    //    if (m_iCurSkillIndex == 0) return false;

    //    if (!m_pAnimAttack->Try_SkillAttack(m_iCurSkillIndex))
    //        m_pAnimAttack->Reserve_SkillAttack(m_iCurSkillIndex);
    //    Add_State(CAT::M_SKILL);
    //    return true;
    //}

    return false;
}

_bool CKhazan_GSword::Attack_Input(_float fTimeDelta)
{

    ///* 예약중인 공격 대기가 있으면 true */
    //if (Has_State(CAT::M_ATTACK) && m_pAnimAttack->Is_Reserve())
    //    return true;

    //// 공격이 끝났는지 체크
    //if (Has_State(CAT::M_ATTACK) && m_pAnimAttack && !m_pAnimAttack->Is_Attacking())
    //{
    //    Clear_Step0();
    //    return false;
    //}

    //if (Has_State(CAT::M_ATTACK) && m_pAnimAttack && !m_pAnimAttack->Can_NextCombo() && !Has_Status(CHARGING_STRONG_ATTACK))
    //    return false;

    ///* 스태미나 확인 */
    //if (m_pPlayerData->fCulStamina < m_pPlayerData->fUsedStamina) {
    //    Clear_Step0();
    //    return false;
    //}

    //_bool isAttack = { false };



    ///*  브루탈 공격.*/
    //if (Has_Status(BRUTAL_READY) && m_pGameInstance->Key_Down(DIK_F))
    //{
    //    if (m_pAnimAttack->Try_GrappleAttack())
    //    {
    //        Clear_Step0();
    //        Add_State(CAT::M_ATTACK);
    //        Add_SubState(ATT::ATK_GRAPPLE);
    //        Add_Status(BRUTAL_SUCCESS);
    //    }
    //}


    ///* dodge 공격 */
    //else if ((m_iPrevMainState & CAT::M_MOVE) && (m_iPrevSubState & MOV::MOVE_DODGE) && m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::LB))
    //{
    //    if (m_pAnimAttack->Try_DodgeAttack(m_ePrevDir))
    //    {
    //        Clear_Step0();
    //        Add_State(CAT::M_ATTACK);
    //        Add_SubState(ATT::ATK_DODGEATK);
    //        // Remove_State(CAT::M_MOVE);
    //        m_eHitReaction = ENUM_CLASS(HITREACTION::KNOCKBACK_WEAK);

    //        return true;
    //    }
    //}

    ///* Sprint 공격  */
    //else if ((m_iPrevMainState & CAT::M_MOVE) && (m_iPrevSubState & MOV::MOVE_SPRINT))
    //{
    //    /* 빠른 공격*/
    //    if (m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::LB))
    //    {
    //        if (m_pAnimAttack->Try_SprintFastAttack())
    //        {
    //            Clear_Step0();

    //            Add_State(CAT::M_ATTACK);
    //            Add_SubState(ATT::ATK_SPRINTATK);

    //            m_eHitReaction = ENUM_CLASS(HITREACTION::KNOCKBACK_WEAK);

    //            return true;
    //        }
    //    }
    //    /* 강한 공격 */
    //    else if (m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::RB))
    //    {
    //        if (m_pAnimAttack->Try_SprintStrongAttack())
    //        {
    //            Clear_Step0();

    //            Add_State(CAT::M_ATTACK);
    //            Add_SubState(ATT::ATK_SPRINTATK);

    //            if (m_pAnimAttack->Get_CurrentCombo() >= 2)
    //                m_eHitReaction = ENUM_CLASS(HITREACTION::KNOCKBACK_STRONG);
    //            else
    //                m_eHitReaction = ENUM_CLASS(HITREACTION::KNOCKBACK_WEAK);


    //            return true;
    //        }
    //    }
    //}

    ///* 스킬 : 강습  (빠른 공격 2단계까지만 가능)*/
    //else if (Has_Status(READY_ASSAULT)
    //    && m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::RB)
    //    && 0 < m_pAnimAttack->Get_CurrentCombo()
    //    && m_pAnimAttack->Get_CurrentCombo() < 2
    //    && m_pAnimAttack->Is_FastAttacking())
    //{
    //    Clear_Step0();
    //    Add_State(CAT::M_ATTACK);
    //    Add_SubState(SKI::ASSAULT);
    //    cout << "READY_ASSAULT" << endl;

    //    if (m_pAnimAttack->Try_SkillAttack(SKI::ASSAULT))
    //    {
    //        m_eHitReaction = ENUM_CLASS(HITREACTION::KNOCKBACK_STRONG);
    //        return true;
    //    }
    //    else
    //    {
    //        m_pAnimAttack->Reserve_SkillAttack(SKI::ASSAULT);
    //        m_eHitReaction = ENUM_CLASS(HITREACTION::KNOCKBACK_STRONG);
    //        return true;
    //    }

    //}

    ///* 빠른 공격 3연타 + 스킬 배우면 3타 바뀜 */
    //else if (m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::LB))
    //{
    //    if (m_pAnimAttack->Try_FastAttack())
    //    {
    //        // Move 상태 완전 제거
    //        Remove_State(CAT::M_MOVE);
    //        Clear_SubState();
    //        AllClear_CycleState();
    //        Remove_Status(CHARGING_SPRINT | SPRINT_AGAIN_REQUEST);

    //        Add_State(CAT::M_ATTACK);
    //        Add_SubState(ATT::ATK_FAST);
    //        Add_Status(READY_ASSAULT);


    //        m_eHitReaction = ENUM_CLASS(HITREACTION::KNOCKBACK_WEAK);

    //        return true;
    //    }
    //}

    ///* 강한 공격 3연타  차징 */
    //else if (m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::RB))
    //{
    //    m_fChargingStrongTime = 0.f;
    //    Remove_State(CAT::M_MOVE);
    //    Remove_Status(CHARGING_STRONG_ATTACK);
    //    Remove_Status(READY_ASSAULT);
    //}
    //else if (m_pGameInstance->Mouse_Pressing(MOUSEKEYSTATE::RB))
    //{
    //    if (!Has_Status(CHARGING_STRONG_ATTACK))
    //    {
    //        m_fChargingStrongTime += fTimeDelta;

    //        if (m_fChargingStrongTime >= m_fChargingStrongIntervalTime)
    //        {

    //            if (m_pAnimAttack->Try_ChageStrongAttack()) {
    //                Remove_State(CAT::M_MOVE);
    //                AllClear_CycleState();
    //                Clear_SubState();
    //                Remove_Status(CHARGING_SPRINT | SPRINT_AGAIN_REQUEST);

    //                Add_Status(CHARGING_STRONG_ATTACK);
    //                Add_SubState(ATT::ATK_CHARGE);
    //                Add_State(CAT::M_ATTACK);
    //                Remove_Status(CHARGING_SPRINT | SPRINT_AGAIN_REQUEST);

    //                m_eHitReaction = ENUM_CLASS(HITREACTION::KNOCKBACK_STRONG);
    //                return true;
    //            }
    //        }
    //    }

    //}
    //else if (!Has_Status(CHARGING_STRONG_ATTACK) && m_pGameInstance->Mouse_Up(MOUSEKEYSTATE::RB))
    //{

    //    _bool wasCharging = Has_Status(CHARGING_STRONG_ATTACK);
    //    Remove_Status(CHARGING_STRONG_ATTACK);

    //    // 차징 안했으면 일반 강공격
    //    if (!wasCharging && m_fChargingStrongTime < m_fChargingStrongIntervalTime)
    //    {
    //        if (m_pAnimAttack->Try_StrongAttack())
    //        {
    //            Remove_State(CAT::M_MOVE);
    //            Clear_SubState();
    //            AllClear_CycleState();
    //            Remove_Status(CHARGING_SPRINT | SPRINT_AGAIN_REQUEST);

    //            Add_SubState(ATT::ATK_STRONG);
    //            Add_State(CAT::M_ATTACK);

    //            m_eHitReaction = ENUM_CLASS(HITREACTION::KNOCKBACK_STRONG);
    //            return true;
    //        }
    //    }
    //    else if (wasCharging)
    //    {
    //        // 공격 상태 해제
    //        if (m_pAnimAttack)
    //        {
    //            m_pAnimAttack->Exit();
    //        }
    //        Remove_State(CAT::M_ATTACK);
    //        Clear_SubState();
    //    }
    //}

    return false;
}

_bool CKhazan_GSword::Guard_Input(_float fTimeDelta)
{

    /* 가드 종료*/
    if (/*Has_State(CAT::M_GUARD) && */m_pGameInstance->Key_Up(DIK_LSHIFT))
    {
        m_pAnimGuard->Play_FinishGuard();
        Clear_Step1();

        return true;
    }

    if (/*!Has_State(CAT::M_GUARD) &&*/ m_pGameInstance->Key_Down(DIK_LSHIFT))
    {
        /* 그냥 가드 */
        if (m_pAnimGuard->Try_Guard())
        {
            Clear_Step3();
            Add_State(CAT::M_GUARD);
            return true;
        }

    }

    //   /* 가드 성공  */
       //else if (Has_State(CAT::M_GUARD)&&) //todo 조건 주기
       //{
       //	if (m_pAnimGuard->Try_SuccessGuard(HITDIR))
       //	{
       //		return true;
       //	}
       //}

       /* 가드중 이동 - 방향 입력이 있을 때 */
    if (Has_State(CAT::M_GUARD) && m_eDir.iDirFlag > 0)
    {
        // 방향이 바뀌었거나, 워킹가드가 아닐 때
        if (!m_pAnimGuard->Is_WalkGuarding() || m_eDir.iDirFlag != m_ePrevDir)
        {
            if (m_pAnimGuard->Try_WalkGuard(m_eDir.iDirFlag))
            {
                return true;
            }
        }
    }
    /* 워킹가드중에 방향키 떼면  */
    else if (Has_State(CAT::M_GUARD) && m_eDir.iDirFlag == 0 && m_pAnimGuard->Is_WalkGuarding())
    {
        //Remove_State(CAT::M_MOVE );
        //Remove_SubState(MOV::MOVE_WALK);
        m_pAnimGuard->Try_Guard();  // 정지 가드로 전환
        return true;
    }




    return false;
}

_bool CKhazan_GSword::Interaction_Input(_float fTimeDelta)
{
    //랜턴
    if (m_pGameInstance->Key_Down(DIK_2)) {
        _bool isEquip = !m_pLantern->Get_Equipped();
        if (m_pAnimInteraction->Try_Lantern(isEquip))
            m_pLantern->Set_Equipped(isEquip);
    }

    //라크리마 
    if (m_pGameInstance->Key_Down(DIK_1)) {
        if (m_pPlayerData->fCulHp < m_pPlayerData->fMaxHp)
            m_pAnimInteraction->Try_Lachryma();
    }

    return false;
}

void CKhazan_GSword::Change_MoveIdle(_float fTimeDelt)
{
    // 낙하 중에는 애니메이션 변경 금지
    if (Has_Status(FALLING | FALLING_ATTACK | PRE_LAND))
        return;

    /* 닷지하기 */
    if (Has_State(CAT::M_MOVE) && Has_SubState(MOV::MOVE_DODGE)/* && m_pBody->Is_SpearFullExtension()*/) // ZZZZZZZ
    {
        /* 닷지 : 스태미나 소모*/
        if (m_pPlayerData->fCulStamina >= m_pPlayerData->fUsedStamina)
        {
            CKhazan_GS_Anim_Move::GS_MOVEINFO info;

            if (Has_Status(BAREHAND))info.iWeapon = BAREHAND;
            else if (Has_Status(SPEAR))info.iWeapon = SPEAR;
            else if (Has_Status(GSWORD))info.iWeapon = GSWORD;

            info.isLockOn = Has_Status(LOCKON);
            info.iState = m_iCurSubState;
            info.iCycle = m_iCycle;
            info.eDir = m_eDir;

            m_pAnimMove->Try_ChangeAnimation(info);

            m_pPlayerData->fCulStamina -= m_pPlayerData->fUsedStamina;
        }
        else
        {
            Remove_SubState(MOV::MOVE_DODGE);
            Add_SubState(MOV::MOVE_RUN);
        }
    }

    // 공격 중일 때는 Move 애니메이션 변경 금지
    if (Has_State(CAT::M_ATTACK) && m_pAnimAttack->Is_Attacking())
        return;

    // Guard 중일 때도 체크
    if (Has_State(CAT::M_GUARD) && m_pAnimGuard->Is_Guarding())
        return;

    /*  락온 체크 + 백 닷지 + 닷지가 아니면 */
    if (Has_Status(LOCKON) && !Has_Status(BACK_DODGE) && !Has_SubState(MOV::MOVE_DODGE))
        m_eDir = Calculate_LockOnDirection(fTimeDelt);

    /* 이동중 스페이스바 누르는것 때문에 다시 요청하기  */
    if (Has_Status(SPRINT_AGAIN_REQUEST))
    {
        CKhazan_GS_Anim_Move::GS_MOVEINFO info;
        if (Has_Status(BAREHAND))info.iWeapon = BAREHAND;
        else if (Has_Status(SPEAR))info.iWeapon = SPEAR;
        else if (Has_Status(GSWORD))info.iWeapon = GSWORD;
        info.isLockOn = Has_Status(LOCKON);
        info.iState = m_iCurSubState;
        info.iCycle = m_iCycle;
        info.eDir = m_eDir;
        m_pAnimMove->Reserve_Animation(info);
        m_pAnimMove->Try_ChangeAnimation(info);

        Remove_Status(SPRINT_AGAIN_REQUEST);
    }

    /* 락온상태에서 닷지중일 때는 방향 변경 건너뚜ㅣ기*/
    if (Has_Status(LOCKON) && m_eDir.iDirFlag != m_ePrevDir && !Has_SubState(MOV::MOVE_DODGE))
    {

        CKhazan_GS_Anim_Move::GS_MOVEINFO info;
        if (Has_Status(BAREHAND))info.iWeapon = BAREHAND;
        else if (Has_Status(SPEAR))info.iWeapon = SPEAR;
        else if (Has_Status(GSWORD))info.iWeapon = GSWORD;
        info.isLockOn = Has_Status(LOCKON);
        info.iState = m_iCurSubState;
        info.iCycle = m_iCycle;
        info.eDir = m_eDir;
        m_pAnimMove->Try_ChangeAnimation(info);
        return;
    }


    if (m_iCurMainState == m_iPrevMainState
        && m_iCurSubState == m_iPrevSubState
        && m_iCycle == m_iPrevCycle)
    {
        return;
    }

    /* Move  */
    if (((Has_Status(LOCKON) && m_eDir.iDirFlag != m_ePrevDir && m_eDir.iDirFlag > 0)) || Has_State(CAT::M_MOVE) && !Has_State(CAT::M_ATTACK | CAT::M_GUARD))
    {
        CKhazan_GS_Anim_Move::GS_MOVEINFO info;
        info.iWeapon = Has_Status(WEA::GSWORD);
        info.isLockOn = Has_Status(LOCKON);
        info.iState = m_iCurSubState;
        info.iCycle = m_iCycle;
        info.eDir = m_eDir;
        m_pAnimMove->Try_ChangeAnimation(info);

        //Remove_State(CAT::M_IDLE);
    }

    /* Idle */
    else if (!Has_State(CAT::M_END - 2))
    {
        _uint iCurAnimIndex = m_pBody->Get_Model()->Get_CurAnimIndex();
        if (m_pBody->Get_Model()->Check_MinAnimationTime() && iCurAnimIndex != m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_GSword_Stand") && iCurAnimIndex != m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_BareHands_Stand"))
            m_pBody->Get_Model()->Set_Animation(Has_Status(GSWORD) ? m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_GSword_Stand") : m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_BareHands_Stand"));
    }


}

void CKhazan_GSword::ExecuteAnimationExit()
{
    //if(m_iPrevMainState & CAT::M_DIE)
    if ((m_iCurMainState != m_iPrevMainState) && m_iPrevMainState & CAT::M_FALL) m_pAnimFall->Exit();
    //if(m_iPrevMainState &   CAT::M_GROGGY           )
    if ((m_iCurMainState != m_iPrevMainState) && m_iPrevMainState & CAT::M_DAMAGED) m_pAnimDamaged->Exit();
    //if(m_iPrevMainState &   CAT::M_CLIMB            )
    if ((m_iCurMainState != m_iPrevMainState) && m_iPrevMainState & CAT::M_SKILL) m_pAnimAttack->Exit();
    if ((m_iCurMainState != m_iPrevMainState) && m_iPrevMainState & CAT::M_GUARD) m_pAnimGuard->Exit();
    if ((m_iCurMainState != m_iPrevMainState) && m_iPrevMainState & CAT::M_ATTACK) {
        m_pAnimAttack->Exit();
        m_pBody->Event_AttackTiming(false);
    }
    if ((m_iCurMainState != m_iPrevMainState) && m_iPrevMainState & CAT::M_MOVE) m_pAnimMove->Exit();
    //if(m_iPrevMainState &   CAT::M_LOCKON           )
    //if(m_iPrevMainState &   CAT::M_INTERACT         )
    //if(m_iPrevMainState &   CAT::M_WEAPON_CHANGE    )
    //if(m_iPrevMainState &   CAT::M_IDLE             )
    //if(m_iPrevMainState &   CAT::M_END              )

}

void CKhazan_GSword::Apply_PlayerMovement(_float fTimeDelta)
{
    // 낙하 중에는 이동 금지
    if (Has_Status(FALLING | FALLING_ATTACK | PRE_LAND))
        return;

    // 공격 중일 때는 이동하지 않음
    if (m_pAnimAttack->Is_Attacking()) {
        //스프린트 체크 강화
        if (Has_SubState(ATT::ATK_SPRINTATK))
            return;

        //애니메이션 최소보장시간
        if (!m_pBody->Get_Model()->Check_MinAnimationTime())
            return;
    }
    /* 닷지 일때 이동하지 않음*/
    if (Has_State(CAT::M_MOVE) && Has_SubState(MOV::MOVE_DODGE))
        return;
    for (size_t i = 0; i < 9; i++)
        if (m_iStopMoveIndexTable[i] == m_pBody->Get_Model()->Get_CurAnimIndex()) {
            Remove_State(CAT::M_MOVE);
            return;
        }
    _float4x4 CamWorldMatrix = *m_pGameInstance->Get_Transform_Float4x4_Inverse(D3DTS::VIEW);
    _vector vCamLook = XMLoadFloat3((_float3*)&CamWorldMatrix._31);
    _vector vRight = XMVector3Normalize(XMVectorSetW(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vCamLook), 0.f));
    _vector vLook = XMVector3Normalize(XMVectorSetW(XMVector3Cross(vRight, XMVectorSet(0.f, 1.f, 0.f, 0.f)), 0.f));

    _vector vPlayerPosition = m_pTransformCom->Get_State(STATE::POSITION);

    /* 속도 설정 */
    _float fSpeed = 0.f;
    if (m_pAnimGuard->Is_WalkGuarding())fSpeed = m_fWalkSpeed;
    else if (Has_SubState(MOV::MOVE_SPRINT)) fSpeed = m_fSprintSpeed;
    else if (Has_SubState(MOV::MOVE_WALK)) fSpeed = m_fWalkSpeed;
    else if (Has_SubState(MOV::MOVE_RUN)) fSpeed = m_fRunSpeed;
    else if (Has_SubState(MOV::MOVE_INJURED)) fSpeed = m_fInjuredSpeed;

    /*  카메라 기준 이동 방향 벡터 계산  */
    _vector vMoveDirection = XMVectorSet(0.f, 0.f, 0.f, 0.f);
    _bool isMoving = false;

    // m_eWorldDir 사용 (카메라 기준 입력)
    if (m_eWorldDir.Check_Flag(DIR::F))
    {
        vMoveDirection += vLook;
        isMoving = true;
    }
    if (m_eWorldDir.Check_Flag(DIR::B))
    {
        vMoveDirection -= vLook;
        isMoving = true;
    }
    if (m_eWorldDir.Check_Flag(DIR::L))
    {
        vMoveDirection -= vRight;
        isMoving = true;
    }
    if (m_eWorldDir.Check_Flag(DIR::R))
    {
        vMoveDirection += vRight;
        isMoving = true;
    }

    /*  이동 적용*/
    if (isMoving)
    {
        vMoveDirection = XMVector3Normalize(vMoveDirection);
        vPlayerPosition += vMoveDirection * fSpeed * fTimeDelta;
        m_pTransformCom->Set_State(STATE::POSITION, vPlayerPosition);
    }

    // 뒤 방향 닷지인지 체크 (B, BR, BL 모두 포함)
    _bool isBackwardDodge = Has_Status(BACK_DODGE) || (m_eDir.Check_Flag(DIR::B) && Has_SubState(MOV::MOVE_DODGE));

    // 회전 처리
    if (!isBackwardDodge) {
        if (!Has_SubState(MOV::MOVE_SPRINT) && Has_Status(LOCKON) && m_pCamera && m_pCamera->Get_IsLockOn())
        {
            LockOn_Rotation(fTimeDelta);
        }
        else
        {
            /* 락온이 아니면 평상시대로 */
            if (Has_State(CAT::M_MOVE) && isMoving)
            {
                _vector vPlayerLook = XMVector3Normalize(m_pTransformCom->Get_State(STATE::LOOK));
                // 타겟 룩 = 이동 방향 (카메라 기준!!!)
                _vector vTargetLook = vMoveDirection;

                _float fDotProduct = XMVectorGetX(XMVector3Dot(vPlayerLook, vTargetLook));

                // 회전 시작 조건 (약 5도 이상 차이)
                if (!Has_Status(ROTATION) && fDotProduct < 0.996f)
                {
                    Add_Status(ROTATION);
                    m_fRotateTime[0] = 0.f;
                    m_vRotateStart = vPlayerLook;
                }

                if (Has_Status(ROTATION))
                {
                    m_fRotateTime[0] += fTimeDelta;
                    _float t = min(m_fRotateTime[0] / m_fRotateTime[1], 1.0f);

                    if (t >= 1.0f)
                    {
                        // 회전 완료
                        _float yaw = atan2f(XMVectorGetX(vTargetLook), XMVectorGetZ(vTargetLook));
                        _vector q = XMQuaternionRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), yaw);
                        m_pTransformCom->Set_Quaternion(q);
                        Remove_Status(ROTATION);
                    }
                    else
                    {
                        // 회전 중 - Slerp 보간
                        _vector vInterpolated = XMVector3Normalize(XMVectorLerp(m_vRotateStart, vTargetLook, t));
                        _float yaw = atan2f(XMVectorGetX(vInterpolated), XMVectorGetZ(vInterpolated));
                        _vector q = XMQuaternionRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), yaw);
                        m_pTransformCom->Set_Quaternion(q);
                    }
                }
                else
                {
                    // 각도 차이가 작을 때 즉시 회전
                    _float yaw = atan2f(XMVectorGetX(vTargetLook), XMVectorGetZ(vTargetLook));
                    _vector q = XMQuaternionRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), yaw);
                    m_pTransformCom->Set_Quaternion(q);
                }
            }
        }
    }
}



void CKhazan_GSword::Check_KeyInput_Direction(_float fTimeDelta)
{
    // 카메라 방향 계산
    _float4x4 CamWorldMatrix = *m_pGameInstance->Get_Transform_Float4x4_Inverse(D3DTS::VIEW);
    _vector vCamLook = XMLoadFloat3((_float3*)&CamWorldMatrix._31);
    vCamLook = XMVector3Normalize(XMVectorSetW(vCamLook, 0.f));

    _vector vPlayerLook = XMVector3Normalize(m_pTransformCom->Get_State(STATE::LOOK));
    _vector vCamRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vCamLook));

    _float dotLook = XMVectorGetX(XMVector3Dot(vPlayerLook, vCamLook));
    _float dotRight = XMVectorGetX(XMVector3Dot(vPlayerLook, vCamRight));

    PLAYER_CAMERA_DIR playerCamDir = PC_FRONT;
    _float angle = atan2f(dotRight, dotLook);

    if (angle >= -XM_PI / 8.f && angle < XM_PI / 8.f)                      playerCamDir = PC_FRONT;
    else if (angle >= XM_PI / 8.f && angle < 3.f * XM_PI / 8.f)           playerCamDir = PC_FRONT_RIGHT;
    else if (angle >= 3.f * XM_PI / 8.f && angle < 5.f * XM_PI / 8.f)     playerCamDir = PC_RIGHT;
    else if (angle >= 5.f * XM_PI / 8.f && angle < 7.f * XM_PI / 8.f)     playerCamDir = PC_BACK_RIGHT;
    else if (angle >= 7.f * XM_PI / 8.f || angle < -7.f * XM_PI / 8.f)    playerCamDir = PC_BACK;
    else if (angle >= -7.f * XM_PI / 8.f && angle < -5.f * XM_PI / 8.f)   playerCamDir = PC_BACK_LEFT;
    else if (angle >= -5.f * XM_PI / 8.f && angle < -3.f * XM_PI / 8.f)   playerCamDir = PC_LEFT;
    else if (angle >= -3.f * XM_PI / 8.f && angle < -XM_PI / 8.f)         playerCamDir = PC_FRONT_LEFT;

    // 키 입력
    _bool isW = m_pGameInstance->Key_Pressing(DIK_W, fTimeDelta);
    _bool isS = m_pGameInstance->Key_Pressing(DIK_S, fTimeDelta);
    _bool isA = m_pGameInstance->Key_Pressing(DIK_A, fTimeDelta);
    _bool isD = m_pGameInstance->Key_Pressing(DIK_D, fTimeDelta);

    // 카메라 기준 월드 방향 (이동/회전용)
    m_eWorldDir.Clear_Flag();
    if (isW && !isS && !isA && !isD)      m_eWorldDir.Add_Flag(DIR::F);
    else if (!isW && isS && !isA && !isD) m_eWorldDir.Add_Flag(DIR::B);
    else if (!isW && !isS && isA && !isD) m_eWorldDir.Add_Flag(DIR::L);
    else if (!isW && !isS && !isA && isD) m_eWorldDir.Add_Flag(DIR::R);
    else if (isW && !isS && isA && !isD)  m_eWorldDir.Add_Flag(DIR::F | DIR::L);
    else if (isW && !isS && !isA && isD)  m_eWorldDir.Add_Flag(DIR::F | DIR::R);
    else if (!isW && isS && isA && !isD)  m_eWorldDir.Add_Flag(DIR::B | DIR::L);
    else if (!isW && isS && !isA && isD)  m_eWorldDir.Add_Flag(DIR::B | DIR::R);

    // 플레이어 로컬 방향 (애니메이션 선택용)
    m_eDir.Clear_Flag();
    if (m_eWorldDir.Check_Flag(DIR::F | DIR::B | DIR::L | DIR::R))
    {
        m_eDir.Add_Flag(ConvertCameraToPlayerDir(playerCamDir));
    }
}

DIRECTION_INFO CKhazan_GSword::Calculate_LockOnDirection(_float fTimeDelta)
{
    DIRECTION_INFO lockOnDir;
    lockOnDir.Clear_Flag();

    if (!m_pCamera || !m_pCamera->Get_IsLockOn())
        return lockOnDir;

    // 락온 타겟 위치 가져오기
    _float4* pLockOnPos = m_pCamera->Get_LockOnPosition();
    if (!pLockOnPos)
        return lockOnDir;

    _vector vTargetPos = XMLoadFloat4(pLockOnPos);
    _vector vPlayerPos = m_pTransformCom->Get_State(STATE::POSITION);

    // 플레이어에서 타겟으로의 방향 (플레이어의 forward)
    _vector vToTarget = XMVector3Normalize(vTargetPos - vPlayerPos);
    vToTarget = XMVectorSetY(vToTarget, 0.f); // Y축 제거

    // 플레이어의 현재 Look 방향
    _vector vPlayerLook = XMVector3Normalize(m_pTransformCom->Get_State(STATE::LOOK));
    vPlayerLook = XMVectorSetY(vPlayerLook, 0.f);

    // 플레이어의 Right 방향
    _vector vPlayerRight = XMVector3Normalize(m_pTransformCom->Get_State(STATE::RIGHT));

    // 카메라 기준 이동 방향 계산
    _float4x4 CamWorldMatrix = *m_pGameInstance->Get_Transform_Float4x4_Inverse(D3DTS::VIEW);
    _vector vCamLook = XMLoadFloat3((_float3*)&CamWorldMatrix._31);
    _vector vCamRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vCamLook));
    _vector vCamForward = XMVector3Normalize(XMVector3Cross(vCamRight, XMVectorSet(0.f, 1.f, 0.f, 0.f)));

    // 입력 방향을 월드 공간에서의 이동 벡터로 변환
    _vector vMoveDir = XMVectorSet(0.f, 0.f, 0.f, 0.f);

    _bool isW = m_pGameInstance->Key_Pressing(DIK_W, fTimeDelta);
    _bool isS = m_pGameInstance->Key_Pressing(DIK_S, fTimeDelta);
    _bool isA = m_pGameInstance->Key_Pressing(DIK_A, fTimeDelta);
    _bool isD = m_pGameInstance->Key_Pressing(DIK_D, fTimeDelta);

    if (isW) vMoveDir += vCamForward;
    if (isS) vMoveDir -= vCamForward;
    if (isA) vMoveDir -= vCamRight;
    if (isD) vMoveDir += vCamRight;

    if (XMVectorGetX(XMVector3Length(vMoveDir)) < 0.01f)
        return lockOnDir;

    vMoveDir = XMVector3Normalize(vMoveDir);

    // 이동 방향을 타겟 기준 좌표계로 변환
    // Forward = 타겟 방향, Right = 타겟의 오른쪽
    _float fDotForward = XMVectorGetX(XMVector3Dot(vMoveDir, vToTarget));
    _float fDotRight = XMVectorGetX(XMVector3Dot(vMoveDir, XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vToTarget)));

    // 8방향 결정 (45도씩 구분)
    _float angle = atan2f(fDotRight, fDotForward);

    if (angle >= -XM_PI / 8.f && angle < XM_PI / 8.f)
        lockOnDir.Add_Flag(DIRECTION_INFO::F);
    else if (angle >= XM_PI / 8.f && angle < 3.f * XM_PI / 8.f)
        lockOnDir.Add_Flag(DIRECTION_INFO::F | DIRECTION_INFO::R);
    else if (angle >= 3.f * XM_PI / 8.f && angle < 5.f * XM_PI / 8.f)
        lockOnDir.Add_Flag(DIRECTION_INFO::R);
    else if (angle >= 5.f * XM_PI / 8.f && angle < 7.f * XM_PI / 8.f)
        lockOnDir.Add_Flag(DIRECTION_INFO::B | DIRECTION_INFO::R);
    else if (angle >= 7.f * XM_PI / 8.f || angle < -7.f * XM_PI / 8.f)
        lockOnDir.Add_Flag(DIRECTION_INFO::B);
    else if (angle >= -7.f * XM_PI / 8.f && angle < -5.f * XM_PI / 8.f)
        lockOnDir.Add_Flag(DIRECTION_INFO::B | DIRECTION_INFO::L);
    else if (angle >= -5.f * XM_PI / 8.f && angle < -3.f * XM_PI / 8.f)
        lockOnDir.Add_Flag(DIRECTION_INFO::L);
    else if (angle >= -3.f * XM_PI / 8.f && angle < -XM_PI / 8.f)
        lockOnDir.Add_Flag(DIRECTION_INFO::F | DIRECTION_INFO::L);

    return lockOnDir;
}

void CKhazan_GSword::LockOn_Rotation(_float fTimeDelta)
{
    // 락온 상태에서는 항상 타겟을 바라보게
    _float4* pLockOnPos = m_pCamera->Get_LockOnPosition();
    if (pLockOnPos)
    {
        _vector vTargetPos = XMLoadFloat4(pLockOnPos);
        _vector vPlayerPos = m_pTransformCom->Get_State(STATE::POSITION);

        _vector vToTarget = vTargetPos - vPlayerPos;
        vToTarget = XMVectorSetY(vToTarget, 0.f);
        vToTarget = XMVector3Normalize(vToTarget);

        // 부드러운 회전
        _vector vCurrentLook = XMVector3Normalize(m_pTransformCom->Get_State(STATE::LOOK));
        _vector vNewLook = XMVector3Normalize(XMVectorLerp(vCurrentLook, vToTarget, 8.f * fTimeDelta));

        _float yaw = atan2f(XMVectorGetX(vNewLook), XMVectorGetZ(vNewLook));
        _vector q = XMQuaternionRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), yaw);
        m_pTransformCom->Set_Quaternion(q);
    }
}
void CKhazan_GSword::Setting_Guard_Rotation()
{
    m_fGuardRotationTime.x = 0.f;

    _vector vCurrentLook = XMVector3Normalize(m_pTransformCom->Get_State(STATE::LOOK));
    vCurrentLook = XMVectorSetY(vCurrentLook, 0.f);
    m_vGuardRotationStart = vCurrentLook;

    m_vGuardRotationEnd = XMLoadFloat4(&m_vGuardRotationTarget);

    Remove_Status(GUARD_ROTATION_REQUEST);
    Add_Status(GUARD_ROTATION);

}
void CKhazan_GSword::Guard_Rotation(_float fTimeDelta)
{
    m_fGuardRotationTime.x += fTimeDelta;

    // Ease-Out Quadratic
    _float fRatio = m_fGuardRotationTime.x / m_fGuardRotationTime.y;
    fRatio = min(fRatio, 1.0f);
    fRatio = 1.f - (1.f - fRatio) * (1.f - fRatio);

    _vector vCurrentLook = XMVector3Normalize(
        XMVectorLerp(m_vGuardRotationStart, m_vGuardRotationEnd, fRatio)
    );

    // 회전 완료 체크
    if (fRatio >= 1.0f)
    {
        vCurrentLook = m_vGuardRotationEnd;
        Remove_Status(GUARD_ROTATION);
    }

    _float yaw = atan2f(XMVectorGetX(vCurrentLook), XMVectorGetZ(vCurrentLook));
    _vector q = XMQuaternionRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), yaw);
    m_pTransformCom->Set_Quaternion(q);
}
void CKhazan_GSword::Update_LockOn()
{
    // 카메라의 락온 상태와 동기화
    if (m_pCamera)
    {
        _bool   isLockOn = m_pCamera->Get_IsLockOn();
        if (isLockOn != Has_Status(LOCKON))
        {
            if (isLockOn) {
                Add_Status(LOCKON);
            }
            else {
                Remove_Status(LOCKON);
            }
        }
    }
}

void CKhazan_GSword::Update_Die(_float fTimeDelta)
{
    if (m_pBody->Get_Model()->Get_CurAnimIndex() == m_iCurAnimIndex && m_pBody->Get_Model()->IsFinished())
    {
        /* 한번만 들어감.  */
        m_iCurAnimIndex = Has_Status(GSWORD)
            ? m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_Spear_Com_Down_Loop_F")
            : m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_DownDie_F");

        m_pBody->Get_Model()->Set_Animation(m_iCurAnimIndex);
    }
}

void CKhazan_GSword::Clear_Injured()
{
    Clear_State();
    Clear_SubState();
    Clear_CycleState();

    Remove_Status(INJURED);
}


void CKhazan_GSword::Get_HitReaction(_float3 vContactPoint)
{
    // 1. 캐릭터의 위치 구하기
    _vector vCharacterPos = m_pTransformCom->Get_State(STATE::POSITION);

    // 2. 캐릭터 -> 접촉점 방향 벡터 계산
    _vector vHitDir = XMLoadFloat3(&vContactPoint) - vCharacterPos;

    // 3. 높이 차이 계산 (Y축)
    _float fHeightDiff = XMVectorGetY(vHitDir);

    // 4. XZ 평면 방향 계산
    _vector vHitDirXZ = XMVectorSetY(vHitDir, 0.f);

    _float fLengthSq = XMVectorGetX(XMVector3LengthSq(vHitDirXZ));
    if (fLengthSq < 0.0001f) // 거의 0인 경우
    {
        // 기본값: 캐릭터의 정면 방향을 사용
        vHitDirXZ = m_pTransformCom->Get_State(STATE::LOOK);
        vHitDirXZ = XMVectorSetY(vHitDirXZ, 0.f);
    }

    vHitDirXZ = XMVector3Normalize(vHitDirXZ);

    // 5. 캐릭터의 Forward와 Right 벡터 구하기
    _vector vCharacterForward = m_pTransformCom->Get_State(STATE::LOOK);
    vCharacterForward = XMVector3Normalize(vCharacterForward);

    _vector vCharacterRight = m_pTransformCom->Get_State(STATE::RIGHT);
    vCharacterRight = XMVector3Normalize(vCharacterRight);

    // 6. 내적으로 방향 계산
    _float fDotForward = XMVectorGetX(XMVector3Dot(vCharacterForward, vHitDirXZ));
    _float fDotRight = XMVectorGetX(XMVector3Dot(vCharacterRight, vHitDirXZ));

    // 7. 각도 계산
    _float fAngle = atan2f(fDotRight, fDotForward);
    fAngle = XMConvertToDegrees(fAngle);

    // 8. 높이 판단
    _float fHeightThreshold = 0.5f;
    bool bIsUp = (fHeightDiff > fHeightThreshold);


    m_eHitStrongDir.Clear_Flag();

    if (bIsUp) {
        m_eHitStrongDir.Add_Flag(DIRECTION_INFO::DIR::U);
        m_eHitStrongDir.Add_Flag(DIRECTION_INFO::DIR::F);
    }
    else {
        m_eHitStrongDir.Add_Flag(DIRECTION_INFO::DIR::D);

        if (fabs(fDotRight) > fabs(fDotForward) && fDotRight > 0.f) {
            m_eHitStrongDir.Add_Flag(DIRECTION_INFO::DIR::R);
            m_eHitStrongDir.Add_Flag(DIRECTION_INFO::DIR::F);
        }
        else {
            m_eHitStrongDir.Add_Flag(DIRECTION_INFO::DIR::F);
        }
    }


    m_eHitNormalDir.Clear_Flag();

    if (bIsUp) {
        m_eHitNormalDir.Add_Flag(DIRECTION_INFO::DIR::U);

        if (fDotForward < 0.f) {
            m_eHitNormalDir.Add_Flag(DIRECTION_INFO::DIR::B);
        }
        else {
            m_eHitNormalDir.Add_Flag(DIRECTION_INFO::DIR::F);
        }
    }
    else {
        m_eHitNormalDir.Add_Flag(DIRECTION_INFO::DIR::D);

        if (fAngle >= -22.5f && fAngle < 22.5f) {
            m_eHitNormalDir.Add_Flag(DIRECTION_INFO::DIR::F);
        }
        else if (fAngle >= 22.5f && fAngle < 157.5f) {
            m_eHitNormalDir.Add_Flag(DIRECTION_INFO::DIR::R);
            m_eHitNormalDir.Add_Flag(DIRECTION_INFO::DIR::F);
        }
        else if (fAngle >= 157.5f || fAngle < -157.5f) {
            m_eHitNormalDir.Add_Flag(DIRECTION_INFO::DIR::B);
        }
        else if (fAngle >= -157.5f && fAngle < -22.5f) {
            m_eHitNormalDir.Add_Flag(DIRECTION_INFO::DIR::L);
            m_eHitNormalDir.Add_Flag(DIRECTION_INFO::DIR::F);
        }
    }

}


void CKhazan_GSword::Check_IsInAir(_float fTimeDelta)
{
    if (m_pCharVirCom->Get_isGround())
        return;

    _vector vRayStart = m_pTransformCom->Get_State(STATE::POSITION);
    _vector vRayEnd = vRayStart + XMVectorSet(0.f, -m_fRayLength, 0.f, 0.f);

    _float fFraction;
    _float4 vPosition;
    _float3 outNormal;

    if (m_pGameInstance->RayCast(
        _float3(XMVectorGetX(vRayStart), XMVectorGetY(vRayStart), XMVectorGetZ(vRayStart)),
        _float3(XMVectorGetX(vRayEnd), XMVectorGetY(vRayEnd), XMVectorGetZ(vRayEnd)),
        fFraction,
        vPosition,
        &outNormal
    ))
    {
        /* 지면에 도달 - 착지 */
        if (fFraction <= 0.2f)
        {
            // 낙하 중이었다면 착지 완료
            if (Has_Status(FALLING | FALLING_ATTACK | PRE_LAND))
            {
                // PRE_LAND 상태가 아니면 바로 PRE_LAND로 전환
                if (!Has_Status(PRE_LAND))
                {
                    Remove_Status(FALLING);
                    Add_Status(PRE_LAND);
                    cout << " === LANDING !!! ===" << endl;
                }
            }
        }
        /* 착지 직전 - 착지 애니메이션 준비 */
        else if ((Has_Status(FALLING) || Has_Status(FALLING_ATTACK)) && !Has_Status(PRE_LAND) && fFraction <= 0.4f)
        {
            Add_Status(PRE_LAND);
            cout << " === PRE_LAND !!! ===" << endl;
        }
        /* 공중 - 낙하 시작 */
        else if (!Has_Status(FALLING | FALLING_ATTACK | PRE_LAND) && fFraction > 0.2f)
        {
            Add_Status(FALLING);
            Add_State(CAT::M_FALL);
            cout << " === FALLING START !!! ===" << endl;
        }


    }
}

HRESULT CKhazan_GSword::Ready_Components()
{
    return S_OK;
}

HRESULT CKhazan_GSword::Ready_AnimationStateMachine()
{

    m_pAnimMove = CKhazan_GS_Anim_Move::Create();
    if (m_pAnimMove == nullptr)
        return E_FAIL;
    m_pAnimMove->Set_Model(m_pBody->Get_Model());
    m_pAnimMove->Initialize();

    m_pAnimAttack = CKhazan_GS_Anim_Attack::Create();
    if (m_pAnimAttack == nullptr)
        return E_FAIL;
    m_pAnimAttack->Set_Model(m_pBody->Get_Model());

    m_pAnimGuard = CKhazan_GS_Anim_Guard::Create();
    if (m_pAnimGuard == nullptr)
        return E_FAIL;
    m_pAnimGuard->Set_Model(m_pBody->Get_Model());
    m_pBody->Set_IsGuarding(m_pAnimGuard->Get_IsGuarding());

    m_pAnimInteraction = CKhazan_GS_Anim_Interaction::Create();
    if (m_pAnimInteraction == nullptr)
        return E_FAIL;
    m_pAnimInteraction->Set_Model(m_pBody->Get_Model());

    m_pAnimDamaged = CKhazan_GS_Anim_Damaged::Create();
    if (m_pAnimDamaged == nullptr)
        return E_FAIL;
    m_pAnimDamaged->Set_Model(m_pBody->Get_Model());

    m_pAnimFall = CKhazan_GS_Anim_Fall::Create();
    if (m_pAnimFall == nullptr)
        return E_FAIL;
    m_pAnimFall->Set_Model(m_pBody->Get_Model());


    return S_OK;
}

void CKhazan_GSword::Clear_Step0()
{
    Clear_CycleState();
    Clear_SubState();
    Clear_State();
    Remove_Status(STATUS_CLEARS);

    m_eDir.iDirFlag = 0;
    m_eWorldDir.iDirFlag = 0;
    //m_ePrevDir = m_iPrevCycle = m_iPrevMainState = m_iPrevSubState = 0;
    m_fRotateTime[0] = 0.f;
    m_fSprintTime = 0.f;
    m_fChargingStrongTime = 0.f;
    m_iCurSkillIndex = 0;

    // m_f180TurnTime = { 0.f, 0.f };

    if (m_pAnimMove)
    {
        m_pAnimMove->Clear();
        // m_pAnimMove->Clear_Turn180();
    }

    if (m_pAnimAttack) m_pAnimAttack->Clear_All();
    if (m_pAnimDamaged) m_pAnimDamaged->Clear_Damaged();
    if (m_pAnimGuard) m_pAnimGuard->Clear();
    if (m_pAnimFall) m_pAnimFall->Clear();
}

void CKhazan_GSword::Clear_Step1()
{
    Clear_CycleState();
    Clear_SubState();
    Clear_State();
    Remove_Status(RESERVED | CHARGING_SPRINT | BACK_DODGE | CHARGING_STRONG_ATTACK | SPRINT_AGAIN_REQUEST | READY_ASSAULT | GUARD | GUARD_SUCCESS | JUST_GUARD | GUARD_ROTATION_REQUEST);
    //m_eDir.iDirFlag = 0;
    //m_eWorldDir.iDirFlag = 0;
    //m_ePrevDir = m_iPrevCycle = m_iPrevMainState = m_iPrevSubState = 0;
    m_fRotateTime[0] = 0.f;
    m_fSprintTime = 0.f;
    m_fChargingStrongTime = 0.f;
    m_iCurSkillIndex = 0;

    if (m_pAnimMove) m_pAnimMove->Clear();
    if (m_pAnimAttack) m_pAnimAttack->Clear_All();
    if (m_pAnimDamaged) m_pAnimDamaged->Clear_Damaged();
    if (m_pAnimGuard) m_pAnimGuard->Clear();
}

void CKhazan_GSword::Clear_Step2()
{
    if (Has_State(CAT::M_MOVE))
    {
        Clear_CycleState();
        Clear_SubState();
        Remove_State(CAT::M_MOVE);
    }
    Remove_Status(RESERVED | CHARGING_SPRINT | BACK_DODGE | ROTATION | SPRINT_AGAIN_REQUEST | READY_ASSAULT);

    m_eDir.iDirFlag = 0;
    m_eWorldDir.iDirFlag = 0;
    m_fRotateTime[0] = 0.f;
    m_fSprintTime = 0.f;

    if (m_pAnimMove)
    {
        m_pAnimMove->Clear();
        //  m_pAnimMove->Clear_Turn180();
    }
}

void CKhazan_GSword::Clear_Step3()
{
    Clear_CycleState();
    Clear_SubState();
    _bool isGuard = Has_State(CAT::M_GUARD);
    Clear_State();
    if (isGuard)
        Add_State(CAT::M_GUARD);

    Remove_Status(RESERVED | CHARGING_SPRINT | BACK_DODGE | CHARGING_STRONG_ATTACK | SPRINT_AGAIN_REQUEST | READY_ASSAULT);
    m_eDir.iDirFlag = 0;
    m_eWorldDir.iDirFlag = 0;
    //m_ePrevDir = m_iPrevCycle = m_iPrevMainState = m_iPrevSubState = 0;
    m_fRotateTime[0] = 0.f;
    m_fSprintTime = 0.f;
    m_fChargingStrongTime = 0.f;
    m_iCurSkillIndex = 0;

    if (m_pAnimMove) m_pAnimMove->Clear();
    if (m_pAnimAttack) m_pAnimAttack->Clear_All();
    if (m_pAnimDamaged) m_pAnimDamaged->Clear_Damaged();

}

HRESULT CKhazan_GSword::Ready_PartObjects()
{
    LEVEL eCurrentLevel = CClientInstance::GetInstance()->Get_CurrLevel();

    CBody_Khazan_GS::BODY_KHAZAN_GS_DESC         BodyDesc{};
    BodyDesc.pState = &m_iCurMainState;
    BodyDesc.pStatus = &m_iStatus;
    //BodyDesc.pIsGuarding = m_pAnimGuard->Get_IsGuarding();
    BodyDesc.pHitReation = &m_eHitReaction;
    BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    BodyDesc.pGuardRotationTarget = &m_vGuardRotationTarget;
    BodyDesc.pParentTransform = m_pTransformCom;
    if (FAILED(__super::Add_PartObject(TEXT("Part_Body"), ENUM_CLASS(eCurrentLevel), TEXT("Prototype_GameObject_Body_Khazan_GS"), &BodyDesc)))
        return E_FAIL;
    m_pBody = static_cast<CBody_Khazan_GS*>(Find_PartObject(TEXT("Part_Body")));
    m_pWeaponR_Matrix = m_pBody->Get_BoneMatrix("Weapon_R");
    m_BackPack_Matrix = m_pBody->Get_BoneMatrix("Weapon_R_BackPack_GSword");
    m_LanternSocket_Matrix = m_pBody->Get_BoneMatrix("Lantern_Socket_L");
    m_LHandSocket_Matrix = m_pBody->Get_BoneMatrix("L_Hand_Socket");

    CGSword_Khazan_GS::GSWORD_KHAZAN_GS_DESC         GSwordDesc{};
    GSwordDesc.pState = &m_iCurMainState;
    GSwordDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    GSwordDesc.pParentTransform = m_pTransformCom;
    if (FAILED(__super::Add_PartObject(TEXT("Part_Weapon"), ENUM_CLASS(eCurrentLevel), TEXT("Prototype_GameObject_GSword_Khazan_GS"), &GSwordDesc)))
        return E_FAIL;
    m_pGSword = static_cast<CGSword_Khazan_GS*>(Find_PartObject(TEXT("Part_Weapon")));
    //m_pSpearFX_Matrix = m_pGSword->Get_BoneMatrix("FX");
    //m_SpearOffset_Matrix = m_pGSword->Get_OffestMatrix();

    CLantern_Khazan_GS::LANTERN_KHAZAN_SPEAR_DESC         LanternDesc{};
    LanternDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    LanternDesc.pParentTransform = m_pTransformCom;
    LanternDesc.pLHandSocket_Matrix = m_LHandSocket_Matrix;
    LanternDesc.pLanternSocket_Matrix = m_LanternSocket_Matrix;
    if (FAILED(__super::Add_PartObject(TEXT("Part_Lantern"), ENUM_CLASS(eCurrentLevel), TEXT("Prototype_GameObject_Lantern_Khazan_GS"), &LanternDesc)))
        return E_FAIL;
    m_pLantern = static_cast<CLantern_Khazan_GS*>(Find_PartObject(TEXT("Part_Lantern")));


    /* 넘겨주기  */
    m_pGSword->Set_matBackPack(m_BackPack_Matrix);
    m_pGSword->Set_matWeaponR(m_pWeaponR_Matrix);
    m_pBody->Set_matGSword(m_pGSword_Matrix);
    m_pBody->Set_GSword(m_pGSword);
    //m_pBody->Set_IsLantern(m_pLantern->Get_isEquip());
    return S_OK;

}

HRESULT CKhazan_GSword::Ready_Collision()
{
    CCharacterVirtual::CV_CAPSULESHAPE_DESC tCharVirDesc{};
    _float3 vPos{};
    _float4 vQuat{};
    XMStoreFloat3(&vPos, m_pTransformCom->Get_State(STATE::POSITION));
    XMStoreFloat4(&vQuat, m_pTransformCom->Get_Rotation_Quat());
    tCharVirDesc.eShapeType = SHAPE::CAPSULE;
    tCharVirDesc.vPos = vPos;
    tCharVirDesc.vQuat = vQuat;
    tCharVirDesc.vShapeOffset = _float3(0.f, 0.75f, 0.f);
    tCharVirDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::PLAYER);
    tCharVirDesc.fRadius = 0.3f;
    tCharVirDesc.fHeight = 1.f;
    tCharVirDesc.fMaxSlopeAngle = 45.f;
    tCharVirDesc.fMass = 60.f;
    tCharVirDesc.fMaxStrength = 0.f;
    tCharVirDesc.fPredictiveContactDistance = 0.3f;
    tCharVirDesc.iMaxConstraintIterations = 20;
    tCharVirDesc.fCollisionTolerance = 0.03f;
    tCharVirDesc.fPenetrationRecoverySpeed = 1.7f;
    m_tCollisionDesc.pGameObject = this;
    m_tCollisionDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::PLAYER);
    m_tCollisionDesc.strName = TEXT("Khazan_Body");
    tCharVirDesc.pCollisionDesc = &m_tCollisionDesc;
    tCharVirDesc.vStickToFloorStepDown = _float3(0.f, -0.5f, 0);
    tCharVirDesc.vWalkStairsStepUp = _float3(0.f, 0.5f, 0.f);
    tCharVirDesc.fWalkStairsMinStepForward = 0.06f;
    tCharVirDesc.fWalkStairsStepForwardTest = 0.15f;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_CharacterVirtual"),
        TEXT("Com_CharacterVirtual"), reinterpret_cast<CComponent**>(&m_pCharVirCom), &tCharVirDesc)))
        return E_FAIL;

    return S_OK;
}

_uint CKhazan_GSword::ConvertCameraToPlayerDir(PLAYER_CAMERA_DIR playerCamDir)
{
    // 8방향 변환 테이블
    // [플레이어가 카메라 기준으로 보는 방향][입력 방향] = 플레이어 기준 방향
    static const _uint conversionTable[8][8] = {
        // 카메라 입력:    F,  R,  B,  L,  FR, BR, BL, FL
        {DIR::F, DIR::R, DIR::B, DIR::L, (DIR::F | DIR::R), (DIR::B | DIR::R), (DIR::B | DIR::L), (DIR::F | DIR::L)},               /* PC_FRONT */
        {(DIR::F | DIR::L), DIR::F, (DIR::F | DIR::R), DIR::L, DIR::F, (DIR::F | DIR::R), (DIR::B | DIR::R), (DIR::B | DIR::L)},    /* PC_FRONT_RIGHT */
        {DIR::L, DIR::F, DIR::R, DIR::B, (DIR::F | DIR::L), (DIR::F | DIR::R), (DIR::B | DIR::R), (DIR::B | DIR::L)},               /* PC_RIGHT */
        {(DIR::B | DIR::L), (DIR::F | DIR::L), DIR::F, (DIR::B | DIR::R), (DIR::F | DIR::L), DIR::F, (DIR::F | DIR::R), DIR::B},    /* PC_BACK_RIGHT */
        {DIR::B, DIR::L, DIR::F, DIR::R, (DIR::B | DIR::L), (DIR::F | DIR::L), (DIR::F | DIR::R), (DIR::B | DIR::R)},               /* PC_BACK */
        {(DIR::B | DIR::R), DIR::B, (DIR::F | DIR::L), (DIR::F | DIR::R), DIR::B, (DIR::B | DIR::L), DIR::F, (DIR::F | DIR::R)},    /* PC_BACK_LEFT */
        {DIR::R, DIR::B, DIR::L, DIR::F, (DIR::B | DIR::R), (DIR::B | DIR::L), (DIR::F | DIR::L), (DIR::F | DIR::R)},               /* PC_LEFT */
        {(DIR::F | DIR::R), (DIR::B | DIR::R), DIR::B, DIR::F, (DIR::B | DIR::R), DIR::B, (DIR::B | DIR::L), (DIR::F | DIR::L)}     /* PC_FRONT_LEFT */
    };
    // 입력 방향을 인덱스로 변환
    int inputIdx = 0;

    if (m_eWorldDir.AllCheck_Flag((DIR::F | DIR::R))) inputIdx = 4;
    else if (m_eWorldDir.AllCheck_Flag((DIR::B | DIR::R))) inputIdx = 5;
    else if (m_eWorldDir.AllCheck_Flag((DIR::B | DIR::L))) inputIdx = 6;
    else if (m_eWorldDir.AllCheck_Flag((DIR::F | DIR::L))) inputIdx = 7;
    else if (m_eWorldDir.AllCheck_Flag(DIR::F)) inputIdx = 0;
    else if (m_eWorldDir.AllCheck_Flag(DIR::R)) inputIdx = 1;
    else if (m_eWorldDir.AllCheck_Flag(DIR::B)) inputIdx = 2;
    else if (m_eWorldDir.AllCheck_Flag(DIR::L)) inputIdx = 3;


    return conversionTable[playerCamDir][inputIdx];
}

#pragma region 상호 작용 맵 오브젝트 이벤트
void CKhazan_GSword::Subscribe_Events()
{
#pragma region 상호 작용 맵 오브젝트 이벤트
    m_pGameInstance->Subscribe_Event<EventInteractType>(ENUM_CLASS(EVENT_TYPE::INTERACT_TYPE), [&](const EventInteractType& e) { m_EventInteract = e; });

    m_pGameInstance->Subscribe_Event<EventObject>(ENUM_CLASS(EVENT_TYPE::OBJECT_INTERACT), [&](const EventObject& e) {
        if (e.isOff())
        {
            m_pBody->Get_Model()->AnimationSetIndexIncrease();
            m_pGSword->Set_Enble(true);
            static_cast<CUI_HUD*>(CClientInstance::GetInstance()->Get_RootUI(TEXT("HUD")))->Switch_Panel(true);
            Add_Status(GSWORD);
            Remove_Status(BAREHAND | INJURED);
        }  });

#pragma endregion
}

void CKhazan_GSword::Event_Interact_Object(_float fTimeDelta)
{
    // 상호 작용 오브젝트 쪽에서 BEGIN STATE 내보내면 플레이어에서 행동 후, 행동 완료 시 이벤트 발생으로 상호 작용 오브젝트 동작
    if (EventInteractType::EVENT_STATE::BEGIN == m_EventInteract.eState)
    {
        if (false == m_isInteractEventSetting)
        {
            m_isInteractEventSetting = true;

            /*  창 들고 있으면 UnArmed 애니메이션 재생 */
            if (Has_Status(GSWORD))
                m_pBody->Get_Model()->Set_Animation(m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_GSword_UnArmed"));

            XMStoreFloat4(&m_vStartPos_Event, m_pTransformCom->Get_State(STATE::POSITION));
            m_fLerpTime_Event = 0.f;

        }
        // 플레이어 이동, LOOK 보간?? | 완료하면 이벤트 반대로 던져주기
        _bool isDone = { true };

        switch (m_EventInteract.eInteractType)
        {
        case INTERACTIVE_TYPE::CHEST:
        {
            isDone = false;
            _bool isMove = false;
            CModel* pBodyModel = m_pBody->Get_Model();

           /* 현재 재생되는 애니메이션이 UnArmed이고 끝났으면 true로 */
            if (m_pBody->Get_Model()->IsFinished()) {
                isDone = true;
            }

            break;
        }
        case INTERACTIVE_TYPE::CHECKPOINT:
        {
            isDone = false;

            if (m_pBody->Get_Model()->IsFinished()) {
                isDone = true;
            }

            break;
        }
        case INTERACTIVE_TYPE::LEVER:
        {
            isDone = false;

            if (m_pBody->Get_Model()->IsFinished()) {
                isDone = true;
            }

            break;
        }
        default:
            break;
        }

        if (isDone)               // 특정 조건 완성하면 이벤트 발생
        {
            // 이벤트에 필요한 세팅을 다음에 또 발생시 변경 가능하게 false로 변경
            m_isInteractEventSetting = false;

            // 상호작용 활성화시 맵 오브젝트한테 EVENT_STATE를 ON 으로 던져준다
            m_pGameInstance->Emit_Event<EventObject>(ENUM_CLASS(EVENT_TYPE::OBJECT_INTERACT), { EventObject::OnEvent() });
            // 내 상태를 STATE::NONE 으로 변경해준다.
            m_EventInteract.eState = EventInteractType::EVENT_STATE::NONE;
        }
    }

    // 상호 작용 오브젝트 쪽에서 END STATE 내보낼 시
    if (EventInteractType::EVENT_STATE::END == m_EventInteract.eState)
    {
        if (true)               // 특정 조건 완성하면 이벤트 발생
        {
            // 상호작용 비활성화시 맵 오브젝트한테 EVENT_STATE를 OFF 로 던져준다
            m_pGameInstance->Emit_Event<EventObject>(ENUM_CLASS(EVENT_TYPE::OBJECT_INTERACT), { EventObject::OffEvent() });
            // 내 상태를 STATE::NONE 으로 변경해준다.
            m_EventInteract.eState = EventInteractType::EVENT_STATE::NONE;
        }
    }

    // 이벤트가 발생 했을 때
    if (true == m_EventInteract.isEvent)
    {
        // 귀검일때
        if (INTERACTIVE_TYPE::CHECKPOINT == m_EventInteract.eInteractType)
        {
            BladeNexus_Event(fTimeDelta);
        }
        // 상자일때 ( 나중에 창고, 파밍 상자 나눌 예정 )
        if (INTERACTIVE_TYPE::CHEST == m_EventInteract.eInteractType)
        {
            Chest_Event(fTimeDelta);
        }
        // 경계의 틈 툼스톤일때
        if (INTERACTIVE_TYPE::TOMBSTONE == m_EventInteract.eInteractType)
        {
            TombStone_Event(fTimeDelta);
        }
        // 엠바스 레버일 때
        if (INTERACTIVE_TYPE::LEVER == m_EventInteract.eInteractType)
        {
            Lever_Event(fTimeDelta);
        }
    }
}

void CKhazan_GSword::BladeNexus_Event(_float fTimeDelta)
{
    EventBladeNexus BNEvent = m_EventInteract.BNEvent;

    // 귀검에 접촉 후 상호 작용 ( 귀검 가동 )
    if (false == BNEvent.isBNOpened)
    {

        // 귀검 첫 해금 시
        if (true == BNEvent.isUnLock)
        {
            // 첫 해금 플레이어    애니메이션 재생 
            if (m_pAnimInteraction->Try_DamagedTS_Before(Has_Status(GSWORD)))
            {
                Clear_State();
                Clear_SubState();
                Clear_CycleState();

            }
        }
        // 이미 해금된 귀검
        else if (false == BNEvent.isUnLock)
        {
            // 해금된 귀검 플레이어 애니메이션 재생
            if (m_pAnimInteraction->Try_DamagedTS_After(Has_Status(GSWORD)))
            {
                Clear_State();
                Clear_SubState();
                Clear_CycleState();
            }
        }

        // 플레이어 Look -> 귀검 ( 기우는거 보정하려고 이렇게 코드 넣어놨습니다. )
        BNEvent.vPosition.y = m_pTransformCom->Get_State(STATE::POSITION).m128_f32[1];
        m_pTransformCom->LookAt(XMLoadFloat4(&BNEvent.vPosition));
        m_pClientInstance->Start_ForceOrbit(CAMERA_FORCE_DIR::FRONT);
    }
    // 귀검 가동 끝나고 UI 팝업 ( 귀검 UI 창 활성화 ) ( 플레이어는 LOOP 애니메이션 )
    else if (true == BNEvent.isBNOpened)
    {
        // 귀검 첫 해금 시
        if (true == BNEvent.isUnLock)
        {
            // 첫 해금 플레이어 귀검 LOOP Animation?
        }
        // 이미 해금된 귀검
        else if (false == BNEvent.isUnLock)
        {
            // 해금 된 플레이어 귀검 LOOP Animation?
        }
    }

    m_EventInteract.End_Event();
}

void CKhazan_GSword::Chest_Event(_float fTimeDelta)
{
    EventChest ChestEvent = m_EventInteract.ChestEvent;

    // 상자에 접촉 후 상호 작용 ( 닫힌 상태 )
    if (false == ChestEvent.isChestOpened)
    {
        /* 애니메이션 재생 */
        if (m_pAnimInteraction->Try_BoxOpen(Has_Status(GSWORD)))
        {
            Clear_State();
            Clear_SubState();
            Clear_CycleState();
        }

        ChestEvent.vPlayerPosition.y = m_pTransformCom->Get_State(STATE::POSITION).m128_f32[1];
        // 플레이어 Look -> 상자, Position 상자 본 위치로 이동 ( 기우는거 보정 )
        m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat4(&ChestEvent.vPlayerPosition));
        ChestEvent.vPosition.y = m_pTransformCom->Get_State(STATE::POSITION).m128_f32[1];
        m_pTransformCom->LookAt(XMLoadFloat4(&ChestEvent.vPosition));

        m_EventInteract.End_Event();
    }
    // 상자 열리는 애니메이션 종료되면 ( 열린 상태 )
    else if (true == ChestEvent.isChestOpened)
    {
        m_fEventTimeAcc += fTimeDelta;

        if (0.2f <= m_fEventTimeAcc)
        {
            switch (m_sNextItem)
            {
            case 0:
                if (0 != ChestEvent.Items.iItem_0)
                    static_cast<CUI_Inven*>(CClientInstance::GetInstance()->Get_RootUI(TEXT("Inven")))->Add_Item(ChestEvent.Items.iItem_0);
                m_sNextItem = 1;
                break;
            case 1:
                if (0 != ChestEvent.Items.iItem_1)
                    static_cast<CUI_Inven*>(CClientInstance::GetInstance()->Get_RootUI(TEXT("Inven")))->Add_Item(ChestEvent.Items.iItem_1);
                m_sNextItem = 2;
                break;
            case 2:
                if (0 != ChestEvent.Items.iItem_2)
                    static_cast<CUI_Inven*>(CClientInstance::GetInstance()->Get_RootUI(TEXT("Inven")))->Add_Item(ChestEvent.Items.iItem_2);
                m_sNextItem = 0;

                ZeroMemory(&ChestEvent.Items, sizeof(BOX_ITEMS));
                m_EventInteract.End_Event();
                break;
            }

            m_fEventTimeAcc = 0.f;
        }

    }
}

void CKhazan_GSword::TombStone_Event(_float fTimeDelta)
{
    EventTombStone TSEvent = m_EventInteract.TSEvent;

    // 툼스톤에 접촉 후 상호 작용 ( 툼스톤 가동 )
    if (false == TSEvent.isTSOpened)
    {
        // 플레이어 Look -> 툼스톤 ( 기우는거 보정하려고 이렇게 코드 넣어놨습니다. )
        m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat4(&TSEvent.vPlayerPosition));
        TSEvent.vPosition.y = m_pTransformCom->Get_State(STATE::POSITION).m128_f32[1];
        m_pTransformCom->LookAt(XMLoadFloat4(&TSEvent.vPosition));
    }
    // 툼스톤 가동 끝나고 가동 LOOP 진입
    else if (true == TSEvent.isTSOpened)
    {
        // 플레이어 툼스톤 LOOP 애니메이션?
    }

    m_EventInteract.End_Event();
}
void CKhazan_GSword::Lever_Event(_float fTimeDelta)
{
    EventLever LeverEvent = m_EventInteract.LeverEvent;

    // 레버가 Active 로 전환 중일 때
    if (EventLever::ACTIVE == LeverEvent.eState)
    {
        // 플레이어가 레버를 Active 시키는 애니메이션 재생

        LeverEvent.vPlayerPosition.y = m_pTransformCom->Get_State(STATE::POSITION).m128_f32[1];
        // 플레이어 Look -> 레버, Position 레버 본 위치로 이동 ( 기우는거 보정 )
        m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat4(&LeverEvent.vPlayerPosition));
        LeverEvent.vPosition.y = m_pTransformCom->Get_State(STATE::POSITION).m128_f32[1];
        m_pTransformCom->LookAt(XMLoadFloat4(&LeverEvent.vPosition));
    }
    // 레버가 DeActive 로 전환 중일 때
    else if (EventLever::DEACTIVE == LeverEvent.eState)
    {
        // 플레이어가 레버를 DeActive 시키는 애니메이션 재생

        LeverEvent.vPlayerPosition.y = m_pTransformCom->Get_State(STATE::POSITION).m128_f32[1];
        // 플레이어 Look -> 레버, Position 레버 본 위치로 이동 ( 기우는거 보정 )
        m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat4(&LeverEvent.vPlayerPosition));
        LeverEvent.vPosition.y = m_pTransformCom->Get_State(STATE::POSITION).m128_f32[1];
        m_pTransformCom->LookAt(XMLoadFloat4(&LeverEvent.vPosition));
    }

    m_EventInteract.End_Event();
}
void CKhazan_GSword::Lerp_Position_ByInteractEvent(_float4 vTargetPos, _float4 vStartPos, _float fDuration, _float fTimeDelta, _bool& isDone)
{
    _float4 vPos = vTargetPos;

    // y값 보정
    vPos.y = vStartPos.y;

    m_fLerpTime_Event += fTimeDelta;

    _float fLerpTime = m_fLerpTime_Event / fDuration;

    _float4 vLerpPos = Lerp(vStartPos, vPos, fLerpTime);

    _float fDistance = XMVectorGetX(XMVector4Length(XMLoadFloat4(&vPos) - XMLoadFloat4(&vLerpPos)));

    if (0.1f > fDistance)
    {
        isDone = true;
    }
    else
    {
        m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat4(&vLerpPos));
        vPos.y = m_pTransformCom->Get_State(STATE::POSITION).m128_f32[1];
        m_pTransformCom->LookAt(XMLoadFloat4(&vPos));
    }
}
#pragma endregion

#ifdef _DEBUG

void CKhazan_GSword::Debug_Widget()
{
    m_pGameInstance->AddWidget(TEXT("Client"), [this]() {

        ImGui::Begin("Khazan GSWORD Debug", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

        if (ImGui::BeginTabBar("DebugTabs"))
        {
            if (ImGui::BeginTabItem("States"))
            {
                Debug_Widget_States();
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Combat"))
            {
                Debug_Widget_Combat();
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Animation"))
            {
                Debug_Widget_Animation();
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Movement"))
            {
                Debug_Widget_Movement();
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Guard Debug"))
            {
                Debug_Widget_Guard();
                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }

        ImGui::End();
        });
}

void CKhazan_GSword::Debug_Widget_States()
{
    ImGui::SeparatorText("State Flags");

    // === Main State ===
    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Main State (0x%08X)", m_iCurMainState);
    ImGui::Indent();

    auto StateFlag = [this](const char* label, _uint flag, ImVec4 color = ImVec4(0.0f, 1.0f, 0.0f, 1.0f)) {
        bool isActive = (m_iCurMainState & flag) != 0;
        if (isActive)
            ImGui::TextColored(color, "[ok] %s", label);
        else
            ImGui::TextColored(ImVec4(0.4f, 0.4f, 0.4f, 1.0f), "[ ] %s", label);
        };

    StateFlag("DIE", CAT::M_DIE, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
    StateFlag("FALL", CAT::M_FALL);
    StateFlag("GROGGY", CAT::M_GROGGY);
    StateFlag("DAMAGED", CAT::M_DAMAGED, ImVec4(1.0f, 0.5f, 0.0f, 1.0f));
    StateFlag("CLIMB", CAT::M_CLIMB);
    StateFlag("SKILL", CAT::M_SKILL, ImVec4(0.5f, 0.0f, 1.0f, 1.0f));
    StateFlag("GUARD", CAT::M_GUARD, ImVec4(0.0f, 1.0f, 1.0f, 1.0f));
    StateFlag("ATTACK", CAT::M_ATTACK, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
    StateFlag("MOVE", CAT::M_MOVE, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
    StateFlag("LOCKON", CAT::M_LOCKON);
    StateFlag("INTERACT", CAT::M_INTERACT);
    StateFlag("WEAPON_CHANGE", CAT::M_WEAPON_CHANGE);
    StateFlag("IDLE", CAT::M_IDLE);

    ImGui::Unindent();
    ImGui::Separator();

    // === Sub State ===
    ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "Sub State (0x%08X)", m_iCurSubState);
    ImGui::Indent();

    auto SubStateFlag = [this](const char* label, _uint flag) {
        bool isActive = (m_iCurSubState & flag) != 0;
        if (isActive)
            ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "[ok] %s", label);
        else
            ImGui::TextColored(ImVec4(0.4f, 0.4f, 0.4f, 1.0f), "[ ] %s", label);
        };

    // Movement
    ImGui::Text("Movement:");
    ImGui::Indent();
    SubStateFlag("Walk", MOV::MOVE_WALK);
    SubStateFlag("Run", MOV::MOVE_RUN);
    SubStateFlag("Sprint", MOV::MOVE_SPRINT);
    SubStateFlag("Dodge", MOV::MOVE_DODGE);
    SubStateFlag("Injured", MOV::MOVE_INJURED);
    SubStateFlag("Climb", MOV::MOVE_CLIMB);
    SubStateFlag("Fall", MOV::MOVE_FALL);
    SubStateFlag("GetUp", MOV::MOVE_GETUP);
    ImGui::Unindent();

    // Attack
    ImGui::Text("Attack:");
    ImGui::Indent();
    SubStateFlag("Fast", ATT::ATK_FAST);
    SubStateFlag("Strong", ATT::ATK_STRONG);
    SubStateFlag("Charge", ATT::ATK_CHARGE);
    SubStateFlag("Sprint Attack", ATT::ATK_SPRINTATK);
    SubStateFlag("Dodge Attack", ATT::ATK_DODGEATK);
    SubStateFlag("Counter", ATT::ATK_COUNTER);
    SubStateFlag("Grapple", ATT::ATK_GRAPPLE);
    ImGui::Unindent();

    // Skill
    ImGui::Text("Skill:");
    ImGui::Indent();
    SubStateFlag("Assault", SKI::ASSAULT);
    SubStateFlag("Full Moon", SKI::FULL_MOON);
    SubStateFlag("Moonlight Slash", SKI::MOONLIGHT_SLASH);
    SubStateFlag("Spiral Thrust", SKI::SPIRAL_THRUST);
    ImGui::Unindent();

    ImGui::Unindent();
    ImGui::Separator();

    // === Cycle State ===
    ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "Cycle (0x%08X)", m_iCycle);
    ImGui::Indent();

    auto CycleFlag = [this](const char* label, _uint flag) {
        bool isActive = (m_iCycle & flag) != 0;
        if (isActive)
            ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "[ok] %s", label);
        else
            ImGui::TextColored(ImVec4(0.4f, 0.4f, 0.4f, 1.0f), "[ ] %s", label);
        };

    CycleFlag("Start", CYC::CYCLE_START);
    CycleFlag("Loop", CYC::CYCLE_LOOP);
    CycleFlag("End", CYC::CYCLE_END);
    CycleFlag("Break", CYC::CYCLE_BREAK);

    ImGui::Unindent();
    ImGui::Separator();

    // === Status ===
    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Status (0x%08X)", m_iStatus);
    ImGui::Indent();

    auto StatusFlag = [this](const char* label, _uint flag) {
        bool isActive = (m_iStatus & flag) != 0;
        if (isActive)
            ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "[ok] %s", label);
        else
            ImGui::TextColored(ImVec4(0.4f, 0.4f, 0.4f, 1.0f), "[ ] %s", label);
        };

    StatusFlag("Barehand", BAREHAND);
    StatusFlag("Spear", SPEAR);
    StatusFlag("LockOn", LOCKON);
    StatusFlag("Injured", INJURED);
    StatusFlag("Reserved", RESERVED);
    StatusFlag("Charging Sprint", CHARGING_SPRINT);
    StatusFlag("Back Dodge", BACK_DODGE);
    StatusFlag("Rotation", ROTATION);
    StatusFlag("Charging Strong", CHARGING_STRONG_ATTACK);
    StatusFlag("Again Request", SPRINT_AGAIN_REQUEST);
    StatusFlag("Ready Assault", READY_ASSAULT);
    StatusFlag("GUARD", GUARD);
    StatusFlag("GUARD_SUCCESS", GUARD_SUCCESS);
    StatusFlag("RJUST_GUARD", JUST_GUARD);
    StatusFlag("GUARD_ROTATION", GUARD_ROTATION);
    StatusFlag("GUARD_ROTATION_REQUEST", GUARD_ROTATION_REQUEST);
    StatusFlag("FALLING", FALLING);
    StatusFlag("PRE_LAND", PRE_LAND);
    StatusFlag("FALLING_ATTACK", FALLING_ATTACK);
    StatusFlag("STAMINA_RECOVERY", STAMINA_RECOVERY);
    StatusFlag("BRUTAL_BEGIN", BRUTAL_BEGIN);
    StatusFlag("BRUTAL_READY", BRUTAL_READY);
    StatusFlag("BRUTAL_SUCCESS", BRUTAL_SUCCESS);

    //StatusFlag("TURN180", TURN180);
    //StatusFlag("TURN180_REQUESTED", TURN180_REQUESTED);
    //StatusFlag("TURN180_COMPLETE", TURN180_COMPLETE);
    //StatusFlag("MOVE_AFTER_TURN", MOVE_AFTER_TURN);

    ImGui::Unindent();
    ImGui::Separator();

    // === Direction ===
    ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "Direction");

    ImGui::Text("Player Dir: %s", GetDirectionString(m_eDir).c_str());
    ImGui::Text("World Dir: %s", GetDirectionString(m_eWorldDir).c_str());

    // Direction Visualization
    ImVec2 canvas_pos = ImGui::GetCursorScreenPos();
    ImVec2 canvas_size = ImVec2(120, 120);
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 center = ImVec2(canvas_pos.x + canvas_size.x * 0.5f, canvas_pos.y + canvas_size.y * 0.5f);
    float radius = 50.0f;

    draw_list->AddCircle(center, radius, IM_COL32(100, 100, 100, 255), 32, 2.0f);

    using DIR = DIRECTION_INFO::DIR;
    if (m_eDir.Check_Flag(DIR::F))
        draw_list->AddLine(center, ImVec2(center.x, center.y - radius), IM_COL32(0, 255, 0, 255), 3.0f);
    if (m_eDir.Check_Flag(DIR::B))
        draw_list->AddLine(center, ImVec2(center.x, center.y + radius), IM_COL32(0, 255, 0, 255), 3.0f);
    if (m_eDir.Check_Flag(DIR::L))
        draw_list->AddLine(center, ImVec2(center.x - radius, center.y), IM_COL32(0, 255, 0, 255), 3.0f);
    if (m_eDir.Check_Flag(DIR::R))
        draw_list->AddLine(center, ImVec2(center.x + radius, center.y), IM_COL32(0, 255, 0, 255), 3.0f);

    draw_list->AddCircleFilled(center, 5.0f, IM_COL32(255, 255, 0, 255));

    ImGui::Dummy(canvas_size);
}

void CKhazan_GSword::Debug_Widget_Combat()
{
    ImGui::SeparatorText("Combat Stats");

    // HP
    ImGui::Text("Health");
    ImGui::ProgressBar(m_fCurrentHP / max(m_fMaxHP, 0.001f), ImVec2(-1, 0),
        (std::to_string((_int)m_fCurrentHP) + " / " + std::to_string((_int)m_fMaxHP)).c_str());

    ImGui::SliderFloat("Current HP", &m_fCurrentHP, 0.0f, m_fMaxHP, "%.1f");
    ImGui::InputFloat("Max HP", &m_fMaxHP, 0, 0, "%.0f");

    ImGui::Separator();

    // Stamina
    ImGui::Text("Stamina");
    ImGui::ProgressBar(m_fCurrentStamina / max(m_fMaxStamina, 0.001f), ImVec2(-1, 0),
        (std::to_string((_int)m_fCurrentStamina) + " / " + std::to_string((_int)m_fMaxStamina)).c_str());

    ImGui::SliderFloat("Current Stamina", &m_fCurrentStamina, 0.0f, m_fMaxStamina, "%.1f");
    ImGui::InputFloat("Max Stamina", &m_fMaxStamina, 0, 0, "%.0f");

    ImGui::Separator();

    // Attack
    ImGui::InputFloat("Attack Power", &m_fAttack, 0, 0, "%.1f");

    ImGui::Separator();

    // Sprint Charge
    ImGui::Text("Sprint Charge");
    ImGui::ProgressBar(m_fSprintTime / m_fMinSprintTime, ImVec2(-1, 0),
        (std::to_string((_int)(m_fSprintTime * 100)) + "%").c_str());

    // Strong Attack Charge
    ImGui::Text("Strong Attack Charge");
    ImGui::ProgressBar(m_fChargingStrongTime / m_fChargingStrongIntervalTime, ImVec2(-1, 0),
        (std::to_string((_int)(m_fChargingStrongTime * 100)) + "%").c_str());

    ImGui::Separator();

    // Hit Reaction
    ImGui::Text("Hit Reaction: %s", GetHitReactionString().c_str());

    ImGui::Text("Hit Normal Dir: %s", GetDirectionString(m_eHitNormalDir).c_str());
    ImGui::Text("Hit Strong Dir: %s", GetDirectionString(m_eHitStrongDir).c_str());

    ImGui::Separator();

    // Quick Actions
    if (ImGui::Button("Restore HP", ImVec2(-1, 0)))
        m_fCurrentHP = m_fMaxHP;

    if (ImGui::Button("Restore Stamina", ImVec2(-1, 0)))
        m_fCurrentStamina = m_fMaxStamina;
}

void CKhazan_GSword::Debug_Widget_Animation()
{
    ImGui::SeparatorText("Animation Info");

    if (!m_pBody || !m_pBody->Get_Model())
    {
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Model not available");
        return;
    }

    CModel* pModel = m_pBody->Get_Model();

    // Current Animation
    ImGui::Text("Current Anim Index: %d", pModel->Get_CurAnimIndex());
    ImGui::Text("Reserved Anim Index: %d", m_iReserveAnimIndex);

    ImGui::Separator();

    // Animation Progress
    float trackPos = *pModel->Get_CurTrackPosition();
    float duration = pModel->Get_CurDuration();

    ImGui::Text("Track Position");
    ImGui::ProgressBar(trackPos / max(duration, 0.001f), ImVec2(-1, 0),
        (std::to_string((_int)trackPos) + " / " + std::to_string((_int)duration)).c_str());

    ImGui::Separator();

    // Animation State
    ImGui::Text("Is Finished: %s", m_pBody->Get_FinishedAnimation() ? "YES" : "NO");
    ImGui::Text("Min Time Passed: %s", pModel->Check_MinAnimationTime() ? "YES" : "NO");

    ImGui::Separator();

    // Animation System States
    if (m_pAnimMove)
    {
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Move Animation");
        ImGui::Indent();
        ImGui::Text("Selected Index: %d", m_pAnimMove->Get_AnimationIndex());
        ImGui::Text("Is Finished: %s", m_pAnimMove->Is_Finished() ? "YES" : "NO");
        ImGui::Text("Is Dodging: %s", m_pAnimMove->IsDodge() ? "YES" : "NO");
        ImGui::Unindent();
    }

    if (m_pAnimAttack)
    {
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Attack Animation");
        ImGui::Indent();
        ImGui::Text("Is Attacking: %s", m_pAnimAttack->Is_Attacking() ? "YES" : "NO");
        ImGui::Text("Can Next Combo: %s", m_pAnimAttack->Can_NextCombo() ? "YES" : "NO");
        ImGui::Text("Current Combo: %d", m_pAnimAttack->Get_CurrentCombo());
        ImGui::Text("Is Reserved: %s", m_pAnimAttack->Is_Reserve() ? "YES" : "NO");
        ImGui::Unindent();
    }

    if (m_pAnimGuard)
    {
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "Guard Animation");
        ImGui::Indent();
        ImGui::Text("Is Guarding: %s", m_pAnimGuard->Is_Guarding() ? "YES" : "NO");
        ImGui::Text("Is Walk Guarding: %s", m_pAnimGuard->Is_WalkGuarding() ? "YES" : "NO");
        ImGui::Text("Is Just Guarding: %s", m_pAnimGuard->Is_JustGuarding() ? "YES" : "NO");
        ImGui::Unindent();
    }

    if (m_pAnimDamaged)
    {
        ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "Damaged Animation");
        ImGui::Indent();
        ImGui::Text("Is Damaged: %s", m_pAnimDamaged->Is_Damaged() ? "YES" : "NO");
        ImGui::Unindent();
    }
}

void CKhazan_GSword::Debug_Widget_Movement()
{
    ImGui::SeparatorText("Movement Control");

    // Enable/Disable Control
    if (ImGui::Button(m_isEnableControl ? "Disable Control" : "Enable Control", ImVec2(-1, 30)))
    {
        m_isEnableControl = !m_isEnableControl;
    }

    ImGui::Separator();

    //// Speed Settings
    ImGui::DragFloat("Walk Speed", &m_fWalkSpeed, 0.1f, 0.f, 20.f);
    ImGui::DragFloat("Run Speed", &m_fRunSpeed, 0.1f, 0.f, 30.f);
    ImGui::DragFloat("Sprint Speed", &m_fSprintSpeed, 0.1f, 0.f, 50.f);
    ImGui::DragFloat("Injured Speed", &m_fInjuredSpeed, 0.1f, 0.f, 10.f);

    //ImGui::Separator();

    // Rotation Settings
    ImGui::Text("Rotation Time: %.2f / %.2f", m_fRotateTime[0], m_fRotateTime[1]);
    ImGui::DragFloat("Rotation Duration", &m_fRotateTime[1], 0.01f, 0.01f, 1.0f);

    ImGui::Separator();

    // Teleport
    static _float teleportPos[3] = { 516.f, -11.f, 264.f };
    ImGui::DragFloat3("Teleport Position YETUGA", teleportPos, 0.1f);

    if (ImGui::Button("Teleport", ImVec2(-1, 0)))
    {
        m_pTransformCom->Set_State(STATE::POSITION,
            XMVectorSet(teleportPos[0], teleportPos[1], teleportPos[2], 1.f));
    }

    static _float teleportPos2[3] = { 114.64f, 5.2f, 99.f };
    ImGui::DragFloat3("Teleport Position HeinMach Low Cliff", teleportPos2, 0.1f);

    if (ImGui::Button("Teleport", ImVec2(-1, 0)))
    {
        m_pTransformCom->Set_State(STATE::POSITION,
            XMVectorSet(teleportPos2[0], teleportPos2[1], teleportPos2[2], 1.f));
    }

    ImGui::Separator();

    // Key Bindings
    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Key Bindings");
    ImGui::Columns(2);
    ImGui::Text("Movement"); ImGui::NextColumn(); ImGui::Text("W/A/S/D"); ImGui::NextColumn();
    ImGui::Text("Walk"); ImGui::NextColumn(); ImGui::Text("LALT + WASD"); ImGui::NextColumn();
    ImGui::Text("Dodge"); ImGui::NextColumn(); ImGui::Text("SPACE (Tap)"); ImGui::NextColumn();
    ImGui::Text("Sprint"); ImGui::NextColumn(); ImGui::Text("SPACE (Hold)"); ImGui::NextColumn();
    ImGui::Text("Guard"); ImGui::NextColumn(); ImGui::Text("LSHIFT"); ImGui::NextColumn();
    ImGui::Text("Attack"); ImGui::NextColumn(); ImGui::Text("LMB/RMB"); ImGui::NextColumn();
    ImGui::Columns(1);
}

void CKhazan_GSword::Debug_Widget_Guard()
{
    ImGui::SeparatorText("Guard Debug Info");

    if (!m_pAnimGuard)
    {
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Guard Animation not available");
        return;
    }

    if (!m_pBody)
    {
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Body not available");
        return;
    }

    // Guard States
    ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "Guard States");
    ImGui::Separator();

    bool isGuarding = m_pAnimGuard->Is_Guarding();
    bool isWalkGuarding = m_pAnimGuard->Is_WalkGuarding();
    bool isJustGuarding = m_pAnimGuard->Is_JustGuarding();

    ImGui::Text("Is Guarding: ");
    ImGui::SameLine();
    if (isGuarding)
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "TRUE");
    else
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "FALSE");

    ImGui::Text("Is Walk Guarding: ");
    ImGui::SameLine();
    if (isWalkGuarding)
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "TRUE");
    else
        ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "FALSE");

    ImGui::Text("Is Just Guarding: ");
    ImGui::SameLine();
    if (isJustGuarding)
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "TRUE");
    else
        ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "FALSE");

    ImGui::Separator();

    // Body Guard Info
    ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "Body Collision States");
    ImGui::Separator();

    // 여기서 Body의 정보를 가져올 수 있다면 표시
    ImGui::Text("Note: Add Get functions to Body for collision states");

    ImGui::Separator();

    // Guard Rotation Info
    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Guard Rotation (Body)");
    ImGui::Separator();

    ImGui::Text("Note: Guard rotation is handled in Body");
    ImGui::Text("Add Get functions to access rotation states");

    ImGui::Separator();

    // Test Buttons
    if (ImGui::Button("Test Start Guard", ImVec2(-1, 0)))
    {
        if (!Has_State(CAT::M_GUARD))
        {
            m_pAnimGuard->Try_Guard();
            Add_State(CAT::M_GUARD);
        }
    }

    if (ImGui::Button("Test End Guard", ImVec2(-1, 0)))
    {
        if (Has_State(CAT::M_GUARD))
        {
            m_pAnimGuard->Play_FinishGuard();
            Remove_State(CAT::M_GUARD);
        }
    }
}

std::string CKhazan_GSword::GetDirectionString(DIRECTION_INFO dir)
{
    if (dir.iDirFlag == 0) return "NONE";

    std::string result;
    using DIR = DIRECTION_INFO::DIR;

    if (dir.Check_Flag(DIR::F)) result += "F";
    if (dir.Check_Flag(DIR::B)) result += "B";
    if (dir.Check_Flag(DIR::L)) result += "L";
    if (dir.Check_Flag(DIR::R)) result += "R";
    if (dir.Check_Flag(DIR::U)) result += "U";
    if (dir.Check_Flag(DIR::D)) result += "D";

    return result.empty() ? "NONE" : result;
}

std::string CKhazan_GSword::GetHitReactionString()
{
    switch (m_eHitReaction)
    {
    case ENUM_CLASS(HITREACTION::NONE): return "NONE";
    case ENUM_CLASS(HITREACTION::KNOCKBACK_WEAK): return "KNOCKBACK_WEAK";
    case ENUM_CLASS(HITREACTION::KNOCKBACK_NORMAL): return "KNOCKBACK_NORMAL";
    case ENUM_CLASS(HITREACTION::KNOCKBACK_STRONG): return "KNOCKBACK_STRONG";
    case ENUM_CLASS(HITREACTION::PARRY): return "PARRY";
    case ENUM_CLASS(HITREACTION::GRAB): return "GRAB";
    default: return "UNKNOWN";
    }
}

#endif // _DEBUG


CKhazan_GSword* CKhazan_GSword::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CKhazan_GSword* pInstance = new CKhazan_GSword(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CKhazan_GSword"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CKhazan_GSword::Clone(void* pArg)
{
    CKhazan_GSword* pInstance = new CKhazan_GSword(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Clone : CKhazan_GSword"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CKhazan_GSword::Free()
{
    __super::Free();

    Safe_Release(m_pClientInstance);
    Safe_Release(m_pCamera);
    //   Safe_Release(m_pBody);
       //Safe_Release(m_pSpear);
    Safe_Release(m_pAnimMove);
    Safe_Release(m_pAnimAttack);
    Safe_Release(m_pAnimGuard);
    Safe_Release(m_pAnimInteraction);
    Safe_Release(m_pAnimDamaged);
    Safe_Release(m_pAnimFall);
    // Safe_Release(m_pCharVirCom);

     //Safe_Release(m_pASMachine);
    // Safe_Release(m_pASManager);
}











#pragma region Try Later

//#include "Khazan_GSword.h"
//#include "GameInstance.h"
//#include "ClientInstance.h"
//
//#include "RigidBody.h"
//#include "CharacterVirtual.h"
//
//#include "Body_Khazan_GS.h"
//#include "Khazan_GS_InputHandler.h"
//#include "Khazan_GS_AnimationController.h"
//
//#include "Camera_Compre.h"
//#include "UI_HUD.h"
//#include "Damage_Text.h"
//
//
//CKhazan_GSword::CKhazan_GSword(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
//    : CCreature{ pDevice, pContext }
//{
//}
//
//CKhazan_GSword::CKhazan_GSword(const CKhazan_GSword& Prototype)
//    : CCreature{ Prototype }
//    , m_pClientInstance{ CClientInstance::GetInstance() }
//{
//    Safe_AddRef(m_pClientInstance);
//}
//
//HRESULT CKhazan_GSword::Initialize_Prototype()
//{
//    return S_OK;
//}
//
//HRESULT CKhazan_GSword::Initialize_Clone(void* pArg)
//{
//    CREATURE_DESC desc{};
//
//    desc.fAttack = 10.f;
//    desc.fMaxHP = 100.f;
//    desc.fMaxStamina = 100.f;
//    desc.fMoveSpeed = 10.f;
//    desc.fRotationPerSec = XMConvertToRadians(180.f);
//    desc.fSpeedPerSec = 1.f;
//
//
//    if (FAILED(__super::Initialize_Clone(&desc)))
//        return E_FAIL;
//
//    if (FAILED(Ready_Components()))
//        return E_FAIL;
//
//    if (FAILED(Ready_PartObjects()))
//        return E_FAIL;
//
//    if (FAILED(Ready_Collision()))
//        return E_FAIL;
//
//    if (FAILED(Ready_Modules()))
//        return E_FAIL;
//
//    m_pPlayerData = m_pClientInstance->Get_pInitailizePlayerData();
//
//    m_strName = "Khazan";
//
//    return S_OK;
//}
//
//void CKhazan_GSword::Priority_Update(_float fTimeDelta)
//{
//
//    __super::Priority_Update(fTimeDelta);
//
//}
//
//void CKhazan_GSword::Update(_float fTimeDelta)
//{
//    if (!m_isEnableControl || m_isDead)
//        return;
//
//    Update_LockOn();
//
//    // 입력 → Command 생성
//    Process_Input(fTimeDelta);
//
//    // 생선된 commands로 어떤 애니메이션을 재생할지 정하기 
//    Update_State(fTimeDelta);
//
//    // 애니메이션 실행 및 상태별 로직 실행시키기 (최소 보장 시간으로 위에서 정한 애니메이션 무시하기 )
//    Update_Animation(fTimeDelta);
//
//    // 이동/회전 적용
//    Update_Movement(fTimeDelta);
//
//    // 전투 로직( hp / stamina / status 관리 / 넉백..?  )
//    Update_Combat(fTimeDelta);
//
//
//
//
//    __super::Update(fTimeDelta);
//
//}
//
//void CKhazan_GSword::Late_Update(_float fTimeDelta)
//{
//
//
//    __super::Late_Update(fTimeDelta);
//}
//
//HRESULT CKhazan_GSword::Render()
//{
//    return S_OK;
//}
//
//void CKhazan_GSword::Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal)
//{
//}
//
//void CKhazan_GSword::Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal)
//{
//}
//
//void CKhazan_GSword::Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer)
//{
//}
//
//void CKhazan_GSword::Take_Damage(_float fDamage, HITREACTION eHitreaction, CGameObject* pGameObject)
//{
//}
//
//void CKhazan_GSword::Set_Camera(CCamera_Compre* pCamera)
//{
//    m_pCamera = pCamera;
//    Safe_AddRef(m_pCamera);
//}
//
//void CKhazan_GSword::Set_Position(_float4 vPos)
//{
//}
//
//
//void CKhazan_GSword::Process_Input(_float fTimeDelta)
//{
//    if (!m_pClientInstance->Get_PlayerInput())
//        return;
//
//    m_pInputHandler->Update(fTimeDelta);
//
//}
//
//void CKhazan_GSword::Update_State(_float fTimeDelta)
//{
//
//    m_pStateMachine->Update(fTimeDelta);
//
//    m_pStateMachine->Request_StateChange(m_pInputHandler->Get_CommandsThisFrame());
//
//}
//
//void CKhazan_GSword::Update_Animation(_float fTimeDelta)
//{
//    /* 선별된 커맨드 넘겨주기  */
//    m_pAnimController->Set_Command(m_pStateMachine->Get_SelectedCommand());
//    m_pAnimController->Update(fTimeDelta);
//}
//
//void CKhazan_GSword::Update_Movement(_float fTimeDelta)
//{
//    
//}
//
//void CKhazan_GSword::Update_Combat(_float fTimeDelta)
//{
//}
//
//void CKhazan_GSword::Update_LockOn()
//{
//    // 카메라의 락온 상태와 동기화
//    if (m_pCamera)
//        m_isLockOn = m_pCamera->Get_IsLockOn();
//}
//
//void CKhazan_GSword::Handle_StateChanged()
//{
//}
//
//void CKhazan_GSword::Handle_Death()
//{
//}
//
//HRESULT CKhazan_GSword::Ready_Components()
//{
//    return S_OK;
//}
//
//HRESULT CKhazan_GSword::Ready_PartObjects()
//{
//    LEVEL eCurrentLevel = CClientInstance::GetInstance()->Get_CurrLevel();
//
//    CBody_Khazan_GS::BODY_KHAZAN_GS_DESC         BodyDesc{};
//    //BodyDesc.pState = &m_iCurMainState;
//    //BodyDesc.pStatus = &m_iStatus;
//    //BodyDesc.pIsGuarding = m_pAnimGuard->Get_IsGuarding();
//    //BodyDesc.pHitReation = &m_eHitReaction;
//    BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
//    //BodyDesc.pGuardRotationTarget = &m_vGuardRotationTarget;
//    BodyDesc.pParentTransform = m_pTransformCom;
//    if (FAILED(__super::Add_PartObject(TEXT("Part_Body"), ENUM_CLASS(eCurrentLevel), TEXT("Prototype_GameObject_Body_Khazan_GS"), &BodyDesc)))
//        return E_FAIL;
//    m_pBody = static_cast<CBody_Khazan_GS*>(Find_PartObject(TEXT("Part_Body")));
//
//
//    return S_OK;
//}
//
//HRESULT CKhazan_GSword::Ready_Collision()
//{
//    CCharacterVirtual::CV_CAPSULESHAPE_DESC tCharVirDesc{};
//    _float3 vPos{};
//    _float4 vQuat{};
//    XMStoreFloat3(&vPos, m_pTransformCom->Get_State(STATE::POSITION));
//    XMStoreFloat4(&vQuat, m_pTransformCom->Get_Rotation_Quat());
//    tCharVirDesc.eShapeType = SHAPE::CAPSULE;
//    tCharVirDesc.vPos = vPos;
//    tCharVirDesc.vQuat = vQuat;
//    tCharVirDesc.vShapeOffset = _float3(0.f, 0.75f, 0.f);
//    tCharVirDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::PLAYER);
//    tCharVirDesc.fRadius = 0.3f;
//    tCharVirDesc.fHeight = 1.f;
//    tCharVirDesc.fMaxSlopeAngle = 45.f;
//    tCharVirDesc.fMass = 60.f;
//    tCharVirDesc.fMaxStrength = 0.f;
//    tCharVirDesc.fPredictiveContactDistance = 0.3f;
//    tCharVirDesc.iMaxConstraintIterations = 20;
//    tCharVirDesc.fCollisionTolerance = 0.03f;
//    tCharVirDesc.fPenetrationRecoverySpeed = 1.7f;
//    m_tCollisionDesc.pGameObject = this;
//    m_tCollisionDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::PLAYER);
//    m_tCollisionDesc.strName = TEXT("Khazan_Body");
//    tCharVirDesc.pCollisionDesc = &m_tCollisionDesc;
//    tCharVirDesc.vStickToFloorStepDown = _float3(0.f, -0.5f, 0);
//    tCharVirDesc.vWalkStairsStepUp = _float3(0.f, 0.5f, 0.f);
//    tCharVirDesc.fWalkStairsMinStepForward = 0.06f;
//    tCharVirDesc.fWalkStairsStepForwardTest = 0.15f;
//
//    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_CharacterVirtual"),
//        TEXT("Com_CharacterVirtual"), reinterpret_cast<CComponent**>(&m_pCharVirCom), &tCharVirDesc)))
//        return E_FAIL;
//
//    return S_OK;
//}
//
//HRESULT CKhazan_GSword::Ready_Modules()
//{
//
//    m_pStateMachine = CKhazan_GS_StateMachine::Create();
//    if (m_pStateMachine == nullptr) return E_FAIL;
//
//    m_pInputHandler = CKhazan_GS_InputHandler::Create();
//    if (m_pInputHandler == nullptr) return E_FAIL;
//
//    m_pAnimController = CKhazan_GS_AnimationController::Create(m_pBody->Get_Model(), &m_iCurWeapon, &m_isLockOn);
//    if (m_pAnimController == nullptr) return E_FAIL;
//
//    //m_pMovementController = CKhazan_GS_MovementController::Create();
//    //if (m_pMovementController == nullptr) return E_FAIL;
//
//    //m_pCombatModule = CKhazan_GS_CombatModule::Create();
//    //if (m_pCombatModule == nullptr) return E_FAIL;
//
//    return S_OK;
//}
//
//
//CKhazan_GSword* CKhazan_GSword::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
//{
//    CKhazan_GSword* pInstance = new CKhazan_GSword(pDevice, pContext);
//
//    if (FAILED(pInstance->Initialize_Prototype()))
//    {
//        MSG_BOX(TEXT("Failed to Created : CKhazan_GSword"));
//        Safe_Release(pInstance);
//    }
//
//    return pInstance;
//}
//
//CGameObject* CKhazan_GSword::Clone(void* pArg)
//{
//    CKhazan_GSword* pInstance = new CKhazan_GSword(*this);
//
//    if (FAILED(pInstance->Initialize_Clone(pArg)))
//    {
//        MSG_BOX(TEXT("Failed to Clone : CKhazan_GSword"));
//        Safe_Release(pInstance);
//    }
//
//    return pInstance;
//}
//
//void CKhazan_GSword::Free()
//{
//    __super::Free();
//
//    Safe_Release(m_pClientInstance);
//    Safe_Release(m_pCamera);
//
//}

#pragma endregion
