#pragma once
#include "AI_State.h"
#include "Client_Defines.h"

NS_BEGIN(Client)

class CAS_SlashBackJump_Viper final : public CAI_State
{
private:
    CAS_SlashBackJump_Viper();
    virtual ~CAS_SlashBackJump_Viper() = default;

public:
    virtual void Enter(class CStateMachine* pFSM, class CGameObject* pOwner = nullptr) override;
    virtual void Update(class CStateMachine* pFSM, class CGameObject* pOwner, _float fTimeDelta) override;
    virtual void Exit(class CStateMachine* pFSM, class CGameObject* pOwner) override;

public:
    virtual void OnCollision(COLLISION_DESC* pDesc, _uint iCollisionLayer, class CGameObject* pOwner = nullptr) override;


private:
    _float3                         m_vStartPos = {};
    _float3                         m_vGoalPos = {};

public:
    static CAS_SlashBackJump_Viper* Create();
    virtual void                    Free() override;

};

NS_END