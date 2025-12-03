#include "AS_LockOn_ImpRange.h"
#include "Imp_Range.h"
#include "Body_Imp_Range.h"
#include "AI_Controller.h"
#include "GameInstance.h"

CAS_LockOn_ImpRange::CAS_LockOn_ImpRange()
{
}

void CAS_LockOn_ImpRange::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    CImp_Range* pImp = static_cast<CImp_Range*>(pOwner);
    CBlackBoard* pBB = pImp->Get_Controller()->Get_BlackBoard();
    CGameObject* pTarget = pBB->Get_Value<CGameObject*>(pImp->Get_Name(), "Target");
    CTransform* pTransform = static_cast<CTransform*>(pOwner->Get_Component(TEXT("Com_Transform")));
    CTransform* pTargetTransform = static_cast<CTransform*>(pTarget->Get_Component(TEXT("Com_Transform")));
    CModel* pModel = static_cast<CModel*>(pImp->Get_Body()->Get_Component(TEXT("Com_Model")));
    // 여기서 방향에 따른 애니메이션 세팅
    
    m_isRetreat = pBB->Get_Value<_bool>(pImp->Get_Name(), "isRetreat");
    m_isCircleRight = pBB->Get_Value<_bool>(pImp->Get_Name(), "isCircleRight");

    _vector vTargetPos = pTargetTransform->Get_State(STATE::POSITION);
    _vector vOwnerPos = pTransform->Get_State(STATE::POSITION);
    _vector vDir = XMVector3Normalize(vTargetPos - vOwnerPos);
    _vector vUp = XMVectorSet(0.f,1.f,0.f,0.f);
    _vector vRight = XMVector3Normalize(XMVector3Cross(vUp,vDir));

    if (m_isRetreat)
    {
        XMStoreFloat3(&m_vMoveDir, -vDir);
        pModel->Set_Animation(10);
    }
    else if (m_isCircleRight)
    {
        XMStoreFloat3(&m_vMoveDir, vRight);
        pModel->Set_Animation(12);
    }
    else
    {
        XMStoreFloat3(&m_vMoveDir, -vRight);
        pModel->Set_Animation(11);
    }

}

void CAS_LockOn_ImpRange::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    CImp_Range* pImp = static_cast<CImp_Range*>(pOwner);
    CBlackBoard* pBB = pImp->Get_Controller()->Get_BlackBoard();
    CGameObject* pTarget = pBB->Get_Value<CGameObject*>(pImp->Get_Name(), "Target");
    CTransform* pTransform = static_cast<CTransform*>(pOwner->Get_Component(TEXT("Com_Transform")));
    CTransform* pTargetTransform = static_cast<CTransform*>(pTarget->Get_Component(TEXT("Com_Transform")));
    CModel* pModel = static_cast<CModel*>(pImp->Get_Body()->Get_Component(TEXT("Com_Model")));
    _vector vTargetPos = pTargetTransform->Get_State(STATE::POSITION);
    _vector vPosition = pTransform->Get_State(STATE::POSITION);
    pTransform->LookAt(vTargetPos);

    _vector vResult = XMLoadFloat3(&m_vMoveDir) * fTimeDelta * 3.f;

    pModel->Play_Animation(fTimeDelta);

    _float fDist = XMVectorGetX(XMVector3Length(vTargetPos - vPosition));
    if (fDist <= 20.f)
    {
        pBB->Set_Value<_bool>(pImp->Get_Name(),"isLockOn",false);
    }
}

void CAS_LockOn_ImpRange::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
}

CAS_LockOn_ImpRange* CAS_LockOn_ImpRange::Create()
{
    return new CAS_LockOn_ImpRange;
}

void CAS_LockOn_ImpRange::Free()
{
    __super::Free();
}
