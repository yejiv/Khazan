#include "AS_Elamein_Brutal.h "

CAS_Elamein_Brutal::CAS_Elamein_Brutal()
{
}

void CAS_Elamein_Brutal::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    if (m_pMonData == nullptr)
        m_pMonData = &static_cast<CElamein*>(pOwner)->Get_Data();

    TARGET_DIR eDir = m_pMonData->pOwner->Get_DIR();

    if (eDir == TARGET_DIR::B || eDir == TARGET_DIR::BR || eDir == TARGET_DIR::BL)
    {
        m_pMonData->iAnimIndex = 8;
        m_isF = false;
    }
    else
    {
        m_pMonData->iAnimIndex = 6;
        m_isF = true;
    }
    m_fAccTime = 0.3f;
    m_eState = START;
}

void CAS_Elamein_Brutal::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    if (m_isF)
    {
        if (m_eState == START)
        {
            m_fAccTime -= fTimeDelta;
            if (m_pMonData->iBrutalHit >= 2)
            {
                m_pMonData->iAnimIndex = 7;
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
                *m_pMonData->pCulStamina = 0.f;
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
                m_pMonData->iAnimIndex = 9;
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
                *m_pMonData->pCulStamina = 0.f;
            }
        }
    }
}

void CAS_Elamein_Brutal::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
    m_pMonData->iBrutalHit = 0;
}

CAS_Elamein_Brutal* CAS_Elamein_Brutal::Create()
{
    return new CAS_Elamein_Brutal();
}

void CAS_Elamein_Brutal::Free()
{
    __super::Free();
}
