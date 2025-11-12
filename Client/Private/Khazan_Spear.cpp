#include "Khazan_Spear.h"
#include "Body_Khazan_Spear.h"
#include "Spear_Khazan_Spear.h"
#include "GameInstance.h"

#include "RigidBody.h"
#include "CharacterVirtual.h"
//#include "Khazan_Spear_ASManager.h"
#include "Khazan_Spear_ASMachine.h"

#include "ClientInstance.h"
#include "Khazan_Spear_Anim_Move.h"
#include "Khazan_Spear_Anim_Attack.h"
#include "Khazan_Spear_Anim_Guard.h"
#include "Khazan_Spear_Anim_Interaction.h"
#include "Khazan_Spaer_Anim_Damaged.h"

#include "Camera_Compre.h"

#pragma region 이벤트 - 인벤토리
#include "UI_Inven.h"
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


CKhazan_Spear::CKhazan_Spear(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CCreature{ pDevice, pContext }
{
}

CKhazan_Spear::CKhazan_Spear(const CKhazan_Spear& Prototype)
    : CCreature{ Prototype }
    , m_pClientInstance{ CClientInstance::GetInstance() }
{
    Safe_AddRef(m_pClientInstance);
}

HRESULT CKhazan_Spear::Initialize_Prototype()
{
    return S_OK;

}

HRESULT CKhazan_Spear::Initialize_Clone(void* pArg)
{
    CREATURE_DESC desc{};

    desc.fAttack = 10.f;
    desc.fMaxHP = 100.f;
    desc.fMaxStamina = 100.f;
    desc.fMoveSpeed = 10.f;
    desc.fRotationPerSec = XMConvertToRadians(180.f);
    desc.fSpeedPerSec = 1.f;

    m_fCurrentHP = 100.f;
    m_fCurrentStamina = 100.f;
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
    Add_Status(INJURED);
    Add_Status(SPEAR);

    m_iStopMoveIndexTable[0] = m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_BareHands_Walk_Stop_F_RF");
    m_iStopMoveIndexTable[1] = m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_Spear_Walk_Stop_F_RF");
    m_iStopMoveIndexTable[2] = m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_BareHands_Walk_Stop_F_LF");
    m_iStopMoveIndexTable[3] = m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_Spear_Walk_Stop_F_LF");
    m_iStopMoveIndexTable[4] = m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_BareHands_Run_Stop_F_RF");
    m_iStopMoveIndexTable[5] = m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_Spear_Run_Stop_F_RF");
    m_iStopMoveIndexTable[6] = m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_BareHands_Run_Stop_F_LF");
    m_iStopMoveIndexTable[7] = m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_Spear_Run_Stop_F_LF");
    m_iStopMoveIndexTable[8] = m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_Spear_Sprint_Stop_F");


    /* 플레이어 데이터 연결  */
    m_pPlayerData = m_pClientInstance->Get_pInitailizePlayerData();
    //m_fCurrentHP = m_pData->fCulHp;
    //m_fMaxHP = m_pData->fMaxHp;
    //m_fAttack = m_pData->fDamage;
    //m_fCurrentStamina = m_pData->fCulStamina;
    //m_fMaxStamina = m_pData->fMaxStamina;
    return S_OK;

}

void CKhazan_Spear::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);

    if (m_pGameInstance->Key_Down(DIK_F5))
        m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(516.f, -11.f, 264.f, 1.f));


}

void CKhazan_Spear::Update(_float fTimeDelta)
{
    if (m_isEnableControl)
    {
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

    XMStoreFloat4x4(&m_pSpearFX_WorldMatrix, m_SpearOffset_Matrix * XMLoadFloat4x4(m_pSpearFX_Matrix) * m_pTransformCom->Get_WorldMatrix());


    /* Test Injured key input */
    if (m_pGameInstance->Key_Pressing(DIK_LSHIFT, fTimeDelta) && m_pGameInstance->Key_Down(DIK_0))
    {
        Clear_Injured();
        Add_Status(INJURED);
    }
    if (m_pGameInstance->Key_Pressing(DIK_LSHIFT, fTimeDelta) && m_pGameInstance->Key_Down(DIK_9))
    {
        Clear_Injured();
    }
}

void CKhazan_Spear::Late_Update(_float fTimeDelta)
{




    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::DYNAMIC, this)))
        return;

    __super::Late_Update(fTimeDelta);
}

HRESULT CKhazan_Spear::Render()
{


    return S_OK;

}

void CKhazan_Spear::Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal)
{

    if (iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::MONSTERATTACK)) {
      //  cout << " hit !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
       // cout << "ContactNormal : " << ContactNormal.x << " " << ContactNormal.y << " " << ContactNormal.z << endl;

        Get_HitReaction(ContactNormal);
    }


}

void CKhazan_Spear::Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal)
{

}

void CKhazan_Spear::Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer)
{
}

void CKhazan_Spear::Take_Damage(_float fDamage, HITREACTION eHitreaction, CGameObject* pGameObject)
{
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

        /* 상태 DIE로 재정비 */
        m_iStatus = Has_Status(SPEAR) ? SPEAR : BAREHAND;
        Add_State(CAT::M_DIE);
        m_iCurAnimIndex = m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_Die_F");
        m_pBody->Get_Model()->Set_Animation(m_iCurAnimIndex);
    }


    switch (eHitreaction)
    {
    case Client::HITREACTION::NONE:

        break;
    //case Client::HITREACTION::GROGGY:

    //    break;
    case Client::HITREACTION::KNOCKBACK_WEAK:
        Add_State(CAT::M_DAMAGED);
        m_pAnimDamaged->Force_DamagedNormal(Has_Status(SPEAR), m_eHitNormalDir.iDirFlag);
        break;
    case Client::HITREACTION::KNOCKBACK_NORMAL:
        Add_State(CAT::M_DAMAGED);
        m_pAnimDamaged->Force_DamagedNormal(Has_Status(SPEAR), m_eHitNormalDir.iDirFlag);
        break;
    case Client::HITREACTION::KNOCKBACK_STRONG:
        Add_State(CAT::M_DAMAGED);
        m_pAnimDamaged->Force_DamagedStrong(Has_Status(SPEAR), m_eHitNormalDir.iDirFlag);
        break;
    case Client::HITREACTION::PARRY:

        break;
    case Client::HITREACTION::GRAB:

        break;

    }

}

void CKhazan_Spear::Set_Camera(CCamera_Compre* pCamera)
{
    m_pCamera = pCamera;
    Safe_AddRef(m_pCamera);
}

void CKhazan_Spear::Update_State(_float fTimeDelta)
{
    /* 이전 상태 저장*/
    m_iPrevMainState = m_iCurMainState;
    m_iPrevSubState = m_iCurSubState;
    m_ePrevDir = m_eDir.iDirFlag;
    m_iPrevCycle = m_iCycle;

    /* 락온상태 체크  */
    Update_LockOn();

    /* 방향 결정 */
    Check_KeyInput_Direction(fTimeDelta);

    /* 키 입력 막기  */
    if (m_pClientInstance->Get_PlayerInput())
    {
        if (Has_Status(INJURED))
        {
            InjuredMove_Input(fTimeDelta);
        }
        else
        {
            /* 키 입력 */
            Interaction_Input(fTimeDelta);
            Guard_Input(fTimeDelta);
            Skill_Input(fTimeDelta);
            Attack_Input(fTimeDelta);


            // 공격 중일 때는 Move_Input을 완전히 차단  - 무브 애니메이션 결정
            if (!Has_State(CAT::M_ATTACK | CAT::M_GUARD | CAT::M_SKILL) && !m_pAnimAttack->Is_Attacking()) Move_Input(fTimeDelta);
            else if (Has_State(CAT::M_ATTACK | CAT::M_SKILL))
            {
                // 공격 중일 때 Move 상태와 Reserve 초기화
                if (Has_State(CAT::M_MOVE))
                {
                    Remove_State(CAT::M_MOVE);
                    Clear_SubState();
                    AllClear_CycleState();
                }

                // Move Reserve 취소
                if (m_pAnimMove) m_pAnimMove->Clear_Reserve();

                /* 공격 중이고 락온 상태일 때 회전 처리만  */
                if (Has_Status(LOCKON)) LockOn_Rotation(fTimeDelta);

            }
        }
    }

    /* 대미지 및 스태미나 업데이트 */

    /*  상태 전환 여부*/
    _bool isEnter = (m_iCurMainState != m_iPrevMainState) || (m_iCurSubState != m_iPrevSubState);
    _bool isContinue = (m_iCurMainState == m_iPrevMainState) && (m_iCurSubState == m_iPrevSubState);

    /* (move , idle 애니메이션 재생 시도 */
    if (!Has_Status(INJURED))
        Change_MoveIdle(fTimeDelta);

    /* 실제 이동값 주기 */
    if (Has_State(CAT::M_MOVE | CAT::M_GUARD) && !Has_State(CAT::M_ATTACK | CAT::M_SKILL) && !m_pAnimMove->IsDodgeing() && !m_pAnimAttack->Is_Attacking())
        Apply_PlayerMovement(fTimeDelta);

    /* Exit 실행 */
    if (isEnter)
        ExecuteAnimationExit();


    /* 상태별 로직 실행 */
    if (Has_State(CAT::M_DIE)) {
        Update_Die(fTimeDelta);
    }
    else if (Has_State(CAT::ORDER2))
    {
        //if (Has_State(CAT::M_HOLD));
        //if (Has_State(CAT::M_GROGGY));
        if (Has_State(CAT::M_DAMAGED))
        {
            if (isEnter) m_pAnimDamaged->Enter();
            m_pAnimDamaged->Continue(fTimeDelta);
            if (!m_pAnimDamaged->Is_Damaged())
            {
                Remove_State(CAT::M_DAMAGED);
            }
        }
        //if (Has_State(CAT::M_CLIMB));
    }
    else if (Has_State(CAT::M_SKILL))
    {
        if (isEnter) m_pAnimAttack->Enter();
        m_pAnimAttack->Continue(fTimeDelta);

        if (!m_pAnimAttack->Is_Skilling())
        {
            Remove_State(CAT::M_SKILL);
            Clear_SubState();

            // 공격이 끝나고 방향키가 눌려있으면 Move로 전환
            if (m_eDir.iDirFlag > 0)
            {
                Add_State(CAT::M_MOVE);
                Add_SubState(MOV::MOVE_RUN);
                Add_CycleState(CYC::CYCLE_START);
            }
        }
    }
    else if (Has_State(CAT::M_GUARD))
    {
        if (isEnter) m_pAnimGuard->Enter();
        if (isEnter || isContinue) m_pAnimGuard->Continue(fTimeDelta);

        if (!m_pAnimGuard->Is_Guarding())
        {
            Remove_State(CAT::M_GUARD | CAT::M_MOVE);
            Clear_SubState();
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

                // 공격이 끝나고 방향키가 눌려있으면 Move로 전환
                if (m_eDir.iDirFlag > 0)
                {
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
        //if (Has_State(CAT::M_LOCKON));
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

void CKhazan_Spear::InjuredMove_Input(_float fTimeDelta)
{
    _bool isPrevMove = Has_State(CAT::M_MOVE);

    _uint curAnimIndex = m_pBody->Get_Model()->Get_CurAnimIndex();
    _uint stopRF = m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_Injured_Walk_Stop_F_RF");
    _uint stopLF = m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_Injured_Walk_Stop_F_LF");
    _bool isPlayingStop = (curAnimIndex == stopRF || curAnimIndex == stopLF);

    AllClear_CycleState();

    /* 방향키가 눌려있을 때 */
    if (m_eDir.iDirFlag > 0)
    {
        Add_State(CAT::M_MOVE);
        Add_SubState(MOV::MOVE_INJURED);

        // 새로 시작하는지, Loop인지 판단
        if (!isPrevMove || isPlayingStop)
        {
            Add_CycleState(CYC::CYCLE_START);
        }
        else
        {
            Add_CycleState(CYC::CYCLE_LOOP);
        }

        CKhazan_Spear_Anim_Move::SPEAR_MOVE info;
        info.isEquipWeapon = Has_Status(WEA::BAREHAND);
        info.isLockOn = Has_Status(LOCKON);
        info.iSubState = m_iCurSubState;
        info.iCycle = m_iCycle;
        info.eDir = m_eDir;

        m_pAnimMove->Try_InjuredAnimaition(info);
    }
    /* 방향키를 뗐을 때 (이동 중지) */
    else if (isPrevMove && !isPlayingStop)
    {
        Add_State(CAT::M_MOVE);
        Add_SubState(MOV::MOVE_INJURED);
        Add_CycleState(CYC::CYCLE_END);

        CKhazan_Spear_Anim_Move::SPEAR_MOVE info;
        info.isEquipWeapon = Has_Status(WEA::BAREHAND);
        info.isLockOn = Has_Status(LOCKON);
        info.iSubState = m_iCurSubState;
        info.iCycle = m_iCycle;
        info.eDir = m_eDir;

        m_pAnimMove->Try_InjuredAnimaition(info);
    }
    /* Stop 애니메이션이 끝났을 때 → Idle */
    else if (isPlayingStop && m_pBody->Get_Model()->IsFinished())
    {
        Remove_State(CAT::M_MOVE);
        Clear_SubState();
        Clear_CycleState();

        _uint idleAnim = m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_Injured_Stand");
        m_pBody->Get_Model()->Set_Animation(idleAnim);

    }


}

void CKhazan_Spear::Move_Input(_float fTimeDelta)
{
    /*  공격중 닷지는 예외 처리*/
    if (m_pGameInstance->Key_Down(DIK_SPACE) && m_pBody->Is_SpearFullExtension() && Has_State(CAT::M_ATTACK | CAT::M_SKILL))
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

    //// Dodge 종료 체크
    //if ((m_iPrevSubState & MOV::MOVE_DODGE) && m_pAnimMove->IsEndMoveAnimantionFinished())
    //{
    //    Remove_SubState(MOV::MOVE_DODGE);
    //    Remove_Status(CHARGING_SPRINT);

    //    // 방향키가 눌려있으면 즉시 Run으로 전환
    //    if (m_eDir.iDirFlag > 0)
    //    {
    //        Add_State(CAT::M_MOVE);
    //        Add_SubState(MOV::MOVE_RUN);
    //        Add_CycleState(CYC::CYCLE_START);

    //        CKhazan_Spear_Anim_Move::SPEAR_MOVE info;
    //        info.isEquipWeapon = Has_Status(WEA::SPEAR);
    //        info.isLockOn = Has_Status(LOCKON);
    //        info.iSubState = m_iCurSubState;
    //        info.iCycle = m_iCycle;
    //        info.eDir = m_eDir;

    //        m_pAnimMove->Try_ChangeAnimation(info);
    //        return;
    //    }
    //}
    //Dodge 종료 체크 - 최우선 처리
    if ((m_iPrevSubState & MOV::MOVE_DODGE))
    {
        Remove_SubState(MOV::MOVE_DODGE);
        Remove_Status(CHARGING_SPRINT);

        // Dodge 애니메이션이 끝났는지 확인
        if (m_pAnimMove->IsEndMoveAnimantionFinished())
        {
            // 방향키만 눌려있으면 즉시 Run으로 전환
            if (!m_pGameInstance->Key_Pressing(DIK_SPACE, fTimeDelta) && m_eDir.iDirFlag > 0)
            {
                Add_State(CAT::M_MOVE);
                Add_SubState(MOV::MOVE_RUN);
                Add_CycleState(CYC::CYCLE_START);

                CKhazan_Spear_Anim_Move::SPEAR_MOVE info;
                info.isEquipWeapon = Has_Status(WEA::SPEAR);
                info.isLockOn = Has_Status(LOCKON);
                info.iSubState = m_iCurSubState;
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
            m_fSprintTime = 0.f;
            Add_Status(CHARGING_SPRINT);
            Add_SubState(MOV::MOVE_DODGE);
            isSpaceHandled = true;
            cout << "---------------------------------------------- " << endl;
        }
        /* 스페이스 떼고 방향키를 누르고 있다는 예약이 걸려있을 때 다시 스페이스를 누를 경우*/
        else if (Has_State(AGAIN_REQUEST) && m_pGameInstance->Key_Pressing(DIK_SPACE, fTimeDelta))
        {
            Add_SubState(MOV::MOVE_SPRINT);
            Remove_SubState(MOV::MOVE_RUN);
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
            Remove_Status(CHARGING_SPRINT | AGAIN_REQUEST);

            if (m_eDir.iDirFlag > 0)
            {
                Remove_SubState(MOV::MOVE_SPRINT);
                Add_SubState(MOV::MOVE_RUN);
                Add_Status(AGAIN_REQUEST);
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
            Add_State(CAT::M_MOVE);
            Add_SubState(MOV::MOVE_DODGE);
            m_eDir.Add_Flag(DIR::B);  // 뒤로 회피
            m_fSprintTime = 0.f;
            Add_Status(BACK_DODGE);
            Remove_Status(CHARGING_SPRINT);
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

_bool CKhazan_Spear::Skill_Input(_float fTimeDelta)
{
    // 스킬이 끝났는지 체크
    if (Has_State(CAT::M_SKILL) && m_pAnimAttack && !m_pAnimAttack->Is_Skilling())
    {
        Remove_State(CAT::M_SKILL);
        Clear_SubState();

        // Move Reserve 취소
        if (m_pAnimMove)
            m_pAnimMove->Clear_Reserve();

        /* 스킬 관련 초기화 */
        Remove_Status(READY_ASSAULT);

        m_pClientInstance->Set_UsedSkill(m_iCurSkillIndex, false);
        return false;
    }


    if (m_pGameInstance->Key_Down(DIK_Q))
    {
        m_iCurSkillIndex = m_pClientInstance->Get_ButtonSkill(CONTROL_BUTTON::Q);
        if (m_iCurSkillIndex == 0) return false;

        if (!m_pAnimAttack->Try_SkillAttack(m_iCurSkillIndex))
            m_pAnimAttack->Reserve_SkillAttack(m_iCurSkillIndex);
        Add_State(CAT::M_SKILL);
        return true;

    }
    if (m_pGameInstance->Key_Down(DIK_E))
    {
        m_iCurSkillIndex = m_pClientInstance->Get_ButtonSkill(CONTROL_BUTTON::E);
        if (m_iCurSkillIndex == 0) return false;

        if (!m_pAnimAttack->Try_SkillAttack(m_iCurSkillIndex))
            m_pAnimAttack->Reserve_SkillAttack(m_iCurSkillIndex);
        Add_State(CAT::M_SKILL);
        return true;

    }
    if (m_pGameInstance->Key_Down(DIK_R))
    {
        m_iCurSkillIndex = m_pClientInstance->Get_ButtonSkill(CONTROL_BUTTON::R);
        if (m_iCurSkillIndex == 0) return false;

        if (!m_pAnimAttack->Try_SkillAttack(m_iCurSkillIndex))
            m_pAnimAttack->Reserve_SkillAttack(m_iCurSkillIndex);
        Add_State(CAT::M_SKILL);
        return true;
    }
    if (m_pGameInstance->Key_Pressing(DIK_LCONTROL, fTimeDelta) && m_pGameInstance->Key_Down(DIK_F))
    {
        m_iCurSkillIndex = m_pClientInstance->Get_ButtonSkill(CONTROL_BUTTON::CTRL_F);
        if (m_iCurSkillIndex == 0) return false;

        if (!m_pAnimAttack->Try_SkillAttack(m_iCurSkillIndex))
            m_pAnimAttack->Reserve_SkillAttack(m_iCurSkillIndex);
        Add_State(CAT::M_SKILL);
        return true;
    }
    if (m_pGameInstance->Key_Pressing(DIK_LCONTROL, fTimeDelta) && m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::LB))
    {
        m_iCurSkillIndex = m_pClientInstance->Get_ButtonSkill(CONTROL_BUTTON::CTRL_LB);
        if (m_iCurSkillIndex == 0) return false;

        if (!m_pAnimAttack->Try_SkillAttack(m_iCurSkillIndex))
            m_pAnimAttack->Reserve_SkillAttack(m_iCurSkillIndex);
        Add_State(CAT::M_SKILL);
        return true;
    }
    if (m_pGameInstance->Key_Pressing(DIK_LCONTROL, fTimeDelta) && m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::RB))
    {
        m_iCurSkillIndex = m_pClientInstance->Get_ButtonSkill(CONTROL_BUTTON::CTRL_RB);
        if (m_iCurSkillIndex == 0) return false;

        if (!m_pAnimAttack->Try_SkillAttack(m_iCurSkillIndex))
            m_pAnimAttack->Reserve_SkillAttack(m_iCurSkillIndex);
        Add_State(CAT::M_SKILL);
        return true;
    }

    return false;
}


_bool CKhazan_Spear::Attack_Input(_float fTimeDelta)
{

    /* 예약중인 공격 대기가 있으면 true */
    if (Has_State(CAT::M_ATTACK) && m_pAnimAttack->Is_Reserve())
        return true;

    // 공격이 끝났는지 체크
    if (Has_State(CAT::M_ATTACK) && m_pAnimAttack && !m_pAnimAttack->Is_Attacking())
    {
        Remove_State(CAT::M_ATTACK);
        Clear_SubState();

        // Move Reserve 취소
        if (m_pAnimMove)
            m_pAnimMove->Clear_Reserve();

        /* 스킬 관련 초기화 */
        Remove_Status(READY_ASSAULT);

        return false;
    }

    if (Has_State(CAT::M_ATTACK) && m_pAnimAttack && !m_pAnimAttack->Can_NextCombo() && !Has_Status(CHARGING_STRONG_ATTACK))
        return false;

    _bool isAttack = { false };


    /*  브루탈 공격 + 추후에 어떤 조건을 추가해야함 .*/
    //if (( ?? && m_pGameInstance->Key_Down(DIK_F))
    //{
    //    isAttack = m_pAnimAttack->Try_GrappleAttack();
    //}


    ///* 카운터 공격 + 추후에 어떤 조건을 추가해야함 .*/
    //if ( ??? && m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::LB))
    //{

    //}

    /* dodge 공격 */
    if ((m_iPrevMainState & CAT::M_MOVE) && (m_iPrevSubState & MOV::MOVE_DODGE) && m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::LB))
    {
        if (m_pAnimAttack->Try_DodgeAttack(m_ePrevDir))
        {
            Add_State(CAT::M_ATTACK);
            Add_SubState(ATT::ATK_DODGEATK);
            Remove_State(CAT::M_MOVE);
            m_eHitReaction = ENUM_CLASS(HITREACTION::KNOCKBACK_WEAK);
            OutputDebugStringA("[Input] Dodge Attack\n");
            return true;
        }
    }

    /* Sprint 공격  */
    else if ((m_iPrevMainState & CAT::M_MOVE) && (m_iPrevSubState & MOV::MOVE_SPRINT))
    {
        /* 빠른 공격*/
        if (m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::LB))
        {
            if (m_pAnimAttack->Try_SprintFastAttack())
            {
                Remove_State(CAT::M_MOVE);
                Clear_SubState();
                AllClear_CycleState();
                Remove_Status(CHARGING_SPRINT | AGAIN_REQUEST);

                // Move 예약도 취소
                //if (m_pAnimMove)
                //{
                //    m_pAnimMove->Clear_Reserve();
                //    m_pAnimMove->Exit();  // 내부 상태도 초기화
                //}

                Add_State(CAT::M_ATTACK);
                Add_SubState(ATT::ATK_SPRINTATK);

                m_eHitReaction = ENUM_CLASS(HITREACTION::KNOCKBACK_WEAK);

                return true;
            }
        }
        /* 강한 공격 */
        else if (m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::RB))
        {
            if (m_pAnimAttack->Try_SprintStrongAttack())
            {
                Remove_State(CAT::M_MOVE);
                Clear_SubState();
                AllClear_CycleState();
                Remove_Status(CHARGING_SPRINT | AGAIN_REQUEST);

                //if (m_pAnimMove)
                //{
                //    m_pAnimMove->Clear_Reserve();
                //    m_pAnimMove->Exit(); 
                //}

                Add_State(CAT::M_ATTACK);
                Add_SubState(ATT::ATK_SPRINTATK);

                if(m_pAnimAttack->Get_CurrentCombo()  >= 2)
                    m_eHitReaction = ENUM_CLASS(HITREACTION::KNOCKBACK_STRONG);
                else
                    m_eHitReaction = ENUM_CLASS(HITREACTION::KNOCKBACK_WEAK);


                return true;
            }
        }
        //else if (m_pBody->Get_Model()->Check_MinAnimationTime() && m_eDir.iDirFlag > 0)
        //{
        //    CKhazan_Spear_Anim_Move::SPEAR_MOVE info;
        //    info.isEquipWeapon = Has_Status(WEA::SPEAR);
        //    info.iSubState = m_iCurSubState;
        //    info.iCycle = m_iCycle;
        //    info.eDir = m_eDir;

        //    m_pAnimMove->Try_ChangeAnimation(info);
        //}
    }

    ///* 떨어지면서 공격 + 추후에 어떤 조건을 추가해야함  */
    //else if (m_pGameInstance->Mouse_Up(MOUSEKEYSTATE::LB))
    //{
    //    isAttack = m_pAnimAttack->Try_FallAttack();
    //}

    /* 스킬 : 강습  (빠른 공격 2단계까지만 가능)*/
    else if (Has_Status(READY_ASSAULT)
        && m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::RB)
        && 0 < m_pAnimAttack->Get_CurrentCombo()
        && m_pAnimAttack->Get_CurrentCombo() < 2
        && m_pAnimAttack->Is_FastAttacking())
    {

        // Move 상태 완전 제거
        Remove_State(CAT::M_MOVE);
        Clear_SubState();
        AllClear_CycleState();
        Remove_Status(CHARGING_SPRINT | AGAIN_REQUEST);

        Remove_Status(READY_ASSAULT);
        Add_State(CAT::M_ATTACK);
        Add_SubState(SKI::ASSAULT);
        cout << "READY_ASSAULT" << endl;

        if (m_pAnimAttack->Try_SkillAttack(SKI::ASSAULT))
        {
            m_eHitReaction = ENUM_CLASS(HITREACTION::KNOCKBACK_STRONG);
            return true;
        }
        else
        {
            m_pAnimAttack->Reserve_SkillAttack(SKI::ASSAULT);
            m_eHitReaction = ENUM_CLASS(HITREACTION::KNOCKBACK_STRONG);
            return true;
        }

    }

    /* 빠른 공격 3연타 + 스킬 배우면 3타 바뀜 */
    else if (m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::LB))
    {
        if (m_pAnimAttack->Try_FastAttack())
        {
            // Move 상태 완전 제거
            Remove_State(CAT::M_MOVE);
            Clear_SubState();
            AllClear_CycleState();
            Remove_Status(CHARGING_SPRINT | AGAIN_REQUEST);

            Add_State(CAT::M_ATTACK);
            Add_SubState(ATT::ATK_FAST);
            Add_Status(READY_ASSAULT);


            m_eHitReaction = ENUM_CLASS(HITREACTION::KNOCKBACK_WEAK);

            return true;
        }
    }

    /* 강한 공격 3연타  차징 */
    else if (m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::RB))
    {
        m_fChargingStrongTime = 0.f;
        Remove_State(CAT::M_MOVE);
        Remove_Status(CHARGING_STRONG_ATTACK);
        Remove_Status(READY_ASSAULT);
    }
    else if (m_pGameInstance->Mouse_Pressing(MOUSEKEYSTATE::RB))
    {
        if (!Has_Status(CHARGING_STRONG_ATTACK))
        {
            m_fChargingStrongTime += fTimeDelta;

            if (m_fChargingStrongTime >= m_fChargingStrongIntervalTime)
            {

                if (m_pAnimAttack->Try_ChageStrongAttack()) {
                    Remove_State(CAT::M_MOVE);
                    AllClear_CycleState();
                    Clear_SubState();
                    Remove_Status(CHARGING_SPRINT | AGAIN_REQUEST);

                    Add_Status(CHARGING_STRONG_ATTACK);
                    Add_SubState(ATT::ATK_CHARGE);
                    Add_State(CAT::M_ATTACK);
                    Remove_Status(CHARGING_SPRINT | AGAIN_REQUEST);

                    m_eHitReaction = ENUM_CLASS(HITREACTION::KNOCKBACK_STRONG);
                    return true;
                }
            }
        }

    }
    else if (!Has_Status(CHARGING_STRONG_ATTACK) && m_pGameInstance->Mouse_Up(MOUSEKEYSTATE::RB))
    {

        _bool wasCharging = Has_Status(CHARGING_STRONG_ATTACK);
        Remove_Status(CHARGING_STRONG_ATTACK);

        // 차징 안했으면 일반 강공격
        if (!wasCharging && m_fChargingStrongTime < m_fChargingStrongIntervalTime)
        {
            if (m_pAnimAttack->Try_StrongAttack())
            {
                Remove_State(CAT::M_MOVE);
                Clear_SubState();
                AllClear_CycleState();
                Remove_Status(CHARGING_SPRINT | AGAIN_REQUEST);

                Add_SubState(ATT::ATK_STRONG);
                Add_State(CAT::M_ATTACK);

                m_eHitReaction = ENUM_CLASS(HITREACTION::KNOCKBACK_STRONG);
                return true;
            }
        }
        else if (wasCharging)
        {
            // 공격 상태 해제
            if (m_pAnimAttack)
            {
                m_pAnimAttack->Exit();
            }
            Remove_State(CAT::M_ATTACK);
            Clear_SubState();
        }
    }

    return isAttack;

}

_bool CKhazan_Spear::Guard_Input(_float fTimeDelta)
{

    /* 가드 종료*/
    if (Has_State(CAT::M_GUARD) && m_pGameInstance->Key_Up(DIK_LSHIFT))
    {
        m_pAnimGuard->Play_FinishGuard();
        Remove_State(CAT::M_GUARD | CAT::M_MOVE);
        Remove_SubState(MOV::MOVE_WALK);

        return true;
    }

    if (!Has_State(CAT::M_GUARD) && m_pGameInstance->Key_Down(DIK_LSHIFT))
    {
        /* 저스트 가드 */


        /* 그냥 가드 */
        if (m_pAnimGuard->Try_Guard())
        {
            Add_State(CAT::M_GUARD);
            Remove_State(CAT::M_MOVE);
            Clear_SubState();
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

_bool CKhazan_Spear::Interaction_Input(_float fTimeDelta)
{
    /* 임시 */

    if (m_pGameInstance->Key_Pressing(DIK_LCONTROL,fTimeDelta) &&  m_pGameInstance->Key_Down(DIK_T))
    {
        if (m_pAnimInteraction->Try_DamagedTS_Before(Has_Status(SPEAR)))
        {
            Clear_State();
            Clear_SubState();
            Clear_CycleState();
            return true;
        }
    }

    if (m_pGameInstance->Key_Pressing(DIK_LCONTROL, fTimeDelta) && m_pGameInstance->Key_Down(DIK_Y))
    {
        if (m_pAnimInteraction->Try_DamagedTS_After(Has_Status(SPEAR)))
        {
            Clear_State();
            Clear_SubState();
            Clear_CycleState();
            return true;
        }
    }

    if (m_pGameInstance->Key_Pressing(DIK_LCONTROL, fTimeDelta) && m_pGameInstance->Key_Down(DIK_U))
    {
        if (m_pAnimInteraction->Try_TobStone(Has_Status(SPEAR)))
        {
            Clear_State();
            Clear_SubState();
            Clear_CycleState();
            return true;
        }
    }

    if (m_pGameInstance->Key_Pressing(DIK_LCONTROL, fTimeDelta) && m_pGameInstance->Key_Down(DIK_I))
    {
        if (m_pAnimInteraction->Try_BoxOpen(true))
        {
            Clear_State();
            Clear_SubState();
            Clear_CycleState();
            return true;
        }
    }

    if (m_pGameInstance->Key_Pressing(DIK_LCONTROL, fTimeDelta) && m_pGameInstance->Key_Down(DIK_O))
    {
        if (m_pAnimInteraction->Try_Lantern(true))
        {
            Clear_State();
            Clear_SubState();
            Clear_CycleState();
            return true;
        }
    }

    if (m_pGameInstance->Key_Pressing(DIK_LCONTROL, fTimeDelta) && m_pGameInstance->Key_Down(DIK_P))
    {
        if (m_pAnimInteraction->Try_Lantern(false))
        {
            Clear_State();
            Clear_SubState();
            Clear_CycleState();
            return true;
        }
    }

    if (m_pGameInstance->Key_Pressing(DIK_LCONTROL, fTimeDelta) && m_pGameInstance->Key_Down(DIK_G))
    {
        m_pBody->Get_Model()->AnimationSetIndexIncrease();
    }



    return false; 
}   

void CKhazan_Spear::Change_MoveIdle(_float fTimeDelt)
{
    if (Has_State(CAT::M_MOVE) && Has_SubState(MOV::MOVE_DODGE) && m_pBody->Is_SpearFullExtension())
    {
        CKhazan_Spear_Anim_Move::SPEAR_MOVE info;
        info.isEquipWeapon = Has_Status(WEA::SPEAR);
        info.isLockOn = Has_Status(LOCKON);
        info.iSubState = m_iCurSubState;
        info.iCycle = m_iCycle;
        info.eDir = m_eDir;
        m_pAnimMove->Try_ChangeAnimation(info);

    }

    // 공격 중일 때는 Move 애니메이션 변경 금지
    if (Has_State(CAT::M_ATTACK) && m_pAnimAttack->Is_Attacking())
        return;

    // Guard 중일 때도 체크
    if (Has_State(CAT::M_GUARD) && m_pAnimGuard->Is_Guarding())
        return;

    /*  락온 체크*/
    if (Has_Status(LOCKON))
        m_eDir = Calculate_LockOnDirection(fTimeDelt);

    /* 이동중 스페이스바 누르는것 때문에 다시 요청하기  */
    if (Has_Status(AGAIN_REQUEST))
    {
        CKhazan_Spear_Anim_Move::SPEAR_MOVE info;
        info.isEquipWeapon = Has_Status(WEA::SPEAR);
        info.isLockOn = Has_Status(LOCKON);
        info.iSubState = m_iCurSubState;
        info.iCycle = m_iCycle;
        info.eDir = m_eDir;
        m_pAnimMove->Reserve_Animation(info);
        Remove_Status(AGAIN_REQUEST);
    }

    if (Has_Status(LOCKON) && m_eDir.iDirFlag != m_ePrevDir)
    {
        CKhazan_Spear_Anim_Move::SPEAR_MOVE info;
        info.isEquipWeapon = Has_Status(WEA::SPEAR);
        info.isLockOn = Has_Status(LOCKON);
        info.iSubState = m_iCurSubState;
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
        CKhazan_Spear_Anim_Move::SPEAR_MOVE info;
        info.isEquipWeapon = Has_Status(WEA::SPEAR);
        info.isLockOn = Has_Status(LOCKON);
        info.iSubState = m_iCurSubState;
        info.iCycle = m_iCycle;
        info.eDir = m_eDir;

        _bool test = m_pAnimMove->Try_ChangeAnimation(info);

    }

    /* Idle */
    else if (!Has_State(CAT::M_END - 2))
    {

        _uint iCurAnimIndex = m_pBody->Get_Model()->Get_CurAnimIndex();
        if (m_pBody->Get_Model()->Check_MinAnimationTime() && iCurAnimIndex != 279 && iCurAnimIndex != 19)
            m_pBody->Get_Model()->Set_Animation(Has_Status(SPEAR) ? m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_Spear_Stand") : m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_BareHands_Stand"));

    }

}

void CKhazan_Spear::ExecuteAnimationExit()
{
    //if(m_iPrevMainState & CAT::M_DIE)
    //if(m_iPrevMainState &   CAT::M_HOLD             )
    //if(m_iPrevMainState &   CAT::M_GROGGY           )
    //if(m_iPrevMainState &   CAT::M_DAMAGED          )
    //if(m_iPrevMainState &   CAT::M_CLIMB            )

    if ((m_iCurMainState != m_iPrevMainState) && m_iPrevMainState & CAT::M_SKILL) m_pAnimAttack->Exit();
    if ((m_iCurMainState != m_iPrevMainState) && m_iPrevMainState & CAT::M_GUARD) m_pAnimGuard->Exit();
    if ((m_iCurMainState != m_iPrevMainState) && m_iPrevMainState & CAT::M_ATTACK) m_pAnimAttack->Exit();
    if ((m_iCurMainState != m_iPrevMainState) && m_iPrevMainState & CAT::M_MOVE) m_pAnimMove->Exit();
    //if(m_iPrevMainState &   CAT::M_LOCKON           )
    //if(m_iPrevMainState &   CAT::M_INTERACT         )
    //if(m_iPrevMainState &   CAT::M_WEAPON_CHANGE    )
    //if(m_iPrevMainState &   CAT::M_IDLE             )
    //if(m_iPrevMainState &   CAT::M_END              )

}

void CKhazan_Spear::Apply_PlayerMovement(_float fTimeDelta)
{
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

    // 회전 처리
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



void CKhazan_Spear::Check_KeyInput_Direction(_float fTimeDelta)
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

DIRECTION_INFO CKhazan_Spear::Calculate_LockOnDirection(_float fTimeDelta)
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
void CKhazan_Spear::Update_PlayerDate()
{
    /* todo.... 플레이어 스탯 테이블 만들어서 가지고 오자. (프로토타입 이후에) */
    

}
void CKhazan_Spear::LockOn_Rotation(_float fTimeDelta)
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
void CKhazan_Spear::Update_LockOn()
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

void CKhazan_Spear::Update_Die(_float fTimeDelta)
{
    if (m_pBody->Get_Model()->Get_CurAnimIndex() == m_iCurAnimIndex && m_pBody->Get_Model()->IsFinished())
    {
        /* 한번만 들어감.  */
        m_iCurAnimIndex = Has_Status(SPEAR)
            ? m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_Spear_Com_Down_Loop_F")
            : m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_DownDie_F");

        m_pBody->Get_Model()->Set_Animation(m_iCurAnimIndex);
    }
}

void CKhazan_Spear::Clear_Injured()
{
    Clear_State();
    Clear_SubState();
    Clear_CycleState();

    Remove_Status(INJURED);
}

void CKhazan_Spear::Get_HitReaction(const _float3& vContactNormal)
{
    _vector vPlayerLook = XMVector3Normalize(m_pTransformCom->Get_State(STATE::LOOK));
    _vector vPlayerRight = XMVector3Normalize(m_pTransformCom->Get_State(STATE::RIGHT));
    _vector vNormal = XMVector3Normalize(XMLoadFloat3(&vContactNormal));

    _vector vFlatNormal = XMVectorSet(XMVectorGetX(vNormal), 0.f, XMVectorGetZ(vNormal), 0.f);
    if (XMVector3Equal(vFlatNormal, XMVectorZero())) 
        vFlatNormal = XMVectorSet(0.f, 0.f, 1.f, 0.f);  // 수평 성분이 전혀 없음 (즉, 위나 아래에서 맞은 경우)
    else 
        vFlatNormal = XMVector3Normalize(vFlatNormal);
    
    _float fForwardDot = XMVectorGetX(XMVector3Dot(vFlatNormal, vPlayerLook));
    _float fRightDot = XMVectorGetX(XMVector3Dot(vFlatNormal, vPlayerRight));
    _float fUpDot = XMVectorGetY(vNormal);

    m_eHitNormalDir.Clear_Flag();

    if (fUpDot > 0.5f) m_eHitNormalDir.Add_Flag(DIRECTION_INFO::U);
    else if (fUpDot < -0.5f) m_eHitNormalDir.Add_Flag(DIRECTION_INFO::D);

    if (fRightDot > 0.3f) m_eHitNormalDir.Add_Flag(DIRECTION_INFO::R);
    else if (fRightDot < -0.3f) m_eHitNormalDir.Add_Flag(DIRECTION_INFO::L);

    if (fForwardDot > 0.3f) m_eHitNormalDir.Add_Flag(DIRECTION_INFO::F);
    else if (fForwardDot < -0.3f) m_eHitNormalDir.Add_Flag(DIRECTION_INFO::B);
}


HRESULT CKhazan_Spear::Ready_Components()
{
    return S_OK;
}

HRESULT CKhazan_Spear::Ready_AnimationStateMachine()
{

    m_pAnimMove = CKhazan_Spear_Anim_Move::Create();
    if (m_pAnimMove == nullptr)
        return E_FAIL;
    m_pAnimMove->Set_Model(m_pBody->Get_Model());


    m_pAnimAttack = CKhazan_Spear_Anim_Attack::Create();
    if (m_pAnimAttack == nullptr)
        return E_FAIL;
    m_pAnimAttack->Set_Model(m_pBody->Get_Model());

    m_pAnimGuard = CKhazan_Spear_Anim_Guard::Create();
    if (m_pAnimGuard == nullptr)
        return E_FAIL;
    m_pAnimGuard->Set_Model(m_pBody->Get_Model());
    m_pBody->Set_IsGuarding(m_pAnimGuard->Get_IsGuarding());

    m_pAnimInteraction = CKhazan_Spear_Anim_Interaction::Create();
    if (m_pAnimInteraction == nullptr)
        return E_FAIL;
    m_pAnimInteraction->Set_Model(m_pBody->Get_Model());


    m_pAnimDamaged = CKhazan_Spaer_Anim_Damaged::Create();
    if (m_pAnimDamaged == nullptr)
        return E_FAIL;
    m_pAnimDamaged->Set_Model(m_pBody->Get_Model());


    return S_OK;
}

HRESULT CKhazan_Spear::Ready_PartObjects()
{
    LEVEL eCurrentLevel = CClientInstance::GetInstance()->Get_CurrLevel();

    CBody_Khazan_Spear::BODY_KHAZAN_SPEAR_DESC         BodyDesc{};
    BodyDesc.pState = &m_iCurMainState;
    //BodyDesc.pIsGuarding = m_pAnimGuard->Get_IsGuarding();
    BodyDesc.pHitReation = &m_eHitReaction;
    BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    BodyDesc.pParentTransform = m_pTransformCom;
    if (FAILED(__super::Add_PartObject(TEXT("Part_Body"), ENUM_CLASS(eCurrentLevel), TEXT("Prototype_GameObject_Body_Khazan_Spear"), &BodyDesc)))
        return E_FAIL;

    m_pBody = static_cast<CBody_Khazan_Spear*>(Find_PartObject(TEXT("Part_Body")));
    m_pWeaponR_Matrix = m_pBody->Get_BoneMatrix("Weapon_R");

    CSpear_Khazan_Spear::SPEAR_KHAZAN_SPEAR_DESC         SpearDesc{};
    SpearDesc.pState = &m_iCurMainState;
    SpearDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    SpearDesc.pParentTransform = m_pTransformCom;
    if (FAILED(__super::Add_PartObject(TEXT("Part_Weapon_Spear"), ENUM_CLASS(eCurrentLevel), TEXT("Prototype_GameObject_Spear_Khazan_Spear"), &SpearDesc)))
        return E_FAIL;

    m_pSpear = static_cast<CSpear_Khazan_Spear*>(Find_PartObject(TEXT("Part_Weapon_Spear")));
    m_pSpearFX_Matrix = m_pSpear->Get_BoneMatrix("FX");
    m_SpearOffset_Matrix = m_pSpear->Get_OffestMatrix();

    /* 넘겨주기  */
    m_pSpear->Set_matWeaponR(m_pWeaponR_Matrix);
    m_pBody->Set_matSpearFX(m_pSpearFX_Matrix);
    m_pBody->Set_matSpearOffset(m_SpearOffset_Matrix);
    return S_OK;

}

HRESULT CKhazan_Spear::Ready_Collision()
{
    CCharacterVirtual::CV_CAPSULESHAPE_DESC tCharVirDesc{};
    _float3 vPos{};
    _float4 vQuat{};
    XMStoreFloat3(&vPos, m_pTransformCom->Get_State(STATE::POSITION));
    XMStoreFloat4(&vQuat, m_pTransformCom->Get_Rotation_Quat());
    tCharVirDesc.eShapeType = SHAPE::CAPSULE;
    tCharVirDesc.vPos = vPos;
    tCharVirDesc.vQuat = vQuat;
    tCharVirDesc.vShapeOffset = _float3(0.f, 0.7f, 0.f);
    tCharVirDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::PLAYER);
    tCharVirDesc.fRadius = 0.5f;
    tCharVirDesc.fHeight = 0.5f;
    tCharVirDesc.fMaxSlopeAngle = 45.f;
    //tCharVirDesc.fMass = 100000.f;
    m_tCollisionDesc.pGameObject = this;
    m_tCollisionDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::PLAYER);
    //pCollDesc.pInfo = ?? // 작성하기
    tCharVirDesc.pCollisionDesc = &m_tCollisionDesc;
    tCharVirDesc.fMaxStrength = 0.f;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_CharacterVirtual"),
        TEXT("Com_CharacterVirtual"), reinterpret_cast<CComponent**>(&m_pCharVirCom), &tCharVirDesc)))
        return E_FAIL;

    return S_OK;
}



inline _bool CKhazan_Spear::Has_States()
{
    for (_uint i = 0; i < GetBitPosition(CAT::M_END); ++i)
    {
        if (Has_State(1 << i))
            return true;

    }
    return false;
}

inline _bool CKhazan_Spear::Has_SubStates()
{
    for (_uint i = 0; i < GetBitPosition(CAT::M_END); ++i)
    {
        if (Has_SubState(1 << i))
            return true;

    }
    return false;
}

_uint CKhazan_Spear::ConvertCameraToPlayerDir(PLAYER_CAMERA_DIR playerCamDir)
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
void CKhazan_Spear::Subscribe_Events()
{
#pragma region 상호 작용 맵 오브젝트 이벤트
    m_pGameInstance->Subscribe_Event<EventInteractType>(ENUM_CLASS(EVENT_TYPE::INTERACT_TYPE), [&](const EventInteractType& e) { m_EventInteract = e; });

    m_pGameInstance->Subscribe_Event<EventObject>(ENUM_CLASS(EVENT_TYPE::OBJECT_INTERACT), [&](const EventObject& e) {if (e.isOff()) m_pBody->Get_Model()->AnimationSetIndexIncrease();  });

#pragma endregion
}
void CKhazan_Spear::Event_Interact_Object(_float fTimeDelta)
{
    // 상호 작용 오브젝트 쪽에서 BEGIN STATE 내보내면 플레이어에서 행동 후, 행동 완료 시 이벤트 발생으로 상호 작용 오브젝트 동작
    if (EventInteractType::EVENT_STATE::BEGIN == m_EventInteract.eState)
    {
        if (false == m_isInteractEventSetting)
        {
            m_isInteractEventSetting = true;

            /*  창 들고 있으면 UnArmed 애니메이션 재생 */
            if (Has_Status(SPEAR))
                m_pBody->Get_Model()->Set_Animation(m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_Spear_UnArmed"));

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

            if (pBodyModel->Get_CurAnimIndex() != pBodyModel->Get_AnimIndexByName("CA_P_Kazan_Spear_UnArmed"))
                isMove = true;
            else if (pBodyModel->IsFinished())
            {
                isMove = true;
                pBodyModel->Set_Animation(pBodyModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Walk_F"));
            }

            if (true == isMove)
                Lerp_Position_ByInteractEvent(m_EventInteract.ChestEvent.vPlayerPosition, m_vStartPos_Event, 0.3f, fTimeDelta, isDone);

            break;
        }
        case INTERACTIVE_TYPE::CHECKPOINT:
        {
            isDone = false;

            // Lerp_Position_ByInteractEvent(m_EventInteract.ChestEvent.vPlayerPosition, m_vStartPos_Event, 0.3f, fTimeDelta, isDone);

                 /* 현재 재생되는 애니메이션이 UnArmed이고 끝났으면 true로 */
            if (/*m_pBody->Get_Model()->Get_CurAnimIndex() == m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_Spear_UnArmed") && */m_pBody->Get_Model()->IsFinished())
                isDone = true;

            break;
        }
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
    }
}

void CKhazan_Spear::BladeNexus_Event(_float fTimeDelta)
{
    EventBladeNexus BNEvent = m_EventInteract.BNEvent;

    // 귀검에 접촉 후 상호 작용 ( 귀검 가동 )
    if (false == BNEvent.isBNOpened)
    {


        // 귀검 첫 해금 시
        if (true == BNEvent.isUnLock)
        {
            // 첫 해금 플레이어    애니메이션 재생 
            if (m_pAnimInteraction->Try_DamagedTS_Before(Has_Status(SPEAR)&&!Has_Status(INJURED)))
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
            if (m_pAnimInteraction->Try_DamagedTS_After(Has_Status(SPEAR) && !Has_Status(INJURED)))
            {
                Clear_State();
                Clear_SubState();
                Clear_CycleState();
            }
        }

        // 플레이어 Look -> 귀검 ( 기우는거 보정하려고 이렇게 코드 넣어놨습니다. )
        BNEvent.vPosition.y = m_pTransformCom->Get_State(STATE::POSITION).m128_f32[1];
        m_pTransformCom->LookAt(XMLoadFloat4(&BNEvent.vPosition));
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

void CKhazan_Spear::Chest_Event(_float fTimeDelta)
{
    EventChest ChestEvent = m_EventInteract.ChestEvent;

    // 상자에 접촉 후 상호 작용 ( 닫힌 상태 )
    if (false == ChestEvent.isChestOpened)
    {
        /* 애니메이션 재생 */
        if (m_pAnimInteraction->Try_BoxOpen(Has_Status(SPEAR) && !Has_Status(INJURED)))
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

void CKhazan_Spear::TombStone_Event(_float fTimeDelta)
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
void CKhazan_Spear::Lerp_Position_ByInteractEvent(_float4 vTargetPos, _float4 vStartPos, _float fDuration, _float fTimeDelta, _bool& isDone)
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

void CKhazan_Spear::Debug_Widget()
{
    m_pGameInstance->AddWidget(TEXT("Client"), [this]() {

        // 메인 윈도우
        ImGui::Begin("Khazan Spear Debug", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

        // 탭 바
        if (ImGui::BeginTabBar("DebugTabs"))
        {
            if (ImGui::BeginTabItem("Control"))
            {
                Debug_Widget_Movement();
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("States"))
            {
                Debug_Widget_States();
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Animation"))
            {
                Debug_Widget_Animation();
                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }

        ImGui::End();
        });
}

void CKhazan_Spear::Debug_Widget_States()
{
    // === 상태 개요 ===
    ImGui::SeparatorText("State Overview");

    ImGui::BeginTable("StateOverview", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg);
    ImGui::TableSetupColumn("Property", ImGuiTableColumnFlags_WidthFixed, 120.0f);
    ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);

    ImGui::TableNextRow();
    ImGui::TableNextColumn(); ImGui::Text("Control");
    ImGui::TableNextColumn();
    if (m_isEnableControl)
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "ENABLED");
    else
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "DISABLED");

    ImGui::TableNextRow();
    ImGui::TableNextColumn(); ImGui::Text("Main State");
    ImGui::TableNextColumn();
    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", GetStateName(m_iCurMainState));

    ImGui::TableNextRow();
    ImGui::TableNextColumn(); ImGui::Text("Sub State");
    ImGui::TableNextColumn();
    ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "%s", GetSubStateName(m_iCurSubState));

    ImGui::TableNextRow();
    ImGui::TableNextColumn(); ImGui::Text("Cycle");
    ImGui::TableNextColumn();
    ImGui::Text("%s", GetCycleName(m_iCycle));

    ImGui::TableNextRow();
    ImGui::TableNextColumn(); ImGui::Text("Direction");
    ImGui::TableNextColumn();
    ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "%s", GetDirectionString().c_str());

    ImGui::EndTable();

    ImGui::Spacing();

    // === 상세 상태 비트 플래그 ===
    ImGui::SeparatorText("State Bit Flags");

    ImGui::BeginChild("StateBits", ImVec2(0, 200), true);

    // Main States
    ImGui::Text("Main States (0x%08X)", m_iCurMainState);
    ImGui::Indent();

    auto DisplayState = [](const char* label, bool isActive) {
        if (isActive)
            ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "[ok] %s", label);
        else
            ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "[ ] %s", label);
        };

    DisplayState("M_DIE", m_iCurMainState & CAT::M_DIE);
    DisplayState("M_HOLD", m_iCurMainState & CAT::M_HOLD);
    DisplayState("M_GROGGY", m_iCurMainState & CAT::M_GROGGY);
    DisplayState("M_DAMAGED", m_iCurMainState & CAT::M_DAMAGED);
    DisplayState("M_CLIMB", m_iCurMainState & CAT::M_CLIMB);
    DisplayState("M_SKILL", m_iCurMainState & CAT::M_SKILL);
    DisplayState("M_GUARD", m_iCurMainState & CAT::M_GUARD);
    DisplayState("M_ATTACK", m_iCurMainState & CAT::M_ATTACK);
    DisplayState("M_MOVE", m_iCurMainState & CAT::M_MOVE);
    DisplayState("M_LOCKON", m_iCurMainState & CAT::M_LOCKON);
    DisplayState("M_INTERACT", m_iCurMainState & CAT::M_INTERACT);
    DisplayState("M_WEAPON_CHANGE", m_iCurMainState & CAT::M_WEAPON_CHANGE);
    DisplayState("M_IDLE", m_iCurMainState & CAT::M_IDLE);

    ImGui::Unindent();

    ImGui::Separator();

    // Sub States (Movement)
    ImGui::Text("Sub States (0x%08X)", m_iCurSubState);
    ImGui::Indent();

    DisplayState("MOVE_WALK", m_iCurSubState & MOV::MOVE_WALK);
    DisplayState("MOVE_RUN", m_iCurSubState & MOV::MOVE_RUN);
    DisplayState("MOVE_SPRINT", m_iCurSubState & MOV::MOVE_SPRINT);
    DisplayState("MOVE_CLIMB", m_iCurSubState & MOV::MOVE_CLIMB);
    DisplayState("MOVE_MIRAGE_STEP", m_iCurSubState & MOV::MOVE_MIRAGE_STEP);
    DisplayState("MOVE_GETUP", m_iCurSubState & MOV::MOVE_GETUP);
    DisplayState("MOVE_FALL", m_iCurSubState & MOV::MOVE_FALL);
    DisplayState("MOVE_DODGE", m_iCurSubState & MOV::MOVE_DODGE);
    DisplayState("MOVE_INJURED", m_iCurSubState & MOV::MOVE_INJURED);

    ImGui::Unindent();

    ImGui::EndChild();

    ImGui::Spacing();

    // === 상태 변화 추적 ===
    ImGui::SeparatorText("State Changes");

    ImGui::BeginTable("StateChanges", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg);
    ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, 80.0f);
    ImGui::TableSetupColumn("Previous", ImGuiTableColumnFlags_WidthFixed, 150.0f);
    ImGui::TableSetupColumn("Current", ImGuiTableColumnFlags_WidthFixed, 150.0f);
    ImGui::TableHeadersRow();

    ImGui::TableNextRow();
    ImGui::TableNextColumn(); ImGui::Text("Main");
    ImGui::TableNextColumn(); ImGui::Text("%s", GetStateName(m_iPrevMainState));
    ImGui::TableNextColumn();
    if (m_iPrevMainState != m_iCurMainState)
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "%s", GetStateName(m_iCurMainState));
    else
        ImGui::Text("%s", GetStateName(m_iCurMainState));

    ImGui::TableNextRow();
    ImGui::TableNextColumn(); ImGui::Text("Sub");
    ImGui::TableNextColumn(); ImGui::Text("%s", GetSubStateName(m_iPrevSubState));
    ImGui::TableNextColumn();
    if (m_iPrevSubState != m_iCurSubState)
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "%s", GetSubStateName(m_iCurSubState));
    else
        ImGui::Text("%s", GetSubStateName(m_iCurSubState));

    ImGui::EndTable();

    // === 무기 상태 ===
    ImGui::Spacing();
    ImGui::SeparatorText("Weapon Status");

    _uint currentStatus = m_iStatus;
    bool isBarehand = (currentStatus & BAREHAND) != 0;
    bool isSpear = (currentStatus & SPEAR) != 0;

    if (ImGui::Checkbox("Barehand", &isBarehand))
    {
        if (isBarehand) Add_Status(BAREHAND);
        else Remove_Status(BAREHAND);
    }
    ImGui::SameLine();
    if (ImGui::Checkbox("Spear", &isSpear))
    {
        if (isSpear) Add_Status(SPEAR);
        else Remove_Status(SPEAR);
    }
}

void CKhazan_Spear::Debug_Widget_Animation()
{
    ImGui::SeparatorText("Animation Info");

    if (!m_pBody || !m_pBody->Get_Model())
    {
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Model not available");
        return;
    }

    CModel* pModel = m_pBody->Get_Model();

    // 현재 애니메이션 정보
    ImGui::BeginTable("AnimInfo", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg);
    ImGui::TableSetupColumn("Property", ImGuiTableColumnFlags_WidthFixed, 150.0f);
    ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);

    ImGui::TableNextRow();
    ImGui::TableNextColumn(); ImGui::Text("Current Anim Index");
    ImGui::TableNextColumn();
    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%d", m_iCurAnimIndex);

    ImGui::TableNextRow();
    ImGui::TableNextColumn(); ImGui::Text("Animation Name");
    ImGui::TableNextColumn();

    ImGui::TableNextRow();
    ImGui::TableNextColumn(); ImGui::Text("Track Position");
    ImGui::TableNextColumn();
    float trackPos = *pModel->Get_CurTrackPosition();
    ImGui::ProgressBar(trackPos / pModel->Get_CurDuration(),
        ImVec2(-1, 0),
        (std::to_string((_int)trackPos) + " / " +
            std::to_string((_int)pModel->Get_CurDuration())).c_str());

    ImGui::TableNextRow();
    ImGui::TableNextColumn(); ImGui::Text("Is Finished");
    ImGui::TableNextColumn();
    bool isFinished = m_pBody->Get_FinishedAnimation();
    if (isFinished)
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "YES");
    else
        ImGui::Text("NO");

    ImGui::TableNextRow();
    ImGui::TableNextColumn(); ImGui::Text("Reserved Anim");
    ImGui::TableNextColumn();
    ImGui::Text("%d", m_iReserveAnimIndex);

    ImGui::EndTable();

    ImGui::Spacing();

    // Move Animation 정보
    if (m_pAnimMove)
    {
        ImGui::SeparatorText("Move Animation State");

        ImGui::Text("Selected Animation: %d", m_pAnimMove->Get_AnimationIndex());
        ImGui::Text("Is Finished: %s", m_pAnimMove->Is_Finished() ? "YES" : "NO");
    }

    // 애니메이션 속도 조절
    ImGui::SeparatorText("Animation Control");

    static float animSpeed = 1.0f;
    if (ImGui::SliderFloat("Speed", &animSpeed, 0.1f, 3.0f))
    {
        // pModel->Set_AnimSpeed(animSpeed); // 모델에 속도 설정 함수가 있다면
    }

    if (ImGui::Button("Reset Speed"))
    {
        animSpeed = 1.0f;
    }
}

void CKhazan_Spear::Debug_Widget_Movement()
{
    // === 컨트롤 토글 ===
    ImGui::SeparatorText("Control");

    if (ImGui::Button(m_isEnableControl ? "Disable Control" : "Enable Control",
        ImVec2(-1.0f, 30.0f)))
    {
        m_isEnableControl = !m_isEnableControl;
    }

    ImGui::Spacing();

    // === 키 입력 가이드 ===
    ImGui::SeparatorText("Key Bindings");

    ImGui::BeginTable("KeyBindings", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg);
    ImGui::TableSetupColumn("Action", ImGuiTableColumnFlags_WidthFixed, 150.0f);
    ImGui::TableSetupColumn("Key", ImGuiTableColumnFlags_WidthStretch);
    ImGui::TableHeadersRow();

    auto AddKeyRow = [](const char* action, const char* key) {
        ImGui::TableNextRow();
        ImGui::TableNextColumn(); ImGui::Text("%s", action);
        ImGui::TableNextColumn(); ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "%s", key);
        };

    AddKeyRow("Move Forward", "W");
    AddKeyRow("Move Backward", "S");
    AddKeyRow("Move Left", "A");
    AddKeyRow("Move Right", "D");
    AddKeyRow("Walk", "W/A/S/D + LALT");
    AddKeyRow("Dodge", "SPACE (Tap)");
    AddKeyRow("Sprint", "SPACE (Hold > 0.15s)");
    AddKeyRow("Teleport", "LSHIFT + Mouse LB");

    ImGui::EndTable();

    ImGui::Spacing();

    // === 이동 정보 ===
    ImGui::SeparatorText("Movement Info");

    ImGui::DragFloat("Move Speed", &m_fMoveSpeed, 0.1f, 0.f, 100.f);
    ImGui::ProgressBar(m_fSprintTime / m_fMinSprintTime,
        ImVec2(-1, 0),
        ("Sprint Charge: " + std::to_string((_int)(m_fSprintTime * 100.0f)) + "%").c_str());

    ImGui::Spacing();

    // === 방향 시각화 ===
    ImGui::SeparatorText("Direction Visualization");

    ImVec2 canvas_pos = ImGui::GetCursorScreenPos();
    ImVec2 canvas_size = ImVec2(150, 150);
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    ImVec2 center = ImVec2(canvas_pos.x + canvas_size.x * 0.5f,
        canvas_pos.y + canvas_size.y * 0.5f);
    float radius = 60.0f;

    // 배경 원
    draw_list->AddCircle(center, radius, IM_COL32(100, 100, 100, 255), 32, 2.0f);

    // 방향 표시
    using DIR = DIRECTION_INFO::DIR;
    if (m_eDir.Check_Flag(DIR::F))
        draw_list->AddLine(center,
            ImVec2(center.x, center.y - radius),
            IM_COL32(0, 255, 0, 255), 3.0f);
    if (m_eDir.Check_Flag(DIR::B))
        draw_list->AddLine(center,
            ImVec2(center.x, center.y + radius),
            IM_COL32(0, 255, 0, 255), 3.0f);
    if (m_eDir.Check_Flag(DIR::L))
        draw_list->AddLine(center,
            ImVec2(center.x - radius, center.y),
            IM_COL32(0, 255, 0, 255), 3.0f);
    if (m_eDir.Check_Flag(DIR::R))
        draw_list->AddLine(center,
            ImVec2(center.x + radius, center.y),
            IM_COL32(0, 255, 0, 255), 3.0f);

    // 중심점
    draw_list->AddCircleFilled(center, 5.0f, IM_COL32(255, 255, 0, 255));

    // 텍스트
    draw_list->AddText(ImVec2(center.x - 5, center.y - radius - 20),
        IM_COL32(255, 255, 255, 255), "F");
    draw_list->AddText(ImVec2(center.x - 5, center.y + radius + 5),
        IM_COL32(255, 255, 255, 255), "B");
    draw_list->AddText(ImVec2(center.x - radius - 15, center.y - 7),
        IM_COL32(255, 255, 255, 255), "L");
    draw_list->AddText(ImVec2(center.x + radius + 5, center.y - 7),
        IM_COL32(255, 255, 255, 255), "R");

    ImGui::Dummy(canvas_size);

    ImGui::Spacing();

    // === 캐릭터 스탯 ===
    ImGui::SeparatorText("Character Stats");

    // HP
    ImGui::Text("Health");
    ImGui::SliderFloat("##CurrentHP", &m_fCurrentHP, 0.0f, m_fMaxHP, "%.1f");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(80);
    ImGui::InputFloat("##MaxHP", &m_fMaxHP, 0, 0, "%.0f");
    ImGui::ProgressBar(m_fCurrentHP / max(0.0001f, m_fMaxHP),
        ImVec2(-1.0f, 0.0f), "");

    // Stamina
    ImGui::Text("Stamina");
    ImGui::SliderFloat("##CurrentStamina", &m_fCurrentStamina, 0.0f, m_fMaxStamina, "%.1f");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(80);
    ImGui::InputFloat("##MaxStamina", &m_fMaxStamina, 0, 0, "%.0f");
    ImGui::ProgressBar(m_fCurrentStamina / max(0.0001f, m_fMaxStamina),
        ImVec2(-1.0f, 0.0f), "");

    // Attack
    ImGui::Text("Attack Power");
    ImGui::SliderFloat("##Attack", &m_fAttack, 0.0f, 500.0f, "%.1f");
}

// === 헬퍼 함수들 ===
const char* CKhazan_Spear::GetStateName(_uint state)
{
    if (state == 0) return "NONE";

    static std::string result;
    result.clear();

    if (state & CAT::M_DIE) result += "DIE | ";
    if (state & CAT::M_HOLD) result += "HOLD | ";
    if (state & CAT::M_GROGGY) result += "GROGGY | ";
    if (state & CAT::M_DAMAGED) result += "DAMAGED | ";
    if (state & CAT::M_CLIMB) result += "CLIMB | ";
    if (state & CAT::M_SKILL) result += "SKILL | ";
    if (state & CAT::M_GUARD) result += "GUARD | ";
    if (state & CAT::M_ATTACK) result += "ATTACK | ";
    if (state & CAT::M_MOVE) result += "MOVE | ";
    if (state & CAT::M_LOCKON) result += "LOCKON | ";
    if (state & CAT::M_INTERACT) result += "INTERACT | ";
    if (state & CAT::M_WEAPON_CHANGE) result += "WEAPON_CHANGE | ";
    if (state & CAT::M_IDLE) result += "IDLE | ";

    if (!result.empty())
        result = result.substr(0, result.length() - 3); // 마지막 " | " 제거

    return result.c_str();
}

const char* CKhazan_Spear::GetSubStateName(_uint subState)
{
    if (subState == 0) return "NONE";

    static std::string result;
    result.clear();

    if (subState & MOV::MOVE_WALK) result += "WALK | ";
    if (subState & MOV::MOVE_RUN) result += "RUN | ";
    if (subState & MOV::MOVE_SPRINT) result += "SPRINT | ";
    if (subState & MOV::MOVE_CLIMB) result += "CLIMB | ";
    if (subState & MOV::MOVE_MIRAGE_STEP) result += "MIRAGE_STEP | ";
    if (subState & MOV::MOVE_GETUP) result += "GETUP | ";
    if (subState & MOV::MOVE_FALL) result += "FALL | ";
    if (subState & MOV::MOVE_DODGE) result += "DODGE | ";
    if (subState & MOV::MOVE_INJURED) result += "INJURED | ";

    if (!result.empty())
        result = result.substr(0, result.length() - 3);

    return result.c_str();
}

const char* CKhazan_Spear::GetCycleName(_uint cycle)
{
    if (cycle == 0) return "NONE";

    static std::string result;
    result.clear();

    if (cycle & CYC::CYCLE_START) result += "START | ";
    if (cycle & CYC::CYCLE_LOOP) result += "LOOP | ";
    if (cycle & CYC::CYCLE_END) result += "END | ";
    if (cycle & CYC::CYCLE_ENDSTART) result += "ENDSTART | ";
    if (cycle & CYC::CYCLE_ENDEND) result += "ENDEND | ";
    if (cycle & CYC::CYCLE_BREAK) result += "BREAK | ";
    if (cycle & CYC::CYCLE_SHOT) result += "SHOT | ";
    if (cycle & CYC::CYCLE_FAIL) result += "FAIL | ";

    if (!result.empty())
        result = result.substr(0, result.length() - 3);

    return result.c_str();
}

std::string CKhazan_Spear::GetDirectionString()
{
    using DIR = DIRECTION_INFO::DIR;

    if (m_eDir.iDirFlag == 0) return "NONE";

    std::string result;

    if (m_eDir.Check_Flag(DIR::F)) result += "F";
    if (m_eDir.Check_Flag(DIR::B)) result += "B";
    if (m_eDir.Check_Flag(DIR::L)) result += "L";
    if (m_eDir.Check_Flag(DIR::R)) result += "R";

    return result;
}

#endif // _DEBUG
CKhazan_Spear* CKhazan_Spear::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CKhazan_Spear* pInstance = new CKhazan_Spear(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CKhazan_Spear"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CKhazan_Spear::Clone(void* pArg)
{
    CKhazan_Spear* pInstance = new CKhazan_Spear(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Clone : CKhazan_Spear"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CKhazan_Spear::Free()
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

    // Safe_Release(m_pCharVirCom);

     //Safe_Release(m_pASMachine);
    // Safe_Release(m_pASManager);
}
