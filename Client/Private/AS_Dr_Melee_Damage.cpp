#include "AS_Dr_Melee_Damage.h"
#include "GameInstance.h"
CAS_Dr_Melee_Damage::CAS_Dr_Melee_Damage()
{
}

void CAS_Dr_Melee_Damage::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    if (m_pMonData == nullptr)
        m_pMonData = &static_cast<CDragonian_Melee*>(pOwner)->Get_Data();

    if (m_pMonData->isSleep)
        m_pMonData->isSleep = false;

    if(m_pMonData->eHitType == HITREACTION::KNOCKBACK_NORMAL)
        m_pMonData->iAnimIndex = m_pGameInstance->Rand(1, 2);
    else  if (m_pMonData->eHitType == HITREACTION::KNOCKBACK_WEAK)
        m_pMonData->iAnimIndex = 14;
    else  if (m_pMonData->eHitType == HITREACTION::KNOCKBACK_STRONG)
        m_pMonData->iAnimIndex = m_pGameInstance->Rand(8, 9);

    m_pMonData->eHitType = HITREACTION::END;
    m_IsDamage = true;
}

void CAS_Dr_Melee_Damage::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    if (m_pMonData->eHitType != HITREACTION::END && m_IsDamage)
    {
        m_IsDamage = false;
        m_pMonData->iAnimIndex = 51;
    }
    else if (!m_IsDamage)
    {
        if (m_pMonData->eHitType == HITREACTION::KNOCKBACK_NORMAL)
            m_pMonData->iAnimIndex == 1 ? m_pMonData->iAnimIndex = 2 : m_pMonData->iAnimIndex = 1;
        else  if (m_pMonData->eHitType == HITREACTION::KNOCKBACK_WEAK)
            m_pMonData->iAnimIndex == 14 ? m_pMonData->iAnimIndex = m_pGameInstance->Rand(8, 9) : m_pMonData->iAnimIndex = 14;
        else  if (m_pMonData->eHitType == HITREACTION::KNOCKBACK_STRONG)
            m_pMonData->iAnimIndex == 8 ? m_pMonData->iAnimIndex = 9 : m_pMonData->iAnimIndex = 8;
        m_IsDamage = true;
        m_pMonData->eHitType = HITREACTION::END;
    }
    else if (m_IsDamage)
    {
        if (m_pMonData->isAnimFinash)
        {
            m_pMonData->isDamage = false;
            m_pMonData->eHitType = HITREACTION::END;
        }
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
