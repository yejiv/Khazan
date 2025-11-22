#include "AS_LockOn_Viper.h"
#include "AI_Controller.h"
#include "Viper.h"
#include "GameInstance.h"
#include "BlackBoard.h"
#include "FSM_Viper.h"
#include "Body_Viper.h"


CAS_LockOn_Viper::CAS_LockOn_Viper()
{

}

void CAS_LockOn_Viper::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{

    m_fDotThreshold = cosf(XMConvertToRadians(10.f));
    m_fMoveSpeed = 0.9f;

    CViper* pViper = static_cast<CViper*>(pOwner);
    CBlackBoard* pBB = pViper->Get_Controller()->Get_BlackBoard();
    CModel* pModel = static_cast<CModel*>(pViper->Get_Body()->Get_Component(TEXT("Com_Model")));
    CGameObject* pTarget = static_cast<CGameObject*>(pBB->Get_Value<CGameObject*>(pViper->Get_Name(), "Target"));
    CTransform* pOwnerTransform = static_cast<CTransform*>(pOwner->Get_Component(TEXT("Com_Transform")));
    CTransform* pTargetTransform = static_cast<CTransform*>(pTarget->Get_Component(TEXT("Com_Transform")));
    _vector vPosition = pOwnerTransform->Get_State(STATE::POSITION);
    _vector vTargetPos = pTargetTransform->Get_State(STATE::POSITION);
    _vector vDir = XMVector3Normalize(vTargetPos - vPosition);
    _vector vRight = XMVector3Normalize(pOwnerTransform->Get_State(STATE::RIGHT));
    _float fDot = XMVectorGetX(XMVector3Dot(vDir, vRight));

    if (fDot < -0.3f)
    {
        pModel->Set_Animation(99); //¿ÞÂÊ
        m_eLockOnDir = LOCKONDIR::LOCKON_LEFT;
    }
    else if (fDot > 0.3f)
    {
        pModel->Set_Animation(100); // ¿À¸¥ÂÊ
        m_eLockOnDir = LOCKONDIR::LOCKON_RIGHT;
    }



}

void CAS_LockOn_Viper::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    CViper* pViper = static_cast<CViper*>(pOwner);
    CBlackBoard* pBB = pViper->Get_Controller()->Get_BlackBoard();

    CTransform* pOwnerTransform = static_cast<CTransform*>(pOwner->Get_Component(TEXT("Com_Transform")));
    CGameObject* pTarget = pBB->Get_Value<CGameObject*>(pViper->Get_Name(), "Target");
    CTransform* pTargetTransform = static_cast<CTransform*>(pTarget->Get_Component(TEXT("Com_Transform")));

    pOwnerTransform->LookAt_Lerp(pTargetTransform->Get_State(STATE::POSITION), fTimeDelta, 0.8f);

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
        pBB->Set_Value<_bool>(pViper->Get_Name(), "isP1_LockOnFinished", true);
        pFSM->Change_State(ENUM_CLASS(VIPER_STATE_P1::IDLE), pOwner);
    }

    CModel* pModel = static_cast<CModel*>(pViper->Get_Body()->Get_Component(TEXT("Com_Model")));
    pModel->Play_Animation(fTimeDelta);
}

void CAS_LockOn_Viper::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{

}

CAS_LockOn_Viper* CAS_LockOn_Viper::Create()
{
    return new CAS_LockOn_Viper();
}

void CAS_LockOn_Viper::Free()
{

    __super::Free();
}
