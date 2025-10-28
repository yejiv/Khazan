#include "FSM_Yetuga.h"
#include "IdleState_Yetuga.h"
#include "MoveState_Yetuga.h"
#include "AttackState_Yetuga.h"
#include "AS_RightHand_5Hit_Yetuga.h"
#include "AS_LieDown_Yetuga.h"

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

    m_pCurrentState = m_States[ENUM_CLASS(YETUGA_STATE::IDLE)];
    if (nullptr == m_pCurrentState)
        return E_FAIL;

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
