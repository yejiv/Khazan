#include "AS_P2_SwingCombo_VIper.h"
#include "Viper.h"
#include "Body_Phase2_Viper.h"
#include "BlackBoard.h"
#include "AI_Controller.h"
#include "GameInstance.h"
#include "FSM_Viper.h"

CAS_SwingCombo_VIper::CAS_SwingCombo_VIper()
{

}

void CAS_SwingCombo_VIper::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{

    CViper* pViper = static_cast<CViper*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pViper->Get_P2Body()->Get_Component(TEXT("Com_Model")));
    pModel->Set_Animation(48);

    CBlackBoard* pBB = pViper->Get_Controller()->Get_BlackBoard();
    pBB->Set_Value<_uint>(pViper->Get_Name(), "AttackCount", 0);
    pBB->Set_Value(pViper->Get_Name(), "isP2_Combo_Abort", false);


}

void CAS_SwingCombo_VIper::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    CViper* pViper = static_cast<CViper*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pViper->Get_P2Body()->Get_Component(TEXT("Com_Model")));
    CBlackBoard* pBB = pViper->Get_Controller()->Get_BlackBoard();

    
    if (pBB->Get_Value<_bool>(pViper->Get_Name(), "is_P2_ComboMove"))
    {
        _float fAttackRange = pBB->Get_Value<_float>(pViper->Get_Name(), "AttackRange");
        CGameObject* pTarget = pBB->Get_Value<CGameObject*>(pViper->Get_Name(), "Target");
        pViper->Get_Controller()->AI_MoveTo(pViper,pTarget, fAttackRange + 10.f, 10.f,fTimeDelta);
    }

    if (pBB->Get_Value<_bool>(pViper->Get_Name(), "is_P2_Rush"))
    {
        
        CTransform* pOwnerTransform = static_cast<CTransform*>(pOwner->Get_Component(TEXT("Com_Transform")));
        pOwnerTransform->Go_Straight(fTimeDelta);
    }

    if (pBB->Get_Value<_uint>(pViper->Get_Name(), "AttackCount") == 4)
    {
        if (!pBB->Get_Value<_bool>(pViper->Get_Name(), "isP2_Combo_Abort"))
        {
            CTransform* pOwnerTransform = static_cast<CTransform*>(pOwner->Get_Component(TEXT("Com_Transform")));
            pOwnerTransform->Go_Straight(fTimeDelta * 6.f);
        }
    }


    if (pBB->Get_Value<_bool>(pViper->Get_Name(), "isP2_Combo_Abort"))
    {
        pFSM->Change_State(ENUM_CLASS(VIPER_STATE_P1::IDLE), pViper);
        pBB->Set_Value<_bool>(pViper->Get_Name(), "is_P2_SwingComboFinished", true);
    }


    if (pModel->Play_Animation(fTimeDelta))
    {
        pFSM->Change_State(ENUM_CLASS(VIPER_STATE_P1::IDLE), pViper);
        pBB->Set_Value<_bool>(pViper->Get_Name(), "is_P2_SwingComboFinished", true);
    }
}

void CAS_SwingCombo_VIper::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
   
}

void CAS_SwingCombo_VIper::OnCollision(COLLISION_DESC* pDesc, _uint iCollisionLayer, CGameObject* pOwner)
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
            pTarget->KnockBack(vLook, 20.f, 20.f);
        }
        else if (iAttackCnt == 2)
        {
            pTarget->Take_Damage(10.f, HITREACTION::KNOCKBACK_WEAK);
            _vector vLook = pOwnerTransform->Get_State(STATE::LOOK);
            pTarget->KnockBack(vLook, 20.f, 20.f);
        }

        if (iAttackCnt == 3)
        {
            pTarget->Take_Damage(10.f, HITREACTION::KNOCKBACK_NORMAL);
            _vector vLook = pOwnerTransform->Get_State(STATE::LOOK);
            pTarget->KnockBack(vLook, 20.f, 40.f);
        }
        else if (iAttackCnt == 4)
        {
            pTarget->Take_Damage(10.f, HITREACTION::KNOCKBACK_NORMAL);
            _vector vLook = pOwnerTransform->Get_State(STATE::LOOK);
            pTarget->KnockBack(vLook, 20.f, 40.f);
        }
        else if (iAttackCnt == 5)
        {
            pTarget->Take_Damage(10.f, HITREACTION::KNOCKBACK_STRONG);
            _vector vLook = pOwnerTransform->Get_State(STATE::LOOK);
            pTarget->KnockBack(vLook, 20.f, 60.f);
        }

    }
}

CAS_SwingCombo_VIper* CAS_SwingCombo_VIper::Create()
{
    return new CAS_SwingCombo_VIper();
}

void CAS_SwingCombo_VIper::Free()
{
    __super::Free();
}
