#include "AS_CutScene_2Phase_Viper.h"
#include "AI_Controller.h"
#include "FSM_Viper.h"
#include "Viper.h"
#include "Body_Viper.h"
#include "Body_Cinematic_Viper.h"
#include "Body_Phase2_Viper.h"
#include "GameInstance.h"
#include "Core_Viper.h"
#include "ClientInstance.h"
#include "UI_HUD.h"
#include "BossHp.h"

CAS_CutScene_2Phase_Viper::CAS_CutScene_2Phase_Viper()
{

}

void CAS_CutScene_2Phase_Viper::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    CViper* pViper = static_cast<CViper*>(pOwner);
    CTransform* pTransform = static_cast<CTransform*>(pOwner->Get_Component(TEXT("Com_Transform")));
    static_cast<CUI_HUD*>(CClientInstance::GetInstance()->Get_RootUI(TEXT("HUD")))->Switch_Panel(false);
    CBossHp::BOSSMON_UPDATE_DESC Desc;
    Desc.isOpen = false;
    CClientInstance::GetInstance()->UI_UpdateSwitch(TEXT("BossHp"), &Desc);

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

        if (m_pGameInstance->Key_Pressing(DIK_RCONTROL, fTimeDelta))
        {
            if (m_pGameInstance->Key_Down(DIK_0))
                ViperScene_Down(pViper);
            else if (m_pGameInstance->Key_Down(DIK_1))
                ViperScene_PickUp(pViper);
            else if (m_pGameInstance->Key_Down(DIK_2))
                ViperScene_Injure(pViper);
            else if (m_pGameInstance->Key_Down(DIK_3))
                ViperScene_Heptysis(pViper);
        }
        pModel->Play_Animation(fTimeDelta);
    }
    break;

    case Client::CViper::PHASE::CINEMATIC:
    {
        CModel* pModel = static_cast<CModel*>(pViper->Get_Cinematic_Body()->Get_Component(TEXT("Com_Model")));

        if (m_pGameInstance->Key_Pressing(DIK_RCONTROL, fTimeDelta))
        {
            if (m_pGameInstance->Key_Down(DIK_1))
                ViperScene_Congestion(pViper);
            else if (m_pGameInstance->Key_Down(DIK_2))
                ViperScene_HandDn(pViper);
        }

        pModel->Play_Animation(fTimeDelta);

    }
    break;

    case Client::CViper::PHASE::PHASE2:
    {
        CModel* pModel = static_cast<CModel*>(pViper->Get_P2Body()->Get_Component(TEXT("Com_Model")));

        if (m_pGameInstance->Key_Pressing(DIK_RCONTROL, fTimeDelta))
        {
            if (m_pGameInstance->Key_Down(DIK_1))
                ViperScene_PullOut(pViper);
            else if (m_pGameInstance->Key_Down(DIK_2))
                ViperScene_Walk(pViper);
        }
        if (pModel->Play_Animation(fTimeDelta))
        {

        }
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
        pViper->Set_ViperPosition(XMVectorSet(-30.103f, -29.9f, 188.961f, 1.f));
        break;
    case Client::P2CUTSCENE_STATE::PICKUP:

        pModel->Set_Animation(ENUM_CLASS(P2CUTSCENE_STATE::PICKUP));
        pViper->Set_PhaseWeapon_Cinematic();
        pViper->Set_ViperPosition(XMVectorSet(-30.103f, -29.9f, 185.861f, 1.f));
        break;
    case Client::P2CUTSCENE_STATE::INJURE:
        pModel->Set_Animation(ENUM_CLASS(P2CUTSCENE_STATE::INJURE));
        pViper->Set_ViperPosition(XMVectorSet(-30.103f, -29.9f, 185.861f, 1.f));
        break;
    case Client::P2CUTSCENE_STATE::HEMPTYSIS:
        pModel->Set_Animation(ENUM_CLASS(P2CUTSCENE_STATE::HEMPTYSIS));
        pViper->Set_ViperPosition(XMVectorSet(-30.103f, -29.9f, 185.861f, 1.f));
        break;
    case Client::P2CUTSCENE_STATE::CONGESTION:
        pModel->Set_Animation(ENUM_CLASS(P2CUTSCENE_STATE::CONGESTION));
        pViper->Set_ViperPosition(XMVectorSet(-30.103f, -29.9f, 185.861f, 1.f));
        break;
    case Client::P2CUTSCENE_STATE::HANDDN:
        pModel->Set_Animation(ENUM_CLASS(P2CUTSCENE_STATE::HANDDN));
        pModel->Set_BlendTime(0.25f);
        pViper->Set_ViperPosition(XMVectorSet(-30.103f, -29.9f, 185.861f, 1.f));
        break;

    case Client::P2CUTSCENE_STATE::PULLOUT:
    {
        pModel->Set_Animation(ENUM_CLASS(P2CUTSCENE_STATE::PULLOUT));
        CBlackBoard* pBB = pViper->Get_Controller()->Get_BlackBoard();
        pBB->Set_Value<_bool>(pViper->Get_Name(), "isP2Cinematic_Walk", true);
        pViper->Set_ViperPosition(XMVectorSet(-30.103f, -29.9f, 185.861f, 1.f));
        m_pGameInstance->PlaySoundOnce(TEXT("Mon_CIN_Embars_BE_SFX_01 (SFX).wav"), 30.f, pViper->Get_SoundChannel(ENUM_CLASS(MONSFX::ATVO)));
        break;
    }
    case Client::P2CUTSCENE_STATE::WALK:
    {
        pModel->Set_Animation(ENUM_CLASS(P2CUTSCENE_STATE::WALK));
        CBlackBoard* pBB = pViper->Get_Controller()->Get_BlackBoard();
        pBB->Set_Value<_bool>(pViper->Get_Name(), "isP2Cinematic_Walk", false);
        pViper->Set_ViperPosition(XMVectorSet(-30.103f, -29.9f, 185.861f, 1.f));
        //static_cast<CUI_HUD*>(CClientInstance::GetInstance()->Get_RootUI(TEXT("HUD")))->Switch_Panel(true);
        m_pGameInstance->StopByKey_FadeOut(TEXT("Mon_CIN_Embars_BE_SFX_01 (SFX).wav"), 3.f);
        break;
    }


    }

}

void CAS_CutScene_2Phase_Viper::ViperScene_Down(CViper* pViper)
{
    CModel* pModel = static_cast<CModel*>(pViper->Get_Body()->Get_Component(TEXT("Com_Model")));
    Change_CutSceneState(P2CUTSCENE_STATE::DOWN, pModel, pViper);
}

void CAS_CutScene_2Phase_Viper::ViperScene_PickUp(CViper* pViper)
{
    CModel* pModel = static_cast<CModel*>(pViper->Get_Body()->Get_Component(TEXT("Com_Model")));
    Change_CutSceneState(P2CUTSCENE_STATE::PICKUP, pModel, pViper);
}

void CAS_CutScene_2Phase_Viper::ViperScene_Injure(CViper* pViper)
{
    CModel* pModel = static_cast<CModel*>(pViper->Get_Body()->Get_Component(TEXT("Com_Model")));
    Change_CutSceneState(P2CUTSCENE_STATE::INJURE, pModel, pViper);
}

void CAS_CutScene_2Phase_Viper::ViperScene_Heptysis(CViper* pViper)
{
    CModel* pModel = static_cast<CModel*>(pViper->Get_Body()->Get_Component(TEXT("Com_Model")));
    Change_CutSceneState(P2CUTSCENE_STATE::HEMPTYSIS, pModel, pViper);
}

void CAS_CutScene_2Phase_Viper::ViperScene_Congestion(CViper* pViper)
{
    CModel* pModel = static_cast<CModel*>(pViper->Get_Cinematic_Body()->Get_Component(TEXT("Com_Model")));
    Change_CutSceneState(P2CUTSCENE_STATE::CONGESTION, pModel, pViper);
}

void CAS_CutScene_2Phase_Viper::ViperScene_HandDn(CViper* pViper)
{
    CModel* pModel = static_cast<CModel*>(pViper->Get_Cinematic_Body()->Get_Component(TEXT("Com_Model")));
    Change_CutSceneState(P2CUTSCENE_STATE::HANDDN, pModel, pViper);
}

void CAS_CutScene_2Phase_Viper::ViperScene_PullOut(CViper* pViper)
{
    CModel* pModel = static_cast<CModel*>(pViper->Get_P2Body()->Get_Component(TEXT("Com_Model")));
    Change_CutSceneState(P2CUTSCENE_STATE::PULLOUT, pModel, pViper);
}

void CAS_CutScene_2Phase_Viper::ViperScene_Walk(CViper* pViper)
{
    CModel* pModel = static_cast<CModel*>(pViper->Get_P2Body()->Get_Component(TEXT("Com_Model")));
    Change_CutSceneState(P2CUTSCENE_STATE::WALK, pModel, pViper);
}


CAS_CutScene_2Phase_Viper* CAS_CutScene_2Phase_Viper::Create()
{
    return new CAS_CutScene_2Phase_Viper;
}

void CAS_CutScene_2Phase_Viper::Free()
{
    __super::Free();
}
