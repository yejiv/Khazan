#include "Sequence_Viper_CutScene.h"
#include "GameInstance.h"
#include "ClientInstance.h"
#include "Transform.h"
#include "GameObject.h"
#include "Viper.h"
#include "Camera_Compre.h"
#include "AS_CutScene_Start_Viper.h"

CSequence_Viper_CutScene::CSequence_Viper_CutScene(CViper* pViper)
    : m_pGameInstance{ CGameInstance::GetInstance() }
    , m_pClientInstance { CClientInstance::GetInstance() }
    , m_pViper { pViper } 
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


    // 1차 페이드 아웃
    if (m_fTime > 0.f && !m_isFadeOut)
    {
        m_pClientInstance->Fade_Out();
        m_isFadeOut = true;
    }

    // 카메라 애니메이션 재생
    if (m_fTime > 1.f && !m_isCameraSet)
    {
        // 카메라 애니메이션 재생 시작
    }

    // 1차 페이드 인
    if (m_fTime > 1.5f && !m_isFadeIn)
    {
        m_pClientInstance->Fade_In(nullptr, 3.f);
        m_isFadeIn = true;
    }

    // 바이퍼 Sit 애니메이션 진행
    if (m_fTime > 2.5f && !m_isFirstScene)
    {
        CAS_CutScene_Start_Viper* pCutSceneState = m_pViper->Get_Viper_CutSceneState();
        pCutSceneState->ViperScene_Sit(m_pViper);
        m_isFirstScene = true;
    }

    // 바이퍼 Sit 애니메이션 점프 뒤로 갈시 다음 애니메이션으로 재생
    if (m_fTime > 10.f && !m_isFirstSceneEnd)
    {
        CAS_CutScene_Start_Viper* pCutSceneState = m_pViper->Get_Viper_CutSceneState();
        pCutSceneState->ViperScene_Jump(m_pViper);
        m_isFirstSceneEnd = true;
    }

    // 카메라 떨어지는곳이 보이는곳으로 텔포
    if (m_fTime > 11.f && !m_isSecondCameraSet)
    {
        m_isSecondCameraSet = true;
    }

    // 점프 후 땅도착시 착지 애니메이션으로 전환
    if (m_fTime > 12.5 && !m_isSecondSceneEnd)
    {
        CAS_CutScene_Start_Viper* pCutSceneState = m_pViper->Get_Viper_CutSceneState();
        pCutSceneState->ViperScene_Land(m_pViper);
        m_isSecondSceneEnd = true;
    }

    // 착지하여 살짝 보고 페이드아웃
    if (m_fTime > 17.f && !m_isSecondFadeOut)
    {
        m_pClientInstance->Fade_Out(nullptr, 5.f);
    }

    // 으르렁 애니메이션재생 및 카메라 이동 필요
    if (m_fTime > 18.5f && !m_isThirdScene)
    {
        CAS_CutScene_Start_Viper* pCutSceneState = m_pViper->Get_Viper_CutSceneState();
        pCutSceneState->ViperScene_Roar(m_pViper);
        m_isThirdScene = true;
    }

    // 페이드 인
    if (m_fTime > 19.2f && !m_isSecondFadeIn)
    {
        m_pClientInstance->Fade_In(nullptr, 5.f);
        m_isSecondFadeIn = true;
    }


    if (m_fTime >= 100.f)
        m_isEnd = true;
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

CSequence_Viper_CutScene* CSequence_Viper_CutScene::Create(CViper* pViper)
{
    return new CSequence_Viper_CutScene(pViper);
}

void CSequence_Viper_CutScene::Free()
{
    __super::Free();

    Safe_Release(m_pGameInstance);
    Safe_Release(m_pClientInstance);
}
