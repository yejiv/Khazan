#include "AS_P2_LockOn_Viper.h"
#include "Viper.h"
#include "Body_Phase2_Viper.h"
#include "BlackBoard.h"
#include "AI_Controller.h"
#include "GameInstance.h"
#include "FSM_Viper.h"

CAS_P2_LockOn_Viper::CAS_P2_LockOn_Viper()
{
}

void CAS_P2_LockOn_Viper::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    CViper* pViper = static_cast<CViper*>(pOwner);
    CBlackBoard* pBB = pViper->Get_Controller()->Get_BlackBoard();
    CModel* pModel = static_cast<CModel*>(pViper->Get_P2Body()->Get_Component(TEXT("Com_Model")));
    CTransform* pOwnerTransform = static_cast<CTransform*>(pOwner->Get_Component(TEXT("Com_Transform")));

    CGameObject* pTarget = pBB->Get_Value<CGameObject*>(pViper->Get_Name(), "Target");
    CTransform* pTargetTransform = static_cast<CTransform*>(pTarget->Get_Component(TEXT("Com_Transform")));

    _vector vOwnerPos = pOwnerTransform->Get_State(STATE::POSITION);
    _vector vTargetPos = pTargetTransform->Get_State(STATE::POSITION);
    _vector vDir = XMVector3Normalize(vTargetPos - vOwnerPos);
    vDir.m128_f32[1] = 0.f;

    _vector vLook = XMVector3Normalize(pOwnerTransform->Get_State(STATE::LOOK));
    _vector vRight = XMVector3Normalize(pOwnerTransform->Get_State(STATE::RIGHT));

    _float fDotF = XMVectorGetX(XMVector3Dot(vDir, vLook));
    _float fDotR = XMVectorGetX(XMVector3Dot(vDir, vRight));

    //방향 계산
    if (fDotF > 0.7f)
    {
        m_eDirState = DIRECTION::F;
        m_fMoveSpeed = 0.5f;
        pModel->Set_Animation(37); 
    }
    if (fDotF < -0.7f)
    {
        m_eDirState = DIRECTION::B;
        pModel->Set_Animation(36);
    }
    else if (fDotR > 0.0f)
    {
        m_eDirState = DIRECTION::R;
        pModel->Set_Animation(39);
    }
    else
    {
        m_eDirState = DIRECTION::L;
        pModel->Set_Animation(38);
    }

    // 블랙보드에 저장
    pBB->Set_Value(pViper->Get_Name(), "LockDir", static_cast<_uint>(m_eDirState));

    m_fMoveSpeed = 1.2f;
    m_fDotThreshold = cosf(XMConvertToRadians(20.f));
}

void CAS_P2_LockOn_Viper::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    CViper* pViper = static_cast<CViper*>(pOwner);
    CBlackBoard* pBB = pViper->Get_Controller()->Get_BlackBoard();

    CModel* pModel = static_cast<CModel*>(pViper->Get_P2Body()->Get_Component(TEXT("Com_Model")));
    CTransform* pOwnerTransform = static_cast<CTransform*>(pOwner->Get_Component(TEXT("Com_Transform")));

    CGameObject* pTarget = pBB->Get_Value<CGameObject*>(pViper->Get_Name(), "Target");
    CTransform* pTargetTransform = static_cast<CTransform*>(pTarget->Get_Component(TEXT("Com_Transform")));

    _vector vTargetPos = pTargetTransform->Get_State(STATE::POSITION);
    _vector vOwnerPos = pOwnerTransform->Get_State(STATE::POSITION);

    // 타겟 바라보기
    pOwnerTransform->LookAt_Lerp(vTargetPos, fTimeDelta, 0.18f);

    // 방향에 따른 이동
    switch (m_eDirState)
    {
    case DIRECTION::F:
        pOwnerTransform->Go_Straight(m_fMoveSpeed * fTimeDelta);
        break;
    case DIRECTION::B:
        pOwnerTransform->Go_Backward(m_fMoveSpeed * fTimeDelta);
        break;
    case DIRECTION::L:
        pOwnerTransform->Go_Left(m_fMoveSpeed * fTimeDelta);
        break;
    case DIRECTION::R:
        pOwnerTransform->Go_Right(m_fMoveSpeed * fTimeDelta);
        break;
    }

    // 탈출 조건: Look 방향이 타겟 방향과 맞음
    _vector vDir = XMVector3Normalize(vTargetPos - vOwnerPos);
    vDir.m128_f32[1] = 0.f;
    _vector vLook = XMVector3Normalize(pOwnerTransform->Get_State(STATE::LOOK));

    _float fDotF = XMVectorGetX(XMVector3Dot(vDir, vLook));

    if (fDotF > m_fDotThreshold)
    {
        pBB->Set_Value(pViper->Get_Name(), "isP2_LockOn_Finished", true);
        //pFSM->Change_State(ENUM_CLASS(VIPER_STATE_P1::IDLE), pOwner);
    }
    pModel->Play_Animation(fTimeDelta);


}

void CAS_P2_LockOn_Viper::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
}

CAS_P2_LockOn_Viper* CAS_P2_LockOn_Viper::Create()
{
    return new CAS_P2_LockOn_Viper();
}

void CAS_P2_LockOn_Viper::Free()
{
    __super::Free();
}
