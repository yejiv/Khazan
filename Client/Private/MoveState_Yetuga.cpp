#include "MoveState_Yetuga.h"

CMoveState_Yetuga::CMoveState_Yetuga()
{
}

void CMoveState_Yetuga::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
}

void CMoveState_Yetuga::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
}

void CMoveState_Yetuga::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
}

CMoveState_Yetuga* CMoveState_Yetuga::Create()
{
    return new CMoveState_Yetuga();
}

void CMoveState_Yetuga::Free()
{
    __super::Free();
}
