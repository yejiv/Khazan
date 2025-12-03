#pragma once
#include "AI_State.h"
#include "Client_Defines.h"

NS_BEGIN(Client)

enum class DRIFTSTATE { START, LOOP, FINISH, END };

class CAS_P2_DashDrift_Viper final : public CAI_State
{
private:
    CAS_P2_DashDrift_Viper();
    virtual ~CAS_P2_DashDrift_Viper() = default;

public:
    virtual void Enter(class CStateMachine* pFSM, class CGameObject* pOwner = nullptr) override;
    virtual void Update(class CStateMachine* pFSM, class CGameObject* pOwner, _float fTimeDelta) override;
    virtual void Exit(class CStateMachine* pFSM, class CGameObject* pOwner) override;


private:
    DRIFTSTATE                          m_eState = { DRIFTSTATE::END };
    _float                              m_fMoveSpeed = {};
    _float                              m_fAttackRange = {};
public:
    static CAS_P2_DashDrift_Viper*      Create();
    virtual void                        Free() override;

};

NS_END