#pragma once
#include "AI_State.h"

class CIdleState_Yetuga : public CAI_State
{
private:
	CIdleState_Yetuga();
	virtual ~CIdleState_Yetuga() = default;

public:
	virtual void				Enter(class CStateMachine* pFSM, class CGameObject* pOwner = nullptr) override;
	virtual void				Update(class CStateMachine* pFSM, class CGameObject* pOwner, _float fTimeDelta) override;
	virtual void				Exit(class CStateMachine* pFSM, class CGameObject* pOwner) override;

private:


public:
	static CIdleState_Yetuga*	Create();
	virtual void				Free() override;

};

