#include "AS_Dr_Rampage_Attack_Back.h"

CAS_Dr_Rampage_Attack_Back::CAS_Dr_Rampage_Attack_Back()
{
}

void CAS_Dr_Rampage_Attack_Back::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    if (m_pMonData == nullptr)
        m_pMonData = &static_cast<CDragonian_Rampage*>(pOwner)->Get_Data();
}

void CAS_Dr_Rampage_Attack_Back::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
}

void CAS_Dr_Rampage_Attack_Back::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
}

CAS_Dr_Rampage_Attack_Back* CAS_Dr_Rampage_Attack_Back::Create()
{
    return new CAS_Dr_Rampage_Attack_Back();
}

void CAS_Dr_Rampage_Attack_Back::Free()
{
    __super::Free();
}
