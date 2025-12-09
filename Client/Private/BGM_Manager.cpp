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

void CBGM_Manager::Mute_BGM()
{
    m_pGameInstance->SetVolumeByKey(m_wstrCurrentKey_BGM.c_str(), 0.f);
}

void CBGM_Manager::UnMute_BGM()
{
    m_pGameInstance->SetVolumeByKey(m_wstrCurrentKey_BGM.c_str(), m_fVolume_BGM);
}

void CBGM_Manager::Clear_CurrentKey()
{
    m_wstrCurrentKey_BGM.clear();
}

void CBGM_Manager::PlayBGM(const _tchar* pSoundKey, _float fFadeTime)
{
    _wstring wstrNewKey = pSoundKey;
    wstrNewKey += TEXT(".wav");

    if (m_pGameInstance->IsPlayingByKey(wstrNewKey.c_str()))
        return;

    Change_CurrentBGM(pSoundKey);

    m_pGameInstance->PlaySound_FadeIn(m_wstrCurrentKey_BGM.c_str(), m_fVolume_BGM, fFadeTime);
}

void CBGM_Manager::ChangeBGM(const _tchar* pSoundKey, _float fFadeTime)
{
    _wstring wstrNewKey = pSoundKey;
    wstrNewKey += TEXT(".wav");

    if (wstrNewKey == m_wstrCurrentKey_BGM)
        return;

    m_pGameInstance->StopByKey_FadeOut(m_wstrCurrentKey_BGM.c_str(), fFadeTime);

    Change_CurrentBGM(pSoundKey);

    m_pGameInstance->PlaySound_FadeIn(m_wstrCurrentKey_BGM.c_str(), m_fVolume_BGM, fFadeTime);
}

void CBGM_Manager::HeinMach_Entry()
{
    Change_CurrentBGM(TEXT("AMB_BG_Field_Cold_Windy"));

    m_pGameInstance->PlaySound_FadeIn(m_wstrCurrentKey_BGM.c_str(), m_fVolume_BGM);
}

void CBGM_Manager::HeinMach_CutScene()
{
    ChangeBGM(TEXT("AMB_BG_Field_Cold_Windy_Whistle"));
}

void CBGM_Manager::HeinMach_Cave_Entry()
{
    ChangeBGM(TEXT("AMB_BG_Field_Cold_Windy_Harsh"), 1.f);
}

void CBGM_Manager::HeinMach_Halberd()
{
    ChangeBGM(TEXT("AMB_BG_Field_Cold_Windy_Narrow"), 1.f);
}

void CBGM_Manager::HeinMach_Yetuga_Entry()
{
    ChangeBGM(TEXT("AMB_BG_Field_Cold_Windy"), 1.f);
}

void CBGM_Manager::BGM_Stop(_float fFadeTime)
{
    m_pGameInstance->StopByKey_FadeOut(m_wstrCurrentKey_BGM.c_str(), fFadeTime);

    Clear_CurrentKey();
}

void CBGM_Manager::BGM_Resume(_bool isFade, _float fFadeTime)
{
    if (isFade)
        m_pGameInstance->Sound_Resume_Fade(m_wstrCurrentKey_BGM.c_str(), fFadeTime);
    else
        m_pGameInstance->Sound_Resume(m_wstrCurrentKey_BGM.c_str());
}

void CBGM_Manager::BGM_Pause(_bool isFade, _float fFadeTime)
{
    if (isFade)
        m_pGameInstance->Sound_Pause_Fade(m_wstrCurrentKey_BGM.c_str(), fFadeTime);
    else
        m_pGameInstance->Sound_Pause(m_wstrCurrentKey_BGM.c_str());
}

void CBGM_Manager::Change_CurrentBGM(const _tchar* pSoundKey)
{
    m_wstrCurrentKey_BGM = pSoundKey;
    m_wstrCurrentKey_BGM += TEXT(".wav");
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
