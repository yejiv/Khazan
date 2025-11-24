#pragma once
#include "AI_State.h"
#include "Client_Defines.h"

NS_BEGIN(Client)

class CAS_Slow2Hit_VIper final : public CAI_State
{
private:
    CAS_Slow2Hit_VIper();
    virtual ~CAS_Slow2Hit_VIper() = default;

public:
    virtual void Enter(class CStateMachine* pFSM, class CGameObject* pOwner = nullptr) override;
    virtual void Update(class CStateMachine* pFSM, class CGameObject* pOwner, _float fTimeDelta) override;
    virtual void Exit(class CStateMachine* pFSM, class CGameObject* pOwner) override;


public:
    static CAS_Slow2Hit_VIper*  Create();
    virtual void                Free() override;

};

NS_END