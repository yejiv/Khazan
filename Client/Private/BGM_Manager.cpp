#include "BGM_Manager.h"

#include "GameInstance.h"

CBGM_Manager::CBGM_Manager()
    : m_pGameInstance { CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CBGM_Manager::Initialize()
{
    return S_OK;
}

void CBGM_Manager::Update(_float fTimeDelta)
{
}

void CBGM_Manager::Set_Volume_BGM(_float fVolume)
{
    m_fVolume_BGM = fVolume;

    m_pGameInstance->SetVolumeByKey(m_wstrCurrentKey_BGM.c_str(), m_fVolume_BGM);
}

void CBGM_Manager::Mute_BGM()
{
    CHECK_TRUE(m_wstrCurrentKey_BGM.empty(), );

    m_pGameInstance->SetVolumeByKey(m_wstrCurrentKey_BGM.c_str(), 0.f);
}

void CBGM_Manager::UnMute_BGM()
{
    CHECK_TRUE(m_wstrCurrentKey_BGM.empty(), );

    m_pGameInstance->SetVolumeByKey(m_wstrCurrentKey_BGM.c_str(), m_fVolume_BGM);
}

void CBGM_Manager::Set_Volume_AMB(_float fVolume)
{
    m_fVolume_AMB = fVolume;

    m_pGameInstance->SetVolumeByKey(m_wstrCurrentKey_AMB.c_str(), m_fVolume_AMB);
}

void CBGM_Manager::Mute_AMB()
{
    CHECK_TRUE(m_wstrCurrentKey_AMB.empty(), );

    m_pGameInstance->SetVolumeByKey(m_wstrCurrentKey_AMB.c_str(), 0.f);
}

void CBGM_Manager::UnMute_AMB()
{
    CHECK_TRUE(m_wstrCurrentKey_AMB.empty(), );

    m_pGameInstance->SetVolumeByKey(m_wstrCurrentKey_AMB.c_str(), m_fVolume_AMB);
}

void CBGM_Manager::Clear_CurrentKey()
{
    m_wstrCurrentKey_BGM.clear();
    m_wstrStoreKey_BGM.clear();

    m_wstrCurrentKey_AMB.clear();
    m_wstrStoreKey_AMB.clear();
}

void CBGM_Manager::PlayBGM(const _tchar* pSoundKey, _float fFadeTime)
{
    _wstring wstrNewKey = pSoundKey;

    CHECK_TRUE(wstrNewKey.empty(), );

    wstrNewKey += TEXT(".wav");

    CHECK_TRUE(m_pGameInstance->IsPlayingByKey(wstrNewKey.c_str()), );

    Change_CurrentBGM(pSoundKey);

    m_pGameInstance->PlaySound_FadeIn(m_wstrCurrentKey_BGM.c_str(), m_fVolume_BGM, fFadeTime);
}

void CBGM_Manager::ChangeBGM(const _tchar* pSoundKey, _float fFadeTime, _bool isWav)
{
    _wstring wstrNewKey = pSoundKey;

    CHECK_TRUE(wstrNewKey.empty(), );

    if(isWav)
       wstrNewKey += TEXT(".wav");
    else
        wstrNewKey += TEXT(".mp3");

    CHECK_EQUAL(wstrNewKey, m_wstrCurrentKey_BGM, );

    if (!m_wstrCurrentKey_BGM.empty())
        m_pGameInstance->StopByKey_FadeOut(m_wstrCurrentKey_BGM.c_str(), fFadeTime);

    if (isWav)
        Change_CurrentBGM(pSoundKey);
    else
        m_wstrCurrentKey_BGM = wstrNewKey;

    m_pGameInstance->PlaySound_FadeIn(m_wstrCurrentKey_BGM.c_str(), m_fVolume_BGM, fFadeTime);
}

void CBGM_Manager::PlayAMB(const _tchar* pSoundKey, _float fFadeTime)
{
    _wstring wstrNewKey = pSoundKey;

    CHECK_TRUE(wstrNewKey.empty(), );

    wstrNewKey += TEXT(".wav");

    CHECK_TRUE(m_pGameInstance->IsPlayingByKey(wstrNewKey.c_str()), );

    Change_CurrentAMB(pSoundKey);

    m_pGameInstance->PlaySound_FadeIn(m_wstrCurrentKey_AMB.c_str(), m_fVolume_AMB, fFadeTime);
}

void CBGM_Manager::ChangeAMB(const _tchar* pSoundKey, _float fFadeTime, _bool isWav)
{
    _wstring wstrNewKey = pSoundKey;

    CHECK_TRUE(wstrNewKey.empty(), );

    if (isWav)
        wstrNewKey += TEXT(".wav");
    else
        wstrNewKey += TEXT(".mp3");

    CHECK_EQUAL(wstrNewKey, m_wstrCurrentKey_AMB, );

    if (!m_wstrCurrentKey_AMB.empty())
        m_pGameInstance->StopByKey_FadeOut(m_wstrCurrentKey_AMB.c_str(), fFadeTime);

    if (isWav)
        Change_CurrentAMB(pSoundKey);
    else
        m_wstrCurrentKey_AMB = wstrNewKey;

    m_pGameInstance->PlaySound_FadeIn(m_wstrCurrentKey_AMB.c_str(), m_fVolume_AMB, fFadeTime);
}

void CBGM_Manager::HeinMach_Entry(_float fFadeTime)
{
    Change_CurrentAMB(TEXT("AMB_BG_Field_Cold_Windy"));

    m_pGameInstance->PlaySound_FadeIn(m_wstrCurrentKey_AMB.c_str(), m_fVolume_AMB);

    Change_CurrentBGM(TEXT("bgm_Field_HeinIntro"));

    m_pGameInstance->PlaySound_FadeIn(m_wstrCurrentKey_BGM.c_str(), m_fVolume_BGM);
}

void CBGM_Manager::HeinMach_Dawn(_float fFadeTime)
{
    ChangeAMB(TEXT("AMB_BG_Field_Cold_Windy"), fFadeTime);
    ChangeBGM(TEXT("bgm_Field_HeinIntro"), fFadeTime);
}

void CBGM_Manager::HeinMach_Cave(_float fFadeTime)
{
    ChangeAMB(TEXT("AMB_BG_Field_Cold_Windy_Whistle"), fFadeTime);
    BGM_Stop(fFadeTime);
}

void CBGM_Manager::HeinMach_Day(_float fFadeTime)
{
    ChangeAMB(TEXT("AMB_BG_Field_Cold_Windy_Narrow"), fFadeTime);
    ChangeBGM(TEXT("bgm_Field_HeinCliff"), fFadeTime);
}

void CBGM_Manager::HeinMach_CutScene(_float fFadeTime)
{
    ChangeAMB(TEXT("AMB_BG_Field_Cold_Windy_Harsh"), fFadeTime);
    ChangeBGM(TEXT("bgm_Field_HeinTheme"), fFadeTime);
}

void CBGM_Manager::HeinMach_Halberd(_float fFadeTime)
{
    AMB_Stop(fFadeTime);
    ChangeBGM(TEXT("bgm_Elite_Halberd"), fFadeTime);
}

void CBGM_Manager::HeinMach_Yetuga_CutScene(_float fFadeTime)
{
    ChangeAMB(TEXT("AMB_BG_Embars_Loop_B"), fFadeTime);
    BGM_Stop(fFadeTime);
}

void CBGM_Manager::HeinMach_Yetuga_1Phase(_float fFadeTime)
{
    AMB_Stop(fFadeTime);
    ChangeBGM(TEXT("bgm_Yetuga_1Phase"), fFadeTime);
}

void CBGM_Manager::Embars_Entry(_float fFadeTime)
{
    Change_CurrentAMB(TEXT("AMB_BG_Embars_Loop_A"));

    m_pGameInstance->PlaySound_FadeIn(m_wstrCurrentKey_AMB.c_str(), m_fVolume_AMB, 3.f);

    Change_CurrentBGM(TEXT("bgm_Field_Embars"));

    m_pGameInstance->PlaySound_FadeIn(m_wstrCurrentKey_BGM.c_str(), m_fVolume_BGM, 3.f);
}

void CBGM_Manager::Embars_B1(_float fFadeTime)
{
    ChangeAMB(TEXT("AMB_BG_Embars_Loop_B"), fFadeTime);
    ChangeBGM(TEXT("bgm_Field_Embars_Under"), fFadeTime);
}

void CBGM_Manager::Embars_Club(_float fFadeTime)
{
    AMB_Stop();
    ChangeBGM(TEXT("BGM_MiniGame_Default"), fFadeTime);
}

void CBGM_Manager::Embars_Club_Game(_float fFadeTime)
{
    AMB_Stop();
    ChangeBGM(TEXT("BGM_MiniGame_Club"), fFadeTime, false);
}

void CBGM_Manager::Embars_Club_Shuffle_0(_float fFadeTime)
{
    AMB_Stop();
    ChangeBGM(TEXT("BGM_Minigame_Shuffle_0"), fFadeTime, false);
}

void CBGM_Manager::Embars_Club_Shuffle_1(_float fFadeTime)
{
    AMB_Stop();
    ChangeBGM(TEXT("BGM_Minigame_Shuffle_1"), fFadeTime, false);
}

void CBGM_Manager::Embars_Club_Shuffle_2(_float fFadeTime)
{
    AMB_Stop();
    ChangeBGM(TEXT("BGM_Minigame_Shuffle_2"), fFadeTime, false);
}

void CBGM_Manager::Embars_1F(_float fFadeTime)
{
    ChangeAMB(TEXT("AMB_BG_Embars_Loop_A"), fFadeTime);
    ChangeBGM(TEXT("bgm_Field_Embars"), fFadeTime, false);
}

void CBGM_Manager::Embars_Elamein(_float fFadeTime)
{
    AMB_Stop();
    ChangeBGM(TEXT("bgm_Elite_Elamein"), fFadeTime);
}

void CBGM_Manager::Viper_Entry(_float fFadeTime)
{
    Change_CurrentAMB(TEXT("AMB_BG_Field_Cold_Windy_Narrow"));

    m_pGameInstance->PlaySound_FadeIn(m_wstrCurrentKey_AMB.c_str(), m_fVolume_AMB, fFadeTime);

    //Change_CurrentBGM(TEXT("BGM_BG_Field_Cold_Windy_Narrow"));

    //m_pGameInstance->PlaySound_FadeIn(m_wstrCurrentKey_BGM.c_str(), m_fVolume_BGM, fFadeTime);
}

void CBGM_Manager::Viper_1PhaseCutScene(_float fFadeTime)
{
    ChangeAMB(TEXT("AMB_BG_Field_Cold_Windy_Narrow"), fFadeTime);
    //ChangeBGM(TEXT("bgm_Elite_Elamein"), fFadeTime);
    BGM_Stop(fFadeTime);
}

void CBGM_Manager::Viper_1Phase(_float fFadeTime)
{
    //ChangeAMB(TEXT("AMB_BG_Embars_Loop_B"), fFadeTime);
    AMB_Stop(fFadeTime);
    ChangeBGM(TEXT("bgm_Viper_1phase"), fFadeTime);
}

void CBGM_Manager::Viper_2PhaseCutScene(_float fFadeTime)
{
    ChangeAMB(TEXT("AMB_BG_Field_Cold_Windy_Narrow"), fFadeTime);
    //ChangeBGM(TEXT("bgm_Viper_1phase"), fFadeTime);
    BGM_Stop(fFadeTime);
}

void CBGM_Manager::Viper_2Phase(_float fFadeTime)
{
    //ChangeAMB(TEXT("AMB_BG_Field_Cold_Windy_Narrow"), fFadeTime);
    AMB_Stop(fFadeTime);
    ChangeBGM(TEXT("bgm_Viper_2phase"), fFadeTime);
}

void CBGM_Manager::Viper_End(_float fFadeTime)
{
    ChangeAMB(TEXT("AMB_BG_Field_Cold_Windy"), fFadeTime);
    //ChangeBGM(TEXT("bgm_Viper_2phase"), fFadeTime);
    BGM_Stop(fFadeTime);
}

void CBGM_Manager::BGM_Stop(_float fFadeTime)
{
    CHECK_TRUE(m_wstrCurrentKey_BGM.empty(), );

    m_pGameInstance->StopByKey_FadeOut(m_wstrCurrentKey_BGM.c_str(), fFadeTime);
}

void CBGM_Manager::BGM_Resume(_bool isFade, _float fFadeTime)
{
    CHECK_TRUE(m_wstrCurrentKey_BGM.empty(), );

    if (isFade)
        m_pGameInstance->Sound_Resume_Fade(m_wstrCurrentKey_BGM.c_str(), fFadeTime);
    else
        m_pGameInstance->Sound_Resume(m_wstrCurrentKey_BGM.c_str());
}

void CBGM_Manager::BGM_Pause(_bool isFade, _float fFadeTime)
{
    CHECK_TRUE(m_wstrCurrentKey_BGM.empty(), );

    if (isFade)
        m_pGameInstance->Sound_Pause_Fade(m_wstrCurrentKey_BGM.c_str(), fFadeTime);
    else
        m_pGameInstance->Sound_Pause(m_wstrCurrentKey_BGM.c_str());
}

void CBGM_Manager::AMB_Stop(_float fFadeTime)
{
    CHECK_TRUE(m_wstrCurrentKey_AMB.empty(), );

    m_pGameInstance->StopByKey_FadeOut(m_wstrCurrentKey_AMB.c_str(), fFadeTime);
}

void CBGM_Manager::AMB_Resume(_bool isFade, _float fFadeTime)
{
    CHECK_TRUE(m_wstrCurrentKey_AMB.empty(), );

    if (isFade)
        m_pGameInstance->Sound_Resume_Fade(m_wstrCurrentKey_AMB.c_str(), fFadeTime);
    else
        m_pGameInstance->Sound_Resume(m_wstrCurrentKey_AMB.c_str());
}

void CBGM_Manager::AMB_Pause(_bool isFade, _float fFadeTime)
{
    CHECK_TRUE(m_wstrCurrentKey_AMB.empty(), );

    if (isFade)
        m_pGameInstance->Sound_Pause_Fade(m_wstrCurrentKey_AMB.c_str(), fFadeTime);
    else
        m_pGameInstance->Sound_Pause(m_wstrCurrentKey_AMB.c_str());
}

void CBGM_Manager::Change_CurrentBGM(const _tchar* pSoundKey)
{
    m_wstrCurrentKey_BGM = pSoundKey;
    m_wstrCurrentKey_BGM += TEXT(".wav");
}

void CBGM_Manager::Change_CurrentAMB(const _tchar* pSoundKey)
{
    m_wstrCurrentKey_AMB = pSoundKey;
    m_wstrCurrentKey_AMB += TEXT(".wav");
}

CBGM_Manager* CBGM_Manager::Create()
{
    CBGM_Manager* pInstance = new CBGM_Manager();

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX(TEXT("Failed To Created : CBGM_Manager"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBGM_Manager::Free()
{
    __super::Free();

    Safe_Release(m_pGameInstance);
}
