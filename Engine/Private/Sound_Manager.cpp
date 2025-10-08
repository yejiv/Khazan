#include "EnginePch.h"
#include "Sound_Manager.h"

CSound_Manager::CSound_Manager() {}

HRESULT CSound_Manager::Initialize()
{
    FMOD_System_Create(&m_pSystem, FMOD_VERSION);
    FMOD_System_Init(m_pSystem, m_iMaxChannels, FMOD_INIT_NORMAL, nullptr);

    LoadSoundFile();
    return S_OK;
}

void CSound_Manager::Update()
{
    if (m_pSystem) FMOD_System_Update(m_pSystem);
}

void CSound_Manager::PlaySoundOnce(const TCHAR* pSoundKey, float fVolume, FMOD_CHANNEL** ppOutChannel)
{
    FMOD_SOUND* pSound = nullptr;
    if (!FindSound(pSoundKey, &pSound)) return;

    FMOD_CHANNEL* pCh = nullptr;
    FMOD_System_PlaySound(m_pSystem, pSound, nullptr, FALSE, &pCh);
    if (pCh)
    {
        FMOD_Channel_SetMode(pCh, FMOD_DEFAULT);
        FMOD_Channel_SetVolume(pCh, fVolume);
        if (ppOutChannel) *ppOutChannel = pCh;
    }
    FMOD_System_Update(m_pSystem);
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
        FMOD_Channel_SetVolume(pCh, fVolume);
        if (ppOutChannel) *ppOutChannel = pCh;
    }
    FMOD_System_Update(m_pSystem);
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
    FMOD_System_Update(m_pSystem);
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
                FMOD_Channel_SetVolume(ch, fVolume);
            }
        });
    FMOD_System_Update(m_pSystem);
}

void CSound_Manager::StopAll()
{
    FMOD_CHANNELGROUP* pMaster = nullptr;
    if (FMOD_System_GetMasterChannelGroup(m_pSystem, &pMaster) == FMOD_OK && pMaster)
    {
        FMOD_ChannelGroup_Stop(pMaster);
    }
    FMOD_System_Update(m_pSystem);
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

    FMOD_System_Update(m_pSystem);
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