#include "AS_P2_DashDrift_Viper.h"
#include "Viper.h"
#include "Body_Phase2_Viper.h"
#include "BlackBoard.h"
#include "AI_Controller.h"
#include "GameInstance.h"
#include "FSM_Viper.h"



CAS_P2_DashDrift_Viper::CAS_P2_DashDrift_Viper()
{

}

void CAS_P2_DashDrift_Viper::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    CViper* pViper = static_cast<CViper*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pViper->Get_P2Body()->Get_Component(TEXT("Com_Model")));

    pModel->Set_Animation(22);

    CBlackBoard* pBB = pViper->Get_Controller()->Get_BlackBoard();
  
    m_fMoveSpeed = 18.f;

    m_fAttackRange = pBB->Get_Value<_float>(pViper->Get_Name(), "AttackRange");


    m_eState = DRIFTSTATE::START;
}

void CAS_P2_DashDrift_Viper::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    CViper* pViper = static_cast<CViper*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pViper->Get_P2Body()->Get_Component(TEXT("Com_Model")));
    CBlackBoard* pBB = pViper->Get_Controller()->Get_BlackBoard();


    if (m_eState == DRIFTSTATE::LOOP)
    {
        pViper->Set_IsGhost(true);
        CTransform* pOwnerTransform = static_cast<CTransform*>(pViper->Get_Component(TEXT("Com_Transform")));
        CGameObject* pTarget = pBB->Get_Value<CGameObject*>(pViper->Get_Name(), "Target");
        CTransform* pTargetTransform = static_cast<CTransform*>(pTarget->Get_Component(TEXT("Com_Transform")));
        _vector vOwnerPos = pOwnerTransform->Get_State(STATE::POSITION);
        _vector vTargetPos = pTargetTransform->Get_State(STATE::POSITION);
        pViper->Get_Controller()->AI_MoveTo(pViper,pTarget,10.f, m_fMoveSpeed,fTimeDelta);
        //pOwnerTransform->AI_Chase(vTargetPos, fTimeDelta, m_fMoveSpeed, m_fAttackRange);

        _float fDist = pBB->Get_Value<_float>(pViper->Get_Name(), "TargetDist");

        if (fDist < 10 +0.1f /*|| pBB->Get_Value<_bool>(pViper->Get_Name(),"isP2_Dash_Abort")*/)
        {
            m_eState = DRIFTSTATE::FINISH;
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_viper_p2_dash_drift_end_atk_whoosh_01 (SFX).wav"), pViper->Get_Position(), pViper->Get_SoundChannel(ENUM_CLASS(MONSFX::SWISH)), 30.f);
            pModel->Set_Animation(20);
        }
    }


    if (pModel->Play_Animation(fTimeDelta))
    {

        switch (m_eState)
        {
        case Client::DRIFTSTATE::START:
        {
            m_eState = DRIFTSTATE::LOOP;
            pViper->SFX_DASHDRIFT();
            pModel->Set_Animation(21);
        }
        break;
      
        case Client::DRIFTSTATE::FINISH:
        {
            //CGameObject* pTarget = pBB->Get_Value<CGameObject*>(pViper->Get_Name(), "Target");
            //CTransform* pTargetTransform = static_cast<CTransform*>(pTarget->Get_Transform());
            //_vector vTargetPos = pTargetTransform->Get_State(STATE::POSITION);
            //pViper->Get_Transform()->LookAt(vTargetPos);
            pBB->Set_Value<_bool>(pViper->Get_Name(), "is_P2_DashDriftFinished",true);
            pBB->Set_Value<_bool>(pViper->Get_Name(), "isP2_Dash_Abort", false);
            //pViper->Set_IsGhost(false);
        }
        break;
        }
    }

}

void CAS_P2_DashDrift_Viper::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{

}

void CAS_P2_DashDrift_Viper::OnCollision(COLLISION_DESC* pDesc, _uint iCollisionLayer, CGameObject* pOwner)
{
    COLLISION_LAYER eLayer = static_cast<COLLISION_LAYER>(iCollisionLayer);

    if (COLLISION_LAYER::PLAYER == eLayer)
    {
        CViper* pViper = static_cast<CViper*>(pOwner);
        CBlackBoard* pBB = pViper->Get_Controller()->Get_BlackBoard();
        CCreature* pTarget = static_cast<CCreature*>(pDesc->pGameObject);
        CTransform* pOwnerTransform = static_cast<CTransform*>(pOwner->Get_Component(TEXT("Com_Transform")));
        if (nullptr == pOwnerTransform)
            return;

        pTarget->Take_Damage(10.f, HITREACTION::KNOCKBACK_NORMAL);
        _vector vLook = pOwnerTransform->Get_State(STATE::LOOK);
        pTarget->KnockBack(vLook, 20.f, 60.f);

    }
}

CAS_P2_DashDrift_Viper* CAS_P2_DashDrift_Viper::Create()
{
    return new CAS_P2_DashDrift_Viper;
}

void CAS_P2_DashDrift_Viper::Free()
{
    __super::Free();
}
