#include "AttackState_Yetuga.h"
#include "AI_Controller.h"
#include "Yetuga.h"
#include "GameInstance.h"
#include "BlackBoard.h"
#include "FSM_Yetuga.h"
#include "Body_Yetuga.h"

CAttackState_Yetuga::CAttackState_Yetuga()
{
}

void CAttackState_Yetuga::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    CYetuga* pYetuga = static_cast<CYetuga*>(pOwner);

    CModel* pModel = static_cast<CModel*>(pYetuga->Get_Body()->Get_Component(TEXT("Com_Model")));
    pModel->Set_Animation(31);
    pModel->Set_AnimationLoop(false);
}

void CAttackState_Yetuga::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    CYetuga* pYetuga = static_cast<CYetuga*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pYetuga->Get_Body()->Get_Component(TEXT("Com_Model")));

    m_fCurrentTime += fTimeDelta;

    if (pModel->Play_Animation(fTimeDelta))
    {
        CBlackBoard* pBB = pYetuga->Get_Controller()->Get_BlackBoard();
        pBB->Set_Value<_bool>(pYetuga->Get_Name(), "is2HitFinished", true);
        pFSM->Change_State(ENUM_CLASS(YETUGA_STATE::IDLE), pOwner);
    }

}

void CAttackState_Yetuga::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{

}

void CAttackState_Yetuga::OnCollision(COLLISION_DESC* pDesc, _uint iCollisionLayer, CGameObject* pOwner)
{
    COLLISION_LAYER eLayer = static_cast<COLLISION_LAYER>(iCollisionLayer);

    if (COLLISION_LAYER::PLAYER == eLayer)
    {
        CCreature* pTarget = static_cast<CCreature*>(pDesc->pGameObject);
        pTarget->Take_Damage(10.f, HITREACTION::KNOCKBACK_NORMAL);
        CTransform* pOwnerTransform = static_cast<CTransform*>(pOwner->Get_Component(TEXT("Com_Transform")));
        if (nullptr == pOwnerTransform)
            return;

        _vector vLook = pOwnerTransform->Get_State(STATE::LOOK);
        pTarget->KnockBack(vLook, 20.f, 60.f);        
    }
}

CAttackState_Yetuga* CAttackState_Yetuga::Create()
{
    return new CAttackState_Yetuga();
}

void CAttackState_Yetuga::Free()
{
    __super::Free();
}
