#include "AS_JumpSmash_Viper.h"
#include "Viper.h"
#include "Body_Viper.h"
#include "BlackBoard.h"
#include "AI_Controller.h"
#include "GameInstance.h"


CAS_JumpSmash_Viper::CAS_JumpSmash_Viper()
{
}

void CAS_JumpSmash_Viper::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    CViper* pViper = static_cast<CViper*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pViper->Get_Body()->Get_Component(TEXT("Com_Model")));
    pModel->Set_Animation(34);

    //// 시작 위치 저장
    //CTransform* pOwnerTransform = static_cast<CTransform*>(pOwner->Get_Component(TEXT("Com_Transform")));
    //XMStoreFloat3(&m_vStartPos, pOwnerTransform->Get_State(STATE::POSITION));

    //// 정점(상승 끝 위치)
    //m_vPeakPos = m_vStartPos;
    //m_vPeakPos.y += 350.f;

    // 목표 착지 위치는 Enter에서 한 번만 계산 (플레이어 위치 저장)
    CBlackBoard* pBB = pViper->Get_Controller()->Get_BlackBoard();
    CGameObject* pTarget = pBB->Get_Value<CGameObject*>(pViper->Get_Name(), "Target");
    CTransform* pTargetTransform = static_cast<CTransform*>(pTarget->Get_Component(TEXT("Com_Transform")));
    XMStoreFloat3(&m_vGoalPos, pTargetTransform->Get_State(STATE::POSITION));
}

void CAS_JumpSmash_Viper::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    CViper* pViper = static_cast<CViper*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pViper->Get_Body()->Get_Component(TEXT("Com_Model")));
    CBlackBoard* pBB = pViper->Get_Controller()->Get_BlackBoard();

 
    if (pBB->Get_Value<_bool>(pViper->Get_Name(), "P1_LandStart"))
    {
        _float fAnimRatio = pModel->MakeRatio() * 0.5f;
        CTransform* pOwnerTransform = static_cast<CTransform*>(pOwner->Get_Component(TEXT("Com_Transform")));
        _float t = fAnimRatio;
        t = t * t * t; //점점 빨라지며 떨어짐

        _vector vPeak = XMLoadFloat3(&m_vPeakPos);
        _vector vGoalPos = XMLoadFloat3(&m_vGoalPos);

        // XZ만 보간 (공중에서 수직 낙하 보정)
        _vector vNew = XMVectorLerp(vPeak, vGoalPos, t);
        vNew.m128_f32[1] = vPeak.m128_f32[1] - (350.f * t);

        pOwnerTransform->Set_State(STATE::POSITION, vNew);
    }


    if (pModel->Play_Animation(fTimeDelta))
    {
        CBlackBoard* pBB = pViper->Get_Controller()->Get_BlackBoard();
        pBB->Set_Value<_bool>(pViper->Get_Name(), "isP1_JumpSmashFinished", true);
    }
}

void CAS_JumpSmash_Viper::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
}

CAS_JumpSmash_Viper* CAS_JumpSmash_Viper::Create()
{
    return new CAS_JumpSmash_Viper();
}

void CAS_JumpSmash_Viper::Free()
{
    __super::Free();
}

