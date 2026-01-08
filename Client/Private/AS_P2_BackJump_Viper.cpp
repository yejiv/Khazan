#include "AS_P2_BackJump_Viper.h"
#include "Viper.h"
#include "Body_Phase2_Viper.h"
#include "BlackBoard.h"
#include "AI_Controller.h"
#include "GameInstance.h"
#include "FSM_Viper.h"

CAS_P2_BackJump_Viper::CAS_P2_BackJump_Viper()
{

}

void CAS_P2_BackJump_Viper::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    CViper* pViper = static_cast<CViper*>(pOwner);
    CBlackBoard* pBB = pViper->Get_Controller()->Get_BlackBoard();
    CGameObject* pTarget = pBB->Get_Value<CGameObject*>(pViper->Get_Name(), "Target");
    CModel* pModel = static_cast<CModel*>(pViper->Get_P2Body()->Get_Component(TEXT("Com_Model")));
    CTransform* pTargetTransform = static_cast<CTransform*>(pTarget->Get_Component(TEXT("Com_Transform")));
    CTransform* pOwnerTransform = static_cast<CTransform*>(pOwner->Get_Component(TEXT("Com_Transform")));
    _vector vPosition = pOwnerTransform->Get_State(STATE::POSITION);
    XMStoreFloat3(&m_vStartPos, vPosition);

    _vector vTargetPos = pTargetTransform->Get_State(STATE::POSITION);
    _vector vDir = XMVector3Normalize(vPosition - vTargetPos);
    _float fJumpDist = 30.f;

    XMStoreFloat3(&m_vGoalPos, vTargetPos + vDir * fJumpDist);
    _uint iIndex = pBB->Get_Value<_uint>(pViper->Get_Name(), "DebugIndex");
    //_uint iRandomIndex = static_cast<_uint>(m_pGameInstance->Rand(0,1));
    //pModel->Set_Animation(iRandomIndex);

    pModel->Set_Animation(iIndex);



}

void CAS_P2_BackJump_Viper::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    CViper* pViper = static_cast<CViper*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pViper->Get_P2Body()->Get_Component(TEXT("Com_Model")));

    CBlackBoard* pBB = pViper->Get_Controller()->Get_BlackBoard();

    if (pBB->Get_Value<_bool>(pViper->Get_Name(), "P2_BackJump"))
    {
        _float fAnimRatio = pModel->MakeRatio();

        fAnimRatio = fAnimRatio * fAnimRatio * (3.f - 2.f * fAnimRatio);

        CTransform* pOwnerTransform = static_cast<CTransform*>(pOwner->Get_Component(TEXT("Com_Transform")));
        _vector vStart = XMLoadFloat3(&m_vStartPos);
        _vector vEnd = XMLoadFloat3(&m_vGoalPos);
        _vector vNewPos = XMVectorLerp(vStart, vEnd, fAnimRatio);

        pOwnerTransform->Set_State(STATE::POSITION, vNewPos);
    }

    if (pModel->Play_Animation(fTimeDelta))
    {
        pViper->Get_Controller()->Get_BlackBoard()->Set_Value<_bool>(pViper->Get_Name(), "is_P2_BackJumpFinished", true);
        pFSM->Change_State(ENUM_CLASS(VIPER_STATE_P1::IDLE), pOwner);
    }

}

void CAS_P2_BackJump_Viper::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
   
}

void CAS_P2_BackJump_Viper::OnCollision(COLLISION_DESC* pDesc, _uint iCollisionLayer, CGameObject* pOwner)
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
        pTarget->KnockBack(vLook, 20.f, 40.f);
    }
}

CAS_P2_BackJump_Viper* CAS_P2_BackJump_Viper::Create()
{
    return new CAS_P2_BackJump_Viper();
}

void CAS_P2_BackJump_Viper::Free()
{
    __super::Free();
}
