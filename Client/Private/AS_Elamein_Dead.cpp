#include "AS_Elamein_Dead.h"

CAS_Elamein_Dead::CAS_Elamein_Dead()
{
}

void CAS_Elamein_Dead::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    if (m_pMonData == nullptr)
        m_pMonData = &static_cast<CElamein*>(pOwner)->Get_Data();

    TARGET_DIR eDir = m_pMonData->pOwner->Get_DIR();

    m_pMonData->iAnimIndex = 29;
    m_eState = DIE;

}

void CAS_Elamein_Dead::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    if (m_eState == DIE)
    {
        if (m_pMonData->isAnimFinash)
        {
            m_eState = RELEASSE;
            //작업 완료 후 살려야함
            //m_pMonData->pOwner->Hp_Dead();
        }
    }
    else if (m_eState == RELEASSE)
    {
    }
}

void CAS_Elamein_Dead::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
}

CAS_Elamein_Dead* CAS_Elamein_Dead::Create()
{
    return new CAS_Elamein_Dead();
}

void CAS_Elamein_Dead::Free()
{
    __super::Free();
}
