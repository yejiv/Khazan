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
        pViper->Get_Controller()->AI_MoveTo(pViper,pTarget,0.5f, fAttackRange,fTimeDelta);
    }

    if (pBB->Get_Value<_bool>(pViper->Get_Name(), "is_P2_Rush"))
    {
        CGameObject* pTarget = pBB->Get_Value<CGameObject*>(pViper->Get_Name(), "Target");
        CTransform* pOwnerTransform = static_cast<CTransform*>(pOwner->Get_Component(TEXT("Com_Transform")));
        _vector vTargetPos = pTarget->Get_Transform()->Get_State(STATE::POSITION);
        pOwnerTransform->Go_Straight(fTimeDelta);
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

CAS_SwingCombo_VIper* CAS_SwingCombo_VIper::Create()
{
    return new CAS_SwingCombo_VIper();
}

void CAS_SwingCombo_VIper::Free()
{
    __super::Free();
}
