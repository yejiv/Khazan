#pragma once
#include "AI_State.h"

class CAS_Dr_Melee_Brutal : public CAI_State
{
private:
    CAS_Dr_Melee_Brutal();
    virtual ~CAS_Dr_Melee_Brutal() = default;

public:
    virtual void				Enter(class CStateMachine* pFSM, class CGameObject* pOwner = nullptr) override;
    virtual void				Update(class CStateMachine* pFSM, class CGameObject* pOwner, _float fTimeDelta) override;
    virtual void				Exit(class CStateMachine* pFSM, class CGameObject* pOwner) override;

public:
    static CAS_Dr_Melee_Brutal* Create();
    virtual void				Free() override;

};

