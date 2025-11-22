#include "AS_Walk_Viper.h"
#include "Viper.h"
#include "Body_Viper.h"
#include "BlackBoard.h"
#include "AI_Controller.h"
#include "GameInstance.h"

CAS_Walk_Viper::CAS_Walk_Viper()
{

}

void CAS_Walk_Viper::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    CViper* pViper = static_cast<CViper*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pViper->Get_Body()->Get_Component(TEXT("Com_Model")));

    pModel->Set_Animation(0);
    m_fSpeed = pViper->Get_Controller()->Get_BlackBoard()->Get_Value<_float>(pViper->Get_Name(), "WalkSpeed");
}

void CAS_Walk_Viper::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    CViper* pViper = static_cast<CViper*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pViper->Get_Body()->Get_Component(TEXT("Com_Model")));
    CTransform* pOwnerTransform = static_cast<CTransform*>(pOwner->Get_Component(TEXT("Com_Transform")));
    CBlackBoard* pBB = pViper->Get_Controller()->Get_BlackBoard();
    CGameObject* pTarget = pBB->Get_Value<CGameObject*>(pViper->Get_Name(), "Target");
    _float fTargetDist = pBB->Get_Value<_float>(pViper->Get_Name(), "TargetDist");
    _float fMoveRange = pBB->Get_Value<_float>(pViper->Get_Name(), "AttackRange") - 0.5f;

    pViper->Get_Controller()->AI_MoveTo(pOwner, pTarget, fMoveRange, m_fSpeed, fTimeDelta);

    pModel->Play_Animation(fTimeDelta);

}

void CAS_Walk_Viper::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
}

CAS_Walk_Viper* CAS_Walk_Viper::Create()
{
    return new CAS_Walk_Viper();
}

void CAS_Walk_Viper::Free()
{
    __super::Free();
}
