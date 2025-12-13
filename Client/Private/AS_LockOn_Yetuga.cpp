#include "AS_LockOn_Yetuga.h"
#include "AI_Controller.h"
#include "Yetuga.h"
#include "GameInstance.h"
#include "BlackBoard.h"
#include "FSM_Yetuga.h"
#include "Body_Yetuga.h"
#include "ClientInstance.h"


CAS_LockOn_Yetuga::CAS_LockOn_Yetuga()
{

}

void CAS_LockOn_Yetuga::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{

    m_fDotThreshold = cosf(XMConvertToRadians(10.f));
    m_fMoveSpeed = 0.5f;

    CYetuga* pYetuga = static_cast<CYetuga*>(pOwner);
    CBlackBoard* pBB = pYetuga->Get_Controller()->Get_BlackBoard();
    CModel* pModel = static_cast<CModel*>(pYetuga->Get_Body()->Get_Component(TEXT("Com_Model")));

    CGameObject* pTarget = static_cast<CGameObject*>(pBB->Get_Value<CGameObject*>(pYetuga->Get_Name(),"Target"));
    CTransform* pOwnerTransform = static_cast<CTransform*>(pOwner->Get_Component(TEXT("Com_Transform")));
    CTransform* pTargetTransform = static_cast<CTransform*>(pTarget->Get_Component(TEXT("Com_Transform")));
    _vector vPosition = pOwnerTransform->Get_State(STATE::POSITION);
    _vector vTargetPos = pTargetTransform->Get_State(STATE::POSITION);
    _vector vDir = XMVector3Normalize(vTargetPos - vPosition);
    _vector vRight = XMVector3Normalize(pOwnerTransform->Get_State(STATE::RIGHT));
    _float fDot = XMVectorGetX(XMVector3Dot(vDir, vRight));

    if (fDot < -0.3f)
    {
        pModel->Set_Animation(4); //왼쪽
        m_eLockOnDir = LOCKONDIR::LOCKON_LEFT;
    }
    else if (fDot > 0.3f)
    {
        pModel->Set_Animation(5); // 오른쪽
        m_eLockOnDir = LOCKONDIR::LOCKON_RIGHT;
    }
    else
    {
        pModel->Set_Animation(10);
    }

}

void CAS_LockOn_Yetuga::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    CYetuga* pYetuga = static_cast<CYetuga*>(pOwner);
    CBlackBoard* pBB = pYetuga->Get_Controller()->Get_BlackBoard();

    CTransform* pOwnerTransform = static_cast<CTransform*>(pOwner->Get_Component(TEXT("Com_Transform")));
    CGameObject* pTarget = pBB->Get_Value<CGameObject*>(pYetuga->Get_Name(), "Target");
    CTransform* pTargetTransform = static_cast<CTransform*>(pTarget->Get_Component(TEXT("Com_Transform")));

    pOwnerTransform->LookAt_Lerp(pTargetTransform->Get_State(STATE::POSITION),fTimeDelta, 3.f);

    _vector vDir = XMVector3Normalize(pTargetTransform->Get_State(STATE::POSITION) - pOwnerTransform->Get_State(STATE::POSITION));
    vDir.m128_f32[1] = 0.f;
    if (LOCKONDIR::LOCKON_LEFT == m_eLockOnDir)
        pOwnerTransform->Go_Left(m_fMoveSpeed * fTimeDelta);
    else if (LOCKONDIR::LOCKON_RIGHT == m_eLockOnDir)
        pOwnerTransform->Go_Right(m_fMoveSpeed * fTimeDelta);

    _vector vLook = XMVector3Normalize(pOwnerTransform->Get_State(STATE::LOOK));
    _float fDot = XMVectorGetX(XMVector3Dot(vDir, vLook));

    if (fDot > m_fDotThreshold)
    {
        pBB->Set_Value<_bool>(pYetuga->Get_Name(), "isLockOnFinished",true);
        pFSM->Change_State(ENUM_CLASS(YETUGA_STATE::IDLE),pOwner);
        return;
    }

    CModel* pModel = static_cast<CModel*>(pYetuga->Get_Body()->Get_Component(TEXT("Com_Model")));
    pModel->Play_Animation(fTimeDelta);
}

void CAS_LockOn_Yetuga::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{

}

CAS_LockOn_Yetuga* CAS_LockOn_Yetuga::Create()
{
    return new CAS_LockOn_Yetuga();
}

void CAS_LockOn_Yetuga::Free()
{

    __super::Free();
}
