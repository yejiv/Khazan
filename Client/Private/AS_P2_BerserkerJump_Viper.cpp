#include "AS_P2_BerserkerJump_Viper.h"
#include "Viper.h"
#include "Body_Phase2_Viper.h"
#include "BlackBoard.h"
#include "AI_Controller.h"
#include "GameInstance.h"
#include "FSM_Viper.h"

CAS_P2_BerserkerJump_Viper::CAS_P2_BerserkerJump_Viper()
{

}

void CAS_P2_BerserkerJump_Viper::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{

    CViper* pViper = static_cast<CViper*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pViper->Get_P2Body()->Get_Component(TEXT("Com_Model")));
    CBlackBoard* pBB = pViper->Get_Controller()->Get_BlackBoard();

    CGameObject* pTarget = static_cast<CGameObject*>(pBB->Get_Value<CGameObject*>(pViper->Get_Name(), "Target"));
    CTransform* pOwnerTransform = static_cast<CTransform*>(pOwner->Get_Component(TEXT("Com_Transform")));
    CTransform* pTargetTransform = static_cast<CTransform*>(pTarget->Get_Component(TEXT("Com_Transform")));

    _vector vOwnerRight = pOwnerTransform->Get_State(STATE::RIGHT);
    _vector vTargetLook = pTargetTransform->Get_State(STATE::LOOK);

    _float fDot = XMVectorGetX(XMVector3Dot(vOwnerRight, vTargetLook));
    if (fDot < -0.1f)
        pModel->Set_Animation(45);

    else
        pModel->Set_Animation(46);

    m_eState = BSJUMPSTATE::SIDEJUMP;
    //m_iJumpCnt = pBB->Get_Value<_uint>(pViper->Get_Name(), "BerserkerJumpCount"); 
    m_iJumpCnt = 1;
    m_fMoveSpeed = 18.f;

}

void CAS_P2_BerserkerJump_Viper::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    CViper* pViper = static_cast<CViper*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pViper->Get_P2Body()->Get_Component(TEXT("Com_Model")));
    CBlackBoard* pBB = pViper->Get_Controller()->Get_BlackBoard();


    if (m_eState == BSJUMPSTATE::SIDEJUMP)
    {
        if (pBB->Get_Value<_bool>(pViper->Get_Name(), "SkipMontion"))
        {
            pModel->Set_Animation(21);
            pViper->SFX_DASHDRIFT();
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_viper_p2_dash_drift_roar_02 (SFX).wav"), 1.f);

            m_eState = BSJUMPSTATE::FRONTJUMP;
        }
    }


    if (m_eState == BSJUMPSTATE::FRONTJUMP)
    {
        CTransform* pOwnerTransform = static_cast<CTransform*>(pViper->Get_Component(TEXT("Com_Transform")));
        CGameObject* pTarget = pBB->Get_Value<CGameObject*>(pViper->Get_Name(), "Target");
        CTransform* pTargetTransform = static_cast<CTransform*>(pTarget->Get_Component(TEXT("Com_Transform")));
        _vector vOwnerPos = pOwnerTransform->Get_State(STATE::POSITION);
        _vector vTargetPos = pTargetTransform->Get_State(STATE::POSITION);
        _float fDist = pBB->Get_Value<_float>(pViper->Get_Name(), "TargetDist");
        
        _float fMinSpeed = 6.f;
        _float fMaxSpeed = 22.f;
        _float fMinDist = 3.f;
        _float fMaxDist = 25.f;

        _float t = (fDist - fMinDist) / (fMaxDist - fMinDist);
        t = clamp(t, 0.f, 1.f);
        m_fMoveSpeed = fMinSpeed + (fMaxSpeed - fMinSpeed) * t;
        pViper->Get_Controller()->AI_MoveTo(pViper, pTarget, 10.f, m_fMoveSpeed, fTimeDelta);



        if (fDist < 10 + 0.1f)
        {
            m_eState = BSJUMPSTATE::ATTACK;
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_viper_p2_dash_drift_end_atk_whoosh_01 (SFX).wav"), 1.f);
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_viper_p2_dash_drift_roar_02 (SFX).wav"), 1.f);

            pModel->Set_Animation(20);
        }
    }

    if (pModel->Play_Animation(fTimeDelta))
    {

        //if (BSJUMPSTATE::SIDEJUMP== m_eState)
        //{
        //    pModel->Set_Animation(21);
        //    m_eState = BSJUMPSTATE::FRONTJUMP;
        //}

        if (BSJUMPSTATE::ATTACK == m_eState && m_iJumpCnt != 0)
        {
            m_eState = BSJUMPSTATE::FRONTJUMP;
            pViper->SFX_DASHDRIFT();
            pModel->Set_Animation(21);
            --m_iJumpCnt;
        }

        else if (BSJUMPSTATE::ATTACK == m_eState && m_iJumpCnt == 0)
        {
            pBB->Set_Value<_bool>(pViper->Get_Name(), "SkipMontion", false);
            pBB->Set_Value<_bool>(pViper->Get_Name(), "is_P2_BerserkerJumpFinished", true);
            pFSM->Change_State(ENUM_CLASS(VIPER_STATE_P1::IDLE), pViper);
        }
    }
}

void CAS_P2_BerserkerJump_Viper::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
    CViper* pViper = static_cast<CViper*>(pOwner);
    CBlackBoard* pBB = pViper->Get_Controller()->Get_BlackBoard();
    pBB->Set_Value<_bool>(pViper->Get_Name(), "SkipMontion", false);
    pViper->Set_IsGhost(false);

}

void CAS_P2_BerserkerJump_Viper::OnCollision(COLLISION_DESC* pDesc, _uint iCollisionLayer, CGameObject* pOwner)
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

        pTarget->Take_Damage(199.f, HITREACTION::KNOCKBACK_NORMAL);
        _vector vLook = pOwnerTransform->Get_State(STATE::LOOK);
        pTarget->KnockBack(vLook, 20.f, 60.f);

    }
}

CAS_P2_BerserkerJump_Viper* CAS_P2_BerserkerJump_Viper::Create()
{
    return new CAS_P2_BerserkerJump_Viper();
}

void CAS_P2_BerserkerJump_Viper::Free()
{
    __super::Free();
}
