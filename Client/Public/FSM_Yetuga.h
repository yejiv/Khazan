#pragma once
#include "StateMachine.h"
#include "Client_Defines.h"

NS_BEGIN(Client)

enum class YETUGA_STATE
{
	IDLE,
	MOVE,
	TURN,
	ATTACK,
	RIGHTHAND_5HIT,
	LIE_DOWN,
	SMASH,
	THROWBALL,
	JUMPATTACK,
	JUMPGRAB,
	RUSH,
	ICEBREATH,
	AMAGEDDON,
	LOCKON,
	HIT,
	GROGGY,
	DODGE,
	DEAD,
	SLEEP,
    CUTSCENE,
	END,
};


class CFSM_Yetuga final : public CStateMachine
{
private:
	CFSM_Yetuga();
	virtual ~CFSM_Yetuga() = default;

public:
    class CAS_CutScene_Yetuga* Get_CutScene_Yetuga();
    class CAS_RightHand_5Hit_Yetuga* Get_RH_5Hit();

public:
	virtual HRESULT				Initialize();
	virtual void				Update(class CGameObject* pOwner, _float fTimeDelta) override;

public:
	static CFSM_Yetuga*			Create();
	virtual void				Free() override;
};

NS_END