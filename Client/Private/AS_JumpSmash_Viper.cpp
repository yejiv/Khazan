#include "AS_JumpSmash_Viper.h"
#include "Viper.h"
#include "Body_Viper.h"
#include "BlackBoard.h"
#include "AI_Controller.h"
#include "GameInstance.h"


CAS_JumpSmash_Viper::CAS_JumpSmash_Viper()
{
}

void CAS_JumpSmash_Viper::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    CViper* pViper = static_cast<CViper*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pViper->Get_Body()->Get_Component(TEXT("Com_Model")));
    pModel->Set_Animation(34);
}

void CAS_JumpSmash_Viper::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    CViper* pViper = static_cast<CViper*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pViper->Get_Body()->Get_Component(TEXT("Com_Model")));

 
    if (pModel->Play_Animation(fTimeDelta))
    {
        CBlackBoard* pBB = pViper->Get_Controller()->Get_BlackBoard();
        pBB->Set_Value<_bool>(pViper->Get_Name(), "isP1_JumpSmashFinished", true);
    }
}

void CAS_JumpSmash_Viper::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
}

void CAS_JumpSmash_Viper::OnCollision(COLLISION_DESC* pDesc, _uint iCollisionLayer, CGameObject* pOwner)
{
    COLLISION_LAYER eLayer = static_cast<COLLISION_LAYER>(iCollisionLayer);

    if (COLLISION_LAYER::PLAYER == eLayer)
    {
        CCreature* pTarget = static_cast<CCreature*>(pDesc->pGameObject);
        pTarget->Take_Damage(40.f, HITREACTION::KNOCKBACK_STRONG);
        CTransform* pOwnerTransform = static_cast<CTransform*>(pOwner->Get_Component(TEXT("Com_Transform")));
        if (nullptr == pOwnerTransform)
            return;
        _vector vLook = pOwnerTransform->Get_State(STATE::LOOK);
        pTarget->KnockBack(vLook, 20.f, 60.f);
    }
}

CAS_JumpSmash_Viper* CAS_JumpSmash_Viper::Create()
{
    return new CAS_JumpSmash_Viper();
}

void CAS_JumpSmash_Viper::Free()
{
    __super::Free();
}

