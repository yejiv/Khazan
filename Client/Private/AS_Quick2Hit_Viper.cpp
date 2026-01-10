#include "AS_Quick2Hit_Viper.h"
#include "Viper.h"
#include "Body_Viper.h"
#include "AI_Controller.h"
#include "BlackBoard.h"
#include "FSM_Viper.h"
#include "GameInstance.h"

CAS_Quick2Hit_Viper::CAS_Quick2Hit_Viper()
{

}

void CAS_Quick2Hit_Viper::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    CViper* pViper = static_cast<CViper*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pViper->Get_Body()->Get_Component(TEXT("Com_Model")));

    pModel->Set_Animation(49);
     
}

void CAS_Quick2Hit_Viper::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    CViper* pViper = static_cast<CViper*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pViper->Get_Body()->Get_Component(TEXT("Com_Model")));


    if (pModel->Play_Animation(fTimeDelta))
    {
        pViper->Get_Controller()->Get_BlackBoard()->Set_Value<_bool>(pViper->Get_Name(), "isP1_Quick2HitFinished", true);
        pFSM->Change_State(ENUM_CLASS(VIPER_STATE_P1::IDLE),pViper);
    }

}

void CAS_Quick2Hit_Viper::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{

}

void CAS_Quick2Hit_Viper::OnCollision(COLLISION_DESC* pDesc, _uint iCollisionLayer, CGameObject* pOwner)
{

    COLLISION_LAYER eLayer = static_cast<COLLISION_LAYER>(iCollisionLayer);

    if (COLLISION_LAYER::PLAYER == eLayer)
    {
        CCreature* pTarget = static_cast<CCreature*>(pDesc->pGameObject);
        pTarget->Take_Damage(90.f, HITREACTION::KNOCKBACK_NORMAL);
        CTransform* pOwnerTransform = static_cast<CTransform*>(pOwner->Get_Component(TEXT("Com_Transform")));
        if (nullptr == pOwnerTransform)
            return;

        _vector vLook = pOwnerTransform->Get_State(STATE::LOOK);
        pTarget->KnockBack(vLook, 20.f, 60.f);
    }


}

CAS_Quick2Hit_Viper* CAS_Quick2Hit_Viper::Create()
{
    return new CAS_Quick2Hit_Viper();
}

void CAS_Quick2Hit_Viper::Free()
{
    __super::Free();
}
