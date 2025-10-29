#pragma once
#include "Client_Defines.h"
#include "AI_State.h"

NS_BEGIN(Client)

class CAS_Turn_Yetuga final : public CAI_State
{
private:
	CAS_Turn_Yetuga();
	virtual ~CAS_Turn_Yetuga() = default;

public:
	virtual void Enter(class CStateMachine* pFSM, class CGameObject* pOwner = nullptr) override;
	virtual void Update(class CStateMachine* pFSM, class CGameObject* pOwner, _float fTimeDelta) override;
	virtual void Exit(class CStateMachine* pFSM, class CGameObject* pOwner) override;


public:
	static CAS_Turn_Yetuga* Create();
	virtual void			Free() override;
};

NS_END