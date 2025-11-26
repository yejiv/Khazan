#include "AS_CutScene_Start_Viper.h"
#include "AI_Controller.h"
#include "FSM_Viper.h"
#include "Viper.h"
#include "Body_Viper.h"
#include "GameInstance.h"

CAS_CutScene_Start_Viper::CAS_CutScene_Start_Viper()
{

}

void CAS_CutScene_Start_Viper::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    CViper* pViper = static_cast<CViper*>(pOwner);
    CTransform* pTransform = static_cast<CTransform*>(pOwner->Get_Component(TEXT("Com_Transform")));
    pTransform->Rotation(0,XMConvertToRadians(180.f),0.f);
}

void CAS_CutScene_Start_Viper::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    CViper* pViper = static_cast<CViper*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pViper->Get_Body()->Get_Component(TEXT("Com_Model")));

   
    if (m_pGameInstance->Key_Down(DIK_0))
        Change_CutSceneState(CUTSCENE_STATE::SIT, pModel,pViper);
    else if(m_pGameInstance->Key_Down(DIK_1))
        Change_CutSceneState(CUTSCENE_STATE::LAND, pModel, pViper);
    else if (m_pGameInstance->Key_Down(DIK_2))
        Change_CutSceneState(CUTSCENE_STATE::STAND, pModel, pViper);

    if (m_eState == CUTSCENE_STATE::LAND)
    {
        pViper->Set_ViperPosition(XMVectorSet(-34.749f, -29.986f, 208.162f, 1.f));
    }
        
    else if (m_eState == CUTSCENE_STATE::STAND)
    {
        pViper->Set_ViperPosition(XMVectorSet(-34.749f, -29.986f, 208.162f, 1.f));
    }
    
    if (pModel->Play_Animation(fTimeDelta))
    {
        if (m_eState == CUTSCENE_STATE::SIT)
            pViper->Set_ViperPosition(XMVectorSet(-34.749f, -29.986f, 208.162f, 1.f));

        if (m_eState == CUTSCENE_STATE::STAND)
        {
            // 여기서 시작 플래그
        }

    }
   
}

void CAS_CutScene_Start_Viper::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{

}

void CAS_CutScene_Start_Viper::Change_CutSceneState(CUTSCENE_STATE eNextState , CModel* pModel, CViper* pViper)
{
    if (m_eState == eNextState)
        return;

    m_eState = eNextState;
    
    switch (m_eState)
    {
    case Client::CUTSCENE_STATE::SIT:
        pModel->Set_Animation(ENUM_CLASS(CUTSCENE_STATE::SIT));
        break;
    case Client::CUTSCENE_STATE::LAND:
        pModel->Set_Animation(ENUM_CLASS(CUTSCENE_STATE::LAND));
        
        break;
    case Client::CUTSCENE_STATE::STAND:
        pModel->Set_Animation(ENUM_CLASS(CUTSCENE_STATE::STAND));
        break;

    }
    
       
}


CAS_CutScene_Start_Viper* CAS_CutScene_Start_Viper::Create()
{
    return new CAS_CutScene_Start_Viper;
}

void CAS_CutScene_Start_Viper::Free()
{
    __super::Free();
}
