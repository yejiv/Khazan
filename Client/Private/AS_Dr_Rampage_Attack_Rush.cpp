#include "AS_Dr_Rampage_Attack_Rush.h"

CAS_Dr_Rampage_Attack_Rush::CAS_Dr_Rampage_Attack_Rush()
{
}

void CAS_Dr_Rampage_Attack_Rush::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    if (m_pMonData == nullptr)
        m_pMonData = &static_cast<CDragonian_Rampage*>(pOwner)->Get_Data();
}

void CAS_Dr_Rampage_Attack_Rush::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
}

void CAS_Dr_Rampage_Attack_Rush::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
}

CAS_Dr_Rampage_Attack_Rush* CAS_Dr_Rampage_Attack_Rush::Create()
{
    return new CAS_Dr_Rampage_Attack_Rush();
}

void CAS_Dr_Rampage_Attack_Rush::Free()
{
    __super::Free();
}
