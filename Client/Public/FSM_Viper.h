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
    CUTSCENE_START,
    CUTSCENE_PHASE2,

    P2_RUN,
    P2_HANDSTOMP, // 한손 찍기
    P2_HANDSTOMPSTR, // 한손 늦게 찍기
    P2_HANDSWING2HIT, // 손 두번 휘두루기
    P2_HANDUPPER, // 어퍼컷
    P2_SLASHDOUBLE, // 칼 두번 휘두루기
    P2_SLASHSTOMP, // 칼 2번 휘두루고 찍기
    P2_HANDSWING3HIT, // 이거 버림
    P2_FAKERUNATTACK, // 근접 통과 점프 공격
    P2_DASHUPPER, // 손쓸면서 달려와서 어퍼컷
    P2_DASTUPPERSTR, // 위에 거 조금 느리게
    P2_BACKJUMP, // 뒤로 닷지 이거 애니메이션 두개인데 스테이트 들어가서 하나씩 추가? // 불러요
    P2_SIDEMOVE, // 옆으로 닷지
    P2_JUMPATTACK, // 불러요
    P2_SWINGCOMBO, // 5연타 후 돌진
    P2_THROWROCK, // 돌던지기
    P2_DASHDRIFT, // 불러요
    P2_SWINGROUND, // 돌돌이
    P2_ROAR, // 소리지르기
    P2_LOCKON, // 락온
    P2_BERSERKERJUMP,
    P2_DEAD,
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