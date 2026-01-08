#pragma once
#include "Engine_Defines.h"
#include "Base.h"

NS_BEGIN(Engine)

#define MAXCHANNEL 64

class CSound_Manager final : public CBase
{
private:
	CSound_Manager();
	~CSound_Manager() = default;

public:
	HRESULT Initialize();
	void Update();

public:
    _float Get_Gloval_Volume() { return m_fGloval_Volume; }
    void Set_Gloval_Volume(_float fVolume) { m_fGloval_Volume = fVolume; }
    void ADD_Gloval_Volume(_float fVolume) { m_fGloval_Volume += fVolume;
    m_fGloval_Volume >= 1.f ? m_fGloval_Volume = 1.f : m_fGloval_Volume <= 0.f ? m_fGloval_Volume = 0.f : m_fGloval_Volume; }

    //플레이어 중점 사운드
    void ListenerPosSet(_vector vPos, _vector vLook, _vector vUp, _float3 vVal = { 0.f, 0.f, 0.f});

	// 한번만 재생될 경우 사용 될 함수 (pSoundKey : 해당 사운드 파일명, fVolume : 사운드 크기, ppOutChannel : 따로 채널을 받아서 관리할경우 해당 채널 받아올 포인터)
	void PlaySoundOnce(const TCHAR* pSoundKey, float fVolume = 1.0f, FMOD_CHANNEL** ppOutChannel = nullptr);
    void PlaySoundOnce(const TCHAR* pSoundKey, _vector vPos, _float3 vVel = { 0.f, 0.f, 0.f }, float fVolume = 1.0f, FMOD_CHANNEL** ppOutChannel = nullptr, _float2 vDis = { 1.f, 25.f });
    void PlaySoundOnce(const TCHAR* pSoundKey, _vector vPos, FMOD_CHANNEL** ppOutChannel, float fVolume = 1.0f, _float2 vDis = { 1.f, 25.f });

    // 반복 재생될 경우 사용 될 함수 (pSoundKey : 해당 사운드 파일명, fVolume : 사운드 크기, ppOutChannel : 따로 채널을 받아서 관리할경우 해당 채널 받아올 포인터)
    void PlaySoundLoop(const TCHAR* pSoundKey, float fVolume = 1.0f, FMOD_CHANNEL** ppOutChannel = nullptr);
    void PlaySoundLoop(const TCHAR* pSoundKey, _vector vPos, _float3 vVel = { 0.f, 0.f, 0.f }, float fVolume = 1.0f, FMOD_CHANNEL** ppOutChannel = nullptr, _float2 vDis = { 1.f, 25.f });
    void PlaySoundLoop(const TCHAR* pSoundKey, _vector vPos, FMOD_CHANNEL** ppOutChannel, float fVolume = 1.0f, _float2 vDis = { 1.f, 25.f });

    // 모든 사운드 정지
	void StopAll();

	// 해당 파일명으로 재생중인 사운드 정지
	void StopByKey(const TCHAR* pSoundKey);
    void StopByChannel(FMOD_CHANNEL** ppOutChannel);

    // 해당 파일명, 파라미터로 서서히 Sound Fade In
    void PlaySoundOnce_FadeIn(const TCHAR* pSoundKey, float fVolume = 1.f, float fFadeTime = 1.f, FMOD_CHANNEL** ppOutChannel = nullptr);
    void PlaySoundLoop_FadeIn(const TCHAR* pSoundKey, float fVolume = 1.f, float fFadeTime = 1.f, FMOD_CHANNEL** ppOutChannel = nullptr);

    // 해당 파일명 + FadeTime으로 서서히 감소되고 0.f 가 되면 사운드 정지
    void StopByKey_FadeOut(const TCHAR* pSoundKey, float fFadeTime);

    // 해당 파일명으로 재생중인 사운드가 재생중인지 확인
	bool IsPlayingByKey(const TCHAR* pSoundKey);
	// 해당 파일명으로 재생중인 사운드의 볼륨 조절
	void SetVolumeByKey(const TCHAR* pSoundKey, float fVolume);

    void Resume(const TCHAR* pSoundKey);
    void Resume_Fade(const TCHAR* pSoundKey, float fFadeTime);
    void Pause(const TCHAR* pSoundKey);
    void Pause_Fade(const TCHAR* pSoundKey, float fFadeTime);

private:
	void LoadSoundFile();
	bool FindSound(const TCHAR* pSoundKey, FMOD_SOUND** ppOutSound);
	void ForEachPlayingChannel(std::function<void(FMOD_CHANNEL*)> fn);

public:
	static CSound_Manager* Create();
	virtual void Free() override;

private:
	FMOD_SYSTEM* m_pSystem = nullptr;
	int          m_iMaxChannels = 64;
	map<wstring, FMOD_SOUND*> m_mapSound;
    _float       m_fGloval_Volume;
    
    //채널 그룹
    FMOD_CHANNELGROUP* m_pGroup2D = nullptr;
    FMOD_CHANNELGROUP* m_pGroup3D = nullptr;
    FMOD_CHANNELGROUP* pMaster = nullptr;
};

NS_END