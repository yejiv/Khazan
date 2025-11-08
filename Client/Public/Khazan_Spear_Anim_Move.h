#pragma once
#include "Client_Defines.h"
#include "Khazan_Spear_Animation.h"

NS_BEGIN(Client)

class CKhazan_Spear_Anim_Move final : public CKhazan_Spear_Animation
{
public:
	typedef struct	tagKhazanSpearAnimMove
	{
		_bool				isEquipWeapon = {};
		_uint				iSubState;
		_uint				iCycle;
		DIRECTION_INFO		eDir;
	}SPEAR_MOVE;

	typedef struct	tagKhazanSpearFootSpearInfo
	{
		vector<_float>	vLeftFootFrames;	//왼발이 땅에 닿는 프레임
		vector<_float>	vRightFootFrames;	//오른발이 땅에 닿는 프레임
		_float			fAnimDuration = {};	//애니메이션 전체 프레임 길이
	}FOOT_INFO;

private:
	enum FOOT{ FOOT_R, FOOT_L, FOOT_END};

private:
	CKhazan_Spear_Anim_Move();
	virtual ~CKhazan_Spear_Anim_Move() = default;

public:
	HRESULT			Initialize_Prototype();
	
public:
	void Enter() override;
	void Continue(_float fTimeDelta) override;
	void Exit() override;

public:
	_bool	Try_ChangeAnimation(SPEAR_MOVE moveInfo);
	_bool	m_isEndMoveAnimantionFinished() const { return m_isEndAnimationFinished; }


private:
	FOOT_INFO	m_FootInfo[3]; // walk, run ,  sprint
	FOOT		m_curFoot;
	_uint		m_iSubState = {};
	_uint		m_iFootPosition[2] = { 0,0 }; // l,r 몇 번째 프레임
	//const _float	m_fMinTrackPostion = { 20.f };

	//_bool		m_isRun_Turn = { false };
	//_bool		m_isEquipSpear = { false }; /* 180도 회전시 사용  */

	_bool		m_isEndAnimationFinished = { false };

	_bool		m_isStopRunAnim = { false };
	_bool		m_isStopWalkAnim = { false };



public:
	static CKhazan_Spear_Anim_Move* Create();
	virtual void Free() override;
};

NS_END