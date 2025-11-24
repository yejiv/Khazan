#include "AS_Dr_Melee_Groggy.h"

CAS_Dr_Melee_Groggy::CAS_Dr_Melee_Groggy()
{
}

void CAS_Dr_Melee_Groggy::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
}

void CAS_Dr_Melee_Groggy::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
}

void CAS_Dr_Melee_Groggy::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
}

CAS_Dr_Melee_Groggy* CAS_Dr_Melee_Groggy::Create()
{
    return new CAS_Dr_Melee_Groggy();
}

void CAS_Dr_Melee_Groggy::Free()
{
    __super::Free();
}
