#include "Timer_Manager.h"

CTimer_Manager::CTimer_Manager()
{

}

_float CTimer_Manager::Get_TimeDelta(const _wstring& strTimerTag)
{
	CTimer* pTimer = Find_Timer(strTimerTag);
	if (nullptr == pTimer)
		return 0.f;

	return pTimer->Get_TimeDelta();
}

_float CTimer_Manager::Get_ScaledDelta(const _wstring& strTimerTag, TIME_CHANNEL eCH)
{
	_float fTimeDelta = Get_TimeDelta(strTimerTag);
	
	return fTimeDelta * m_ChannelScale[(int)eCH];
}

void CTimer_Manager::Compute_TimeDelta(const _wstring& strTimerTag)
{
	CTimer* pTimer = Find_Timer(strTimerTag);
	if (nullptr == pTimer)
		return;

	pTimer->Update_Timer();
}

void CTimer_Manager::Update_HitStop(_float fUnScaleTimeDelta)
{
	for (int i = 0; i < (int)TIME_CHANNEL::END; i++)
	{
		auto& HitStop = m_tHitStop[i];
		if (!HitStop.isActive)
		{
			m_ChannelScale[i] = 1.f;
			continue;
		}

		if (HitStop.isFix)
		{
			continue;
		}

		HitStop.fElapsed += fUnScaleTimeDelta;

		if (HitStop.fElapsed < HitStop.fHold)
		{
			m_ChannelScale[i] = HitStop.fTargetScale;
		}
		else if (HitStop.fElapsed < HitStop.fHold + HitStop.fRecover)
		{
			_float t = (HitStop.fElapsed - HitStop.fHold) / HitStop.fRecover;
			_float u = t * t * (3.f - 2.f * t);
			m_ChannelScale[i] = HitStop.fTargetScale + (1.f - HitStop.fTargetScale) * u;
		}
		else
		{
			HitStop = HitStopState{};
			m_ChannelScale[i] = 1.f;
		}
			
	}
}

void CTimer_Manager::Start_HitStop(TIME_CHANNEL eCH, _float fTargetScale, _float fHold, _float fRecover)
{
	auto& HitStop = m_tHitStop[(int)eCH];
	HitStop.isActive = true;
	HitStop.fHold = max(0.f, fHold);
	HitStop.fRecover = max(0.f, fRecover);
	HitStop.fElapsed = 0.f;
	HitStop.fCurScale = 1.f;
}

void CTimer_Manager::Fix_HitStop(TIME_CHANNEL eCH)
{
	m_tHitStop[(int)eCH].isActive = true;
	m_tHitStop[(int)eCH].isFix = true;
	m_ChannelScale[(int)eCH] = 0.00000000001f;
}

void CTimer_Manager::UnFix_HitStop(TIME_CHANNEL eCH)
{
	m_tHitStop[(int)eCH].isActive = false;
	m_tHitStop[(int)eCH].isFix = false;
	m_ChannelScale[(int)eCH] = 1.f;
}

HRESULT CTimer_Manager::Add_Timer(const _wstring& strTimerTag)
{
	CTimer* pTimer = Find_Timer(strTimerTag);

	if (nullptr != pTimer)
		return E_FAIL;

	pTimer = CTimer::Create();
	if (nullptr == pTimer)
		return E_FAIL;

	/*m_Timers.insert({ strTimerTag, pTimer });*/

	m_Timers.emplace(strTimerTag, pTimer);

	return S_OK;
}

CTimer* CTimer_Manager::Find_Timer(const _wstring& strTimerTag)
{
	/*auto		iter = find_if(m_mapTimer.NS_BEGIN(), m_mapTimer.end(), CTag_Finder(pTimerTag));*/
	auto		iter = m_Timers.find(strTimerTag);

	if (iter == m_Timers.end())
		return nullptr;

	return iter->second;
}

CTimer_Manager* CTimer_Manager::Create()
{
	return new CTimer_Manager();
}

void CTimer_Manager::Free()
{
	__super::Free();

	for (auto& Pair : m_Timers)	
		Safe_Release(Pair.second);

	m_Timers.clear();
	
}
