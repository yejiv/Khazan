#include "AS_Elamein_Attack_Enchant.h"

CAS_Elamein_Attack_Enchant::CAS_Elamein_Attack_Enchant()
{
}

void CAS_Elamein_Attack_Enchant::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    if (m_pMonData == nullptr)
        m_pMonData = &static_cast<CElamein*>(pOwner)->Get_Data();

    m_pMonData->iAnimIndex = 89;
    m_eState = START;
}

void CAS_Elamein_Attack_Enchant::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    if (m_eState == START)
    {
        if (m_pMonData->isAnimFinash)
        {
            m_pMonData->iAnimIndex = 88;
            m_eState = LOOP;
        }
    }
    else if (m_eState == LOOP)
    {
        if (m_pMonData->pOwner->Check_Ranage("AttackRange"))
        {
            m_pMonData->iAnimIndex = 83;
            m_eState = END;
        }
    }
    else if (m_eState == END)
    {
        if (m_pMonData->isAnimFinash)
        {
            m_pMonData->eAttackState = CElamein::ATTACKSTATE::END;
        }
    }

}

void CAS_Elamein_Attack_Enchant::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
    m_pMonData->fSpecial_AttackCool = 30.f;
}

CAS_Elamein_Attack_Enchant* CAS_Elamein_Attack_Enchant::Create()
{
    return new CAS_Elamein_Attack_Enchant();
}

void CAS_Elamein_Attack_Enchant::Free()
{
    __super::Free();
}
