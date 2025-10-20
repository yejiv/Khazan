#include "IdleState_Yetuga.h"

CIdleState_Yetuga::CIdleState_Yetuga()
{
}

void CIdleState_Yetuga::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
}

void CIdleState_Yetuga::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
}

void CIdleState_Yetuga::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
}

CIdleState_Yetuga* CIdleState_Yetuga::Create()
{
    return new CIdleState_Yetuga();
}

void CIdleState_Yetuga::Free()
{
    __super::Free();
}
