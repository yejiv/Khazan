#include "FSM_Gomdol.h"
#include "AS_Sleep_Gomdol.h"
#include "AS_Idle_Gomdol.h"
#include "AS_Move_Gomdol.h"
#include "AS_FrontAttack_Gomdol.h"

CFSM_Gomdol::CFSM_Gomdol()
{
}

HRESULT CFSM_Gomdol::Initialize(CGameObject* pOwner)
{

    if (FAILED(Add_State(ENUM_CLASS(GOMDOL_STATE::SLEEP), CAS_Sleep_Gomdol::Create())))
        return E_FAIL;

    if (FAILED(Add_State(ENUM_CLASS(GOMDOL_STATE::IDLE), CAS_Idle_Gomdol::Create())))
        return E_FAIL;

    if (FAILED(Add_State(ENUM_CLASS(GOMDOL_STATE::MOVE), CAS_Move_Gomdol::Create())))
        return E_FAIL;

    if (FAILED(Add_State(ENUM_CLASS(GOMDOL_STATE::ATTACK), CAS_FrontAttack_Gomdol::Create())))
        return E_FAIL;

    m_pCurrentState = m_States[ENUM_CLASS(GOMDOL_STATE::SLEEP)];

    if (nullptr == m_pCurrentState)
        return E_FAIL;

    m_pCurrentState->Enter(this,pOwner);

    return S_OK;
}

void CFSM_Gomdol::Update(CGameObject* pOwner, _float fTimeDelta)
{
    __super::Update(pOwner, fTimeDelta);
}

CFSM_Gomdol* CFSM_Gomdol::Create(CGameObject* pOwner)
{
    CFSM_Gomdol* pInstance = new CFSM_Gomdol();
    if (FAILED(pInstance->Initialize(pOwner)))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Failed Create : CFSM_Gomdol"));
    }
    return pInstance;
}

void CFSM_Gomdol::Free()
{
    __super::Free();
}
