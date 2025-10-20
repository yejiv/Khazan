#include "AttackState_Yetuga.h"

CAttackState_Yetuga::CAttackState_Yetuga()
{
}

void CAttackState_Yetuga::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
}

void CAttackState_Yetuga::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
}

void CAttackState_Yetuga::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
}

CAttackState_Yetuga* CAttackState_Yetuga::Create()
{
    return new CAttackState_Yetuga();
}

void CAttackState_Yetuga::Free()
{
    __super::Free();
}
