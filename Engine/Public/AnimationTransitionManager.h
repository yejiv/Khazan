#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CAnimationTransitionManager final : public CBase
{
private:
	CAnimationTransitionManager();
	virtual ~CAnimationTransitionManager() = default;
public:
	 inline _bool Check_Transition(
		const ANIM_TRANSITION& condition,
		_float					fCurrentTime,
		const map<_wstring, _bool>& flags
	);


private:
	_bool Check_TimeCondition(
		const ANIM_TRANSITION& condition,
		_float fCurrentTime
	);
	 _bool Check_FlagCondition(
		const ANIM_TRANSITION& condition,
		const map<_wstring, _bool>& flags
	);

	_bool Check_InputCondition(
		const ANIM_TRANSITION& condition
	);

private:
	class CGameInstance* m_pGameInstance = { nullptr };

public:
	static CAnimationTransitionManager* Create();
	virtual void Free()override;
};

NS_END
