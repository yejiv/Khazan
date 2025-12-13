#include "AS_P2_Roar_Viper.h"
#include "Viper.h"
#include "Body_Phase2_Viper.h"
#include "BlackBoard.h"
#include "AI_Controller.h"
#include "GameInstance.h"
#include "FSM_Viper.h"
#include "ClientInstance.h"
#include "SkySphere.h"
#include "CloudSphere.h"

CAS_P2_Roar_Viper::CAS_P2_Roar_Viper()
{

}

void CAS_P2_Roar_Viper::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{

    CViper* pViper = static_cast<CViper*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pViper->Get_P2Body()->Get_Component(TEXT("Com_Model")));
    pModel->Set_Animation(43);
    m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_viper_p2_swing_combo_stand_roar_01 (SFX).wav"), pViper->Get_Position(), pViper->Get_SoundChannel(ENUM_CLASS(MONSFX::ATVO)), 30.f);
    
    // 로어 애니메이션 시작
    Set_ViperBerserkerEnd_ShaderSettings();
}

void CAS_P2_Roar_Viper::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    CViper* pViper = static_cast<CViper*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pViper->Get_P2Body()->Get_Component(TEXT("Com_Model")));


    if (pModel->Play_Animation(fTimeDelta))
    {
        CBlackBoard* pBB = pViper->Get_Controller()->Get_BlackBoard();
        pFSM->Change_State(ENUM_CLASS(VIPER_STATE_P1::IDLE), pViper);
        pBB->Set_Value<_bool>(pViper->Get_Name(), "is_P2_RoarFinished", true);
        pBB->Set_Value<_uint>(pViper->Get_Name(), "BerserkerIndex",0);
        pBB->Set_Value<_bool>(pViper->Get_Name(), "is_P2Loar", false);

        // 애니메이션 종료

    }
}

void CAS_P2_Roar_Viper::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
    // 스테이트 바뀔떄 
}

void CAS_P2_Roar_Viper::Set_ViperBerserkerEnd_ShaderSettings()
{
    // 림라이트 켜기
    m_pGameInstance->Set_EnableRimLight(true);

    _float fDuration = 3.f;

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
    FogDesc.fBias = 0.8f;
    FogDesc.vColor = _float4(0.f, 0.012f, 0.039f, 1.f);
    FogDesc.isUseHeight = false;
    FogDesc.isUseNoise = false;
    m_pGameInstance->Start_FogTransition(fDuration, FogDesc);

    // 스카이 박스 세팅
    SKY_DESC SkyDesc{};
    SkyDesc.vNebulaColorR = _float3(0.f, 0.035f, 0.082f);
    SkyDesc.vNebulaColorG = _float3(0.f, 0.035f, 0.082f);
    SkyDesc.vNebulaColorB = _float3(0.f, 0.f, 0.f);
    SkyDesc.fStarStrength = 2.f;
    SkyDesc.fMoonSize = 0.8f;
    SkyDesc.vMoonDirection = _float3(-0.21f, 0.19f, 1.f);
    SkyDesc.vMoonColor = _float3(0.822f, 0.822f, 0.822f);
    SkyDesc.fMoonIntensity = 0.3f;
    static_cast<CSkySphere*>(m_pGameInstance->Find_GameObject(ENUM_CLASS(LEVEL::VIPER), TEXT("Layer_Sky"), 0))->Start_LerpSky(SkyDesc, fDuration);

    // 클라우드 세팅
    CLOUD_DESC CloudDesc{};
    CloudDesc.vCloudColor = _float3(1.f, 1.f, 1.f);
    CloudDesc.fCloudSpeed = 0.25f;
    CloudDesc.fCloudScale = 1.f;
    CloudDesc.fCloudDensity = 2.f;
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
}

CAS_P2_Roar_Viper* CAS_P2_Roar_Viper::Create()
{
    return new CAS_P2_Roar_Viper();
}

void CAS_P2_Roar_Viper::Free()
{
    __super::Free();
}
