#pragma once
#include "Client_Defines.h"
#include "Base.h"
#include "Khazan_Spear_ASMachine.h"

NS_BEGIN(Engine)

class CModel;
NS_END


NS_BEGIN(Client)

using WEA = CKhazan_Spear_ASMachine::WEAPON;
using CAT = CKhazan_Spear_ASMachine::CATEGORY;
using ATT = CKhazan_Spear_ASMachine::ATTACK;
using SKI = CKhazan_Spear_ASMachine::SKILL;
using MOV = CKhazan_Spear_ASMachine::MOVE;
using MOV_S = CKhazan_Spear_ASMachine::MOVESUB;
using CYC = CKhazan_Spear_ASMachine::CYCLE;
using GUA = CKhazan_Spear_ASMachine::GUARD;
using GRO = CKhazan_Spear_ASMachine::GROGGY;
using INTE = CKhazan_Spear_ASMachine::INTERACT;
using WEA_C = CKhazan_Spear_ASMachine::WEAPONCHANGE;
using HOL = CKhazan_Spear_ASMachine::HOLD;
using DAM = CKhazan_Spear_ASMachine::DAMAGED;


class CKhazan_Spear_Animation abstract : public CBase
{
protected:
	CKhazan_Spear_Animation();
	virtual ~CKhazan_Spear_Animation() = default;

public:
	virtual void Enter() = 0;
	virtual void Continue(_float fTimeDelta) = 0;
	virtual void Exit() = 0;

public:
	_uint    Get_AnimationIndex() { return m_iSelectedAnimationIndex; }
	void    Set_Model(CModel* pModel) { m_pModel = pModel; Safe_AddRef(m_pModel); }
	virtual _bool Is_Finished() { return m_bFinished; }


protected:
	CModel*				m_pModel = { nullptr };
	_uint				m_iState = {};
	_uint				m_iSelectedAnimationIndex = {};
	_bool				m_bFinished = { false };

protected:
	inline void		Add_State(_uint i) { m_iState |= i; }
	inline void		Toggle_State(_uint i) { m_iState ^= i; }
	inline void		Remove_State(_uint i) { m_iState &= ~i; }
	inline _bool	Has_State(_uint i) { return (m_iState & i) != 0; }
	inline _bool	Has_AllStates(_uint i) { return (m_iState & i) == i; }
	inline _bool	Has_States(_uint iMaxState);	//하나의 상태라도 있는지 없는지
	inline void		Clear_State() { m_iState = 0; }

public:
	virtual	void	Free() override;

};

NS_END