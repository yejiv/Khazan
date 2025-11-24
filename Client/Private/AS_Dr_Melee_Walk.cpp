#include "AS_Dr_Melee_Walk.h"

CAS_Dr_Melee_Walk::CAS_Dr_Melee_Walk()
{
}

void CAS_Dr_Melee_Walk::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
}

void CAS_Dr_Melee_Walk::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
}

void CAS_Dr_Melee_Walk::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
}

CAS_Dr_Melee_Walk* CAS_Dr_Melee_Walk::Create()
{
    return new CAS_Dr_Melee_Walk();
}

void CAS_Dr_Melee_Walk::Free()
{
    __super::Free();
}
