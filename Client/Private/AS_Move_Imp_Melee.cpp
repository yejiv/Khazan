#include "AS_Move_Imp_Melee.h"
#include "Imp_Melee.h"
#include "Body_Imp_Melee.h"
#include "GameInstance.h"
#include "BlackBoard.h"
#include "AI_Controller_Imp_Melee.h"

CAS_Move_Imp_Melee::CAS_Move_Imp_Melee()
{
}

void CAS_Move_Imp_Melee::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{

    CImp_Melee* pImp = static_cast<CImp_Melee*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pImp->Get_Body()->Get_Component(TEXT("Com_Model")));
    CBlackBoard* pBB = pImp->Get_Controller()->Get_BlackBoard();
    pModel->Set_Animation(30);
    pModel->Set_AnimationLoop(true);
    m_fSpeed = pBB->Get_Value<_float>(pImp->Get_Name(), "RunSpeed");
    
}

void CAS_Move_Imp_Melee::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    CImp_Melee* pImp = static_cast<CImp_Melee*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pImp->Get_Body()->Get_Component(TEXT("Com_Model")));
    CTransform* pOwnerTransform = static_cast<CTransform*>(pOwner->Get_Component(TEXT("Com_Transform")));
    CBlackBoard* pBB = pImp->Get_Controller()->Get_BlackBoard();
    CGameObject* pTarget = pBB->Get_Value<CGameObject*>(pImp->Get_Name(), "Target");


    _float fTargetDist = pBB->Get_Value<_float>(pImp->Get_Name(), "TargetDist");
    _float fAttackRange = pBB->Get_Value<_float>(pImp->Get_Name(), "RunRange");

    _float fDesireDist = fAttackRange;
    _float fMoveSpeed = m_fSpeed;

    if (fTargetDist < fDesireDist)
        fMoveSpeed *= -1.f;

    // ÀÌµ¿
    _float fMoveRange = pBB->Get_Value<_float>(pImp->Get_Name(), "RunRange") - 0.5f;
    pImp->Get_Controller()->AI_MoveTo(pOwner, pTarget, fMoveRange, m_fSpeed, fTimeDelta);

    pModel->Play_Animation(fTimeDelta);

}

void CAS_Move_Imp_Melee::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
    CImp_Melee* pImp = static_cast<CImp_Melee*>(pOwner);
    CBlackBoard* pBB = pImp->Get_Controller()->Get_BlackBoard();
    pBB->Set_Value<_uint>(pImp->Get_Name(), "iMovementFlag", 0);
}

CAS_Move_Imp_Melee* CAS_Move_Imp_Melee::Create()
{
    return new CAS_Move_Imp_Melee();
}

void CAS_Move_Imp_Melee::Free()
{
    __super::Free();
}
