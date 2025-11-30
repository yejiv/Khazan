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
        ViperScene_Sit(pViper);
    else if (m_pGameInstance->Key_Down(DIK_1))
        ViperScene_Land(pViper);
    else if (m_pGameInstance->Key_Down(DIK_2))
        ViperScene_Roar(pViper);

  
    if (pModel->Play_Animation(fTimeDelta))
    {
       
    }
   
}

void CAS_CutScene_Start_Viper::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{

}

void CAS_CutScene_Start_Viper::ViperScene_Sit(CViper* pViper)
{
    CModel* pModel = static_cast<CModel*>(pViper->Get_Body()->Get_Component(TEXT("Com_Model")));
    Change_CutSceneState(CUTSCENE_STATE::SIT, pModel, pViper);
}

void CAS_CutScene_Start_Viper::ViperScene_Land(CViper* pViper)
{
    CModel* pModel = static_cast<CModel*>(pViper->Get_Body()->Get_Component(TEXT("Com_Model")));
    Change_CutSceneState(CUTSCENE_STATE::LAND, pModel, pViper);
}

void CAS_CutScene_Start_Viper::ViperScene_Roar(CViper* pViper)
{
    CModel* pModel = static_cast<CModel*>(pViper->Get_Body()->Get_Component(TEXT("Com_Model")));
    Change_CutSceneState(CUTSCENE_STATE::STAND, pModel, pViper);
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
        pViper->Set_ViperPosition(XMVectorSet(-37.938f, -15.453f, 223.393f, 1.f));
        break;
    case Client::CUTSCENE_STATE::LAND:
        pModel->Set_Animation(ENUM_CLASS(CUTSCENE_STATE::LAND));
        _vector vPosition = XMVectorSet(-31.938f, -29.986f, 201.162f, 1.f);
        pViper->Set_ViperPosition(vPosition);
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
