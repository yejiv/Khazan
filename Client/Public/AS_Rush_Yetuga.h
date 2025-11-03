#pragma once
#include "AI_State.h"
#include "Client_Defines.h"

NS_BEGIN(Client)

class CAS_Rush_Yetuga final : public CAI_State
{
private:
	CAS_Rush_Yetuga();
	virtual ~CAS_Rush_Yetuga() = default;

public:
	virtual void Enter(class CStateMachine* pFSM, class CGameObject* pOwner = nullptr) override;
	virtual void Update(class CStateMachine* pFSM, class CGameObject* pOwner, _float fTimeDelta) override;
	virtual void Exit(class CStateMachine* pFSM, class CGameObject* pOwner) override;

private:
	_bool						m_isCrashed = { false };

public:
	static CAS_Rush_Yetuga*		Create();
	virtual void				Free() override;

};

NS_END