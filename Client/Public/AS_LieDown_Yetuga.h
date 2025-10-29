#pragma once
#include "AI_State.h"

NS_BEGIN(Client)

class CAS_LieDown_Yetuga : public CAI_State
{
private:
	CAS_LieDown_Yetuga();
	virtual ~CAS_LieDown_Yetuga() = default;

public:
	virtual void Enter(class CStateMachine* pFSM, class CGameObject* pOwner = nullptr);
	virtual void Update(class CStateMachine* pFSM, class CGameObject* pOwner, _float fTimeDelta);
	virtual void Exit(class CStateMachine* pFSM, class CGameObject* pOwner);


public:
	static CAS_LieDown_Yetuga*			Create();
	virtual void						Free() override;

};

NS_END