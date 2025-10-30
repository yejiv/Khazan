#pragma once
#include "AI_State.h"

class CMoveState_Yetuga : public CAI_State
{
private:
	CMoveState_Yetuga();
	virtual ~CMoveState_Yetuga() = default;

public:
	virtual void				Enter(class CStateMachine* pFSM, class CGameObject* pOwner = nullptr) override;
	virtual void				Update(class CStateMachine* pFSM, class CGameObject* pOwner, _float fTimeDelta) override;
	virtual void				Exit(class CStateMachine* pFSM, class CGameObject* pOwner) override;

private:
	_float						m_fSpeedPerSec = {};
	_uint						m_iPrevMovementFlag = {};

public:
	static CMoveState_Yetuga*	Create();
	virtual void				Free() override;

};

