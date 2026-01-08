#pragma once

#include "Base.h"
#include "Client_Defines.h"

NS_BEGIN(Client)

class CBGM_Manager final : public CBase
{
private:
    CBGM_Manager();
    virtual ~CBGM_Manager() = default;

public:
    HRESULT Initialize();
    void Update(_float fTimeDelta);

public:
    _float Get_Volume_BGM() { return m_fVolume_BGM; }
    void Set_Volume_BGM(_float fVolume);

    void Mute_BGM();
    void UnMute_BGM();

    _float Get_Volume_AMB() { return m_fVolume_AMB; }
    void Set_Volume_AMB(_float fVolume);

    void Mute_AMB();
    void UnMute_AMB();

    void Clear_CurrentKey();

public:
    void PlayBGM(const _tchar* pSoundKey, _float fFadeTime = 1.f);
    void ChangeBGM(const _tchar* pSoundKey, _float fFadeTime = 1.f, _bool isWav = true);

    void PlayAMB(const _tchar* pSoundKey, _float fFadeTime = 1.f);
    void ChangeAMB(const _tchar* pSoundKey, _float fFadeTime = 1.f, _bool isWav = true);

#pragma region 하인마흐 프리셋

    void HeinMach_Entry(_float fFadeTime = 1.f);
    void HeinMach_Dawn(_float fFadeTime = 1.f);
    void HeinMach_CutScene(_float fFadeTime = 1.f);
    void HeinMach_Cave(_float fFadeTime = 1.f);
    void HeinMach_Day(_float fFadeTime = 1.f);
    void HeinMach_Halberd(_float fFadeTime = 1.f);
    void HeinMach_Yetuga_CutScene(_float fFadeTime = 1.f);
    void HeinMach_Yetuga_1Phase(_float fFadeTime = 1.f);

#pragma endregion

#pragma region 엠바스 프리셋

    void Embars_Entry(_float fFadeTime = 1.f);
    void Embars_B1(_float fFadeTime = 1.f);
    void Embars_Club(_float fFadeTime = 1.f);
    void Embars_Club_Game(_float fFadeTime = 1.f);
    void Embars_Club_Shuffle_0(_float fFadeTime = 1.f);
    void Embars_Club_Shuffle_1(_float fFadeTime = 1.f);
    void Embars_Club_Shuffle_2(_float fFadeTime = 1.f);
    void Embars_1F(_float fFadeTime = 1.f);
    void Embars_Elamein(_float fFadeTime = 1.f);

#pragma endregion

#pragma region 바이퍼 프리셋

    void Viper_Entry(_float fFadeTime = 1.f);
    void Viper_1PhaseCutScene(_float fFadeTime = 1.f);
    void Viper_1Phase(_float fFadeTime = 1.f);
    void Viper_2PhaseCutScene(_float fFadeTime = 1.f);
    void Viper_2Phase(_float fFadeTime = 1.f);
    void Viper_End(_float fFadeTime = 1.f);

#pragma endregion

public:
    void BGM_Stop(_float fFadeTime = 1.f);

    void BGM_Resume(_bool isFade = false, _float fFadeTime = 1.f);
    void BGM_Pause(_bool isFade = false, _float fFadeTime = 1.f);

    void AMB_Stop(_float fFadeTime = 1.f);

    void AMB_Resume(_bool isFade = false, _float fFadeTime = 1.f);
    void AMB_Pause(_bool isFade = false, _float fFadeTime = 1.f);

private:
    class CGameInstance* m_pGameInstance = { nullptr };

private:
    _wstring m_wstrCurrentKey_BGM = {};
    _wstring m_wstrStoreKey_BGM = {};

    _float m_fVolume_BGM = { 0.25f };

    _wstring m_wstrCurrentKey_AMB = {};
    _wstring m_wstrStoreKey_AMB = {};

    _float m_fVolume_AMB = { 0.35f };

private:
    void Change_CurrentBGM(const _tchar* pSoundKey);
    void Change_CurrentAMB(const _tchar* pSoundKey);

public:
    static CBGM_Manager* Create();
    virtual void Free() override;
};

NS_END
