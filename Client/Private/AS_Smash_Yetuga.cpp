#include "AS_Smash_Yetuga.h"
#include "AI_Controller.h"
#include "Yetuga.h"
#include "GameInstance.h"
#include "BlackBoard.h"
#include "FSM_Yetuga.h"
#include "Body_Yetuga.h"

CAS_Smash_Yetuga::CAS_Smash_Yetuga()
{
}

void CAS_Smash_Yetuga::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    CYetuga* pYetuga = static_cast<CYetuga*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pYetuga->Get_Body()->Get_Component(TEXT("Com_Model")));

    _uint iRand = static_cast<_uint>(m_pGameInstance->Rand(0.f, 4.f));

    _uint animID = 0;

    switch (iRand)
    {
    case 0: 
        animID = 33; 
        break;
    case 1: 
        animID = 32; 
        break;
    case 2:
        animID = 16;
        break;
    case 3:
        animID = 18;
        break;
    }
    
    pModel->Set_Animation(animID);
}

void CAS_Smash_Yetuga::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    CYetuga* pYetuga = static_cast<CYetuga*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pYetuga->Get_Body()->Get_Component(TEXT("Com_Model")));

    if (pModel->Play_Animation(fTimeDelta))
    {
        m_pGameInstance->Get_BlackBoard()->Set_Value<_bool>(pYetuga->Get_Name(),"isNormalSmashFinished",true);
    }


}

void CAS_Smash_Yetuga::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{

}

void CAS_Smash_Yetuga::OnCollision(COLLISION_DESC* pDesc, _uint iCollisionLayer, CGameObject* pOwner)
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
        pTarget->KnockBack(vLook, 10.f, 50.f);

    }
}

CAS_Smash_Yetuga* CAS_Smash_Yetuga::Create()
{
    return new CAS_Smash_Yetuga();
}

void CAS_Smash_Yetuga::Free()
{
    __super::Free();
}
