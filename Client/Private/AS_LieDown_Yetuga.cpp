#include "AS_LieDown_Yetuga.h"
#include "AI_Controller.h"
#include "Yetuga.h"
#include "GameInstance.h"
#include "BlackBoard.h"
#include "FSM_Yetuga.h"
#include "Body_Yetuga.h"

CAS_LieDown_Yetuga::CAS_LieDown_Yetuga()
{
}

void CAS_LieDown_Yetuga::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    CYetuga* pYetuga = static_cast<CYetuga*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pYetuga->Get_Body()->Get_Component(TEXT("Com_Model")));
    pModel->Set_Animation(50);
    pModel->Set_AnimationLoop(false);
}

void CAS_LieDown_Yetuga::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    CYetuga* pYetuga = static_cast<CYetuga*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pYetuga->Get_Body()->Get_Component(TEXT("Com_Model")));
    if (pModel->Play_Animation(fTimeDelta))
    {
        m_pGameInstance->Get_BlackBoard()->Set_Value<_bool>("Yetuga", "isAttackFinished3", true);
    }
}

void CAS_LieDown_Yetuga::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{

}

void CAS_LieDown_Yetuga::OnCollision(COLLISION_DESC* pDesc, _uint iCollisionLayer, CGameObject* pOwner)
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
        pTarget->KnockBack(vLook * -1.f, 20.f, 60.f);
    }

}

CAS_LieDown_Yetuga* CAS_LieDown_Yetuga::Create()
{
    return new CAS_LieDown_Yetuga();
}

void CAS_LieDown_Yetuga::Free()
{
    __super::Free();
}
