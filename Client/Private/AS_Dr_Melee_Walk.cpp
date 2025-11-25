#include "AS_Dr_Melee_Walk.h"

CAS_Dr_Melee_Walk::CAS_Dr_Melee_Walk()
{
}

void CAS_Dr_Melee_Walk::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    if (m_pMonData == nullptr)
        m_pMonData = &static_cast<CDragonian_Melee*>(pOwner)->Get_Data();

    m_pMonData->iAnimIndex = 58;
    m_eState = WALK;
}

void CAS_Dr_Melee_Walk::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    if (m_eState == WALK)
    {
        m_pMonData->pOwner->Move_F();
    }
}

void CAS_Dr_Melee_Walk::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
    m_eState = END;
}

CAS_Dr_Melee_Walk* CAS_Dr_Melee_Walk::Create()
{
    return new CAS_Dr_Melee_Walk();
}

void CAS_Dr_Melee_Walk::Free()
{
    __super::Free();
}
