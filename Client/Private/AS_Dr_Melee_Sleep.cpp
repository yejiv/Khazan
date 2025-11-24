#include "AS_Dr_Melee_Sleep.h"

CAS_Dr_Melee_Sleep::CAS_Dr_Melee_Sleep()
{
}

void CAS_Dr_Melee_Sleep::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
}

void CAS_Dr_Melee_Sleep::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
}

void CAS_Dr_Melee_Sleep::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
}

CAS_Dr_Melee_Sleep* CAS_Dr_Melee_Sleep::Create()
{
    return new CAS_Dr_Melee_Sleep();
}

void CAS_Dr_Melee_Sleep::Free()
{
    __super::Free();
}
