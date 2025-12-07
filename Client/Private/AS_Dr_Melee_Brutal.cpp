#include "AS_Dr_Melee_Brutal.h"
#include "GameInstance.h"
CAS_Dr_Melee_Brutal::CAS_Dr_Melee_Brutal()
{
}

void CAS_Dr_Melee_Brutal::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    if (m_pMonData == nullptr)
        m_pMonData = &static_cast<CDragonian_Melee*>(pOwner)->Get_Data();

    m_pMonData->iAnimIndex = 17;

    m_fAccTime = 0.3f;
    m_eState = START;
}

void CAS_Dr_Melee_Brutal::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    if (m_eState == START)
    {
        m_fAccTime -= fTimeDelta;
        if (m_pMonData->iBrutalHit >= 2 && m_fAccTime <= 0.f)
        {
            m_pMonData->iAnimIndex = 18;
            m_eState = END;
        }
    }
    else
    {
        if (m_pMonData->isAnimFinash)
        {
            m_pMonData->eHitType = HITREACTION::END;
            m_pMonData->isStamina_Regen = true;
            m_pMonData->iBrutalHit = 0;
        }
    }
}

void CAS_Dr_Melee_Brutal::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
}

CAS_Dr_Melee_Brutal* CAS_Dr_Melee_Brutal::Create()
{
    return new CAS_Dr_Melee_Brutal();
}

void CAS_Dr_Melee_Brutal::Free()
{
    __super::Free();
}
