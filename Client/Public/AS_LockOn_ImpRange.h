#pragma once
#include "AI_State.h"
#include "Client_Defines.h"

NS_BEGIN(Client)

class CAS_LockOn_ImpRange : public CAI_State
{
private:
    CAS_LockOn_ImpRange();
    virtual ~CAS_LockOn_ImpRange() = default;

public:
    virtual void				Enter(class CStateMachine* pFSM, class CGameObject* pOwner = nullptr) override;
    virtual void				Update(class CStateMachine* pFSM, class CGameObject* pOwner, _float fTimeDelta) override;
    virtual void				Exit(class CStateMachine* pFSM, class CGameObject* pOwner) override;

private:
    _bool                       m_isRetreat = { false };
    _bool                       m_isCircleRight = { false };
    _float3                     m_vMoveDir = {};
public:
    static CAS_LockOn_ImpRange* Create();
    virtual void				Free() override;

};

NS_END