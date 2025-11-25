#include "AS_Dr_Rampage_Attack_Default.h"

CAS_Dr_Rampage_Attack_Default::CAS_Dr_Rampage_Attack_Default()
{
}

void CAS_Dr_Rampage_Attack_Default::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    if (m_pMonData == nullptr)
        m_pMonData = &static_cast<CDragonian_Rampage*>(pOwner)->Get_Data();
}

void CAS_Dr_Rampage_Attack_Default::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
}

void CAS_Dr_Rampage_Attack_Default::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
}

CAS_Dr_Rampage_Attack_Default* CAS_Dr_Rampage_Attack_Default::Create()
{
    return new CAS_Dr_Rampage_Attack_Default();
}

void CAS_Dr_Rampage_Attack_Default::Free()
{
    __super::Free();
}
