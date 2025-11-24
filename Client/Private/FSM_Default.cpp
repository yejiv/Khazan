#include "FSM_Default.h"
#include "AS_Sleep_Imp_Melee.h"
#include "AS_Idle_Imp_Melee.h"
#include "AS_Move_Imp_Melee.h"
#include "AS_ChainSmash_Imp_Melee.h"
#include "AS_NonStopAttack_Imp_Melee.h"
#include "AS_DashAttack_Imp_Melee.h"
#include "AS_Hit_Imp_Melee.h"
#include "AS_Dead_Imp_Melee.h"


CFSM_Default::CFSM_Default()
{
}

HRESULT CFSM_Default::Add_State(_uint iStateIndex, CAI_State* pState)
{
    return __super::Add_State(iStateIndex, pState);
}

HRESULT CFSM_Default::Initialize(CGameObject* pOwner)
{
    return S_OK;
}

void CFSM_Default::Update(CGameObject* pOwner, _float fTimeDelta)
{
    __super::Update(pOwner, fTimeDelta);
}

CFSM_Default* CFSM_Default::Create(CGameObject* pOwner)
{
    CFSM_Default* pInstance = new CFSM_Default();
    if (FAILED(pInstance->Initialize(pOwner)))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Failed Create : CFSM_Default"));
    }
    return pInstance;
}

void CFSM_Default::Free()
{
    __super::Free();
}
