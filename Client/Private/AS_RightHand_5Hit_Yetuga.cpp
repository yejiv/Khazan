#include "AS_RightHand_5Hit_Yetuga.h"
#include "AI_Controller.h"
#include "Yetuga.h"
#include "GameInstance.h"
#include "BlackBoard.h"
#include "FSM_Yetuga.h"
#include "Body_Yetuga.h"

CAS_RightHand_5Hit_Yetuga::CAS_RightHand_5Hit_Yetuga()
{
}

void CAS_RightHand_5Hit_Yetuga::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    CYetuga* pYetuga = static_cast<CYetuga*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pYetuga->Get_Body()->Get_Component(TEXT("Com_Model")));

    pModel->Set_Animation(61);
    pModel->Set_AnimationLoop(false);

    CBlackBoard* pBB = pYetuga->Get_Controller()->Get_BlackBoard();
    pBB->Set_Value<_uint>(pYetuga->Get_Name(), "AttackCount", 0);

    m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_yetuga_dempseyroll_01 (SFX).wav"), pYetuga->Get_Position(), pYetuga->Get_SoundChannel(ENUM_CLASS(MONSFX::SWISH)), 30.f);
    m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_yetuga_dempseyroll_01 (SFX).wav"), pYetuga->Get_Position(), pYetuga->Get_SoundChannel(ENUM_CLASS(MONSFX::ATVO)), 30.f);
}

void CAS_RightHand_5Hit_Yetuga::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    CYetuga* pYetuga = static_cast<CYetuga*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pYetuga->Get_Body()->Get_Component(TEXT("Com_Model")));

    if (pModel->Play_Animation(fTimeDelta))
    {
        CBlackBoard* pBB = pYetuga->Get_Controller()->Get_BlackBoard();
        pBB->Set_Value<_bool>(pYetuga->Get_Name(), "isAttack2Finished", true);
        pFSM->Change_State(ENUM_CLASS(YETUGA_STATE::IDLE), pYetuga);
    }
}

void CAS_RightHand_5Hit_Yetuga::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
    m_iComboCount = 0;

}

void CAS_RightHand_5Hit_Yetuga::OnCollision(COLLISION_DESC* pDesc, _uint iCollisionLayer, CGameObject* pOwner)
{
    COLLISION_LAYER eLayer = static_cast<COLLISION_LAYER>(iCollisionLayer);
    if (COLLISION_LAYER::PLAYER == eLayer)
    {
        CCreature* pTarget = static_cast<CCreature*>(pDesc->pGameObject);
        if (nullptr == pTarget)
            return;
        CTransform* pOwnerTransform = static_cast<CTransform*>(pOwner->Get_Component(TEXT("Com_Transform")));
        if (nullptr == pOwnerTransform)
            return;
        _vector vLook = pOwnerTransform->Get_State(STATE::LOOK);
        pTarget->KnockBack(vLook, 15.f, 50.f);
        pTarget->Take_Damage(10.f,HITREACTION::KNOCKBACK_NORMAL);
    }
}

void CAS_RightHand_5Hit_Yetuga::On_JustGuard(CGameObject* pOwner)
{
    CYetuga* pYetuga = static_cast<CYetuga*>(pOwner);
    CBlackBoard* pBB = pYetuga->Get_Controller()->Get_BlackBoard();
 
    _uint iAttackCnt = pBB->Get_Value<_uint>(pYetuga->Get_Name(), "AttackCount");
    if (iAttackCnt == 5)
    {
        pBB->Set_Value<_bool>(pYetuga->Get_Name(), "isAttack2Finished", true);
        pBB->Set_Value<_bool>(pYetuga->Get_Name(), "isJustGuard", true);
        pBB->Set_Value<_bool>(pYetuga->Get_Name(), "isGroggy", true);
        pBB->Set_Value<_uint>(pYetuga->Get_Name(), "AttackCount", 0);

    }
}

CAS_RightHand_5Hit_Yetuga* CAS_RightHand_5Hit_Yetuga::Create()
{
    return new CAS_RightHand_5Hit_Yetuga();
}

void CAS_RightHand_5Hit_Yetuga::Free()
{
    __super::Free();
}
