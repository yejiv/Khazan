#pragma once
#include "Client_Defines.h"
#include "Khazan_Spear_Animation.h"

NS_BEGIN(Client)

class CKhazan_Spear_Anim_Attack final :public CKhazan_Spear_Animation
{
public:
	typedef struct	tagKhazanSpearAnimAttack
	{
		_bool				isEquipWeapon = {};
		_uint				iSubState;
		_uint				iSkill;

	}SPEAR_ATTACK;

private:
	CKhazan_Spear_Anim_Attack();
	virtual ~CKhazan_Spear_Anim_Attack() = default;

public:
	HRESULT			Initialize_Prototype();

public:
	void Enter() override;
	void Continue(_float fTimeDelta) override;
	void Exit() override;

public:
	_bool	Try_FallAttack();
	_bool	Try_FastAttack();
	_bool	Try_GrappleAttack(); //브루탈 공격 
	_bool	Try_SingleSkillAttack(_uint iSkill);
	_bool	Try_CounterAttack();
	_bool	Try_DodgeAttack(_uint iDir);  //방향
	//_bool	Try_ReflectionAttack();
	_bool	Try_SprintFastAttack();
	_bool	Try_SprintStrongAttack();
	_bool	Try_StrongAttack();
	//_bool	Try_ChageFastAttack();
	_bool	Try_ChageStrongAttack();

	_bool	Is_Attacking() { return m_isAttacking; }
	_bool	Can_NextCombo() const { return m_isCanNextCombo; }
	_uint	Get_CurrentCombo() const { return m_iCurrentCombo; }


private:
	class CClientInstance* m_pClientInstance = { nullptr };

	_bool			m_isAttacking = { false };
	_bool			m_isCanNextCombo = { false };

	/* Combo */
	_uint			m_iCurrentCombo = { };
	_float			m_fComboTime = { 0.f };
	_bool			m_isFastCombo = { false };
	_bool			m_isStrongCombo = { false };
	_bool			m_isStrongCharge = { false };
	const _float	m_fFastAttackComboPossibleMaxFrame = { 30.f };
	const _float	m_fStrongAttackComboPossibleMaxFrame = { 30.f };

	/* Charge */

public:
	static CKhazan_Spear_Anim_Attack* Create();
	virtual void Free() override;


};


NS_END
