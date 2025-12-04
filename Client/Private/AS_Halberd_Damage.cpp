#include "AS_Halberd_Damage.h"

CAS_Halberd_Damage::CAS_Halberd_Damage()
{
}

void CAS_Halberd_Damage::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    if (m_pMonData == nullptr)
        m_pMonData = &static_cast<CHalberd*>(pOwner)->Get_Data();

    TARGET_DIR eDIr = m_pMonData->pOwner->Get_DIR();
    if (m_pMonData->eHitType == HITREACTION::KNOCKBACK_NORMAL)
    {
        if (eDIr == TARGET_DIR::F || eDIr == TARGET_DIR::FL || eDIr == TARGET_DIR::FR)
            m_pMonData->iAnimIndex = 19;
        else if (eDIr == TARGET_DIR::L)
            m_pMonData->iAnimIndex = 20;
        else if (eDIr == TARGET_DIR::R)
            m_pMonData->iAnimIndex = 21;
        else
            m_pMonData->iAnimIndex = 18;
    }
    else  if (m_pMonData->eHitType == HITREACTION::KNOCKBACK_WEAK)
    {
        if (eDIr == TARGET_DIR::L)
            m_pMonData->iAnimIndex = 28;
        else if (eDIr == TARGET_DIR::R)
            m_pMonData->iAnimIndex = 29;
        else
            m_pMonData->iAnimIndex = 27;
    }
    else  if (m_pMonData->eHitType == HITREACTION::KNOCKBACK_STRONG)
    {
        if (eDIr == TARGET_DIR::L)
            m_pMonData->iAnimIndex = 25;
        else if (eDIr == TARGET_DIR::R)
            m_pMonData->iAnimIndex = 26;
        else
            m_pMonData->iAnimIndex = 23;
    }

}

void CAS_Halberd_Damage::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    if (m_pMonData->isAnimFinash)
    {
        m_pMonData->isDamage = false;
        m_pMonData->eHitType = HITREACTION::END;
    }
}

void CAS_Halberd_Damage::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
}

CAS_Halberd_Damage* CAS_Halberd_Damage::Create()
{
    return new CAS_Halberd_Damage();
}

void CAS_Halberd_Damage::Free()
{
    __super::Free();
}
