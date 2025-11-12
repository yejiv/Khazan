#pragma once
#include "Khazan_Spear_Animation.h"
#include "Client_Defines.h"

NS_BEGIN(Client)

class CKhazan_Spear_Anim_Guard : public CKhazan_Spear_Animation
{
private:
	CKhazan_Spear_Anim_Guard();
	virtual ~CKhazan_Spear_Anim_Guard() = default;

public:
	HRESULT			Initialize_Prototype();

public:
	void Enter() override;
	void Continue(_float fTimeDelta) override;
	void Exit() override;

public:
	_bool	Try_Guard();
	_bool	Try_SuccessGuard(_uint iHitDir);
	_bool	Try_WalkGuard(_uint iMoveDir);
	_bool	Try_JustGuard(_uint	m_iHitDir);
	_bool	Play_FinishGuard();
	
    _bool*  Get_IsGuarding() { return &m_isGuarding; }
	_bool	Is_Guarding() const { return m_isGuarding; }
	_bool	Is_WalkGuarding() const { return m_isWalkGuarding; }
	_bool	Is_JustGuarding() const { return m_isJustGuarding; }
	
private:
	_bool	m_isGuarding = { false };

	_bool	m_isWalkGuarding = { false };
	_bool	m_isJustGuarding = { false };

	_bool	m_isGuardStart = { false };
	_bool	m_isWalkGuardStart = { false };
	_bool	m_isGuardSuccess = { false };
	_bool	m_isFinishedGuard = { false };


	_uint	m_iHitDir = {};
	_uint	m_iMoveDir = {};


public:
	static CKhazan_Spear_Anim_Guard* Create();
	virtual void Free() override;
};

NS_END