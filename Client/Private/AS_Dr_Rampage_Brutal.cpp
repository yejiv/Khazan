#include "AS_Dr_Rampage_Brutal.h"

CAS_Dr_Rampage_Brutal::CAS_Dr_Rampage_Brutal()
{
}

void CAS_Dr_Rampage_Brutal::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    if (m_pMonData == nullptr)
        m_pMonData = &static_cast<CDragonian_Rampage*>(pOwner)->Get_Data();
    TARGET_DIR eDir = m_pMonData->pOwner->Get_DIR();
    if (eDir == TARGET_DIR::B || eDir == TARGET_DIR::BR || eDir == TARGET_DIR::BL)
    {
        m_pMonData->iAnimIndex = 4;
        m_isF = false;
    }
    else
    {
        m_pMonData->iAnimIndex = 28;
        m_isF = true;
    }


    m_fAccTime = 0.3f;
    m_eState = START;
}

void CAS_Dr_Rampage_Brutal::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    if (m_isF)
    {
        if (m_eState == START)
        {
            m_fAccTime -= fTimeDelta;
            if (m_pMonData->iBrutalHit >= 2 && m_fAccTime <= 0.f)
            {
                m_pMonData->iAnimIndex = 29;
                m_eState = END;
            }
        }
        else
        {
            if (m_pMonData->isAnimFinash)
            {
                m_pMonData->eHitType = HITREACTION::END;
                *m_pMonData->pCulStamina = *m_pMonData->pMaxStamina;
                m_pMonData->iBrutalHit = 0;
            }
        }
    }
    else
    {
        if (m_eState == START)
        {
            m_fAccTime -= fTimeDelta;
            if (m_pMonData->iBrutalHit >= 2 && m_fAccTime <= 0.f)
            {
                m_pMonData->iAnimIndex = 5;
                m_eState = END;
            }
        }
        else
        {
            if (m_pMonData->isAnimFinash)
            {
                m_pMonData->eHitType = HITREACTION::END;
                *m_pMonData->pCulStamina = *m_pMonData->pMaxStamina;
                m_pMonData->iBrutalHit = 0;
            }
        }
    }
}

void CAS_Dr_Rampage_Brutal::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
}

CAS_Dr_Rampage_Brutal* CAS_Dr_Rampage_Brutal::Create()
{
    return new CAS_Dr_Rampage_Brutal();
}

void CAS_Dr_Rampage_Brutal::Free()
{
    __super::Free();
}
