#pragma once
#include "AI_State.h"
#include "Client_Defines.h"

NS_BEGIN(Client)

enum LOCKONDIR { LOCKON_LEFT, LOCKON_RIGHT, END };


class CAS_LockOn_Yetuga final : public CAI_State
{
private:
	CAS_LockOn_Yetuga();
	virtual ~CAS_LockOn_Yetuga() = default;

public:
	virtual void Enter(class CStateMachine* pFSM, class CGameObject* pOwner = nullptr) override;
	virtual void Update(class CStateMachine* pFSM, class CGameObject* pOwner, _float fTimeDelta) override;
	virtual void Exit(class CStateMachine* pFSM, class CGameObject* pOwner) override;

private:
	_float						m_fDotThreshold = {};
	_float						m_fMoveSpeed = {};

    LOCKONDIR                   m_eLockOnDir = { LOCKONDIR::END };


public:
	static CAS_LockOn_Yetuga*	Create();
	virtual void				Free() override;

};

NS_END