#include "AS_Dr_Melee_Damage.h"

CAS_Dr_Melee_Damage::CAS_Dr_Melee_Damage()
{
}

void CAS_Dr_Melee_Damage::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    if (m_pMonData == nullptr)
        m_pMonData = &static_cast<CDragonian_Melee*>(pOwner)->Get_Data();

    m_pMonData->iAnimIndex = 1;
}

void CAS_Dr_Melee_Damage::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    if (m_pMonData->isAnimFinash)
    {
        m_pMonData->isDamage = false;
        m_pMonData->eHitType = HITREACTION::END;
    }
}

void CAS_Dr_Melee_Damage::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
}

CAS_Dr_Melee_Damage* CAS_Dr_Melee_Damage::Create()
{
    return new CAS_Dr_Melee_Damage();
}

void CAS_Dr_Melee_Damage::Free()
{
    __super::Free();
}
