#include "AS_P2_SwingRound_Viper.h"
#include "Viper.h"
#include "Body_Phase2_Viper.h"
#include "BlackBoard.h"
#include "AI_Controller.h"
#include "GameInstance.h"
#include "FSM_Viper.h"
#include "ClientInstance.h"
#include "SkySphere.h"
#include "CloudSphere.h"

CAS_P2_SwingRound_Viper::CAS_P2_SwingRound_Viper()
{

}

void CAS_P2_SwingRound_Viper::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{

    CViper* pViper = static_cast<CViper*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pViper->Get_P2Body()->Get_Component(TEXT("Com_Model")));
    CBlackBoard* pBB = pViper->Get_Controller()->Get_BlackBoard();
    pModel->Set_Animation(49);
    m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_viper_p2_swing_round_start_01 (SFX).wav"), pViper->Get_Position(), pViper->Get_SoundChannel(ENUM_CLASS(MONSFX::ATVO)), 30.f);
    pBB->Set_Value<_uint>(pViper->Get_Name(), "BerserkerIndex", 0);

}

void CAS_P2_SwingRound_Viper::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    CViper* pViper = static_cast<CViper*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pViper->Get_P2Body()->Get_Component(TEXT("Com_Model")));


    if (pModel->Play_Animation(fTimeDelta))
    {
        CBlackBoard* pBB = pViper->Get_Controller()->Get_BlackBoard();
        pFSM->Change_State(ENUM_CLASS(VIPER_STATE_P1::IDLE), pViper);
        pBB->Set_Value<_bool>(pViper->Get_Name(), "is_P2_SwingRoundFinished", true);
        pBB->Set_Value<_bool>(pViper->Get_Name(), "is_Berserker", true);

        // 버서커 시작 셰이더 세팅, 카메라 쉐이킹
        //  CClientInstance::GetInstance()->ActiveCamera_Shaking(2.f, 1.f);
        //  Set_ViperBerserkerStart_ShaderSettings();
    }
}

void CAS_P2_SwingRound_Viper::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
   
}

void CAS_P2_SwingRound_Viper::OnCollision(COLLISION_DESC* pDesc, _uint iCollisionLayer, CGameObject* pOwner)
{
    COLLISION_LAYER eLayer = static_cast<COLLISION_LAYER>(iCollisionLayer);

    if (COLLISION_LAYER::PLAYER == eLayer)
    {
        CViper* pViper = static_cast<CViper*>(pOwner);
        CBlackBoard* pBB = pViper->Get_Controller()->Get_BlackBoard();
        CCreature* pTarget = static_cast<CCreature*>(pDesc->pGameObject);
        CTransform* pOwnerTransform = static_cast<CTransform*>(pOwner->Get_Component(TEXT("Com_Transform")));
        if (nullptr == pOwnerTransform)
            return;

        pTarget->Take_Damage(10.f, HITREACTION::KNOCKBACK_NORMAL);
        _vector vLook = pOwnerTransform->Get_State(STATE::LOOK);
        pTarget->KnockBack(vLook, 15.f, 60.f);

    }
}

void CAS_P2_SwingRound_Viper::Set_ViperBerserkerStart_ShaderSettings()
{
    // 림라이트 끄기
    m_pGameInstance->Set_EnableRimLight(false);

    // 광전사 모드 셰이더 세팅
    _float fDuration = 3.f;

    // 메인 조명 끄기
    LIGHT_TRANSITION_DESC LightDesc{};
    LightDesc.fDuration = fDuration;
    LightDesc.vFadeTime = _float2(fDuration, 0.f);
    LightDesc.vDiffuse = _float4(0.f, 0.f, 0.f, 0.f);
    LightDesc.vAmbient = _float4(0.f, 0.f, 0.f, 0.f);
    LightDesc.vSpecular = LightDesc.vDiffuse;
    LightDesc.isReturnToStart = false;
    m_pGameInstance->Start_LightTransition(TEXT("MainLight"), ENUM_CLASS(CClientInstance::GetInstance()->Get_CurrLevel()), LightDesc);

    // 점 조명 : 그레이 조명 켜지기, 오렌지, 화이트, 무기 조명은 꺼지기
    LightDesc.fDuration = fDuration;
    LightDesc.vFadeTime = _float2(fDuration, 0.f);
    LightDesc.vDiffuse = _float4(0.409f, 0.381f, 0.295f, 1.f);
    LightDesc.vAmbient = _float4(0.7f, 0.7f, 0.7f, 0.7f);
    LightDesc.vSpecular = LightDesc.vDiffuse;
    LightDesc.isReturnToStart = false;
    m_pGameInstance->Start_LightTransition(TEXT("Player_PointLight_Gray"), ENUM_CLASS(CClientInstance::GetInstance()->Get_CurrLevel()), LightDesc);

    LightDesc.fDuration = fDuration;
    LightDesc.vFadeTime = _float2(fDuration, 0.f);
    LightDesc.vDiffuse = _float4(0.f, 0.f, 0.f, 0.f);
    LightDesc.vAmbient = _float4(0.f, 0.f, 0.f, 0.f);
    LightDesc.vSpecular = LightDesc.vDiffuse;
    LightDesc.isReturnToStart = false;
    m_pGameInstance->Start_LightTransition(TEXT("Player_PointLight_White"), ENUM_CLASS(CClientInstance::GetInstance()->Get_CurrLevel()), LightDesc);

    LightDesc.fDuration = fDuration;
    LightDesc.vFadeTime = _float2(fDuration, 0.f);
    LightDesc.vDiffuse = _float4(0.f, 0.f, 0.f, 0.f);
    LightDesc.vAmbient = _float4(0.f, 0.f, 0.f, 0.f);
    LightDesc.vSpecular = LightDesc.vDiffuse;
    LightDesc.isReturnToStart = false;
    m_pGameInstance->Start_LightTransition(TEXT("Player_PointLight_Orange"), ENUM_CLASS(CClientInstance::GetInstance()->Get_CurrLevel()), LightDesc);

    LightDesc.fDuration = fDuration;
    LightDesc.vFadeTime = _float2(fDuration, 0.f);
    LightDesc.vDiffuse = _float4(0.f, 0.f, 0.f, 0.f);
    LightDesc.vAmbient = _float4(0.f, 0.f, 0.f, 0.f);
    LightDesc.vSpecular = LightDesc.vDiffuse;
    LightDesc.isReturnToStart = false;
    m_pGameInstance->Start_LightTransition(TEXT("Viper_TwinBlade_R"), ENUM_CLASS(CClientInstance::GetInstance()->Get_CurrLevel()), LightDesc);

    // 포그 검정
    FOG_TRANSITION_DESC FogDesc{};
    FogDesc.fDensity = 0.05f;
    FogDesc.fBias = 0.95f;
    FogDesc.vColor = _float4(0.f, 0.f, 0.f, 0.f);
    FogDesc.isUseHeight = false;
    FogDesc.isUseNoise = false;
    FogDesc.Callback = [&]() { m_pGameInstance->Set_EnableFog(false); };
    m_pGameInstance->Start_FogTransition(fDuration, FogDesc);

    // 스카이 검정
    SKY_DESC SkyDesc{};
    SkyDesc.vNebulaColorR = _float3(0.f, 0.f, 0.f);
    SkyDesc.vNebulaColorG = _float3(0.f, 0.f, 0.f);
    SkyDesc.vNebulaColorB = _float3(0.f, 0.f, 0.f);
    SkyDesc.fStarStrength = 0.f;
    SkyDesc.fMoonSize = 0.8f;
    SkyDesc.vMoonDirection = _float3(-0.21f, 0.19f, 1.f);
    SkyDesc.vMoonColor = _float3(0.f, 0.f, 0.f);
    SkyDesc.fMoonIntensity = 0.f;
    static_cast<CSkySphere*>(m_pGameInstance->Find_GameObject(ENUM_CLASS(LEVEL::VIPER), TEXT("Layer_Sky"), 0))->Start_LerpSky(SkyDesc, fDuration);

    // 클라우드 세팅
    CLOUD_DESC CloudDesc{};
    CloudDesc.vCloudColor = _float3(0.f, 0.f, 0.f);
    CloudDesc.fCloudSpeed = 0.f;
    CloudDesc.fCloudScale = 0.f;
    CloudDesc.fCloudDensity = 0.f;
    CloudDesc.fCloudLightIntensity = 0.f;
    CloudDesc.vLightDir = _float3(0.f, 0.f, 0.f);
    CloudDesc.fDynamic = 0.f;
    static_cast<CCloudSphere*>(m_pGameInstance->Find_GameObject(ENUM_CLASS(LEVEL::VIPER), TEXT("Layer_Sky"), 1))->Start_LerpCloud(CloudDesc, fDuration);
}

CAS_P2_SwingRound_Viper* CAS_P2_SwingRound_Viper::Create()
{
    return new CAS_P2_SwingRound_Viper();
}

void CAS_P2_SwingRound_Viper::Free()
{
    __super::Free();
}
