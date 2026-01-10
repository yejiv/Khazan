#include "AS_StingGrab_Viper.h"
#include "AI_Controller.h"
#include "Viper.h"
#include "GameInstance.h"
#include "BlackBoard.h"
#include "FSM_Viper.h"
#include "Body_Viper.h"

CAS_StingGrab_Viper::CAS_StingGrab_Viper()
{
}

void CAS_StingGrab_Viper::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    CViper* pViper = static_cast<CViper*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pViper->Get_Body()->Get_Component(TEXT("Com_Model")));
    CBlackBoard* BB = pViper->Get_Controller()->Get_BlackBoard();

    pModel->Set_Animation(58);
    //pModel->Set_Animation(59);
    m_eState = VIPERGRAB_STATE::NONE;


}

void CAS_StingGrab_Viper::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    CViper* pViper = static_cast<CViper*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pViper->Get_Body()->Get_Component(TEXT("Com_Model")));
    CBlackBoard* pBB = pViper->Get_Controller()->Get_BlackBoard();
    CTransform* pTransform = pViper->Get_Transform();

    _bool isGrabbed = pBB->Get_Value<_bool>(pViper->Get_Name(), "isGrabbed");


    if (isGrabbed && !m_isGrabbed)
    {
        pBB->Set_Value<_bool>(pViper->Get_Name(), "isP1_StingGrab_Rush", false);
        pModel->Set_Animation(59);
        CClientInstance::GetInstance()->Set_PlayerInput(false);
        CCreature* pTarget = static_cast<CCreature*>(pBB->Get_Value<CGameObject*>(pViper->Get_Name(), "Target"));
        m_eState = VIPERGRAB_STATE::SUCCESS;
        m_isGrabbed = true;
    }


    if (pBB->Get_Value<_bool>(pViper->Get_Name(), "isP1_StingGrab_StepBack"))
    {
        pTransform->Go_Backward(fTimeDelta * 2.5f);
    }


    if (pBB->Get_Value<_bool>(pViper->Get_Name(), "isP1_StingGrab_Rush"))
    {
        pTransform->Go_Straight(fTimeDelta * 8.f);
    }


    if (pModel->Play_Animation(fTimeDelta))
    {

        if (m_eState == VIPERGRAB_STATE::SUCCESS || m_eState == VIPERGRAB_STATE::FAIL)
        {
            pBB->Set_Value<_bool>(pViper->Get_Name(), "isP1_StingGrabFinished", true);
            pFSM->Change_State(ENUM_CLASS(VIPER_STATE_P1::IDLE), pViper);
        }
        else if (m_eState == VIPERGRAB_STATE::NONE)
        {
            pModel->Set_Animation(56);
            m_eState = VIPERGRAB_STATE::FAIL;
        }
    }

}

void CAS_StingGrab_Viper::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
    CViper* pViper = static_cast<CViper*>(pOwner);
    CBlackBoard* pBB = pViper->Get_Controller()->Get_BlackBoard();
    pBB->Set_Value<_bool>(pViper->Get_Name(), "isGrabbed", false);
    m_isGrabbed = false;

    m_pGameInstance->Stop_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Grap"), pViper->Get_FxRotIdx());

}

void CAS_StingGrab_Viper::OnCollision(COLLISION_DESC* pDesc, _uint iCollisionLayer, CGameObject* pOwner)
{
    COLLISION_LAYER eLayer = static_cast<COLLISION_LAYER>(iCollisionLayer);
    if (COLLISION_LAYER::PLAYER == eLayer)
    {
        CViper* pViper = static_cast<CViper*>(pOwner);
        CBlackBoard* pBB = pViper->Get_Controller()->Get_BlackBoard();
        CCreature* pTarget = static_cast<CCreature*>(pDesc->pGameObject);
        pTarget->Take_Damage(0, HITREACTION::GRAB, nullptr);
        pBB->Set_Value<_bool>(pViper->Get_Name(), "isGrabbed", true);

    }
}



CAS_StingGrab_Viper* CAS_StingGrab_Viper::Create()
{
    return new CAS_StingGrab_Viper();
}

void CAS_StingGrab_Viper::Free()
{
    __super::Free();
}
