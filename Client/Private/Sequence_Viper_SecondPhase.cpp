#include "Sequence_Viper_SecondPhase.h"
#include "GameInstance.h"
#include "ClientInstance.h"
#include "Transform.h"
#include "GameObject.h"
#include "Viper.h"
#include "Camera_Compre.h"
#include "AS_CutScene_2Phase_Viper.h"
#include "FSM_Viper.h"
#include "CharacterVirtual.h"
#include "Khazan_GSword.h"
#include "AI_Controller_Viper.h"
#include "SkipButton.h"
#include "Obelisk.h"
#include "SkySphere.h"
#include "CloudSphere.h"

CSequence_Viper_SecondPhase::CSequence_Viper_SecondPhase(CViper* pViper, CKhazan_GSword* pKhazan)
    : m_pGameInstance{ CGameInstance::GetInstance() }
    , m_pClientInstance{ CClientInstance::GetInstance() }
    , m_pViper{ pViper }
    , m_pKhazan{ pKhazan }
{
    Safe_AddRef(m_pGameInstance);
    Safe_AddRef(m_pClientInstance);
}

HRESULT CSequence_Viper_SecondPhase::Initialize(const SEQ_REQ_PLAY_DESC& tDesc)
{
    m_pCamera = dynamic_cast<CCamera_Compre*>(m_pClientInstance->Get_ActiveCamera());
    m_pClientInstance->Camera_Set_Animation_Json("../../Client/Bin/Data/Camera/Animation/Viper_SecondPhase18");

    CCharacterVirtual* pCharVir = dynamic_cast<CCharacterVirtual*>(m_pKhazan->Get_Component(TEXT("Com_CharacterVirtual")));
    pCharVir->Teleport(XMVectorSet(0.f, 0.f, 0.f, 1.f), m_pKhazan->Get_Transform()->Get_Rotation_Quat(), m_pKhazan->Get_Transform());    

    return S_OK;
}

void CSequence_Viper_SecondPhase::Update(_float fTimeDelta)
{
    m_fTime += fTimeDelta;
    
    Skip_KeyInput(fTimeDelta);

    if (!m_isPlayerInpusBlock)
    {
        m_pClientInstance->Set_PlayerInput(false);
        m_isPlayerInpusBlock = true;
    }

    if (!m_isSkip)
    {
        if (m_fTime > 0.f && !m_isFadeOut1)
        {
            //m_pClientInstance->BGM_Viper_2PhaseCutScene(10.f);
            m_pClientInstance->Fade_Out();
            m_isFadeOut1 = true;
        }

        if (m_fTime > 1.f && !m_isCameraSet1)
        {
            //m_pViper->Set_PhaseWeapon_Cinematic();
            m_pViper->Get_Viper_FSM()->Change_State(ENUM_CLASS(VIPER_STATE_P1::CUTSCENE_PHASE2), m_pViper);
            m_pCamera->Get_Transform()->Set_State(STATE::POSITION, XMVectorSet(-31.105f, -27.715f, 177.425f, 1.f));
            m_pCamera->Get_Transform()->Look_Dir(XMVectorSet(0.227f, 0.126f, 0.966f, 0.f));
            m_isCameraSet1 = true;
            
            // 카메라 이동 및 애니메이션 시작
        }

        if (m_fTime > 1.1f && !m_isScene1)
        {
            CAS_CutScene_2Phase_Viper* pCutSceneState = m_pViper->Get_Phase2_Viper_CutSceneState();
            m_pClientInstance->Camera_Set_Animation(TEXT("Viper_SecondPhase1"));
            pCutSceneState->ViperScene_Down(m_pViper);

            m_isScene1 = true;
        }

        if (m_fTime > 1.4f && !m_isFadeIn1)
        {
            m_pClientInstance->Fade_In();           
            m_isFadeIn1 = true;
        }     

        if (m_fTime > 7.f && !m_isCameraSet2)
        {
            m_pCamera->Get_Transform()->Set_State(STATE::POSITION, XMVectorSet(-26.356f, -28.898f, 186.125f, 1.f));
            m_pCamera->Get_Transform()->Look_Dir(XMVectorSet(-0.850f, 0.174f, -0.498f, 0.f));
            m_pClientInstance->Camera_Set_Animation(TEXT("Viper_SecondPhase2"));
            m_isCameraSet2 = true;
        }

        if (m_fTime > 11.0f && !m_isFadeOut2)
        {
            m_pClientInstance->Fade_Out(nullptr, 3.f);
            m_isFadeOut2 = true;
        }        

        if (m_fTime > 11.5f && !m_isScene2)
        {
            CAS_CutScene_2Phase_Viper* pCutSceneState = m_pViper->Get_Phase2_Viper_CutSceneState();
            pCutSceneState->ViperScene_PickUp(m_pViper);
            m_pCamera->Get_Transform()->Set_State(STATE::POSITION, XMVectorSet(-30.142f, -28.693f, 182.208f, 1.f));
            m_pCamera->Get_Transform()->Look_Dir(XMVectorSet(0.109, 0.171, 0.979f, 0.f));
            m_pClientInstance->Camera_Set_Animation(TEXT("Viper_SecondPhase3"));
            m_isScene2 = true;
        }

        if (m_fTime > 11.7f && !m_isFadeIn2)
        {
            m_pClientInstance->Fade_In(nullptr, 3.f);
            m_isFadeIn2 = true;
        }

        if (m_fTime > 12.f && !m_isViperHismaLook)
        {
            // 칼 처음 본 시점
            m_isViperHismaLook = true;
        }

        if (m_fTime > 16.f && !m_isCameraSet3)
        {
            m_pCamera->Get_Transform()->Set_State(STATE::POSITION, XMVectorSet(-29.916, -28.389f, 184.220f, 1.f));
            m_pCamera->Get_Transform()->Look_Dir(XMVectorSet(0.350f, -0.750f, -0.561f, 0.f));
            m_pClientInstance->Camera_Set_Animation(TEXT("Viper_SecondPhase4"));
            m_isCameraSet3 = true;
        }

        if (m_fTime > 26.f && !m_isCameraSet4)
        {
            m_pCamera->Get_Transform()->Set_State(STATE::POSITION, XMVectorSet(-29.444, -26.743, 180.271f, 1.f));
            m_pCamera->Get_Transform()->Look_Dir(XMVectorSet(0.051, -0.277, 0.959, 0.f));
            m_pClientInstance->Camera_Set_Animation(TEXT("Viper_SecondPhase5"));
            m_isCameraSet4 = true;
        }

        if (m_fTime > 30.f && !m_isViperPickUp)
        {
            m_pClientInstance->BGM_Viper_2Phase(7.f);

            // 칼 마지막으로 보는 시점
            m_pGameInstance->StopByKey_FadeOut(TEXT("Fire_Burning.mp3"), 8.f);

            m_isViperPickUp = true;
        }

        if (m_fTime > 30.f && !m_isFadeOut3)
        {
            m_pClientInstance->Fade_Out(nullptr, 5.f);
            m_isFadeOut3 = true;
        }

        if (m_fTime > 30.5f && !m_isScene3)
        { 
            CAS_CutScene_2Phase_Viper* pCutSceneState = m_pViper->Get_Phase2_Viper_CutSceneState();
            pCutSceneState->ViperScene_Injure(m_pViper);
            m_pClientInstance->Camera_Force_AniEnd();
            m_pCamera->Get_Transform()->Set_State(STATE::POSITION, XMVectorSet(-27.953f, -27.933f, 186.859f, 1.f));
            m_pCamera->Get_Transform()->Look_Dir(XMVectorSet(-0.929f, -0.047f, -0.368f, 0.f));            
            m_isScene3 = true;
        }

        if (m_fTime > 30.7f && !m_isFadeIn3)
        {
            m_pClientInstance->Fade_In(nullptr, 5.f);
            m_pClientInstance->Camera_Set_Animation(TEXT("Viper_SecondPhase6"));
            m_isFadeIn3 = true;
        }

        if (m_fTime > 37.f && !m_isCameraSet5)
        {
            m_pCamera->Get_Transform()->Set_State(STATE::POSITION, XMVectorSet(-28.907f, -27.527f, 184.820f, 1.f));
            m_pCamera->Get_Transform()->Look_Dir(XMVectorSet(-0.400f, -0.064f, 0.914f, 0.f));
            m_pClientInstance->Camera_Set_Animation(TEXT("Viper_SecondPhase7"));
            m_isCameraSet5 = true;
        }

        if (m_fTime > 40.5f && !m_isFadeOut4)
        {
            m_pClientInstance->Fade_Out(nullptr, 5.f);
            m_isFadeOut4 = true;
        }

        if (m_fTime > 41.f && !m_isScene4)
        {
            CAS_CutScene_2Phase_Viper* pCutSceneState = m_pViper->Get_Phase2_Viper_CutSceneState();
            pCutSceneState->ViperScene_Heptysis(m_pViper);
            m_pCamera->Get_Transform()->Set_State(STATE::POSITION, XMVectorSet(-28.907f, -27.527f, 184.820f, 1.f));
            m_pCamera->Get_Transform()->Look_Dir(XMVectorSet(-0.400f, -0.064f, 0.914f, 0.f));
            m_pClientInstance->Camera_Set_Animation(TEXT("Viper_SecondPhase8"));
            m_isScene4 = true;
        }

        if (m_fTime > 41.3f && !m_isFadeIn4)
        {
            m_pClientInstance->Fade_In(nullptr, 5.f);
            m_isFadeIn4 = true;
        }

        if (m_fTime > 46.9f && !m_isCameraSet6)
        {
            m_pCamera->Get_Transform()->Set_State(STATE::POSITION, XMVectorSet(-29.079f, -27.962f, 182.926f, 1.f));
            m_pCamera->Get_Transform()->Look_Dir(XMVectorSet(-0.206f, -0.022f, 0.987f, 0.f)); 
            m_pClientInstance->Camera_Set_Animation(TEXT("Viper_SecondPhase9"));
            m_isCameraSet6 = true;
        }

        if (m_fTime > 50.f && !m_isCameraSet7)
        {
            m_pCamera->Get_Transform()->Set_State(STATE::POSITION, XMVectorSet(-30.277f, -29.408f, 188.930f, 1.f));
            m_pCamera->Get_Transform()->Look_Dir(XMVectorSet(0.163f, 0.112f, -0.980f, 0.f));
            m_pClientInstance->Camera_Set_Animation(TEXT("Viper_SecondPhase10"));
            m_isCameraSet7 = true;
        }
        
        if (m_fTime > 50.f && !m_isScene5)
        {
            CAS_CutScene_2Phase_Viper* pCutSceneState = m_pViper->Get_Phase2_Viper_CutSceneState();
            pCutSceneState->ViperScene_Heptysis(m_pViper);

            m_isScene5 = true;
        }

        if (m_fTime > 53.f && !m_isCameraSet8)
        {
            m_pCamera->Get_Transform()->Set_State(STATE::POSITION, XMVectorSet(-27.754f, -28.315f, 185.195f, 1.f));
            m_pCamera->Get_Transform()->Look_Dir(XMVectorSet(-0.181f, 0.022f, 0.983f, 0.f));
            m_pClientInstance->Camera_Set_Animation(TEXT("Viper_SecondPhase11"));
            m_isCameraSet8 = true;
        }

        // 1페이즈 모델 끝

        if (m_fTime > 56.f && !m_isFadeOut5)
        {
            m_pClientInstance->Fade_Out(nullptr, 5.f);
            m_isFadeOut5 = true;
        }


        if (m_fTime > 56.5f && !m_isCameraSet9)
        {
            m_pViper->Set_Phase(CViper::PHASE::CINEMATIC);
            m_pViper->Set_PhaseWeapon_Cinematic();
            CAS_CutScene_2Phase_Viper* pCutSceneState = m_pViper->Get_Phase2_Viper_CutSceneState();
            pCutSceneState->ViperScene_Congestion(m_pViper);

            m_pCamera->Get_Transform()->Set_State(STATE::POSITION, XMVectorSet(-27.980f, -28.012f, 181.585f, 1.f));
            m_pCamera->Get_Transform()->Look_Dir(XMVectorSet(-0.087f, -0.004f, 0.996f, 0.f));
            m_pClientInstance->Camera_Set_Animation(TEXT("Viper_SecondPhase12"));
            m_isCameraSet9 = true;
        }

        if (m_fTime > 56.8f && !m_isFadeIn5)
        {
            m_pClientInstance->Fade_In(nullptr, 5.f);
            m_isFadeIn5 = true;
        }


        if (m_fTime > 66.5f && !m_isCameraSet10)
        {
            m_pClientInstance->Camera_Force_AniEnd();
            m_pCamera->Get_Transform()->Set_State(STATE::POSITION, XMVectorSet(-26.422f, -29.383f, 187.378f, 1.f));
            m_pCamera->Get_Transform()->Look_Dir(XMVectorSet(-0.794f, 0.574f, -0.202f, 0.f));
            m_pClientInstance->Camera_Set_Animation(TEXT("Viper_SecondPhase13"));
            m_isCameraSet10 = true;
        }

        if (m_fTime > 76.5f && !m_isFadeOut6)
        {
            m_pClientInstance->Fade_Out(nullptr, 5.f);
            m_isFadeOut6 = true;
        }


        if (m_fTime > 77.f && !m_isCameraSet11)
        {
            m_isCameraSet11 = true;
            CAS_CutScene_2Phase_Viper* pCutSceneState = m_pViper->Get_Phase2_Viper_CutSceneState();
            pCutSceneState->ViperScene_HandDn(m_pViper);
        }

        if (m_fTime > 77.2f && !m_isFadeIn6)
        {
            m_pClientInstance->Fade_In(nullptr, 5.f);
            m_pClientInstance->Camera_Force_AniEnd();
            m_pClientInstance->Camera_Set_Animation(TEXT("Viper_SecondPhase14"));
            m_pCamera->Get_Transform()->Set_State(STATE::POSITION, XMVectorSet(-30.963f, -20.830f, 182.931f, 1.f));
            m_pCamera->Get_Transform()->Look_Dir(XMVectorSet(-0.167f, -0.857f, 0.488f, 0.f));
            m_isFadeIn6 = true;
        }

        // 시네마틱 모델끝

        if (m_fTime > 86.6f && !m_isFadeOut7)
        {
            m_pClientInstance->Fade_Out(nullptr, 5.f);
            m_isFadeOut7 = true;
        }

        if (m_fTime > 87.f && !m_isCameraSet12)
        {
            m_isCameraSet12 = true;
            m_pViper->Set_Phase(CViper::PHASE::PHASE2);
            m_pViper->Set_PhaseWeapon_Phase2();
            m_pCamera->Get_Transform()->Set_State(STATE::POSITION, XMVectorSet(-33.167f, -26.703f, 186.208f, 1.f));
            m_pCamera->Get_Transform()->Look_Dir(XMVectorSet(0.716f, -0.276f, 0.642f, 0.f));
            m_pClientInstance->Camera_Set_Animation(TEXT("Viper_SecondPhase15"));
            /*CAS_CutScene_2Phase_Viper* pCutSceneState = m_pViper->Get_Phase2_Viper_CutSceneState();
            pCutSceneState->ViperScene_PullOut(m_pViper);*/
        }

        if (m_fTime > 87.2f && !m_isFadeIn7)
        {
            m_pClientInstance->Fade_In(nullptr, 5.f);
            m_isFadeIn7 = true;
        }


        if (m_fTime > 90.5f && !m_isCameraSet13)
        {
            m_pCamera->Get_Transform()->Set_State(STATE::POSITION, XMVectorSet(-25.185f, -26.717f, 184.970f, 1.f));
            m_pCamera->Get_Transform()->Look_Dir(XMVectorSet(-0.817f, -0.132f, 0.562f, 0.f));
            m_pClientInstance->Camera_Set_Animation(TEXT("Viper_SecondPhase16"));
            m_isCameraSet13 = true;
        }

        if (m_fTime > 93.f && !m_isCameraSet14)
        {
            m_pCamera->Get_Transform()->Set_State(STATE::POSITION, XMVectorSet(-25.738f, -27.176f, 181.102f, 1.f));
            m_pCamera->Get_Transform()->Look_Dir(XMVectorSet(-0.129f, -0.026f, 0.991f, 0.f));
            m_pClientInstance->Camera_Set_Animation(TEXT("Viper_SecondPhase17"));
            m_isCameraSet14 = true;
        }

        if (m_fTime > 103.f && !m_isFadeOut8)
        {
            m_pClientInstance->Fade_Out(nullptr, 5.f);
            m_isFadeOut8 = true;
        }

        if (m_fTime > 103.5f && !m_isCameraSet15)
        {
            CAS_CutScene_2Phase_Viper* pCutSceneState = m_pViper->Get_Phase2_Viper_CutSceneState();
            pCutSceneState->ViperScene_Walk(m_pViper);
            m_pCamera->Get_Transform()->Set_State(STATE::POSITION, XMVectorSet(-34.227f, -27.305f, 180.030f, 1.f));
            m_pCamera->Get_Transform()->Look_Dir(XMVectorSet(0.811f, -0.126f, 0.571f, 0.f));
            m_pClientInstance->Camera_Set_Animation(TEXT("Viper_SecondPhase18"));
            CCharacterVirtual* pCharVir = dynamic_cast<CCharacterVirtual*>(m_pKhazan->Get_Component(TEXT("Com_CharacterVirtual")));
            pCharVir->Teleport(XMVectorSet(-30.149f, -29.976f, 160.233f, 1.f), m_pKhazan->Get_Transform()->Get_Rotation_Quat(), m_pKhazan->Get_Transform());
            m_pKhazan->Get_Transform()->Look_Dir(XMVectorSet(0.033f, 0.f, 0.999f, 0.f));
            m_isCameraSet15 = true;
        }

        if (m_fTime > 103.8f && !m_isFadeIn8)
        {
            m_pClientInstance->Fade_In(nullptr, 5.f);
            m_isFadeIn8 = true;
        }


        if (m_fTime >= 122.f)
        {
            m_pClientInstance->Camera_Force_AniEnd();

            //dynamic_cast<CAI_Controller_Viper*>(m_pViper->Get_Controller())->Set_ControllerActivate(true);
            dynamic_cast<CAI_Controller_Viper*>(m_pViper->Get_Controller())->Set_CutSceneFinished();
            m_pClientInstance->Set_PlayerInput(true);
            m_isEnd = true;
        }

        // 2페이즈 모델 끝
            
    }
    else {
        m_fSkipTime += fTimeDelta;
        if (m_fSkipTime > 0.f && !m_isSkipFadeOut)
        {
            CSkipButton::SKIPBUTTON_DESC SkipBtnDesc{};
            SkipBtnDesc.isOpen = false;
            m_pClientInstance->UI_UpdateSwitch(TEXT("SkipButton"), &SkipBtnDesc);
            m_pClientInstance->Fade_Out();
            m_isSkipFadeOut = true;
        }

        if (m_fSkipTime > 0.f && !m_isFadeOut8)
        {
            m_pClientInstance->Fade_Out(nullptr, 5.f);
            m_isFadeOut8 = true;
        }

        if (m_fSkipTime > 0.5f && !m_isSkipCameraSet)
        {
            Set_Viper2Phase_ShaderSettings();
            m_pGameInstance->Stop_Effect(ENUM_CLASS(LEVEL::VIPER), TEXT("Viper_blood2_loop"));
            m_pGameInstance->Stop_Effect(ENUM_CLASS(LEVEL::VIPER), TEXT("Viper_fire2"));
            m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Viper_snow2"), dynamic_cast<CTransform*>(m_pViper->Get_Component(TEXT("Com_Transform")))->Get_State(STATE::POSITION));
            m_pGameInstance->Set_LightEnable(TEXT("Viper_Core"), ENUM_CLASS(CClientInstance::GetInstance()->Get_CurrLevel()), false);
            m_pGameInstance->Set_LightEnable(TEXT("Viper_CutScene_PointLight"), ENUM_CLASS(CClientInstance::GetInstance()->Get_CurrLevel()), false);

            m_pViper->Set_Phase(CViper::PHASE::PHASE2);
            m_pViper->Set_PhaseWeapon_Phase2();
            CAS_CutScene_2Phase_Viper* pCutSceneState = m_pViper->Get_Phase2_Viper_CutSceneState();
            pCutSceneState->ViperScene_Walk(m_pViper);
            m_pCamera->Get_Transform()->Set_State(STATE::POSITION, XMVectorSet(-34.227f, -27.305f, 180.030f, 1.f));
            m_pCamera->Get_Transform()->Look_Dir(XMVectorSet(0.811f, -0.126f, 0.571f, 0.f));
            m_pClientInstance->Camera_Set_Animation(TEXT("Viper_SecondPhase18"));
            CCharacterVirtual* pCharVir = dynamic_cast<CCharacterVirtual*>(m_pKhazan->Get_Component(TEXT("Com_CharacterVirtual")));
            pCharVir->Teleport(XMVectorSet(-30.149f, -29.976f, 160.233f, 1.f), m_pKhazan->Get_Transform()->Get_Rotation_Quat(), m_pKhazan->Get_Transform());
            m_pKhazan->Get_Transform()->Look_Dir(XMVectorSet(0.033f, 0.f, 0.999f, 0.f));
            m_isSkipCameraSet = true;
        }

        if (m_fSkipTime > 0.8f && !m_isFadeIn8)
        {
            m_pClientInstance->Fade_In(nullptr, 5.f);
            m_isFadeIn8 = true;
        }


        if (m_fSkipTime >= 19.f)
        {
            m_pClientInstance->Camera_Force_AniEnd();
            dynamic_cast<CAI_Controller_Viper*>(m_pViper->Get_Controller())->Set_CutSceneFinished();
            m_pClientInstance->Set_PlayerInput(true);
            m_isEnd = true;

            m_pClientInstance->BGM_Viper_2Phase(1.f);
        }
    }
}

void CSequence_Viper_SecondPhase::Pause()
{

}

void CSequence_Viper_SecondPhase::Resume()
{

}

void CSequence_Viper_SecondPhase::StopImmediate()
{

}

void CSequence_Viper_SecondPhase::Jump(_float fTime)
{
    m_fTime = fTime;
}

_bool CSequence_Viper_SecondPhase::IsEnd() const
{
    return m_isEnd;
}

void CSequence_Viper_SecondPhase::Skip_KeyInput(_float fTimeDelta)
{
    if (!m_isSkipKey)
    {
        if (m_pGameInstance->Key_Down(DIK_G))
        {
            CSkipButton::SKIPBUTTON_DESC SkipBtnDesc{};
            SkipBtnDesc.isOpen = true;
            SkipBtnDesc.Event = [this]() {
                Set_Skip(true);
                };

            CClientInstance::GetInstance()->UI_UpdateSwitch(TEXT("SkipButton"), &SkipBtnDesc);

            m_isSkipKey = true;
        }
    }


}

void CSequence_Viper_SecondPhase::Set_Viper2Phase_ShaderSettings()
{
    _float fDuration = 0.f;

    // 메인 조명
    LIGHT_TRANSITION_DESC LightDesc{};
    LightDesc.fDuration = fDuration;
    LightDesc.vFadeTime = _float2(fDuration, 0.f);
    LightDesc.vDiffuse = _float4(0.7f, 0.7f, 0.7f, 0.7f);
    LightDesc.vAmbient = _float4(0.4f, 0.4f, 0.4f, 0.4f);
    LightDesc.vSpecular = LightDesc.vDiffuse;
    LightDesc.isReturnToStart = false;
    m_pGameInstance->Start_LightTransition(TEXT("MainLight"), ENUM_CLASS(CClientInstance::GetInstance()->Get_CurrLevel()), LightDesc);

    // ON
    // 플레이어 주변광 점조명 주황색
    LightDesc.fDuration = fDuration;
    LightDesc.vFadeTime = _float2(fDuration, 0.f);
    LightDesc.vDiffuse = _float4(1.f, 0.371f, 0.f, 1.f);
    LightDesc.vAmbient = _float4(0.f, 0.f, 0.f, 0.0f);
    LightDesc.vSpecular = _float4(0.5f, 0.185f, 0.0f, 1.f);
    LightDesc.isReturnToStart = false;
    m_pGameInstance->Start_LightTransition(TEXT("Player_PointLight_Orange"), ENUM_CLASS(CClientInstance::GetInstance()->Get_CurrLevel()), LightDesc);

    // 플레이어 주변광 점조명 흰색
    LightDesc.fDuration = fDuration;
    LightDesc.vFadeTime = _float2(fDuration, 0.f);
    LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
    LightDesc.vAmbient = _float4(0.f, 0.f, 0.f, 0.f);
    LightDesc.vSpecular = LightDesc.vDiffuse;
    LightDesc.isReturnToStart = false;
    m_pGameInstance->Start_LightTransition(TEXT("Player_PointLight_White"), ENUM_CLASS(CClientInstance::GetInstance()->Get_CurrLevel()), LightDesc);

    // 바이퍼 무기 조명
    LightDesc.fDuration = fDuration;
    LightDesc.vFadeTime = _float2(fDuration, 0.f);
    LightDesc.vDiffuse = _float4(2.f, 1.5f, 1.2f, 1.f);
    LightDesc.vAmbient = _float4(0.5f, 0.35f, 0.3f, 1.f);
    LightDesc.vSpecular = LightDesc.vDiffuse;
    LightDesc.isReturnToStart = false;
    m_pGameInstance->Start_LightTransition(TEXT("Viper_TwinBlade_R"), ENUM_CLASS(CClientInstance::GetInstance()->Get_CurrLevel()), LightDesc);

    FOG_CONFIG FogConfig = m_pGameInstance->Get_FogConfig();
    FogConfig.isUseSubColor = false;
    FogConfig.isUseHeight = false;
    m_pGameInstance->Set_FogConfig(FogConfig);

    // 포그 세팅 (어두운 보라색)
    FOG_TRANSITION_DESC FogDesc{};
    FogDesc.fDensity = 0.05f;
    FogDesc.fBias = 0.95f;
    FogDesc.vColor = _float4(0.1f, 0.053f, 0.086f, 1.f);
    FogDesc.isUseHeight = false;
    FogDesc.isUseNoise = false;
    m_pGameInstance->Start_FogTransition(fDuration, FogDesc);

    // 스카이 박스 세팅
    SKY_DESC SkyDesc{};
    SkyDesc.vNebulaColorR = _float3(0.147f, 0.076f, 0.125f);
    SkyDesc.vNebulaColorG = _float3(0.147f, 0.076f, 0.125f);
    SkyDesc.vNebulaColorB = _float3(0.f, 0.f, 0.f);
    SkyDesc.fStarStrength = 0.2f;
    SkyDesc.fMoonSize = 0.8f;
    SkyDesc.vMoonDirection = _float3(-0.21f, 0.19f, 1.f);
    SkyDesc.vMoonColor = _float3(1.f, 0.5f, 0.5f);
    SkyDesc.fMoonIntensity = 0.4f;
    static_cast<CSkySphere*>(m_pGameInstance->Find_GameObject(ENUM_CLASS(LEVEL::VIPER), TEXT("Layer_Sky"), 0))->Start_LerpSky(SkyDesc, fDuration);

    // 클라우드 세팅
    CLOUD_DESC CloudDesc{};
    CloudDesc.vCloudColor = _float3(1.f, 1.f, 1.f);
    CloudDesc.fCloudSpeed = 0.25f;
    CloudDesc.fCloudScale = 1.f;
    CloudDesc.fCloudDensity = 1.f;
    CloudDesc.fCloudLightIntensity = 0.2f;
    CloudDesc.vLightDir = _float3(0.f, 1.f, 0.f);
    CloudDesc.fDynamic = 1.f;
    static_cast<CCloudSphere*>(m_pGameInstance->Find_GameObject(ENUM_CLASS(LEVEL::VIPER), TEXT("Layer_Sky"), 1))->Start_LerpCloud(CloudDesc, fDuration);

    LightDesc.fDuration = fDuration;
    LightDesc.vFadeTime = _float2(fDuration, 0.f);
    LightDesc.vDiffuse = _float4(0.f, 0.f, 0.f, 0.f);
    LightDesc.vAmbient = _float4(0.f, 0.f, 0.f, 0.f);
    LightDesc.vSpecular = LightDesc.vDiffuse;
    LightDesc.isReturnToStart = false;
    m_pGameInstance->Start_LightTransition(TEXT("Player_PointLight_Gray"), ENUM_CLASS(CClientInstance::GetInstance()->Get_CurrLevel()), LightDesc);

    LightDesc.fDuration = fDuration;
    LightDesc.vFadeTime = _float2(fDuration, 0.f);
    LightDesc.vDiffuse = _float4(0.f, 0.f, 0.f, 0.f);
    LightDesc.vAmbient = _float4(0.f, 0.f, 0.f, 0.f);
    LightDesc.vSpecular = LightDesc.vDiffuse;
    LightDesc.isReturnToStart = false;
    m_pGameInstance->Start_LightTransition(TEXT("Viper_Core"), ENUM_CLASS(CClientInstance::GetInstance()->Get_CurrLevel()), LightDesc);
}

CSequence_Viper_SecondPhase* CSequence_Viper_SecondPhase::Create(CViper* pViper, CKhazan_GSword* pKhazan)
{
    return new CSequence_Viper_SecondPhase(pViper, pKhazan);
}

void CSequence_Viper_SecondPhase::Free()
{
    __super::Free();

    Safe_Release(m_pGameInstance);

}
