#include "AS_Dr_Rampage_Damage.h"
#include "GameInstance.h"

CAS_Dr_Rampage_Damage::CAS_Dr_Rampage_Damage()
{
}

void CAS_Dr_Rampage_Damage::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    if (m_pMonData == nullptr)
        m_pMonData = &static_cast<CDragonian_Rampage*>(pOwner)->Get_Data();

    TARGET_DIR eDIr = m_pMonData->pOwner->Get_DIR();
    if (m_pMonData->eHitType == HITREACTION::KNOCKBACK_NORMAL)
    {
        if (eDIr == TARGET_DIR::F || eDIr == TARGET_DIR::FL || eDIr == TARGET_DIR::FR)
            m_pMonData->iAnimIndex = 18;
        else if (eDIr == TARGET_DIR::L)
            m_pMonData->iAnimIndex = 19;
        else if (eDIr == TARGET_DIR::R)
            m_pMonData->iAnimIndex = 20;
        else
            m_pMonData->iAnimIndex = 17;
    }
    else  if (m_pMonData->eHitType == HITREACTION::KNOCKBACK_WEAK)
    {
        if (eDIr == TARGET_DIR::L)
            m_pMonData->iAnimIndex = 26;
        else if (eDIr == TARGET_DIR::R)
            m_pMonData->iAnimIndex = 27;
        else
            m_pMonData->iAnimIndex = 25;
    }
    else  if (m_pMonData->eHitType == HITREACTION::KNOCKBACK_STRONG)
    {
        if (eDIr == TARGET_DIR::L)
            m_pMonData->iAnimIndex = 23;
        else if (eDIr == TARGET_DIR::R)
            m_pMonData->iAnimIndex = 24;
        else
            m_pMonData->iAnimIndex = 22;
    }
}

void CAS_Dr_Rampage_Damage::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    if (m_pMonData->isAnimFinash)
    {
        m_pMonData->isDamage = false;
        m_pMonData->eHitType = HITREACTION::END;
    }
}

void CAS_Dr_Rampage_Damage::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
}

CAS_Dr_Rampage_Damage* CAS_Dr_Rampage_Damage::Create()
{
    return new CAS_Dr_Rampage_Damage();
}

void CAS_Dr_Rampage_Damage::Free()
{
    __super::Free();
}
