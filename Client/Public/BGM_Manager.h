#pragma once

#include "Base.h"
#include "Client_Defines.h"

NS_BEGIN(Client)

class CBGM_Manager final : public CBase
{
public:
    enum class BGM_STATE
    {
        NONE,
        HEINMACH_DAWN, HINEMACH_CAVE, HEINMACH_DAY, HEINMACH_BOSS,
        EMBARS_1F, EMBARS_B1,
        VIPER_ENTRY, VIPER_1PHASE, VIPER_2PHASE
    };

private:
    CBGM_Manager();
    virtual ~CBGM_Manager() = default;

public:
    HRESULT Initialize();
    void Update(_float fTimeDelta);

public:
    _float Get_Volume_BGM() { return m_fVolume_BGM; }
    void Set_Volume_BGM(_float fVolume) { m_fVolume_BGM = fVolume; }

    void Mute_BGM();
    void UnMute_BGM();

    void Clear_CurrentKey();

public:
    void PlayBGM(const _tchar* pSoundKey, _float fFadeTime = 1.f);
    void ChangeBGM(const _tchar* pSoundKey, _float fFadeTime = 1.f, _bool isWav = true);
    void PlayBattleBGM(const _tchar* pSoundKey, _float fFadeTime = 1.f);
    void EndBattleBGM(_float fFadeTime = 1.f);

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

private:
    class CGameInstance* m_pGameInstance = { nullptr };

private:
    _wstring m_wstrCurrentKey_BGM = {};
    _wstring m_wstrStoreKey_BGM = {};

    _float m_fVolume_BGM = { 0.25f };

    _bool m_isMute = { false };

    BGM_STATE m_eState = {};

private:
    void Change_CurrentBGM(const _tchar* pSoundKey);

public:
    static CBGM_Manager* Create();
    virtual void Free() override;
};

NS_END
