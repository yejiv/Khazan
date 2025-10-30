#pragma once
#include "StateMachine.h"

enum class YETUGA_STATE
{
	IDLE,
	MOVE,
	TURN,
	ATTACK,
	RIGHTHAND_5HIT,
	LIE_DOWN,
	THROWBALL,
	RUSH,
	HIT,
	DEAD,
	SLEEP,
	END,
};


class CFSM_Yetuga final : public CStateMachine
{
private:
	CFSM_Yetuga();
	virtual ~CFSM_Yetuga() = default;

public:
	virtual HRESULT				Initialize();
	virtual void				Update(class CGameObject* pOwner, _float fTimeDelta) override;

public:
	static CFSM_Yetuga*			Create();
	virtual void				Free() override;
};

