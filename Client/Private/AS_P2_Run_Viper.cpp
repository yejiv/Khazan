#include "AS_P2_Run_Viper.h"
#include "Viper.h"
#include "Body_Phase2_Viper.h"
#include "BlackBoard.h"
#include "AI_Controller.h"
#include "GameInstance.h"

CAS_P2_Run_Viper::CAS_P2_Run_Viper()
{

}

void CAS_P2_Run_Viper::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{

    CViper* pViper = static_cast<CViper*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pViper->Get_P2Body()->Get_Component(TEXT("Com_Model")));
    CBlackBoard* pBB = pViper->Get_Controller()->Get_BlackBoard();
    pModel->Set_Animation(44);
    pModel->Set_AnimationLoop(true);
    _bool isBerserker = pBB->Get_Value<_bool>(pViper->Get_Name(), "is_Berserker");
    if (!isBerserker)
        m_fSpeed = pBB->Get_Value<_float>(pViper->Get_Name(), "RunSpeed");
    else
        m_fSpeed = pBB->Get_Value<_float>(pViper->Get_Name(), "BerserkerSpeed");

}

void CAS_P2_Run_Viper::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    CViper* pViper = static_cast<CViper*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pViper->Get_P2Body()->Get_Component(TEXT("Com_Model")));
    CTransform* pOwnerTransform = static_cast<CTransform*>(pOwner->Get_Component(TEXT("Com_Transform")));
    CBlackBoard* pBB = pViper->Get_Controller()->Get_BlackBoard();
    CGameObject* pTarget = pBB->Get_Value<CGameObject*>(pViper->Get_Name(), "Target");
    _float fTargetDist = pBB->Get_Value<_float>(pViper->Get_Name(), "TargetDist");
    _float fMoveRange = pBB->Get_Value<_float>(pViper->Get_Name(), "AttackRange") - 0.5f;

    pViper->Get_Controller()->AI_MoveTo(pOwner, pTarget, fMoveRange, m_fSpeed, fTimeDelta);

    pModel->Play_Animation(fTimeDelta);
}

void CAS_P2_Run_Viper::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
}

CAS_P2_Run_Viper* CAS_P2_Run_Viper::Create()
{
    return new CAS_P2_Run_Viper();
}

void CAS_P2_Run_Viper::Free()
{
    __super::Free();
}
