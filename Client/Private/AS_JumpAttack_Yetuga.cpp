#include "AS_JumpAttack_Yetuga.h"
#include "AI_Controller.h"
#include "Yetuga.h"
#include "GameInstance.h"
#include "BlackBoard.h"
#include "FSM_Yetuga.h"
#include "Body_Yetuga.h"

CAS_JumpAttack_Yetuga::CAS_JumpAttack_Yetuga()
{
}

void CAS_JumpAttack_Yetuga::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    CYetuga* pYetuga = static_cast<CYetuga*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pYetuga->Get_Body()->Get_Component(TEXT("Com_Model")));

    CTransform* pTransform = static_cast<CTransform*>(pOwner->Get_Component(TEXT("Com_Transform")));
    CTransform* pTargetTransform = static_cast<CTransform*>(m_pGameInstance->Get_BlackBoard()->
        Get_Value<CGameObject*>(pYetuga->Get_Name(), "Target")->Get_Component(TEXT("Com_Transform")));
    _vector vTargetLoc = pTargetTransform->Get_State(STATE::POSITION);

    pTransform->LookAt(vTargetLoc);

    pModel->Set_Animation(60);
}

void CAS_JumpAttack_Yetuga::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    CYetuga* pYetuga = static_cast<CYetuga*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pYetuga->Get_Body()->Get_Component(TEXT("Com_Model")));
    CBlackBoard* pBB = m_pGameInstance->Get_BlackBoard();

    if (pBB->Get_Value<_bool>(pYetuga->Get_Name(), "JumpNotify"))
    {
        pYetuga->Get_Controller()->
            AI_MoveTo(pOwner,
                pBB->Get_Value<CGameObject*>(pYetuga->Get_Name(), "Target"),
                pBB->Get_Value<_float>(pYetuga->Get_Name(), "AttackRange"),
                10,
                fTimeDelta);
    }

    if (pModel->Play_Animation(fTimeDelta))
    {
        m_pGameInstance->Get_BlackBoard()->Set_Value<_bool>(pYetuga->Get_Name(), "isJumpAttackFinished", true);
    }
}

void CAS_JumpAttack_Yetuga::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
}

void CAS_JumpAttack_Yetuga::OnCollision(COLLISION_DESC* pDesc, _uint iCollisionLayer, CGameObject* pOwner)
{
    COLLISION_LAYER eLayer = static_cast<COLLISION_LAYER>(iCollisionLayer);

    if (COLLISION_LAYER::PLAYER == eLayer)
    {
        CCreature* pTarget = static_cast<CCreature*>(pDesc->pGameObject);
        pTarget->Take_Damage(10.f, HITREACTION::KNOCKBACK_WEAK);
        CTransform* pOwnerTransform = static_cast<CTransform*>(pOwner->Get_Component(TEXT("Com_Transform")));
        if (nullptr == pOwnerTransform)
            return;
        _vector vLook = pOwnerTransform->Get_State(STATE::LOOK);
        //pTarget->KnockBack(vLook, 10.f, 50.f);
        pTarget->KnockBack(vLook, 20.f, 60.f);

    }
}



CAS_JumpAttack_Yetuga* CAS_JumpAttack_Yetuga::Create()
{
    return new CAS_JumpAttack_Yetuga();
}

void CAS_JumpAttack_Yetuga::Free()
{
    __super::Free();
}
