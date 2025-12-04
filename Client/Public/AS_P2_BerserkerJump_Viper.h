#pragma once
#include "AI_State.h"
#include "Client_Defines.h"

NS_BEGIN(Client)


enum class BSJUMPSTATE { SIDEJUMP, FRONTJUMP, ATTACK, END };

class CAS_P2_BerserkerJump_Viper final : public CAI_State
{
private:
    CAS_P2_BerserkerJump_Viper();
    virtual ~CAS_P2_BerserkerJump_Viper() = default;

public:
    virtual void Enter(class CStateMachine* pFSM, class CGameObject* pOwner = nullptr) override;
    virtual void Update(class CStateMachine* pFSM, class CGameObject* pOwner, _float fTimeDelta) override;
    virtual void Exit(class CStateMachine* pFSM, class CGameObject* pOwner) override;

    BSJUMPSTATE                         m_eState = { BSJUMPSTATE::END };

    _uint                               m_iJumpCnt = {};
    _float                              m_fMoveSpeed = {};

public:
    static CAS_P2_BerserkerJump_Viper*  Create();
    virtual void                        Free() override;

};

NS_END