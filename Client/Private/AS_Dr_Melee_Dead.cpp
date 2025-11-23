#include "AS_Dr_Melee_Dead.h"

CAS_Dr_Melee_Dead::CAS_Dr_Melee_Dead()
{
}

void CAS_Dr_Melee_Dead::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
}

void CAS_Dr_Melee_Dead::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
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
