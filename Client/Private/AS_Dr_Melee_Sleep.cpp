#include "AS_Dr_Melee_Sleep.h"
#include "Dragonian_Melee.h"
#include "GameInstance.h"

CAS_Dr_Melee_Sleep::CAS_Dr_Melee_Sleep()
{
}

void CAS_Dr_Melee_Sleep::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    if (m_pMonData == nullptr)
        m_pMonData = &static_cast<CDragonian_Melee*>(pOwner)->Get_Data();
    
    m_pMonData->pOwner->Hp_Visivle(false);
    m_pMonData->isMotionSleep ? m_pMonData->iAnimIndex = 34 : m_pMonData->iAnimIndex = 41;

    m_eState = SLEEP;
}

void CAS_Dr_Melee_Sleep::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    if (m_eState == SLEEP && m_pMonData->isSleep)
    {
        m_pGameInstance->PlaySoundOnce(TEXT("Mon_dragonian_foley_rattle_a_02 (SFX).wav"), pOwner->Get_Position());
    }
    else if (m_eState == SLEEP && !m_pMonData->isSleep)
    {
        m_pGameInstance->PlaySoundOnce(TEXT("Mon_dragonian_wakeup_01 (SFX).wav"), pOwner->Get_Position(), m_pMonData->pOwner->Get_SoundChannel(0));
        m_pMonData->isMotionSleep ? m_pMonData->iAnimIndex = 32 : m_pMonData->iAnimIndex = 39;
        m_pMonData->pOwner->Hp_Visivle(true);
        m_eState = GETUP;
    }
    else if (m_eState == GETUP && !m_pMonData->isSleep)
    {
        if (m_pMonData->isAnimFinash || m_pMonData->eHitType != HITREACTION::END)
            m_pMonData->isStateFiash = true;
    }
}

void CAS_Dr_Melee_Sleep::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
    m_pMonData->fAttackCool = 1.5f;
    m_pMonData->isStateFiash = false;
    m_pMonData->pOwner->Hp_Visivle(true);
    m_eState = GETUP;

}

CAS_Dr_Melee_Sleep* CAS_Dr_Melee_Sleep::Create()
{
    return new CAS_Dr_Melee_Sleep();
}

void CAS_Dr_Melee_Sleep::Free()
{
    __super::Free();
}
