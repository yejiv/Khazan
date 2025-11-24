#include "AS_Dr_Melee_Damage.h"

CAS_Dr_Melee_Damage::CAS_Dr_Melee_Damage()
{
}

void CAS_Dr_Melee_Damage::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
}

void CAS_Dr_Melee_Damage::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
}

void CAS_Dr_Melee_Damage::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
}

CAS_Dr_Melee_Damage* CAS_Dr_Melee_Damage::Create()
{
    return new CAS_Dr_Melee_Damage();
}

void CAS_Dr_Melee_Damage::Free()
{
    __super::Free();
}
