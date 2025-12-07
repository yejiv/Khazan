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
        _bool               isLockOn = {};
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

    /* Injured*/
    _bool   Try_InjuredAnimaition(SPEAR_MOVE moveInfo);

    /* Moving*/
	_bool	Try_ChangeAnimation(SPEAR_MOVE moveInfo);
	void	Reserve_Animation(SPEAR_MOVE moveInfo);

    /* turn */
    _bool   Try_Turn180Animation(SPEAR_MOVE moveInfo);

    /* Getter*/
    _bool	IsMoving() const { return m_isMoving; }
    _bool	IsDodgeing() const { return m_isDodging; }
    _bool	IsReserve() const { return m_isReserve; }
    _bool   IsTurning180() const { return m_isTurning180; }
    _bool	IsEndMoveAnimantionFinished() const { return m_isEndAnimationFinished; }


    /* Clear */
    void    Clear_Reserve() { m_isReserve = false; m_eReserveInfo = {}; }
    void    Clear_Turn180() { m_isTurning180 = false; }

private:
    class CClientInstance* m_pClientInstance = { nullptr };
    PLAYER_DATA* m_pPlayerData = { nullptr };
    SPEAR_MOVE	m_eReserveInfo = {};
	FOOT_INFO	m_FootInfo[GetBitPosition(MOV::MOVE_END)]; // walk, run ,  Injured
	FOOT		m_curFoot;
	_uint		m_iSubState = {};
	_uint		m_iFootPosition[2] = { 0,0 }; // l,r 몇 번째 프레임
    _uint       m_iPrevSelectedAnimationIndex = {};

	_bool		m_isEndAnimationFinished = { false };
	_bool		m_isMoving = { false };
	_bool		m_isDodging = { false };
	_bool		m_isReserve = { false };
    _bool       m_isTurning180 = { false };

    static const string s_strSprinStartAnims[];
    static const string s_strDodgeAnims[];

private:
    _uint       GetLockOnWalkAnimation(DIRECTION_INFO eDir);
    _uint       GetLockOnRunAnimation(DIRECTION_INFO eDir);

public:
	static CKhazan_Spear_Anim_Move* Create();
	virtual void Free() override;
};

NS_END