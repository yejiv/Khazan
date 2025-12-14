#pragma once
#include "AI_State.h"
#include "Client_Defines.h"

NS_BEGIN(Client)

class CAS_5HitCombo_Viper final : public CAI_State
{
private:
    CAS_5HitCombo_Viper();
    virtual ~CAS_5HitCombo_Viper() = default;

public:
    virtual void Enter(class CStateMachine* pFSM, class CGameObject* pOwner = nullptr) override;
    virtual void Update(class CStateMachine* pFSM, class CGameObject* pOwner, _float fTimeDelta) override;
    virtual void Exit(class CStateMachine* pFSM, class CGameObject* pOwner) override;

public:
    virtual void OnCollision(COLLISION_DESC* pDesc, _uint iCollisionLayer, class CGameObject* pOwner = nullptr) override;

public:
    void         On_JustGuard(class CGameObject* pOwner);



public:
    static CAS_5HitCombo_Viper*     Create();
    virtual void                    Free() override;

};

NS_END