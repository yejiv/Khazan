#include "FSM_Yetuga.h"
#include "IdleState_Yetuga.h"
#include "MoveState_Yetuga.h"
#include "AttackState_Yetuga.h"
#include "AS_RightHand_5Hit_Yetuga.h"
#include "AS_LieDown_Yetuga.h"
#include "AS_Turn_Yetuga.h"
#include "AS_ThrowBall_Yetuga.h"
#include "AS_Hit_Yetuga.h"
#include "AS_Smash_Yetuga.h"
#include "AS_JumpAttack_Yetuga.h"
#include "AS_Rush_Yetuga.h"
#include "AS_Dodge_Yetuga.h"
#include "AS_JumpGrab_Yetuga.h"
#include "AS_Amageddon_Yetuga.h"
#include "AS_IceBreath_Yetuga.h"
#include "AS_Groggy_Yetuga.h"
#include "AS_Dead_Yetuga.h"
#include "AS_LockOn_Yetuga.h"

CFSM_Yetuga::CFSM_Yetuga()
{
}

HRESULT CFSM_Yetuga::Initialize()
{

    if (FAILED(Add_State(ENUM_CLASS(YETUGA_STATE::IDLE), CIdleState_Yetuga::Create())))
        return E_FAIL;
    if (FAILED(Add_State(ENUM_CLASS(YETUGA_STATE::MOVE), CMoveState_Yetuga::Create())))
        return E_FAIL;
    if (FAILED(Add_State(ENUM_CLASS(YETUGA_STATE::ATTACK), CAttackState_Yetuga::Create())))
        return E_FAIL;
    if (FAILED(Add_State(ENUM_CLASS(YETUGA_STATE::RIGHTHAND_5HIT), CAS_RightHand_5Hit_Yetuga::Create())))
        return E_FAIL;
    if (FAILED(Add_State(ENUM_CLASS(YETUGA_STATE::LIE_DOWN), CAS_LieDown_Yetuga::Create())))
        return E_FAIL;
    if (FAILED(Add_State(ENUM_CLASS(YETUGA_STATE::TURN), CAS_Turn_Yetuga::Create())))
        return E_FAIL;
    if (FAILED(Add_State(ENUM_CLASS(YETUGA_STATE::THROWBALL), CAS_ThrowBall_Yetuga::Create())))
        return E_FAIL;
    if(FAILED(Add_State(ENUM_CLASS(YETUGA_STATE::SMASH), CAS_Smash_Yetuga::Create())))
        return E_FAIL;
    if (FAILED(Add_State(ENUM_CLASS(YETUGA_STATE::JUMPATTACK), CAS_JumpAttack_Yetuga::Create())))
        return E_FAIL;
    if (FAILED(Add_State(ENUM_CLASS(YETUGA_STATE::JUMPGRAB), CAS_JumpGrab_Yetuga::Create())))
        return E_FAIL;
    if (FAILED(Add_State(ENUM_CLASS(YETUGA_STATE::RUSH), CAS_Rush_Yetuga::Create())))
        return E_FAIL;
    if (FAILED(Add_State(ENUM_CLASS(YETUGA_STATE::AMAGEDDON), CAS_Amageddon_Yetuga::Create())))
        return E_FAIL;
    if (FAILED(Add_State(ENUM_CLASS(YETUGA_STATE::ICEBREATH), CAS_IceBreath_Yetuga::Create())))
        return E_FAIL;
    if (FAILED(Add_State(ENUM_CLASS(YETUGA_STATE::LOCKON), CAS_LockOn_Yetuga::Create())))
        return E_FAIL;
    if (FAILED(Add_State(ENUM_CLASS(YETUGA_STATE::HIT), CAS_Hit_Yetuga::Create())))
        return E_FAIL;
    if (FAILED(Add_State(ENUM_CLASS(YETUGA_STATE::DODGE), CAS_Dodge_Yetuga::Create())))
        return E_FAIL;
    if (FAILED(Add_State(ENUM_CLASS(YETUGA_STATE::GROGGY), CAS_Groggy_Yetuga::Create())))
        return E_FAIL;
    if (FAILED(Add_State(ENUM_CLASS(YETUGA_STATE::DEAD), CAS_Dead_Yetuga::Create())))
        return E_FAIL;
    
    m_pCurrentState = m_States[ENUM_CLASS(YETUGA_STATE::IDLE)];
    Safe_AddRef(m_pCurrentState);
    if (nullptr == m_pCurrentState)
        return E_FAIL;
    Safe_AddRef(m_pCurrentState);

    return S_OK;
}

void CFSM_Yetuga::Update(CGameObject* pOwner, _float fTimeDelta)
{
    __super::Update(pOwner, fTimeDelta);
}



CFSM_Yetuga* CFSM_Yetuga::Create()
{
    CFSM_Yetuga* pInstance = new CFSM_Yetuga();
    if (FAILED(pInstance->Initialize()))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Failed Create : CFSM_Yetuga"));
    }
    return pInstance;
}

void CFSM_Yetuga::Free()
{
    __super::Free();
}
