#include "AS_Dr_Rampage_LockOn.h"

CAS_Dr_Rampage_LockOn::CAS_Dr_Rampage_LockOn()
{
}

void CAS_Dr_Rampage_LockOn::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    if (m_pMonData == nullptr)
        m_pMonData = &static_cast<CDragonian_Rampage*>(pOwner)->Get_Data();

    m_pMonData->iAnimIndex = 40;
}

void CAS_Dr_Rampage_LockOn::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
}

void CAS_Dr_Rampage_LockOn::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
}

CAS_Dr_Rampage_LockOn* CAS_Dr_Rampage_LockOn::Create()
{
    return new CAS_Dr_Rampage_LockOn();
}

void CAS_Dr_Rampage_LockOn::Free()
{
    __super::Free();
}
