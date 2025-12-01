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

    m_fMoveSpeed = 0.5f;
    m_fDotThreshold = cosf(XMConvertToRadians(20.f));
    m_fTimeAcc = 0.f;
    m_fMinLockTime = 2.5f;
    m_fMaxLockTime = 10.f;

    _vector vOwnerPos = pOwnerTransform->Get_State(STATE::POSITION);
    _vector vTargetPos = pTargetTransform->Get_State(STATE::POSITION);
  
    _vector vDiff = vTargetPos - vOwnerPos;
    vDiff = XMVectorSetY(vDiff,0.f);
    _float fStartDist = XMVectorGetX(XMVector3Length(vDiff));
    m_fEndDist = fStartDist * 0.5f; // 시작거리의 60% 정도 가까워지면 탈출
    
    Update_Direction(pOwnerTransform, pTargetTransform, pModel);
  
    // 블랙보드에 저장
    pBB->Set_Value<_uint>(pViper->Get_Name(), "LockDir", static_cast<_uint>(m_eDirState));

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

    m_fTimeAcc += fTimeDelta;
    
    // 타겟 바라보기
    //pOwnerTransform->LookAt_Lerp(vTargetPos, fTimeDelta, 0.5f);
    pOwnerTransform->LookAt_Lerp(vTargetPos, fTimeDelta, m_fTurnSpeed);

  
    // 방향으로 이동
    Move_To_Direction(pOwnerTransform,fTimeDelta);



    // 탈출 조건: Look 방향이 타겟 방향과 맞음
    _vector vDir = XMVector3Normalize(vTargetPos - vOwnerPos);
    vDir = XMVectorSetY(vDir, 0.f);

    _matrix matBodyCombined = {};
    _float4x4 matTemp = pViper->Get_P2Body()->Get_CombinedMatrix();
    matBodyCombined = XMLoadFloat4x4(&matTemp);

    _vector vBodyLook = matBodyCombined.r[2];
    vBodyLook = XMVectorSetY(vBodyLook, 0.f);
    vBodyLook = XMVector3Normalize(vBodyLook);

    _float fDotF = XMVectorGetX(XMVector3Dot(vDir, vBodyLook));

    _float fDist = XMVectorGetX(XMVector3Length(vDir));

    _bool isLockOnFinished = { false };

    if (m_fTimeAcc >= m_fMinLockTime)
    {
        if (fDotF > m_fDotThreshold)
            isLockOnFinished = true;

        if (fDist >= m_fEndDist)
            isLockOnFinished = true;

    }

    if (m_fTimeAcc >= m_fMaxLockTime)
        isLockOnFinished = true;

    if (isLockOnFinished)
    {
        pBB->Set_Value(pViper->Get_Name(), "isP2_LockOn_Finished", true);
        pBB->Set_Value<_bool>(pViper->Get_Name(), "isP2LockOn", false);
        //pFSM->Change_State(ENUM_CLASS(VIPER_STATE_P1::IDLE), pOwner);
    }
    pModel->Play_Animation(fTimeDelta);


}

void CAS_P2_LockOn_Viper::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
}

void CAS_P2_LockOn_Viper::Update_Direction(CTransform* pOwnerTransform, CTransform* pTargetTransfrom, CModel* pModel)
{
    _vector vOwnerPos = pOwnerTransform->Get_State(STATE::POSITION);
    _vector vTargetPos = pTargetTransfrom->Get_State(STATE::POSITION);

    _vector vDir = vTargetPos - vOwnerPos;
    vDir = XMVectorSetY(vDir,0.f);
    vDir = XMVector3Normalize(vDir);

    _vector vLook = pOwnerTransform->Get_State(STATE::LOOK);
    vLook = XMVectorSetY(vLook, 0.f);
    vLook = XMVector3Normalize(vLook);

    _vector vRight = pOwnerTransform->Get_State(STATE::RIGHT);
    vRight = XMVectorSetY(vRight, 0.f);
    vRight = XMVector3Normalize(vRight);

    _float fDotF = XMVectorGetX(XMVector3Dot(vDir,vLook));
    _float fDotR = XMVectorGetX(XMVector3Dot(vDir,vRight));


    //DIRECTION ePrevDir = m_eDirState;

    _float fDist = XMVectorGetX(XMVector3Length(vDir));
    _bool isBackward = {false};    
    // 거리기반 랜덤으로 수정

     /*if (fDotF < -0.7f)
     {
         m_eDirState = DIRECTION::B;
     }*/

    if (fDist < 300.f)
    {
        _float fChance = m_pGameInstance->Rand(0,1);
        if (fChance > 0.4)
            isBackward = true;
    }

    if (isBackward)
        m_eDirState = DIRECTION::B;

    else if (fDotF > 0.7f)
        m_eDirState = DIRECTION::F;

    else
    {
        // 좌/우
        if (fDotR > 0.0f)
            m_eDirState = DIRECTION::R;
        else
            m_eDirState = DIRECTION::L;
    }

    // 방향 바뀔 때만 애니 다시 세팅
    //if (ePrevDir != m_eDirState)
    {
        switch (m_eDirState)
        {
        case DIRECTION::F:
            pModel->Set_Animation(37); // 전진 락온
            m_fMoveSpeed = 0.5f;
            m_fTurnSpeed = 0.5f;
            break;
        case DIRECTION::B:
            pModel->Set_Animation(36); // 후진 락온
            m_fMoveSpeed = 0.5f;
            m_fTurnSpeed = 0.5f;
            break;
        case DIRECTION::L:
            pModel->Set_Animation(38); // 좌측 락온
            m_fMoveSpeed = 0.5f;
            m_fTurnSpeed = 2.f;
            break;
        case DIRECTION::R:
            pModel->Set_Animation(39); // 우측 락온
            m_fMoveSpeed = 0.5f;
            m_fTurnSpeed = 2.f;
            break;
        }
    }
}

void CAS_P2_LockOn_Viper::Move_To_Direction(CTransform* pOwnerTransform, _float fTimeDelta)
{
    _float fMove = m_fMoveSpeed * fTimeDelta;

    switch (m_eDirState)
    {
    case DIRECTION::F:
        pOwnerTransform->Go_Straight(fMove);
        break;
    case DIRECTION::B:
        pOwnerTransform->Go_Backward(fMove);
        break;
    case DIRECTION::L:
        pOwnerTransform->Go_Left(fMove);
        break;
    case DIRECTION::R:
        pOwnerTransform->Go_Right(fMove);
        break;
    }
}

CAS_P2_LockOn_Viper* CAS_P2_LockOn_Viper::Create()
{
    return new CAS_P2_LockOn_Viper();
}

void CAS_P2_LockOn_Viper::Free()
{
    __super::Free();
}
