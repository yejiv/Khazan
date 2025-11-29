#pragma once
#include "AI_State.h"
#include "Client_Defines.h"

NS_BEGIN(Client)

class CAS_P2_LockOn_Viper final : public CAI_State
{
private:
    CAS_P2_LockOn_Viper();
    virtual ~CAS_P2_LockOn_Viper() = default;

public:
    virtual void Enter(class CStateMachine* pFSM, class CGameObject* pOwner = nullptr) override;
    virtual void Update(class CStateMachine* pFSM, class CGameObject* pOwner, _float fTimeDelta) override;
    virtual void Exit(class CStateMachine* pFSM, class CGameObject* pOwner) override;


private:
    DIRECTION                               m_eDirState = { DIRECTION::END };
    _float						            m_fDotThreshold = {};
    _float						            m_fMoveSpeed = {};

public:
    static CAS_P2_LockOn_Viper*             Create();
    virtual void                            Free() override;

};

NS_END