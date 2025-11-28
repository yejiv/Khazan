#include "AS_Dr_Rampage_Walk.h"
#include "AI_Controller.h"
CAS_Dr_Rampage_Walk::CAS_Dr_Rampage_Walk()
{
}

void CAS_Dr_Rampage_Walk::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    if (m_pMonData == nullptr)
        m_pMonData = &static_cast<CDragonian_Rampage*>(pOwner)->Get_Data();

    m_pMonData->iAnimIndex = 50;
    m_eState = WALK;
}

void CAS_Dr_Rampage_Walk::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    if(m_pMonData->pOwner->Check_Ranage("WalkRange"))
        m_pMonData->isLockOn = true;

    if (m_eState == WALK)
    {
        m_pMonData->pOwner->LockOnLerp(fTimeDelta);
        pOwner->Get_Transform()->Go_Straight(1.5f * fTimeDelta);
    }
}

void CAS_Dr_Rampage_Walk::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
}

CAS_Dr_Rampage_Walk* CAS_Dr_Rampage_Walk::Create()
{
    return new CAS_Dr_Rampage_Walk();
}

void CAS_Dr_Rampage_Walk::Free()
{
    __super::Free();
}
