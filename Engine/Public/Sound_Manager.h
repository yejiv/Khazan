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
	// 한번만 재생될 경우 사용 될 함수 (pSoundKey : 해당 사운드 파일명, fVolume : 사운드 크기, ppOutChannel : 따로 채널을 받아서 관리할경우 해당 채널 받아올 포인터)
	void PlaySoundOnce(const TCHAR* pSoundKey, float fVolume = 1.0f, FMOD_CHANNEL** ppOutChannel = nullptr);
	// 반복 재생될 경우 사용 될 함수 (pSoundKey : 해당 사운드 파일명, fVolume : 사운드 크기, ppOutChannel : 따로 채널을 받아서 관리할경우 해당 채널 받아올 포인터)
	void PlaySoundLoop(const TCHAR* pSoundKey, float fVolume = 1.0f, FMOD_CHANNEL** ppOutChannel = nullptr);
	// 모든 사운드 정지
	void StopAll();

	// 해당 파일명으로 재생중인 사운드 정지
	void StopByKey(const TCHAR* pSoundKey);
	// 해당 파일명으로 재생중인 사운드가 재생중인지 확인
	bool IsPlayingByKey(const TCHAR* pSoundKey);
	// 해당 파일명으로 재생중인 사운드의 볼륨 조절
	void SetVolumeByKey(const TCHAR* pSoundKey, float fVolume);



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
};

NS_END