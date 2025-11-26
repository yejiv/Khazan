#include "AS_Dr_Rampage_Dead.h"

CAS_Dr_Rampage_Dead::CAS_Dr_Rampage_Dead()
{
}

void CAS_Dr_Rampage_Dead::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    if (m_pMonData == nullptr)
        m_pMonData = &static_cast<CDragonian_Rampage*>(pOwner)->Get_Data();

    m_pMonData->iAnimIndex = 0;
    m_eState = DIE;
}

void CAS_Dr_Rampage_Dead::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
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

void CAS_Dr_Rampage_Dead::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
}

CAS_Dr_Rampage_Dead* CAS_Dr_Rampage_Dead::Create()
{
    return new CAS_Dr_Rampage_Dead();
}

void CAS_Dr_Rampage_Dead::Free()
{
    __super::Free();
}
