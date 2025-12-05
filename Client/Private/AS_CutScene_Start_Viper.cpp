#include "AS_CutScene_Start_Viper.h"
#include "AI_Controller.h"
#include "FSM_Viper.h"
#include "Viper.h"
#include "Body_Viper.h"
#include "GameInstance.h"
#include "ClientInstance.h"
#include "UI_HUD.h"


CAS_CutScene_Start_Viper::CAS_CutScene_Start_Viper()
{

}

void CAS_CutScene_Start_Viper::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    CViper* pViper = static_cast<CViper*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pViper->Get_Body()->Get_Component(TEXT("Com_Model")));
    CTransform* pTransform = static_cast<CTransform*>(pOwner->Get_Component(TEXT("Com_Transform")));
    pTransform->Rotation(0,XMConvertToRadians(180.f),0.f);
    _vector vCutSceneLook = pTransform->Get_State(STATE::LOOK);
    _float3 vTempLook = {};
    XMStoreFloat3(&vTempLook, vCutSceneLook);
    pViper->Set_CutSceneLook(vTempLook);
    m_fTimeHelper = 0.03f;
    pModel->Set_Animation(ENUM_CLASS(CUTSCENE_STATE::SIT));
}

void CAS_CutScene_Start_Viper::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    CViper* pViper = static_cast<CViper*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pViper->Get_Body()->Get_Component(TEXT("Com_Model")));

    
    if (m_pGameInstance->Key_Pressing(DIK_RCONTROL, fTimeDelta))
    {
        if (m_pGameInstance->Key_Down(DIK_0))
            ViperScene_Sit(pViper);
        else if (m_pGameInstance->Key_Down(DIK_1))
            ViperScene_Jump(pViper);
        else if (m_pGameInstance->Key_Down(DIK_2))
            ViperScene_Land(pViper);
        else if (m_pGameInstance->Key_Down(DIK_3))
            ViperScene_Roar(pViper);
    }


    if (pModel->Play_Animation(fTimeDelta * m_fTimeHelper))
    {
        CBlackBoard* pBB = pViper->Get_Controller()->Get_BlackBoard();

        if (m_eState == CUTSCENE_STATE::JUMP)
        {
            
        }
        else if (m_eState == CUTSCENE_STATE::STAND)
        {
            pBB->Set_Value<_bool>(pViper->Get_Name(), "isStartCutSceneJump", false);
            static_cast<CUI_HUD*>(CClientInstance::GetInstance()->Get_RootUI(TEXT("HUD")))->Switch_Panel(true);

        }
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

void CAS_CutScene_Start_Viper::ViperScene_Jump(CViper* pViper)
{
    CModel* pModel = static_cast<CModel*>(pViper->Get_Body()->Get_Component(TEXT("Com_Model")));
    Change_CutSceneState(CUTSCENE_STATE::JUMP, pModel, pViper);
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

    CBlackBoard* pBB = pViper->Get_Controller()->Get_BlackBoard();

    m_eState = eNextState;
    
    switch (m_eState)
    {
    case Client::CUTSCENE_STATE::SIT:
    {
        pModel->Set_Animation(ENUM_CLASS(CUTSCENE_STATE::SIT));
        pViper->Set_ViperPosition(XMVectorSet(-37.938f, -15.453f, 223.393f, 1.f));
        pBB->Set_Value<_bool>(pViper->Get_Name(),"isStartCutSceneSit",true);
        break;
    }

    case Client::CUTSCENE_STATE::JUMP:
    {
        pModel->Set_Animation(ENUM_CLASS(CUTSCENE_STATE::JUMP));        
        pViper->Set_Teleport(XMVectorSet(-30.838f, -5.35f, 199.893f, 1.f));
        break;
    }        
    case Client::CUTSCENE_STATE::LAND:
    {
        pViper->Reset_Viper_Gravity();
        pModel->Set_Animation(ENUM_CLASS(CUTSCENE_STATE::LAND));
        _vector vPosition = XMVectorSet(-31.938f, -29.986f, 201.162f, 1.f);
        pViper->Set_ViperPosition(vPosition);
        break;
    }       
    case Client::CUTSCENE_STATE::STAND:
    {
        pModel->Set_Animation(ENUM_CLASS(CUTSCENE_STATE::STAND));
        _vector vPosition = XMVectorSet(-31.938f, -29.986f, 198.162f, 1.f);
        pViper->Set_ViperPosition(vPosition);
        break;
    }                    
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
