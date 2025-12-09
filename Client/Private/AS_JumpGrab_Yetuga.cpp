#include "AS_JumpGrab_Yetuga.h"
#include "AI_Controller.h"
#include "Yetuga.h"
#include "GameInstance.h"
#include "BlackBoard.h"
#include "FSM_Yetuga.h"
#include "Body_Yetuga.h"

CAS_JumpGrab_Yetuga::CAS_JumpGrab_Yetuga()
{
    
}

void CAS_JumpGrab_Yetuga::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    CYetuga* pYetuga = static_cast<CYetuga*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pYetuga->Get_Body()->Get_Component(TEXT("Com_Model")));
    CBlackBoard* BB = pYetuga->Get_Controller()->Get_BlackBoard();

    CTransform* pTransform = static_cast<CTransform*>(pOwner->Get_Component(TEXT("Com_Transform")));
    CTransform* pTargetTransform = static_cast<CTransform*>(BB->Get_Value<CGameObject*>(pYetuga->Get_Name(), "Target")->Get_Component(TEXT("Com_Transform")));
    _vector vTargetLoc = pTargetTransform->Get_State(STATE::POSITION);

    pTransform->LookAt(vTargetLoc);


    pModel->Set_Animation(57);
    m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_yetuga_rush_grab_wing_whoosh_01 (SFX).wav"), pYetuga->Get_Position(), pYetuga->Get_SoundChannel(ENUM_CLASS(MONSFX::SWISH)), 30.f);
    m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_yetuga_rush_grab_01 (SFX).wav"), pYetuga->Get_Position(), pYetuga->Get_SoundChannel(ENUM_CLASS(MONSFX::ATVO)), 30.f);

}

void CAS_JumpGrab_Yetuga::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    CYetuga* pYetuga = static_cast<CYetuga*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pYetuga->Get_Body()->Get_Component(TEXT("Com_Model")));
    CBlackBoard* pBB = pYetuga->Get_Controller()->Get_BlackBoard();

    _bool isGrabbed = pBB->Get_Value<_bool>(pYetuga->Get_Name(), "isGrabbed");


    if (pBB->Get_Value<_bool>(pYetuga->Get_Name(), "JumpNotify"))
    {
        pYetuga->Get_Controller()->
            AI_MoveTo(pOwner,
                pBB->Get_Value<CGameObject*>(pYetuga->Get_Name(), "Target"),
                pBB->Get_Value<_float>(pYetuga->Get_Name(), "AttackRange"),
                10,
                fTimeDelta);
    }

    if (isGrabbed && !m_isGrabbed)
    {
        pModel->Set_Animation(58);
        m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_yetuga_rush_grab_success_01 (SFX).wav"), pYetuga->Get_Position(), pYetuga->Get_SoundChannel(ENUM_CLASS(MONSFX::SWISH)), 30.f);
        m_isGrabbed = true;
    }

    if (pModel->Play_Animation(fTimeDelta))
    {
        pBB->Set_Value<_bool>(pYetuga->Get_Name(), "isJumpGrabFinished", true);
        
        m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_yetuga_rush_grab_end_01(SFX).wav"), pYetuga->Get_Position(), pYetuga->Get_SoundChannel(ENUM_CLASS(MONSFX::SWISH)), 30.f);

        pFSM->Change_State(ENUM_CLASS(YETUGA_STATE::IDLE), pYetuga);
    }

}

void CAS_JumpGrab_Yetuga::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
    CYetuga* pYetuga = static_cast<CYetuga*>(pOwner);
    CBlackBoard* pBB = pYetuga->Get_Controller()->Get_BlackBoard();
    pBB->Set_Value<_bool>(pYetuga->Get_Name(), "isGrabbed", false);
    m_isGrabbed = false;
}

void CAS_JumpGrab_Yetuga::OnCollision(COLLISION_DESC* pDesc, _uint iCollisionLayer, CGameObject* pOwner)
{
    COLLISION_LAYER eLayer = static_cast<COLLISION_LAYER>(iCollisionLayer);
    if (COLLISION_LAYER::PLAYER == eLayer)
    {
        CYetuga* pYetuga = static_cast<CYetuga*>(pOwner);
        CBlackBoard* pBB = pYetuga->Get_Controller()->Get_BlackBoard();
        CCreature* pTarget = static_cast<CCreature*>(pDesc->pGameObject);
        pTarget->Take_Damage(0, HITREACTION::GRAB, nullptr);
        pBB->Set_Value<_bool>("Yetuga", "isGrabbed", true);

    }
}



CAS_JumpGrab_Yetuga* CAS_JumpGrab_Yetuga::Create()
{
    return new CAS_JumpGrab_Yetuga();
}

void CAS_JumpGrab_Yetuga::Free()
{
    __super::Free();
}
