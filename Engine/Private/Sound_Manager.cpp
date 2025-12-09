#include "Sound_Manager.h"

CSound_Manager::CSound_Manager() {}

HRESULT CSound_Manager::Initialize()
{
    m_fGloval_Volume = 0.7f;
    FMOD_System_Create(&m_pSystem, FMOD_VERSION);
    FMOD_System_Init(m_pSystem, m_iMaxChannels, FMOD_INIT_NORMAL, nullptr);
    LoadSoundFile();
    return S_OK;
}

void CSound_Manager::Update()
{
    if (m_pSystem) FMOD_System_Update(m_pSystem);
}

void CSound_Manager::ListenerPosSet(_vector vPos, _vector vLook, _vector vUp, _float3 vVal)
{
    FMOD_VECTOR fmvListenerPos = { XMVectorGetX(vPos), XMVectorGetY(vPos), XMVectorGetZ(vPos) };
    FMOD_VECTOR fmvListenerForward = { XMVectorGetX(vLook), XMVectorGetY(vLook), XMVectorGetZ(vLook) };
    FMOD_VECTOR fmvListenerUp = { XMVectorGetX(vUp), XMVectorGetY(vUp), XMVectorGetZ(vUp) };
    FMOD_VECTOR fmvListenerVel = { vVal.x, vVal.y, vVal.z };

    FMOD_System_Set3DListenerAttributes(m_pSystem, 0, &fmvListenerPos, &fmvListenerVel, &fmvListenerForward, &fmvListenerUp);
}

void CSound_Manager::PlaySoundOnce(const TCHAR* pSoundKey, float fVolume, FMOD_CHANNEL** ppOutChannel)
{
    FMOD_SOUND* pSound = nullptr;
    if (!FindSound(pSoundKey, &pSound)) return;

    if (ppOutChannel == nullptr)
    {
        FMOD_CHANNEL* pCh = nullptr;
        FMOD_System_PlaySound(m_pSystem, pSound, nullptr, FALSE, &pCh);
        if (pCh)
        {
            FMOD_Channel_SetMode(pCh, FMOD_DEFAULT);
            FMOD_Channel_SetVolume(pCh, fVolume * m_fGloval_Volume);
            if (ppOutChannel) *ppOutChannel = pCh;
        }
    }
    else
    {
        FMOD_BOOL bPlay = false;
        FMOD_Channel_IsPlaying(*ppOutChannel, &bPlay);

        if (bPlay)
            FMOD_Channel_Stop(*ppOutChannel);

        FMOD_System_PlaySound(m_pSystem, pSound, nullptr, FALSE, ppOutChannel);
        FMOD_Channel_SetMode(*ppOutChannel, FMOD_DEFAULT);
        FMOD_Channel_SetVolume(*ppOutChannel, fVolume * m_fGloval_Volume);

    }
}

void CSound_Manager::PlaySoundOnce(const TCHAR* pSoundKey, _vector vPos, _float3 vVel, float fVolume, FMOD_CHANNEL** ppOutChannel)
{
    FMOD_SOUND* pSound = nullptr;
    if (!FindSound(pSoundKey, &pSound)) return;

    if (ppOutChannel == nullptr)
    {
        FMOD_CHANNEL* pCh = nullptr;
        FMOD_System_PlaySound(m_pSystem, pSound, nullptr, FALSE, &pCh);
        if (pCh)
        {
            FMOD_Channel_SetMode(pCh, FMOD_3D);
            FMOD_Channel_SetVolume(pCh, fVolume * m_fGloval_Volume);

            FMOD_VECTOR fmPos = { XMVectorGetX(vPos), XMVectorGetY(vPos), XMVectorGetZ(vPos) };
            FMOD_VECTOR fmVel = { vVel.x, vVel.y, vVel.z };
            FMOD_Channel_Set3DAttributes(pCh, &fmPos, &fmVel);

            if (ppOutChannel) *ppOutChannel = pCh;
        }
    }
    else
    {
        FMOD_BOOL bPlay = false;
        FMOD_Channel_IsPlaying(*ppOutChannel, &bPlay);

        if (bPlay)
            FMOD_Channel_Stop(*ppOutChannel);

        FMOD_System_PlaySound(m_pSystem, pSound, nullptr, FALSE, ppOutChannel);

        FMOD_Channel_SetMode(*ppOutChannel, FMOD_3D);
        FMOD_Channel_SetVolume(*ppOutChannel, fVolume * m_fGloval_Volume);

        FMOD_VECTOR fmPos = { XMVectorGetX(vPos), XMVectorGetY(vPos), XMVectorGetZ(vPos) };
        FMOD_VECTOR fmVel = { vVel.x, vVel.y, vVel.z };
        FMOD_Channel_Set3DAttributes(*ppOutChannel, &fmPos, &fmVel);

    }
}

void CSound_Manager::PlaySoundOnce(const TCHAR* pSoundKey, _vector vPos, FMOD_CHANNEL** ppOutChannel, float fVolume)
{
    FMOD_SOUND* pSound = nullptr;
    if (!FindSound(pSoundKey, &pSound)) return;

    if (ppOutChannel == nullptr)
    {
        FMOD_CHANNEL* pCh = nullptr;
        FMOD_System_PlaySound(m_pSystem, pSound, nullptr, FALSE, &pCh);
        if (pCh)
        {
            FMOD_Channel_SetMode(pCh, FMOD_3D);
            FMOD_Channel_SetVolume(pCh, fVolume * m_fGloval_Volume);

            FMOD_VECTOR fmPos = { XMVectorGetX(vPos), XMVectorGetY(vPos), XMVectorGetZ(vPos) };
            FMOD_VECTOR fmVel = { 0.f, 0.f, 0.f };
            FMOD_Channel_Set3DAttributes(pCh, &fmPos, &fmVel);

            if (ppOutChannel) *ppOutChannel = pCh;
        }
    }
    else
    {
        FMOD_BOOL bPlay = false;
        FMOD_Channel_IsPlaying(*ppOutChannel, &bPlay);

        if (bPlay)
            FMOD_Channel_Stop(*ppOutChannel);

        FMOD_System_PlaySound(m_pSystem, pSound, nullptr, FALSE, ppOutChannel);

        FMOD_Channel_SetMode(*ppOutChannel, FMOD_3D);
        FMOD_Channel_SetVolume(*ppOutChannel, fVolume * m_fGloval_Volume);

        FMOD_VECTOR fmPos = { XMVectorGetX(vPos), XMVectorGetY(vPos), XMVectorGetZ(vPos) };
        FMOD_VECTOR fmVel = { 0.f, 0.f, 0.f };
        FMOD_Channel_Set3DAttributes(*ppOutChannel, &fmPos, &fmVel);

    }
}

void CSound_Manager::PlaySoundLoop(const TCHAR* pSoundKey, float fVolume, FMOD_CHANNEL** ppOutChannel)
{
    FMOD_SOUND* pSound = nullptr;
    if (!FindSound(pSoundKey, &pSound)) return;

    FMOD_CHANNEL* pCh = nullptr;
    FMOD_System_PlaySound(m_pSystem, pSound, nullptr, FALSE, &pCh);
    if (pCh)
    {
        FMOD_Channel_SetMode(pCh, FMOD_LOOP_NORMAL);
        FMOD_Channel_SetVolume(pCh, fVolume * m_fGloval_Volume);
        if (ppOutChannel) *ppOutChannel = pCh;
    }
}

void CSound_Manager::PlaySoundLoop(const TCHAR* pSoundKey, _vector vPos, _float3 vVel, float fVolume, FMOD_CHANNEL** ppOutChannel)
{
    FMOD_SOUND* pSound = nullptr;
    if (!FindSound(pSoundKey, &pSound)) return;

    FMOD_Sound_SetMode(pSound, FMOD_3D);

    FMOD_CHANNEL* pCh = nullptr;
    FMOD_System_PlaySound(m_pSystem, pSound, nullptr, FALSE, &pCh);

    if (pCh)
    {
        FMOD_Channel_SetMode(pCh, FMOD_3D | FMOD_LOOP_NORMAL);
        FMOD_Channel_SetVolume(pCh, fVolume * m_fGloval_Volume);

        FMOD_VECTOR fmPos = { XMVectorGetX(vPos), XMVectorGetY(vPos), XMVectorGetZ(vPos) };
        FMOD_VECTOR fmVel = { vVel.x, vVel.y, vVel.z };
        FMOD_Channel_Set3DAttributes(pCh, &fmPos, &fmVel);
  
        if (ppOutChannel) *ppOutChannel = pCh;
    }
}

void CSound_Manager::PlaySoundLoop(const TCHAR* pSoundKey, _vector vPos, FMOD_CHANNEL** ppOutChannel, float fVolume)
{
    FMOD_SOUND* pSound = nullptr;
    if (!FindSound(pSoundKey, &pSound)) return;

    FMOD_Sound_SetMode(pSound, FMOD_3D);

    FMOD_CHANNEL* pCh = nullptr;
    FMOD_System_PlaySound(m_pSystem, pSound, nullptr, FALSE, &pCh);

    if (pCh)
    {
        FMOD_Channel_SetMode(pCh, FMOD_3D | FMOD_LOOP_NORMAL);
        FMOD_Channel_SetVolume(pCh, fVolume * m_fGloval_Volume);

        FMOD_VECTOR fmPos = { XMVectorGetX(vPos), XMVectorGetY(vPos), XMVectorGetZ(vPos) };
        FMOD_VECTOR fmVel = { 0.f, 0.f, 0.f };
        FMOD_Channel_Set3DAttributes(pCh, &fmPos, &fmVel);

        if (ppOutChannel) *ppOutChannel = pCh;
    }
}

void CSound_Manager::StopByKey(const TCHAR* pSoundKey)
{
    FMOD_SOUND* pTarget = nullptr;
    if (!FindSound(pSoundKey, &pTarget)) return;

    ForEachPlayingChannel([&](FMOD_CHANNEL* ch)
        {
            FMOD_SOUND* cur = nullptr;
            if (FMOD_Channel_GetCurrentSound(ch, &cur) == FMOD_OK && cur == pTarget)
            {
                FMOD_Channel_Stop(ch);
            }
        });
}

void CSound_Manager::StopByChannel(FMOD_CHANNEL** ppOutChannel)
{
    if (ppOutChannel != nullptr)
        FMOD_Channel_Stop(*ppOutChannel);
}

void CSound_Manager::PlaySoundOnce_FadeIn(const TCHAR* pSoundKey, float fVolume, float fFadeTime, FMOD_CHANNEL** ppOutChannel)
{
    FMOD_SOUND* pSound = nullptr;
    if (!FindSound(pSoundKey, &pSound)) return;

    if (ppOutChannel && *ppOutChannel)
    {
        FMOD_BOOL bPlay = FALSE;
        FMOD_Channel_IsPlaying(*ppOutChannel, &bPlay);
        if (bPlay) FMOD_Channel_Stop(*ppOutChannel);
    }

    FMOD_CHANNEL* pCh = nullptr;

    FMOD_System_PlaySound(m_pSystem, pSound, nullptr, TRUE, &pCh);
    if (!pCh) return;

    FMOD_Channel_SetMode(pCh, FMOD_DEFAULT);

    unsigned long long dspClock = 0;
    FMOD_Channel_GetDSPClock(pCh, 0, &dspClock);

    int iSampleRate = 48000;
    FMOD_System_GetSoftwareFormat(m_pSystem, &iSampleRate, nullptr, nullptr);

    float fTargetVolume = fVolume * m_fGloval_Volume;
    unsigned long long fadeEnd = dspClock + (unsigned long long)(fFadeTime * iSampleRate);

    FMOD_Channel_SetVolume(pCh, 1.0f);

    FMOD_Channel_AddFadePoint(pCh, dspClock, 0.0f);
    FMOD_Channel_AddFadePoint(pCh, fadeEnd, fTargetVolume);

    FMOD_Channel_SetPaused(pCh, FALSE);

    if (ppOutChannel)
        *ppOutChannel = pCh;
}

void CSound_Manager::PlaySoundLoop_FadeIn(const TCHAR* pSoundKey, float fVolume, float fFadeTime, FMOD_CHANNEL** ppOutChannel)
{
    FMOD_SOUND* pSound = nullptr;
    if (!FindSound(pSoundKey, &pSound)) return;

    if (ppOutChannel && *ppOutChannel)
    {
        FMOD_BOOL bPlay = FALSE;
        FMOD_Channel_IsPlaying(*ppOutChannel, &bPlay);
        if (bPlay) FMOD_Channel_Stop(*ppOutChannel);
    }

    FMOD_CHANNEL* pCh = nullptr;

    FMOD_System_PlaySound(m_pSystem, pSound, nullptr, TRUE, &pCh);
    if (!pCh) return;

    FMOD_Channel_SetMode(pCh, FMOD_LOOP_NORMAL);

    unsigned long long dspClock = 0;
    FMOD_Channel_GetDSPClock(pCh, 0, &dspClock);

    int iSampleRate = 48000;
    FMOD_System_GetSoftwareFormat(m_pSystem, &iSampleRate, nullptr, nullptr);

    float fTargetVolume = fVolume * m_fGloval_Volume;
    unsigned long long fadeEnd = dspClock + (unsigned long long)(fFadeTime * iSampleRate);

    FMOD_Channel_SetVolume(pCh, 1.0f);

    FMOD_Channel_AddFadePoint(pCh, dspClock, 0.0f);
    FMOD_Channel_AddFadePoint(pCh, fadeEnd, fTargetVolume);

    FMOD_Channel_SetPaused(pCh, FALSE);

    if (ppOutChannel)
        *ppOutChannel = pCh;
}

void CSound_Manager::StopByKey_FadeOut(const TCHAR* pSoundKey, float fFadeTime)
{
    FMOD_SOUND* pTarget = nullptr;
    if (!FindSound(pSoundKey, &pTarget))
        return;

    ForEachPlayingChannel([&](FMOD_CHANNEL* ch)
        {
            FMOD_SOUND* cur = nullptr;
            if (FMOD_Channel_GetCurrentSound(ch, &cur) == FMOD_OK && cur == pTarget)
            {
                float fCurrentAudibility = 0.0f;
                FMOD_Channel_GetAudibility(ch, &fCurrentAudibility);

                FMOD_Channel_RemoveFadePoints(ch, 0, ULLONG_MAX);

                FMOD_Channel_SetVolume(ch, 1.0f);

                float fLocalStartVolume = fCurrentAudibility;
                if (m_fGloval_Volume != 0.0f) {
                    fLocalStartVolume = fCurrentAudibility / m_fGloval_Volume;
                }

                if (fLocalStartVolume > 1.0f)
                    fLocalStartVolume = 1.0f;

                unsigned long long dspClock = 0;
                FMOD_Channel_GetDSPClock(ch, nullptr, &dspClock);

                int iSampleRate = 48000;
                FMOD_System_GetSoftwareFormat(m_pSystem, &iSampleRate, nullptr, nullptr);

                unsigned long long fadeEnd = dspClock + (unsigned long long)(fFadeTime * iSampleRate);

                FMOD_Channel_AddFadePoint(ch, dspClock, fLocalStartVolume);

                FMOD_Channel_AddFadePoint(ch, fadeEnd, 0.0f);

                FMOD_Channel_SetDelay(ch, 0, fadeEnd, false);
            }
        });
}

bool CSound_Manager::IsPlayingByKey(const TCHAR* pSoundKey)
{
    FMOD_SOUND* pTarget = nullptr;
    if (!FindSound(pSoundKey, &pTarget)) return false;

    bool found = false;
    ForEachPlayingChannel([&](FMOD_CHANNEL* ch)
        {
            if (found) return;
            FMOD_SOUND* cur = nullptr;
            if (FMOD_Channel_GetCurrentSound(ch, &cur) == FMOD_OK && cur == pTarget)
            {
                FMOD_BOOL bPlay = FALSE;
                if (FMOD_Channel_IsPlaying(ch, &bPlay) == FMOD_OK && bPlay)
                    found = true;
            }
        });
    return found;
}

void CSound_Manager::SetVolumeByKey(const TCHAR* pSoundKey, float fVolume)
{
    FMOD_SOUND* pTarget = nullptr;
    if (!FindSound(pSoundKey, &pTarget)) return;

    ForEachPlayingChannel([&](FMOD_CHANNEL* ch)
        {
            FMOD_SOUND* cur = nullptr;
            if (FMOD_Channel_GetCurrentSound(ch, &cur) == FMOD_OK && cur == pTarget)
            {
                FMOD_Channel_SetVolume(ch, fVolume * m_fGloval_Volume);
            }
        });
}

void CSound_Manager::Resume(const TCHAR* pSoundKey)
{
    FMOD_SOUND* pTarget = nullptr;
    if (!FindSound(pSoundKey, &pTarget))
        return;

    ForEachPlayingChannel([&](FMOD_CHANNEL* ch) {
            FMOD_SOUND* cur = nullptr;
            if (FMOD_Channel_GetCurrentSound(ch, &cur) == FMOD_OK && cur == pTarget)
            {
                FMOD_Channel_SetPaused(ch, FALSE);
            }
        });
}

void CSound_Manager::Resume_Fade(const TCHAR* pSoundKey, float fFadeTime)
{
    FMOD_SOUND* pTarget = nullptr;
    if (!FindSound(pSoundKey, &pTarget))
        return;

    ForEachPlayingChannel([&](FMOD_CHANNEL* ch)
        {
            FMOD_SOUND* cur = nullptr;
            if (FMOD_Channel_GetCurrentSound(ch, &cur) == FMOD_OK && cur == pTarget)
            {
                FMOD_Channel_RemoveFadePoints(ch, 0, ULLONG_MAX);

                FMOD_Channel_SetPaused(ch, FALSE);

                unsigned long long dspClock = 0;
                FMOD_Channel_GetDSPClock(ch, nullptr, &dspClock);

                int iSampleRate = 48000;
                FMOD_System_GetSoftwareFormat(m_pSystem, &iSampleRate, nullptr, nullptr);

                unsigned long long fadeEnd =
                    dspClock + (unsigned long long)(fFadeTime * iSampleRate);

                float fTargetVolume = m_fGloval_Volume;

                FMOD_Channel_SetVolume(ch, 0.0f);

                FMOD_Channel_AddFadePoint(ch, dspClock, 0.0f);
                FMOD_Channel_AddFadePoint(ch, fadeEnd, fTargetVolume);
            }
        });
}

void CSound_Manager::Pause(const TCHAR* pSoundKey)
{
    FMOD_SOUND* pTarget = nullptr;
    if (!FindSound(pSoundKey, &pTarget))
        return;

    ForEachPlayingChannel([&](FMOD_CHANNEL* ch) {
            FMOD_SOUND* cur = nullptr;
            if (FMOD_Channel_GetCurrentSound(ch, &cur) == FMOD_OK && cur == pTarget)
            {
                FMOD_Channel_SetPaused(ch, TRUE);
            }
        });
}

void CSound_Manager::Pause_Fade(const TCHAR* pSoundKey, float fFadeTime)
{
    FMOD_SOUND* pTarget = nullptr;
    if (!FindSound(pSoundKey, &pTarget))
        return;

    ForEachPlayingChannel([&](FMOD_CHANNEL* ch)
        {
            FMOD_SOUND* cur = nullptr;
            if (FMOD_Channel_GetCurrentSound(ch, &cur) == FMOD_OK && cur == pTarget)
            {
                float fCurrentAudibility = 0.0f;
                FMOD_Channel_GetAudibility(ch, &fCurrentAudibility);

                FMOD_Channel_RemoveFadePoints(ch, 0, ULLONG_MAX);
                FMOD_Channel_SetVolume(ch, 1.0f);

                float fLocalStartVolume = fCurrentAudibility;
                if (m_fGloval_Volume != 0.0f)
                    fLocalStartVolume = fCurrentAudibility / m_fGloval_Volume;

                if (fLocalStartVolume > 1.0f)
                    fLocalStartVolume = 1.0f;

                unsigned long long dspClock = 0;
                FMOD_Channel_GetDSPClock(ch, nullptr, &dspClock);

                int iSampleRate = 48000;
                FMOD_System_GetSoftwareFormat(m_pSystem, &iSampleRate, nullptr, nullptr);

                unsigned long long fadeEnd =
                    dspClock + (unsigned long long)(fFadeTime * iSampleRate);

                FMOD_Channel_AddFadePoint(ch, dspClock, fLocalStartVolume);
                FMOD_Channel_AddFadePoint(ch, fadeEnd, 0.0f);

                FMOD_Channel_SetDelay(ch, 0, fadeEnd, false);

                FMOD_Channel_SetPaused(ch, TRUE);
            }
        });
}

void CSound_Manager::StopAll()
{
    FMOD_CHANNELGROUP* pMaster = nullptr;
    if (FMOD_System_GetMasterChannelGroup(m_pSystem, &pMaster) == FMOD_OK && pMaster)
    {
        FMOD_ChannelGroup_Stop(pMaster);
    }
}

bool CSound_Manager::FindSound(const TCHAR* pSoundKey, FMOD_SOUND** ppOutSound)
{
#ifdef UNICODE
    wstring key = pSoundKey;
#else
    wstring key;
    int len = MultiByteToWideChar(CP_ACP, 0, pSoundKey, -1, nullptr, 0);
    key.resize(len ? (len - 1) : 0);
    if (len > 0) MultiByteToWideChar(CP_ACP, 0, pSoundKey, -1, key.data(), len);
#endif
    auto it = m_mapSound.find(key);
    if (it == m_mapSound.end()) return false;
    *ppOutSound = it->second;
    return true;
}

void CSound_Manager::ForEachPlayingChannel(function<void(FMOD_CHANNEL*)> fn)
{
    for (int i = 0; i < m_iMaxChannels; ++i)
    {
        FMOD_CHANNEL* ch = nullptr;
        if (FMOD_System_GetChannel(m_pSystem, i, &ch) == FMOD_OK && ch)
        {
            FMOD_BOOL bPlay = FALSE;
            if (FMOD_Channel_IsPlaying(ch, &bPlay) == FMOD_OK && bPlay)
                fn(ch);
        }
    }
}

void CSound_Manager::LoadSoundFile()
{
    wchar_t buf[MAX_PATH];
    GetCurrentDirectoryW(MAX_PATH, buf);
    OutputDebugStringW((_wstring(L"[CWD] ") + buf + L"\n").c_str());

    _finddata_t fd;
    long long handle = _findfirst("./../Bin/Sound/*.*", &fd);
    if (handle == -1) return;

    int iResult = 0;
    char szCurPath[128] = "./../Bin/Sound/";
    char szFullPath[128] = "";

    while (iResult != -1)
    {
        strcpy_s(szFullPath, szCurPath);
        strcat_s(szFullPath, fd.name);

        FMOD_SOUND* pSound = nullptr;
        FMOD_RESULT eRes = FMOD_System_CreateSound(m_pSystem, szFullPath, FMOD_DEFAULT, nullptr, &pSound);

        if (eRes == FMOD_OK)
        {
            int wlen = MultiByteToWideChar(CP_ACP, 0, fd.name, -1, nullptr, 0);
            _wstring wkey;
            if (wlen > 0) {
                wkey.resize(wlen - 1);
                MultiByteToWideChar(CP_ACP, 0, fd.name, -1, wkey.data(), wlen);
            }

            m_mapSound.emplace(move(wkey), pSound);
        }

        iResult = _findnext(handle, &fd);
    }

    _findclose(handle);
}

CSound_Manager* CSound_Manager::Create()
{
    CSound_Manager* pInstance = new CSound_Manager();

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX(TEXT("Failed to Created : CSound_Manager"));
        pInstance->Free();
    }

    return pInstance;
}

void CSound_Manager::Free()
{
    for (auto& kv : m_mapSound)
    {
        FMOD_Sound_Release(kv.second);
    }
    m_mapSound.clear();

    if (m_pSystem)
    {
        FMOD_System_Close(m_pSystem);
        FMOD_System_Release(m_pSystem);
        m_pSystem = nullptr;
    }
}