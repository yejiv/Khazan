#include "AS_Dr_Melee_Attack.h"

CAS_Dr_Melee_Attack::CAS_Dr_Melee_Attack()
{
}

void CAS_Dr_Melee_Attack::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
}

void CAS_Dr_Melee_Attack::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
}

void CAS_Dr_Melee_Attack::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
}

CAS_Dr_Melee_Attack* CAS_Dr_Melee_Attack::Create()
{
    return new CAS_Dr_Melee_Attack();
}

void CAS_Dr_Melee_Attack::Free()
{
    __super::Free();
}
