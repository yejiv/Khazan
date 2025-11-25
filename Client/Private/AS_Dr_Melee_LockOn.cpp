#include "AS_Dr_Melee_LockOn.h"

CAS_Dr_Melee_LockOn::CAS_Dr_Melee_LockOn()
{
}

void CAS_Dr_Melee_LockOn::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    if (m_pMonData == nullptr)
        m_pMonData = &static_cast<CDragonian_Melee*>(pOwner)->Get_Data();

    m_pMonData->iAnimIndex = 29;
}

void CAS_Dr_Melee_LockOn::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
}

void CAS_Dr_Melee_LockOn::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
}

CAS_Dr_Melee_LockOn* CAS_Dr_Melee_LockOn::Create()
{
    return new CAS_Dr_Melee_LockOn();
}

void CAS_Dr_Melee_LockOn::Free()
{
    __super::Free();
}
