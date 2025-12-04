#include "AS_Halberd_Brutal.h "

CAS_Halberd_Brutal::CAS_Halberd_Brutal()
{
}

void CAS_Halberd_Brutal::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    if (m_pMonData == nullptr)
        m_pMonData = &static_cast<CHalberd*>(pOwner)->Get_Data();

    TARGET_DIR eDir = m_pMonData->pOwner->Get_DIR();

    if (eDir == TARGET_DIR::B || eDir == TARGET_DIR::BR || eDir == TARGET_DIR::BL)
    {
        m_pMonData->iAnimIndex = 16;
        m_isF = false;
    }
    else
    {
        m_pMonData->iAnimIndex = 14;
        m_isF = true;
    }
    m_fAccTime = 0.3f;
    m_eState = START;
}

void CAS_Halberd_Brutal::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    if (m_isF)
    {
        if (m_eState == START)
        {
            m_fAccTime -= fTimeDelta;
            if (m_pMonData->iBrutalHit >= 2 && m_fAccTime <= 0.f)
            {
                m_pMonData->iAnimIndex = 15;
                m_eState = END;
            }
        }
        else
        {
            if (m_pMonData->isAnimFinash)
            {
                m_pMonData->eHitType = HITREACTION::END;
                m_pMonData->iBrutalHit = 0;
                m_pMonData->isStamina_Regen = true;
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
                m_pMonData->iAnimIndex = 17;
                m_eState = END;
            }
        }
        else
        {
            if (m_pMonData->isAnimFinash)
            {
                m_pMonData->eHitType = HITREACTION::END;
                m_pMonData->iBrutalHit = 0;
                m_pMonData->isStamina_Regen = true;
            }
        }
    }
}

void CAS_Halberd_Brutal::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
    m_pMonData->iBrutalHit = 0;
}

CAS_Halberd_Brutal* CAS_Halberd_Brutal::Create()
{
    return new CAS_Halberd_Brutal();
}

void CAS_Halberd_Brutal::Free()
{
    __super::Free();
}
