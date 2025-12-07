#include "AS_P2_FakeRunAttack_Viper.h"
#include "Viper.h"
#include "Body_Phase2_Viper.h"
#include "BlackBoard.h"
#include "AI_Controller.h"
#include "GameInstance.h"
#include "FSM_Viper.h"

CAS_P2_FakeRunAttack_Viper::CAS_P2_FakeRunAttack_Viper()
{

}

void CAS_P2_FakeRunAttack_Viper::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{

    CViper* pViper = static_cast<CViper*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pViper->Get_P2Body()->Get_Component(TEXT("Com_Model")));
    CBlackBoard* pBB = pViper->Get_Controller()->Get_BlackBoard();
    m_eState = FAKERUNATTACKSTATE::RUN;
    m_fSpeed = pBB->Get_Value<_float>(pViper->Get_Name(), "RunSpeed");
    pModel->Set_Animation(44);

    
    //CGameObject* pTarget = pBB->Get_Value<CGameObject*>(pViper->Get_Name(), "Target");
    //CTransform* pTargetTransform = static_cast<CTransform*>(pTarget->Get_Component(TEXT("Com_Transform")));
    //CTransform* pOwnerTransform = static_cast<CTransform*>(pOwner->Get_Component(TEXT("Com_Transform")));
    //_vector vPosition = pOwnerTransform->Get_State(STATE::POSITION);
    //XMStoreFloat3(&m_vStartPos, vPosition);

    //_vector vTargetPos = pTargetTransform->Get_State(STATE::POSITION);
    //_vector vDir = XMVector3Normalize(vPosition - vTargetPos);
    //_float fJumpDist = 30.f;

    //XMStoreFloat3(&m_vGoalPos, vTargetPos + vDir * fJumpDist);





    m_fAnimSpeed = 1.f;

    pBB->Set_Value<_uint>(pViper->Get_Name(), "AttackCount", 0);
}

void CAS_P2_FakeRunAttack_Viper::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    CViper* pViper = static_cast<CViper*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pViper->Get_P2Body()->Get_Component(TEXT("Com_Model")));
    CBlackBoard* pBB = pViper->Get_Controller()->Get_BlackBoard();



    if (pBB->Get_Value<_bool>(pViper->Get_Name(), "isFakeAttac_Movement1"))
    {
        CTransform* pOwnerTransform = static_cast<CTransform*>(pOwner->Get_Component(TEXT("Com_Transform")));
        pOwnerTransform->Go_Straight(fTimeDelta * 5.f);

    }

    if (pBB->Get_Value<_bool>(pViper->Get_Name(), "FakeAttac_Movement2"))
    {
        CTransform* pOwnerTransform = static_cast<CTransform*>(pOwner->Get_Component(TEXT("Com_Transform")));
        pOwnerTransform->Go_Straight(fTimeDelta);

    }

    if (pBB->Get_Value<_bool>(pViper->Get_Name(), "FakeAttac_Movement3"))
    {
        CTransform* pOwnerTransform = static_cast<CTransform*>(pOwner->Get_Component(TEXT("Com_Transform")));
        pOwnerTransform->Go_Straight(fTimeDelta);

    }


    if (FAKERUNATTACKSTATE::RUN == m_eState)
    {
        CGameObject * pTarget = pBB->Get_Value<CGameObject*>(pViper->Get_Name(), "Target");
        CTransform* pOwnerTransform = static_cast<CTransform*>(pOwner->Get_Component(TEXT("Com_Transform")));
        _float fTargetDist = pBB->Get_Value<_float>(pViper->Get_Name(), "TargetDist");
        pViper->Get_Controller()->AI_MoveTo(pOwner, pTarget, 10.f, m_fSpeed, fTimeDelta);
        if (fTargetDist <= 10.f)
        {
            m_eState = FAKERUNATTACKSTATE::ATTACK;
            pModel->Set_Animation(26);
            m_fAnimSpeed = 1.5f;
        }

    }

    if (pModel->Play_Animation(fTimeDelta * m_fAnimSpeed))
    {
        CBlackBoard* pBB = pViper->Get_Controller()->Get_BlackBoard();
        pFSM->Change_State(ENUM_CLASS(VIPER_STATE_P1::IDLE), pViper);
        pBB->Set_Value<_bool>(pViper->Get_Name(), "is_P2_FakeRunAttackFinished", true);
    }
}

void CAS_P2_FakeRunAttack_Viper::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
   
}

void CAS_P2_FakeRunAttack_Viper::OnCollision(COLLISION_DESC* pDesc, _uint iCollisionLayer, CGameObject* pOwner)
{
    COLLISION_LAYER eLayer = static_cast<COLLISION_LAYER>(iCollisionLayer);

    if (COLLISION_LAYER::PLAYER == eLayer)
    {
        CViper* pViper = static_cast<CViper*>(pOwner);
        CBlackBoard* pBB = pViper->Get_Controller()->Get_BlackBoard();
        _uint iAttackCnt = pBB->Get_Value<_uint>(pViper->Get_Name(), "AttackCount");
        CCreature* pTarget = static_cast<CCreature*>(pDesc->pGameObject);
        CTransform* pOwnerTransform = static_cast<CTransform*>(pOwner->Get_Component(TEXT("Com_Transform")));
        if (nullptr == pOwnerTransform)
            return;

        if (iAttackCnt == 1)
        {
            pTarget->Take_Damage(10.f, HITREACTION::KNOCKBACK_WEAK);
            _vector vLook = pOwnerTransform->Get_State(STATE::LOOK);
            pTarget->KnockBack(vLook, 20.f, 40.f);
        }
        else if (iAttackCnt == 2)
        {
            pTarget->Take_Damage(10.f, HITREACTION::KNOCKBACK_NORMAL);
            _vector vLook = pOwnerTransform->Get_State(STATE::LOOK);
            pTarget->KnockBack(vLook, 20.f, 60.f);
        }
      

    }
}

CAS_P2_FakeRunAttack_Viper* CAS_P2_FakeRunAttack_Viper::Create()
{
    return new CAS_P2_FakeRunAttack_Viper();
}

void CAS_P2_FakeRunAttack_Viper::Free()
{
    __super::Free();
}
