#pragma once
#include "AI_State.h"
#include "Client_Defines.h"

NS_BEGIN(Client)

enum class FAKERUNATTACKSTATE {RUN, ATTACK, END };


class CAS_P2_FakeRunAttack_Viper final : public CAI_State
{
private:
    CAS_P2_FakeRunAttack_Viper();
    virtual ~CAS_P2_FakeRunAttack_Viper() = default;

public:
    virtual void Enter(class CStateMachine* pFSM, class CGameObject* pOwner = nullptr) override;
    virtual void Update(class CStateMachine* pFSM, class CGameObject* pOwner, _float fTimeDelta) override;
    virtual void Exit(class CStateMachine* pFSM, class CGameObject* pOwner) override;

private:
    _float                              m_fSpeed = {};
    FAKERUNATTACKSTATE                  m_eState = { FAKERUNATTACKSTATE::END };
    _float                              m_fAnimSpeed = {};

public:
    static CAS_P2_FakeRunAttack_Viper*  Create();
    virtual void                        Free() override;

};

NS_END