#pragma once
#include "AI_State.h"

class CAS_Move_Gomdol : public CAI_State
{
private:
	CAS_Move_Gomdol();
	virtual ~CAS_Move_Gomdol() = default;

public:
	virtual void				Enter(class CStateMachine* pFSM, class CGameObject* pOwner = nullptr) override;
	virtual void				Update(class CStateMachine* pFSM, class CGameObject* pOwner, _float fTimeDelta) override;
	virtual void				Exit(class CStateMachine* pFSM, class CGameObject* pOwner) override;


private:
	void						Start_Decel();

private:
	_float						m_fSpeedPerSec = {};
	_uint						m_iPrevMovementFlag = {};
	_float						m_AnimationSpeed = {1.f};

    _bool                       m_isFirstFrame = { false };

public:
	static CAS_Move_Gomdol*		Create();
	virtual void				Free() override;

};

