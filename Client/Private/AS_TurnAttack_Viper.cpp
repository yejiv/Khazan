#include "AS_TurnAttack_Viper.h"
#include "Viper.h"
#include "Body_Viper.h"
#include "BlackBoard.h"
#include "AI_Controller.h"
#include "GameInstance.h"


CAS_TurnAttack_Viper::CAS_TurnAttack_Viper()
{

}

void CAS_TurnAttack_Viper::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    CViper* pViper = static_cast<CViper*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pViper->Get_Body()->Get_Component(TEXT("Com_Model")));

    pModel->Set_Animation(53);

    CBlackBoard* pBB = pViper->Get_Controller()->Get_BlackBoard();
    pBB->Set_Value<_uint>(pViper->Get_Name(), "AttackCount", 0);

}

void CAS_TurnAttack_Viper::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    CViper* pViper = static_cast<CViper*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pViper->Get_Body()->Get_Component(TEXT("Com_Model")));

    if (pModel->Play_Animation(fTimeDelta))
    {
        pViper->Get_Controller()->Get_BlackBoard()->Set_Value<_bool>(pViper->Get_Name(), "isP1_TurnAttackFinished", true);
    }

}


void CAS_TurnAttack_Viper::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{

}

void CAS_TurnAttack_Viper::OnCollision(COLLISION_DESC* pDesc, _uint iCollisionLayer, CGameObject* pOwner)
{
    COLLISION_LAYER eLayer = static_cast<COLLISION_LAYER>(iCollisionLayer);

    if (COLLISION_LAYER::PLAYER == eLayer)
    {
        CViper* pViper = static_cast<CViper*>(pOwner);
        CBlackBoard* pBB = pViper->Get_Controller()->Get_BlackBoard();
        _uint iAttackCnt = pBB->Get_Value<_uint>(pViper->Get_Name(), "AttackCount");
        CCreature* pTarget = static_cast<CCreature*>(pDesc->pGameObject);
        CTransform* pOwnerTransform = static_cast<CTransform*>(pOwner->Get_Component(TEXT("Com_Transform")));
        if (nullptr == pOwnerTransform)
            return;

        if (iAttackCnt == 1)
        {
            pTarget->Take_Damage(10.f, HITREACTION::KNOCKBACK_NORMAL);
            _vector vLook = pOwnerTransform->Get_State(STATE::LOOK);
            pTarget->KnockBack(vLook, 20.f, 60.f);
        }
        else if (iAttackCnt == 2)
        {
            pTarget->Take_Damage(10.f, HITREACTION::KNOCKBACK_STRONG);
            _vector vLook = pOwnerTransform->Get_State(STATE::LOOK);
            pTarget->KnockBack(vLook, 25.f, 50.f);
        }
    }
}

CAS_TurnAttack_Viper* CAS_TurnAttack_Viper::Create()
{
    return new CAS_TurnAttack_Viper();
}

void CAS_TurnAttack_Viper::Free()
{
    __super::Free();
}
