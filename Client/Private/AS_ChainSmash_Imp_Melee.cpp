#include "AS_ChainSmash_Imp_Melee.h"
#include "Imp_Melee.h"
#include "GameInstance.h"
#include "Body_Imp_Melee.h"
#include "BlackBoard.h"
#include "AI_Controller.h"
#include "FSM_Imp_Melee.h"


CAS_ChainSmash_Imp_Melee::CAS_ChainSmash_Imp_Melee()
{

}

void CAS_ChainSmash_Imp_Melee::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    CImp_Melee* pImp = static_cast<CImp_Melee*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pImp->Get_Body()->Get_Component(TEXT("Com_Model")));
    pModel->Set_Animation(1);
    CBlackBoard* pBB = pImp->Get_Controller()->Get_BlackBoard();
    pBB->Set_Value<_uint>(pImp->Get_Name(), "AttackCount", 0);
}

void CAS_ChainSmash_Imp_Melee::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    CImp_Melee* pImp = static_cast<CImp_Melee*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pImp->Get_Body()->Get_Component(TEXT("Com_Model")));

    if (pModel->Play_Animation(fTimeDelta))
    {
        pImp->Get_Controller()->Get_BlackBoard()->Set_Value<_bool>(pImp->Get_Name(), "isChainSmashFinished", true);
        pFSM->Change_State(ENUM_CLASS(IMPMELEE_STATE::IDLE), pOwner);
    }
}

void CAS_ChainSmash_Imp_Melee::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
}

void CAS_ChainSmash_Imp_Melee::OnCollision(COLLISION_DESC* pDesc, _uint iCollisionLayer, CGameObject* pOwner)
{
    COLLISION_LAYER eLayer = static_cast<COLLISION_LAYER>(iCollisionLayer);
    if (COLLISION_LAYER::PLAYER == eLayer)
    {
        CCreature* pTarget = static_cast<CCreature*>(pDesc->pGameObject);
        CImp_Melee* pImp = static_cast<CImp_Melee*>(pOwner);
        CBlackBoard* pBB = pImp->Get_Controller()->Get_BlackBoard();
        _uint iAttackCnt = pBB->Get_Value<_uint>(pImp->Get_Name(), "AttackCount");
        CTransform* pOwnerTransform = static_cast<CTransform*>(pOwner->Get_Component(TEXT("Com_Transform")));
        if (nullptr == pOwnerTransform)
            return;

        if (iAttackCnt == 1)
        {
            pTarget->Take_Damage(60.f, HITREACTION::KNOCKBACK_WEAK);
            _vector vLook = pOwnerTransform->Get_State(STATE::LOOK);
            pTarget->KnockBack(vLook, 15.f, 60.f);
        }
        else if (iAttackCnt == 2)
        {
            pTarget->Take_Damage(100.f, HITREACTION::KNOCKBACK_WEAK);
            _vector vLook = pOwnerTransform->Get_State(STATE::LOOK);
            pTarget->KnockBack(vLook, 15.f, 60.f);
        }
        else if (iAttackCnt == 3)
        {
            pTarget->Take_Damage(120.f, HITREACTION::KNOCKBACK_NORMAL);
            _vector vLook = pOwnerTransform->Get_State(STATE::LOOK);
            pTarget->KnockBack(vLook, 20.f, 60.f);
        }
    }
}

CAS_ChainSmash_Imp_Melee* CAS_ChainSmash_Imp_Melee::Create()
{
    return new CAS_ChainSmash_Imp_Melee();
}

void CAS_ChainSmash_Imp_Melee::Free()
{
    __super::Free();
}
