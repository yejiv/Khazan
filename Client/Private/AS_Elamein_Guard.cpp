#include "AS_Elamein_Guard.h"

CAS_Elamein_Guard::CAS_Elamein_Guard()
{
}

void CAS_Elamein_Guard::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    if (m_pMonData == nullptr)
        m_pMonData = &static_cast<CElamein*>(pOwner)->Get_Data();

    m_pMonData->pOwner->Hp_Visivle(false);
    m_pMonData->iAnimIndex = 48;
    m_eState = START;

}

void CAS_Elamein_Guard::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    //m_pMonData->pOwner->LockOnLerp(fTimeDelta);
    if (m_eState == START)
    {
        if (m_pMonData->isAnimFinash)
        {
            m_pMonData->iAnimIndex = 47;
            m_eState = LOOP;
        }

    }
    else if (m_eState == LOOP)
    {
        m_fAcctime += fTimeDelta;
        if (m_pMonData->eHitType != HITREACTION::END && m_pMonData->eHitType != HITREACTION::BRUTAL_ATTACK)
        {
            m_pMonData->iAnimIndex = 51;
            m_eState = COUNT;
        }
        else if (m_fAcctime >= 3.f)
        {
            m_pMonData->iAnimIndex = 46;
            m_eState = END;
        }
    }
    else if (m_eState == COUNT)
    {
        if (m_pMonData->isAnimFinash)
        {
            m_pMonData->isGuard = false;
        }
    }
    else if (m_eState == END)
    {
        if (m_pMonData->isAnimFinash)
        {
            m_pMonData->isGuard = false;
        }
    }
}

void CAS_Elamein_Guard::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
    m_pMonData->fGuardCool = 20.f;
    m_fAcctime = 0.f;
    m_pMonData->eHitType = HITREACTION::END;
}

CAS_Elamein_Guard* CAS_Elamein_Guard::Create()
{
    return new CAS_Elamein_Guard();
}

void CAS_Elamein_Guard::Free()
{
    __super::Free();
}
