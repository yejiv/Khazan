#include "AS_Dr_Rampage_Sleep.h"
#include "GameInstance.h"

CAS_Dr_Rampage_Sleep::CAS_Dr_Rampage_Sleep()
{
}

void CAS_Dr_Rampage_Sleep::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    if (m_pMonData == nullptr)
        m_pMonData = &static_cast<CDragonian_Rampage*>(pOwner)->Get_Data();

    m_pMonData->pOwner->Hp_Visivle(false);
    m_pMonData->isMotionSleep ? m_pMonData->iAnimIndex = 8 : m_pMonData->iAnimIndex = 9;
    m_eState = SLEEP;

}

void CAS_Dr_Rampage_Sleep::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    if (m_eState == SLEEP && m_pMonData->isSleep)
    {
        m_pGameInstance->PlaySoundOnce(TEXT("Mon_dragonian_foley_rattle_a_02 (SFX).wav"), pOwner->Get_Position());
    }
    else if (m_eState == SLEEP && !m_pMonData->isSleep)
    {
        m_pGameInstance->PlaySoundOnce(TEXT("Mon_dragonian_wakeup_01 (SFX).wav"), pOwner->Get_Position(), m_pMonData->pOwner->Get_SoundChannel(0));
        m_pMonData->isMotionSleep ? m_pMonData->iAnimIndex = 6 : m_pMonData->iAnimIndex = 7;
        m_pMonData->pOwner->Hp_Visivle(true);
        m_eState = GETUP;
    }
    else if (m_eState == GETUP)
    {
        if (m_pMonData->isAnimFinash)
            m_pMonData->isStateFiash = true;
    }
}

void CAS_Dr_Rampage_Sleep::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
}

CAS_Dr_Rampage_Sleep* CAS_Dr_Rampage_Sleep::Create()
{
    return new CAS_Dr_Rampage_Sleep();
}

void CAS_Dr_Rampage_Sleep::Free()
{
    __super::Free();
}
