#include "AS_Dr_Melee_Dead.h"

CAS_Dr_Melee_Dead::CAS_Dr_Melee_Dead()
{
}

void CAS_Dr_Melee_Dead::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    if (m_pMonData == nullptr)
        m_pMonData = &static_cast<CDragonian_Melee*>(pOwner)->Get_Data();

    m_pMonData->iAnimIndex = 19;
    m_eState = DIE;
}

void CAS_Dr_Melee_Dead::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
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

void CAS_Dr_Melee_Dead::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
}

CAS_Dr_Melee_Dead* CAS_Dr_Melee_Dead::Create()
{
    return new CAS_Dr_Melee_Dead();
}

void CAS_Dr_Melee_Dead::Free()
{
    __super::Free();
}
