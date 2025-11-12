#include "FSM_Imp_Range.h"
#include "AS_Idle_Imp_Range.h"
#include "AS_Sleep_Imp_Range.h"
#include "AS_Move_Imp_Range.h"
#include "AS_Attack_Imp_Range.h"
#include "AS_Boomarang_Imp_Range.h"
#include "AS_Hit_Imp_Range.h"
#include "AS_Dead_Imp_Range.h"

CFSM_Imp_Range::CFSM_Imp_Range()
{

}

HRESULT CFSM_Imp_Range::Initialize(CGameObject* pOwner)
{

    if (FAILED(Add_State(ENUM_CLASS(IMPRANGE_STATE::SLEEP), CAS_Sleep_Imp_Range::Create())))
        return E_FAIL;
    if (FAILED(Add_State(ENUM_CLASS(IMPRANGE_STATE::IDLE), CAS_Idle_Imp_Range::Create())))
        return E_FAIL;
    if (FAILED(Add_State(ENUM_CLASS(IMPRANGE_STATE::MOVE), CAS_Move_Imp_Range::Create())))
        return E_FAIL;
    if (FAILED(Add_State(ENUM_CLASS(IMPRANGE_STATE::MAGIC), CAS_Attack_Imp_Range::Create())))
        return E_FAIL;
    if (FAILED(Add_State(ENUM_CLASS(IMPRANGE_STATE::BOOMARANG), CAS_Boomarang_Imp_Range::Create())))
        return E_FAIL;
    if (FAILED(Add_State(ENUM_CLASS(IMPRANGE_STATE::HIT), CAS_Hit_Imp_Range::Create())))
        return E_FAIL;
    if (FAILED(Add_State(ENUM_CLASS(IMPRANGE_STATE::DEAD), CAS_Dead_Imp_Range::Create())))
        return E_FAIL;

    m_pCurrentState = m_States[ENUM_CLASS(IMPRANGE_STATE::SLEEP)];
    if (nullptr == m_pCurrentState)
        return E_FAIL;

    Safe_AddRef(m_pCurrentState);

    m_pCurrentState->Enter(this, pOwner);

    return S_OK;
}

void CFSM_Imp_Range::Update(CGameObject* pOwner, _float fTimeDelta)
{
    __super::Update(pOwner, fTimeDelta);
}

CFSM_Imp_Range* CFSM_Imp_Range::Create(CGameObject* pOwner)
{
    CFSM_Imp_Range* pInstance = new CFSM_Imp_Range();
    if (FAILED(pInstance->Initialize(pOwner)))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Failed Create : CFSM_Imp_Range"));
    }
    return pInstance;
}

void CFSM_Imp_Range::Free()
{
    __super::Free();
}
