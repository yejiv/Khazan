#pragma once
#include "AI_State.h"
#include "Client_Defines.h"

NS_BEGIN(Client)

enum class P1SIDEMOVESTATE { MOVE, ATTACK, END };


class CAS_SideMove_P1_Viper final : public CAI_State
{
private:
    CAS_SideMove_P1_Viper();
    virtual ~CAS_SideMove_P1_Viper() = default;

public:
    virtual void Enter(class CStateMachine* pFSM, class CGameObject* pOwner = nullptr) override;
    virtual void Update(class CStateMachine* pFSM, class CGameObject* pOwner, _float fTimeDelta) override;
    virtual void Exit(class CStateMachine* pFSM, class CGameObject* pOwner) override;


private:
    P1SIDEMOVESTATE                         m_eState = { P1SIDEMOVESTATE::END };


public:
    static CAS_SideMove_P1_Viper*           Create();
    virtual void                            Free() override;

};

NS_END