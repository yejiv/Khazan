#include "Minigame_Mirrorball.h"
#include "GameInstance.h"
#include "ClientInstance.h"

CMinigame_Mirrorball::CMinigame_Mirrorball()
    : m_pGameInstance{ CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

void CMinigame_Mirrorball::Start_Mirrorball()
{
    m_pGameInstance->Set_LightEnable(TEXT("MainLight"), ENUM_CLASS(LEVEL::EMBARS), false);
    m_isStart = true;

    m_pGameInstance->Backup_LightDesc(TEXT("Mirroball"), ENUM_CLASS(CClientInstance::GetInstance()->Get_CurrLevel()));
    m_pGameInstance->Set_LightEnable(TEXT("Mirroball"), ENUM_CLASS(LEVEL::EMBARS), true);
    m_pGameInstance->Set_LightEnable(TEXT("GachaSelect1"), ENUM_CLASS(LEVEL::EMBARS), true);
    m_pGameInstance->Set_LightEnable(TEXT("GachaSelect2"), ENUM_CLASS(LEVEL::EMBARS), true);
    m_pGameInstance->Set_LightEnable(TEXT("GachaSelect3"), ENUM_CLASS(LEVEL::EMBARS), true);

    Light_Green();
    m_eState = COLORSTATE::GREEN;
}

void CMinigame_Mirrorball::End_Mirrorball()
{
    m_pGameInstance->Set_LightEnable(TEXT("GachaSelect1"), ENUM_CLASS(LEVEL::EMBARS), false);
    m_pGameInstance->Set_LightEnable(TEXT("GachaSelect2"), ENUM_CLASS(LEVEL::EMBARS), false);
    m_pGameInstance->Set_LightEnable(TEXT("GachaSelect3"), ENUM_CLASS(LEVEL::EMBARS), false);

    m_pGameInstance->Set_LightEnable(TEXT("Mirroball"), ENUM_CLASS(LEVEL::EMBARS), false);
    m_pGameInstance->Set_LightEnable(TEXT("MainLight"), ENUM_CLASS(LEVEL::EMBARS), true);
    m_isStart = false;
}

void CMinigame_Mirrorball::Update(_float fTimeDelta)
{
    if (!m_isStart)
        return;
    //일단 콜백 함수 중 색 변경이 불가해서 임시 조치로 상시로 다음 보간 계속 호출

    if (m_eState == COLORSTATE::RED)
        Light_Red();
    else if (m_eState == COLORSTATE::BLUE)
        Light_Blue();
    else if (m_eState == COLORSTATE::GREEN)
        Light_Green();
}

void CMinigame_Mirrorball::Light_Red()
{
    LIGHT_TRANSITION_DESC LightDesc{};
    LightDesc.fDuration = 1.f;
    LightDesc.vFadeTime = _float2(1.f, 1.f);
    LightDesc.vDiffuse = _float4(2.5f, 0.8f, 0.8f, 0.1f);
    LightDesc.vAmbient = _float4(1.1f, 0.8f, 0.8f, 0.1f);
    LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 0.1f);
    LightDesc.isReturnToStart = false;
    LightDesc.Callback = [this]() { m_eState = COLORSTATE::GREEN; };
    m_pGameInstance->Start_LightTransition(TEXT("Mirroball"), ENUM_CLASS(CClientInstance::GetInstance()->Get_CurrLevel()), LightDesc);
}

void CMinigame_Mirrorball::Light_Blue()
{
    LIGHT_TRANSITION_DESC LightDesc{};
    LightDesc.fDuration = 1.f;
    LightDesc.vFadeTime = _float2(1.f, 1.f);
    LightDesc.vDiffuse = _float4(0.8f, 0.8f, 2.5f, 0.1f);
    LightDesc.vAmbient = _float4(1.1f, 0.8f, 0.8f, 0.1f);
    LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 0.1f);
    LightDesc.isReturnToStart = false;
    LightDesc.Callback = [this]() { m_eState = COLORSTATE::RED; };
    m_pGameInstance->Start_LightTransition(TEXT("Mirroball"), ENUM_CLASS(CClientInstance::GetInstance()->Get_CurrLevel()), LightDesc);
}

void CMinigame_Mirrorball::Light_Green()
{
    LIGHT_TRANSITION_DESC LightDesc{};
    LightDesc.fDuration = 1.f;
    LightDesc.vFadeTime = _float2(1.f, 1.f);
    LightDesc.vDiffuse = _float4(0.8f, 2.5f, 0.8f, 0.1f);
    LightDesc.vAmbient = _float4(1.1f, 0.8f, 0.8f, 0.1f);
    LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 0.1f);
    LightDesc.isReturnToStart = false;
    LightDesc.Callback = [this]() { m_eState = COLORSTATE::BLUE; };
    m_pGameInstance->Start_LightTransition(TEXT("Mirroball"), ENUM_CLASS(CClientInstance::GetInstance()->Get_CurrLevel()), LightDesc);
}

CMinigame_Mirrorball* CMinigame_Mirrorball::Create()
{
    return new CMinigame_Mirrorball();;
}

void CMinigame_Mirrorball::Free()
{
    Safe_Release(m_pGameInstance);
}
