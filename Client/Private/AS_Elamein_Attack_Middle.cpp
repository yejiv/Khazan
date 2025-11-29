#include "AS_Elamein_Attack_Middle.h"

CAS_Elamein_Attack_Middle::CAS_Elamein_Attack_Middle()
{
}

void CAS_Elamein_Attack_Middle::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    if (m_pMonData == nullptr)
        m_pMonData = &static_cast<CElamein*>(pOwner)->Get_Data();

    m_isAttack1 ? m_isAttack1 = false : m_isAttack1 = true;

    if (m_isAttack1)
    {
        m_pMonData->iAnimIndex = 79;
    }
    else
    {
        m_pMonData->iAnimIndex = 58;
    }

    m_eState = ATTACK_1;

}

void CAS_Elamein_Attack_Middle::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    if (m_isAttack1)
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
    else
    {
        if (m_eState == ATTACK_1)
        {
            if (m_pMonData->isAnimFinash)
            {
                m_pMonData->iAnimIndex = 57;
                m_eState = ATTACK_2;
            }
        }
        else if (m_eState == ATTACK_2)
        {
            if (m_pMonData->pOwner->Check_Ranage("AttackRange"))
            {
                m_pMonData->iAnimIndex = 56;
                m_eState = ATTACK_3;
            }
        }
        else if (m_eState == ATTACK_3)
        {
            if (m_pMonData->isAnimFinash)
            {
                //m_pMonData->fQuat = 180.f;
                //m_pMonData->iAnimIndex = 101;
                m_pMonData->eAttackState = CElamein::ATTACKSTATE::END;
            }
        }
    }
}

void CAS_Elamein_Attack_Middle::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
    m_pMonData->fAttackCool = 3.f;
}

CAS_Elamein_Attack_Middle* CAS_Elamein_Attack_Middle::Create()
{
    return new CAS_Elamein_Attack_Middle();
}

void CAS_Elamein_Attack_Middle::Free()
{
    __super::Free();
}
