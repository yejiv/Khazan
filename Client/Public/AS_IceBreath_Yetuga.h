#pragma once
#include "AI_State.h"
#include "Client_Defines.h"

enum class PHASE { START, ROOP, END, };


NS_BEGIN(Client)

class CAS_IceBreath_Yetuga final : public CAI_State
{
private:
	CAS_IceBreath_Yetuga();
	virtual ~CAS_IceBreath_Yetuga() = default;

public:
	virtual void Enter(class CStateMachine* pFSM, class CGameObject* pOwner = nullptr) override;
	virtual void Update(class CStateMachine* pFSM, class CGameObject* pOwner, _float fTimeDelta) override;
	virtual void Exit(class CStateMachine* pFSM, class CGameObject* pOwner) override;

private:
	PHASE							m_eState = { PHASE::END };
    _float                          m_fIceBreathTimeAcc = {};
    _float                          m_fIceBreathSoundInterval = 0.3f;

    _uint                           m_iIceBreathSoundChannelIndex = 0;
    vector<FMOD_CHANNEL**>          m_IceBreathChannels;

public:
	static CAS_IceBreath_Yetuga*	Create();
	virtual void					Free() override;
};

NS_END