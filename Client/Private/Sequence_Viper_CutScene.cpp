#include "Sequence_Viper_CutScene.h"
#include "GameInstance.h"
#include "ClientInstance.h"
#include "Transform.h"
#include "GameObject.h"
#include "Viper.h"
#include "Camera_Compre.h"
#include "AS_CutScene_Start_Viper.h"
#include "FSM_Viper.h"
#include "CharacterVirtual.h"
#include "Khazan_GSword.h"
#include "AI_Controller_Viper.h"
#include "SkipButton.h""
#include "UI_HUD.h"

CSequence_Viper_CutScene::CSequence_Viper_CutScene(CViper* pViper, CKhazan_GSword* pKhazan)
    : m_pGameInstance{ CGameInstance::GetInstance() }
    , m_pClientInstance { CClientInstance::GetInstance() }
    , m_pViper { pViper } 
    , m_pKhazan { pKhazan }
{
    Safe_AddRef(m_pGameInstance);
    Safe_AddRef(m_pClientInstance);
}

HRESULT CSequence_Viper_CutScene::Initialize(const SEQ_REQ_PLAY_DESC& tDesc)
{
    m_pCamera = dynamic_cast<CCamera_Compre*>(m_pClientInstance->Get_ActiveCamera());    
    return S_OK;
}

void CSequence_Viper_CutScene::Update(_float fTimeDelta)
{
    m_fTime += fTimeDelta;

    Skip_KeyInput(fTimeDelta);

    if (!m_isSkip)
    {

        // 1차 페이드 아웃
        if (m_fTime > 0.f && !m_isFadeOut)
        {
            m_pClientInstance->Fade_Out();
            static_cast<CUI_HUD*>(m_pClientInstance->Get_RootUI(TEXT("HUD")))->Switch_Panel(false);
            m_pClientInstance->Set_PlayerInput(false);
            m_pKhazan->Set_Idle();
            m_isFadeOut = true;
        }

        // 카메라 애니메이션 재생
        if (m_fTime > 1.f && !m_isCameraSet)
        {
            CCharacterVirtual* pCharVir = dynamic_cast<CCharacterVirtual*>(m_pKhazan->Get_Component(TEXT("Com_CharacterVirtual")));
            pCharVir->Teleport(XMVectorSet(-22.907f, -29.976f, 153.761f, 1.f), m_pKhazan->Get_Transform()->Get_Rotation_Quat(), m_pKhazan->Get_Transform());
            m_pKhazan->Get_Transform()->Look_Dir(XMVectorSet(-0.225f, 0.f, 0.974f, 0.f));
            m_pClientInstance->Camera_Set_Animation(TEXT("Viper_1Phase_CutScene"));
            m_isCameraSet = true;
        }

        // 1차 페이드 인
        if (m_fTime > 1.5f && !m_isFadeIn)
        {
            m_pClientInstance->BGM_Viper_1PhaseCutScene();

            m_pClientInstance->Fade_In(nullptr, 3.f);
            m_isFadeIn = true;
        }

        // 바이퍼 Sit 애니메이션 진행
        if (m_fTime > 2.5f && !m_isFirstScene)
        {
            m_pViper->Get_Viper_FSM()->Change_State(ENUM_CLASS(VIPER_STATE_P1::CUTSCENE_START), m_pViper);
            CAS_CutScene_Start_Viper* pCutSceneState = m_pViper->Get_Viper_CutSceneState();
            //pCutSceneState->ViperScene_Sit(m_pViper);
            pCutSceneState->Start_CutSceneAnimation();            
            m_isFirstScene = true;
        }

        // 바이퍼 Sit 애니메이션 점프 뒤로 갈시 다음 애니메이션으로 재생
        if (m_fTime > 30.0f && !m_isFirstSceneEnd)
        {
            CAS_CutScene_Start_Viper* pCutSceneState = m_pViper->Get_Viper_CutSceneState();
            pCutSceneState->ViperScene_Jump(m_pViper);
            m_isFirstSceneEnd = true;
        }

        // 카메라 떨어지는곳이 보이는곳으로 텔포
        if (m_fTime > 31.3f && !m_isSecondCameraSet)
        {
            CTransform* pCamera_Transform = m_pCamera->Get_Transform();
            pCamera_Transform->Look_Dir(XMVectorSet(-0.143f, 0.505f, 0.851f, 0.f));
            pCamera_Transform->Set_State(STATE::POSITION, XMVectorSet(-29.861f, -29.177f, 187.630f, 1.f));
            m_pClientInstance->Camera_Set_Animation(TEXT("Viper_1Phase_CutScene2"));
            m_isSecondCameraSet = true;
        }

        if (m_fTime > 32.f && !m_isLandEffect)
        {
            m_pClientInstance->ActiveCamera_Shaking(3.f, 1.f);
            m_isLandEffect = true;
        }

        // 점프 후 땅도착시 착지 애니메이션으로 전환
        if (m_fTime > 32.35f && !m_isSecondSceneEnd)
        {
            CAS_CutScene_Start_Viper* pCutSceneState = m_pViper->Get_Viper_CutSceneState();
            pCutSceneState->ViperScene_Land(m_pViper);
            m_isSecondSceneEnd = true;
        }

        // 착지하여 살짝 보고 페이드아웃
        if (m_fTime > 40.f && !m_isSecondFadeOut)
        {
            m_pClientInstance->BGM_Viper_1Phase(5.f);

            m_pClientInstance->Fade_Out(nullptr, 5.f);
            m_isSecondFadeOut = true;
        }

        // 으르렁 애니메이션재생 및 카메라 이동 필요
        if (m_fTime > 40.5f && !m_isThirdScene)
        {
            CAS_CutScene_Start_Viper* pCutSceneState = m_pViper->Get_Viper_CutSceneState();
            pCutSceneState->ViperScene_Roar(m_pViper);
            m_isThirdScene = true;
        }

        // 페이드 인
        if (m_fTime > 41.f && !m_isSecondFadeIn)
        {
            m_pClientInstance->Fade_In(nullptr, 5.f);
            m_isSecondFadeIn = true;
        }

        if (m_fTime > 41.5f && !m_isRoarEffect)
        {
            // Distortion
            DISTORTION_DESC Desc{};
            _vector vCenterPos = XMVectorSet(-31.938f, -25.6f, 201.162f, 1.f);
            _float fPosY = XMVectorGetY(vCenterPos);
            _float fOffset = 2.f;
            vCenterPos = XMVectorSetY(vCenterPos, fPosY + fOffset);
            XMStoreFloat3(&Desc.vCenter, vCenterPos);
            Desc.fRange = 0.6f;
            Desc.fPower = 0.05f;
            Desc.fDuration = 2.f;
            Desc.vFadeTime = _float2(1.f, 0.2f);
            Desc.fSpeed = 1.f;
            Desc.iNoiseIndex = 17;
            m_pGameInstance->Start_Distortion(Desc);

            RADIAL_BLUR_DESC RadialDesc{};
            RadialDesc.vCenterUV = _float2(0.5f, 0.5f);
            RadialDesc.fSampleRadius = 0.05f;
            RadialDesc.vMaskRadius = _float2(0.f, 0.3f);
            RadialDesc.fExponent = 1.f;
            RadialDesc.iNumSamples = 16;
            RadialDesc.fAttenuation = 0.1f;
            RadialDesc.fStrength = 1.f;
            RadialDesc.fDuration = 2.5f;
            RadialDesc.vFadeTime = _float2(0.7f, 0.5f);
            m_pGameInstance->Start_RadialBlur(RadialDesc);

            m_isRoarEffect = true;
        }


        if (m_fTime >= 45.f)
        {
            Start_FogTransition();
            dynamic_cast<CAI_Controller_Viper*>(m_pViper->Get_Controller())->Set_ControllerActivate(true);  
            m_pClientInstance->Set_PlayerInput(true);
            m_isEnd = true;
        }
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

        if (m_fSkipTime > 1.f && !m_isSkipCameraSet)
        {
            m_pClientInstance->Camera_Force_AniEnd();            
            m_isSkipCameraSet = true;
        }

        if (m_fSkipTime > 2.f && !m_isSkipFadeIn)
        {
            m_pClientInstance->Fade_In();
            m_isSkipFadeIn = true;

            m_pClientInstance->BGM_Viper_1Phase(2.f);
        }

        if (m_fSkipTime > 3.f && !m_isEnd)
        {
            Start_FogTransition();
            CCharacterVirtual* pCharVir = dynamic_cast<CCharacterVirtual*>(m_pViper->Get_Component(TEXT("Com_CharacterVirtual")));
            pCharVir->Teleport(XMVectorSet(-31.938f, -29.986f, 198.162f, 1.f), m_pViper->Get_Transform()->Get_Rotation_Quat(), m_pViper->Get_Transform());
            dynamic_cast<CAI_Controller_Viper*>(m_pViper->Get_Controller())->Set_ControllerActivate(true);
            m_pClientInstance->Set_PlayerInput(true);
            m_isEnd = true;
        }
    }
        
}

void CSequence_Viper_CutScene::Pause()
{

}

void CSequence_Viper_CutScene::Resume()
{

}

void CSequence_Viper_CutScene::StopImmediate()
{

}

void CSequence_Viper_CutScene::Jump(_float fTime)
{
    m_fTime = fTime;
}

_bool CSequence_Viper_CutScene::IsEnd() const
{
    return m_isEnd;
}

void CSequence_Viper_CutScene::Skip_KeyInput(_float fTimeDelta)
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

void CSequence_Viper_CutScene::Start_FogTransition()
{
    FOG_TRANSITION_DESC Desc{};
    Desc.fDensity = 0.03f;
    Desc.fBias = 0.95f;
    Desc.vColor = _float4(0.055f, 0.110f, 0.157f, 1.f);
    Desc.isUseHeight = true;
    Desc.fBaseHeight = -145.f;
    Desc.isUseNoise = false;
    m_pGameInstance->Start_FogTransition(3.f, Desc);
}

CSequence_Viper_CutScene* CSequence_Viper_CutScene::Create(CViper* pViper, CKhazan_GSword* pKhazan)
{
    return new CSequence_Viper_CutScene(pViper, pKhazan);
}

void CSequence_Viper_CutScene::Free()
{
    __super::Free();

    Safe_Release(m_pGameInstance);
    Safe_Release(m_pClientInstance);
}
