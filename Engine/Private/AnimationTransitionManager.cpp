#include "AnimationTransitionManager.h"
#include "GameInstance.h"

CAnimationTransitionManager::CAnimationTransitionManager()
	: m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

inline _bool CAnimationTransitionManager::Check_Transition(const ANIM_TRANSITION& condition, _float fCurrentTime, const map<_wstring, _bool>& flags)
{
	/* 1. 시간 체크 */
	if (!Check_TimeCondition(condition, fCurrentTime))
		return false;

	/* 타입별 체크 */

	switch (condition.type)
	{
	case ANIM_TRANSITIONTYPE::AUTO:		//시간 조건만 없으면 ok
		return true;
		
	case ANIM_TRANSITIONTYPE::FLAG:
		return Check_FlagCondition(condition, flags);
		
	case ANIM_TRANSITIONTYPE::INPUT:
		return Check_InputCondition(condition);

	case ANIM_TRANSITIONTYPE::MANUAL:	//클라에서 트리거로 전환시켜주기
		return false;
	}

	return false;
}

_bool CAnimationTransitionManager::Check_TimeCondition(const ANIM_TRANSITION& condition, _float fCurrentTime)
{
	return condition.fMinPlayTime <= fCurrentTime && fCurrentTime <= condition.fMaxPlayTime;
}

_bool CAnimationTransitionManager::Check_FlagCondition(const ANIM_TRANSITION& condition, const map<_wstring, _bool>& flags)
{
	auto it = flags.find(condition.strflagName);
	if (it == flags.end())
		return false;

	return true;
}

_bool CAnimationTransitionManager::Check_InputCondition(const ANIM_TRANSITION& condition)
{
	switch (condition.iInputType)
	{
	case 0:
		return m_pGameInstance->Key_Down(condition.iKeyCode);
	case 1:
		return m_pGameInstance->Key_Pressing(condition.iKeyCode,0.f);
	case 2:
		return m_pGameInstance->Key_Up(condition.iKeyCode);
	default:
		break;
	}

	return false;
}

CAnimationTransitionManager* CAnimationTransitionManager::Create()
{
	return  new CAnimationTransitionManager;
}

void CAnimationTransitionManager::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
}
