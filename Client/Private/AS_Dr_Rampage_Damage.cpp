#include "AS_Dr_Rampage_Damage.h"

CAS_Dr_Rampage_Damage::CAS_Dr_Rampage_Damage()
{
}

void CAS_Dr_Rampage_Damage::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    if (m_pMonData == nullptr)
        m_pMonData = &static_cast<CDragonian_Rampage*>(pOwner)->Get_Data();
}

void CAS_Dr_Rampage_Damage::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
}

void CAS_Dr_Rampage_Damage::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
}

CAS_Dr_Rampage_Damage* CAS_Dr_Rampage_Damage::Create()
{
    return new CAS_Dr_Rampage_Damage();
}

void CAS_Dr_Rampage_Damage::Free()
{
    __super::Free();
}
