#pragma once
#include "AI_State.h"

class CAttackState_Yetuga : public CAI_State
{
private:
	CAttackState_Yetuga();
	virtual ~CAttackState_Yetuga() = default;

public:
	virtual void				Enter(class CStateMachine* pFSM, class CGameObject* pOwner = nullptr) override;
	virtual void				Update(class CStateMachine* pFSM, class CGameObject* pOwner, _float fTimeDelta) override;
	virtual void				Exit(class CStateMachine* pFSM, class CGameObject* pOwner) override;

public:
	virtual void				OnCollision(COLLISION_DESC* pDesc) override;

public:
	static CAttackState_Yetuga* Create();
	virtual void				Free() override;

};

