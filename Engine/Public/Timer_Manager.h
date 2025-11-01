#pragma once


#include "Timer.h"

NS_BEGIN(Engine)

class CTimer_Manager final : public CBase
{
private:
	CTimer_Manager();
	virtual ~CTimer_Manager() = default;

public:
	_float	Get_TimeDelta(const _wstring& strTimerTag);
	_float	Get_ScaledDelta(const _wstring& strTimerTag, TIME_CHANNEL eCH);
public:
	HRESULT	Add_Timer(const _wstring& strTimerTag);
	void	Compute_TimeDelta(const _wstring& strTimerTag);

public:
	void Update_HitStop(_float fUnScaleTimeDelta);
	void Start_HitStop(TIME_CHANNEL eCH, _float fTargetScale, _float fHold, _float fRecover);

private:		
	map<const _wstring, CTimer*>	m_Timers;

	_float m_ChannelScale[(int)TIME_CHANNEL::END] = { 1, 1, 1, 1, 1 };
	HitStopState m_tHitStop[(int)TIME_CHANNEL::END];

private:
	CTimer* Find_Timer(const _wstring& strTimerTag);

public:
	static CTimer_Manager* Create();
	virtual void Free();
};

NS_END