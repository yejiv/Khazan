#pragma once
#include "StateMachine.h"

enum class YETUGA_STATE
{
	IDLE,
	MOVE,
	ATTACK,
	RUSH,
	HIT,
	DEAD,
	SLEEP,
};


class CFSM_Yetuga final : public CStateMachine
{
private:
	CFSM_Yetuga();
	virtual ~CFSM_Yetuga() = default;

public:
	virtual HRESULT				Initialize();
	virtual void				Update(_float fTimeDelta, class CGameObject* pOwner) override;

public:
	static CFSM_Yetuga*			Create();
	virtual void				Free() override;
};

