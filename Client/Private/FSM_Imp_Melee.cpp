#include "FSM_Imp_Melee.h"
#include "AS_Sleep_Imp_Melee.h"
#include "AS_Idle_Imp_Melee.h"
#include "AS_Move_Imp_Melee.h"

CFSM_Imp_Melee::CFSM_Imp_Melee()
{
}

HRESULT CFSM_Imp_Melee::Initialize(CGameObject* pOwner)
{
    if (FAILED(Add_State(ENUM_CLASS(IMPMELEE_STATE::SLEEP), CAS_Sleep_Imp_Melee::Create())))
        return E_FAIL;
    if (FAILED(Add_State(ENUM_CLASS(IMPMELEE_STATE::IDLE), AS_Idle_Imp_Melee::Create())))
        return E_FAIL;
    if (FAILED(Add_State(ENUM_CLASS(IMPMELEE_STATE::MOVE), CAS_Move_Imp_Melee::Create())))
        return E_FAIL;

    return S_OK;
}

void CFSM_Imp_Melee::Update(CGameObject* pOwner, _float fTimeDelta)
{
    __super::Update(pOwner, fTimeDelta);
}

CFSM_Imp_Melee* CFSM_Imp_Melee::Create(CGameObject* pOwner)
{
    CFSM_Imp_Melee* pInstance = new CFSM_Imp_Melee();
    if (FAILED(pInstance->Initialize(pOwner)))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Failed Create : CFSM_Imp_Melee"));
    }
    return pInstance;
}

void CFSM_Imp_Melee::Free()
{
    __super::Free();
}
