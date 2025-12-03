#pragma once
#include "StateMachine.h"
#include "Client_Defines.h"


NS_BEGIN(Client)

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
    STINGGRAB,
    HIT,
    GROGGY,
    DODGE,
    DEAD,
    CUTSCENE_START,
    CUTSCENE_PHASE2,

    P2_RUN,
    P2_HANDSTOMP,
    P2_HANDSTOMPSTR,
    P2_HANDSWING2HIT,
    P2_HANDUPPER,
    P2_SLASHDOUBLE,
    P2_SLASHSTOMP,
    P2_HANDSWING3HIT,
    P2_FAKERUNATTACK,
    P2_DASHUPPER,
    P2_DASTUPPERSTR,
    P2_BACKJUMP,
    P2_SIDEMOVE,
    P2_JUMPATTACK,
    P2_SWINGCOMBO,
    P2_THROWROCK,
    P2_DASHDRIFT,
    P2_SWINGROUND,
    P2_ROAR,
    P2_LOCKON,
    END,
};



class CFSM_Viper final : public CStateMachine
{
private:
    CFSM_Viper();
    virtual ~CFSM_Viper() = default;

public:
    class CAS_CutScene_Start_Viper* Get_CutScene_Start_Viper();
    class CAS_CutScene_2Phase_Viper* Get_Phase2_CutScene_Start_Viper();


public:
    virtual HRESULT				Initialize(class CGameObject* pOwner);
    virtual void				Update(class CGameObject* pOwner, _float fTimeDelta) override;

public:
    static CFSM_Viper*          Create(class CGameObject* pOwner);
    virtual void				Free() override;
};

NS_END