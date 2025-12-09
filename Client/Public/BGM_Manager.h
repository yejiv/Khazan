#pragma once

#include "Base.h"
#include "Client_Defines.h"

NS_BEGIN(Client)

class CBGM_Manager final : public CBase
{
private:
    enum class BGM_State
    {

    };

private:
    CBGM_Manager();
    virtual ~CBGM_Manager() = default;

public:
    HRESULT Initialize();
    void Update(_float fTimeDelta);

public:
    _float Get_FadeTime_BGM() { return m_fFadeTime_BGM; }
    void Set_FadeTime_BGM(_float fFadeTime) { m_fFadeTime_BGM = fFadeTime; }

    _float Get_Volume_BGM() { return m_fVolume_BGM; }
    void Set_Volume_BGM(_float fVolume) { m_fVolume_BGM = fVolume; }

    void Mute_BGM();
    void UnMute_BGM();

    void Clear_CurrentKey();

public:
    void PlayBGM(const _tchar* pSoundKey, _float fFadeTime = 1.f);
    void ChangeBGM(const _tchar* pSoundKey, _float fFadeTime = 1.f);
    void PlayBattleBGM(const _tchar* pSoundKey, _float fFadeTime = 1.f);
    void EndBattleBGM(_float fFadeTime = 1.f);

#pragma region 하인마흐 프리셋

    void HeinMach_Entry();
    void HeinMach_CutScene();
    void HeinMach_Cave_Entry();
    void HeinMach_Halberd();
    void HeinMach_Yetuga_Entry();

#pragma endregion

#pragma region 엠바스 프리셋

    void Embars_Entry();
    void Embars_FirstElevator();
    void Embars_LastElevator();

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
    _float m_fFadeTime_BGM = { 1.f };

    _bool m_isMute = { false };

    BGM_State m_eState = {};

private:
    void Change_CurrentBGM(const _tchar* pSoundKey);

public:
    static CBGM_Manager* Create();
    virtual void Free() override;
};

NS_END
