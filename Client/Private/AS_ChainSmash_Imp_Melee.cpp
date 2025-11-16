#include "AS_ChainSmash_Imp_Melee.h"
#include "Imp_Melee.h"
#include "GameInstance.h"
#include "Body_Imp_Melee.h"
#include "BlackBoard.h"
#include "AI_Controller.h"
#include "FSM_Imp_Range.h"


CAS_ChainSmash_Imp_Melee::CAS_ChainSmash_Imp_Melee()
{

}

void CAS_ChainSmash_Imp_Melee::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    CImp_Melee* pImp = static_cast<CImp_Melee*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pImp->Get_Body()->Get_Component(TEXT("Com_Model")));
    pModel->Set_Animation(1);
}

void CAS_ChainSmash_Imp_Melee::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    CImp_Melee* pImp = static_cast<CImp_Melee*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pImp->Get_Body()->Get_Component(TEXT("Com_Model")));

    if (pModel->Play_Animation(fTimeDelta))
    {
        pImp->Get_Controller()->Get_BlackBoard()->Set_Value<_bool>(pImp->Get_Name(), "isChainSmashFinished", true);
        pFSM->Change_State(ENUM_CLASS(IMPRANGE_STATE::IDLE), pOwner);
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
        pTarget->Take_Damage(10, HITREACTION::KNOCKBACK_WEAK, nullptr);
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
