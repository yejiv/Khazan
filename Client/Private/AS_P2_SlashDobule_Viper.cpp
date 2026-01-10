#include "AS_P2_SlashDobule_Viper.h"
#include "Viper.h"
#include "Body_Phase2_Viper.h"
#include "BlackBoard.h"
#include "AI_Controller.h"
#include "GameInstance.h"
#include "FSM_Viper.h"

CAS_P2_SlashDobule_Viper::CAS_P2_SlashDobule_Viper()
{

}

void CAS_P2_SlashDobule_Viper::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{

    CViper* pViper = static_cast<CViper*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pViper->Get_P2Body()->Get_Component(TEXT("Com_Model")));
    CBlackBoard* pBB = pViper->Get_Controller()->Get_BlackBoard();
    pModel->Set_Animation(40);

}

void CAS_P2_SlashDobule_Viper::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    CViper* pViper = static_cast<CViper*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pViper->Get_P2Body()->Get_Component(TEXT("Com_Model")));


    if (pModel->Play_Animation(fTimeDelta))
    {
        CBlackBoard* pBB = pViper->Get_Controller()->Get_BlackBoard();
        pFSM->Change_State(ENUM_CLASS(VIPER_STATE_P1::IDLE), pViper);
        pBB->Set_Value<_bool>(pViper->Get_Name(), "is_P2_SlashDoubleFinished", true);
    }
}

void CAS_P2_SlashDobule_Viper::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{

}

void CAS_P2_SlashDobule_Viper::OnCollision(COLLISION_DESC* pDesc, _uint iCollisionLayer, CGameObject* pOwner)
{
    COLLISION_LAYER eLayer = static_cast<COLLISION_LAYER>(iCollisionLayer);

    if (COLLISION_LAYER::PLAYER == eLayer)
    {
        CViper* pViper = static_cast<CViper*>(pOwner);
        CBlackBoard* pBB = pViper->Get_Controller()->Get_BlackBoard();
        CCreature* pTarget = static_cast<CCreature*>(pDesc->pGameObject);
        CTransform* pOwnerTransform = static_cast<CTransform*>(pOwner->Get_Component(TEXT("Com_Transform")));
        if (nullptr == pOwnerTransform)
            return;

        pTarget->Take_Damage(90.f, HITREACTION::KNOCKBACK_WEAK);
        _vector vLook = pOwnerTransform->Get_State(STATE::LOOK);
        pTarget->KnockBack(vLook, 20.f, 60.f);

    }
}

CAS_P2_SlashDobule_Viper* CAS_P2_SlashDobule_Viper::Create()
{
    return new CAS_P2_SlashDobule_Viper();
}

void CAS_P2_SlashDobule_Viper::Free()
{
    __super::Free();
}
