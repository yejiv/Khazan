#include "AS_Elamein_Attack_Long.h"

CAS_Elamein_Attack_Long::CAS_Elamein_Attack_Long()
{
}

void CAS_Elamein_Attack_Long::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    if (m_pMonData == nullptr)
        m_pMonData = &static_cast<CElamein*>(pOwner)->Get_Data();

    m_isSword ? m_isSword = false : m_isSword = true;

    if (m_isSword)
    {
        m_pMonData->iAnimIndex = 96;
    }
    else
    {
        m_pMonData->iAnimIndex = 97;
    }

    m_eState = ATTACK_1;

}

void CAS_Elamein_Attack_Long::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    if (m_isSword)
    {
        if (m_eState == ATTACK_1)
        {
            if (m_pMonData->isAnimFinash)
            {
                m_pMonData->iAnimIndex = 95;
                m_eState = ATTACK_2;
            }
        }
        else if (m_eState == ATTACK_2)
        {
            if (m_pMonData->isAnimFinash)
            {
                m_pMonData->eAttackState = CElamein::ATTACKSTATE::END;
            }
        }

    }
    else
    {
        if (m_eState == ATTACK_1)
        {
            if (m_pMonData->isAnimFinash)
            {
                m_pMonData->iAnimIndex = 78;
                m_eState = ATTACK_2;
            }
        }
        else if (m_eState == ATTACK_2)
        {
            if (m_pMonData->pOwner->Check_Ranage("AttackRange"))
            {
                m_pMonData->iAnimIndex = 77;
                m_eState = ATTACK_3;
            }
        }
        else if (m_eState == ATTACK_3)
        {
            if (m_pMonData->isAnimFinash)
            {
                m_pMonData->fQuat = 180.f;
                m_pMonData->iAnimIndex = 101;
                m_pMonData->eAttackState = CElamein::ATTACKSTATE::END;
            }
        }
    }

}

void CAS_Elamein_Attack_Long::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
}

CAS_Elamein_Attack_Long* CAS_Elamein_Attack_Long::Create()
{
    return new CAS_Elamein_Attack_Long();
}

void CAS_Elamein_Attack_Long::Free()
{
    __super::Free();
}
