#pragma once
#include "StateMachine.h"

enum class VIPER_STATE_P1
{
    IDLE,
    WALK,
    RUN,
    TURN,
    QUICK2HIT,
    SLOW2HIT,
    STINGSLASHCOMBO,
    THROWBLADE,
    LOCKON,
    SLOW3HIT,
    SLASHBACKJUMP,
    TURNATTACK,
    JUMPSMASH,
    DIVOUR,
    SIDEMOVE,
    COMBO5HIT,
    HIT,
    GROGGY,
    DODGE,
    DEAD,
    CUTSCENE,
    END,
};


class CFSM_Viper final : public CStateMachine
{
private:
    CFSM_Viper();
    virtual ~CFSM_Viper() = default;

public:
    virtual HRESULT				Initialize();
    virtual void				Update(class CGameObject* pOwner, _float fTimeDelta) override;

public:
    static CFSM_Viper*          Create();
    virtual void				Free() override;
};

