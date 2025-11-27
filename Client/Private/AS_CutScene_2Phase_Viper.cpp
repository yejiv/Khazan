#include "AS_CutScene_2Phase_Viper.h"
#include "AI_Controller.h"
#include "FSM_Viper.h"
#include "Viper.h"
#include "Body_Viper.h"
#include "Body_Cinematic_Viper.h"
#include "GameInstance.h"

CAS_CutScene_2Phase_Viper::CAS_CutScene_2Phase_Viper()
{

}

void CAS_CutScene_2Phase_Viper::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    CViper* pViper = static_cast<CViper*>(pOwner);
    CTransform* pTransform = static_cast<CTransform*>(pOwner->Get_Component(TEXT("Com_Transform")));
}

void CAS_CutScene_2Phase_Viper::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    CViper* pViper = static_cast<CViper*>(pOwner);
    

    CViper::PHASE ePhase = pViper->Get_Phase();

    switch (ePhase)
    {
    case Client::CViper::PHASE::PHASE1:
    {
        CModel* pModel = static_cast<CModel*>(pViper->Get_Body()->Get_Component(TEXT("Com_Model")));

        if (m_pGameInstance->Key_Down(DIK_0))
            Change_CutSceneState(P2CUTSCENE_STATE::DOWN, pModel, pViper);
        else if (m_pGameInstance->Key_Down(DIK_1))
            Change_CutSceneState(P2CUTSCENE_STATE::PICKUP, pModel, pViper);
        else if (m_pGameInstance->Key_Down(DIK_2))
            Change_CutSceneState(P2CUTSCENE_STATE::INJURE, pModel, pViper);
        else if (m_pGameInstance->Key_Down(DIK_3))
            Change_CutSceneState(P2CUTSCENE_STATE::HEMPTYSIS, pModel, pViper);

        pModel->Play_Animation(fTimeDelta);
    }
        break;

    case Client::CViper::PHASE::CINEMATIC:
    {
        CModel* pModel = static_cast<CModel*>(pViper->Get_Cinematic_Body()->Get_Component(TEXT("Com_Model")));

        if (m_pGameInstance->Key_Down(DIK_0))
            Change_CutSceneState(P2CUTSCENE_STATE::CONGESTION, pModel, pViper);
        else if (m_pGameInstance->Key_Down(DIK_1))
            Change_CutSceneState(P2CUTSCENE_STATE::HANDDN, pModel, pViper);

        pModel->Play_Animation(fTimeDelta);
    }
        break;

    case Client::CViper::PHASE::PHASE2:
    {
        /*  else if (m_pGameInstance->Key_Down(DIK_6))
       Change_CutSceneState(P2CUTSCENE_STATE::HANDDN, pModel, pViper);
   else if (m_pGameInstance->Key_Down(DIK_7))
       Change_CutSceneState(P2CUTSCENE_STATE::HANDDN, pModel, pViper);
   else if (m_pGameInstance->Key_Down(DIK_8))
       Change_CutSceneState(P2CUTSCENE_STATE::HANDDN, pModel, pViper);*/

    }
        break;
 
    }

   

 


}

void CAS_CutScene_2Phase_Viper::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{

}

void CAS_CutScene_2Phase_Viper::Change_CutSceneState(P2CUTSCENE_STATE eNextState, CModel* pModel, CViper* pViper)
{
    if (m_eState == eNextState)
        return;

  

    m_eState = eNextState;

    switch (m_eState)
    {
    case Client::P2CUTSCENE_STATE::DOWN:
        pModel->Set_Animation(ENUM_CLASS(P2CUTSCENE_STATE::DOWN));
        break;
    case Client::P2CUTSCENE_STATE::PICKUP:
        pModel->Set_Animation(ENUM_CLASS(P2CUTSCENE_STATE::PICKUP));
        break;
    case Client::P2CUTSCENE_STATE::INJURE:
        pModel->Set_Animation(ENUM_CLASS(P2CUTSCENE_STATE::INJURE));
        break;
    case Client::P2CUTSCENE_STATE::HEMPTYSIS:
        pModel->Set_Animation(ENUM_CLASS(P2CUTSCENE_STATE::HEMPTYSIS));
        break;
    case Client::P2CUTSCENE_STATE::CONGESTION:
        pModel->Set_Animation(ENUM_CLASS(P2CUTSCENE_STATE::CONGESTION));
        break;
    case Client::P2CUTSCENE_STATE::HANDDN:
        pModel->Set_Animation(ENUM_CLASS(P2CUTSCENE_STATE::HANDDN));
        break;
    }
   
}


CAS_CutScene_2Phase_Viper* CAS_CutScene_2Phase_Viper::Create()
{
    return new CAS_CutScene_2Phase_Viper;
}

void CAS_CutScene_2Phase_Viper::Free()
{
    __super::Free();
}
