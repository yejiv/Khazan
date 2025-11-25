#include "AS_Dr_Rampage_Attack_Jump.h"

CAS_Dr_Rampage_Attack_Jump::CAS_Dr_Rampage_Attack_Jump()
{
}

void CAS_Dr_Rampage_Attack_Jump::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    if (m_pMonData == nullptr)
        m_pMonData = &static_cast<CDragonian_Rampage*>(pOwner)->Get_Data();
}

void CAS_Dr_Rampage_Attack_Jump::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
}

void CAS_Dr_Rampage_Attack_Jump::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
}

CAS_Dr_Rampage_Attack_Jump* CAS_Dr_Rampage_Attack_Jump::Create()
{
    return new CAS_Dr_Rampage_Attack_Jump();
}

void CAS_Dr_Rampage_Attack_Jump::Free()
{
    __super::Free();
}
