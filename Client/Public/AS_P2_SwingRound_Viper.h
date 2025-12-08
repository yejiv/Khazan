#pragma once
#include "AI_State.h"
#include "Client_Defines.h"

NS_BEGIN(Client)

class CAS_P2_SwingRound_Viper final : public CAI_State
{
private:
    CAS_P2_SwingRound_Viper();
    virtual ~CAS_P2_SwingRound_Viper() = default;

public:
    virtual void Enter(class CStateMachine* pFSM, class CGameObject* pOwner = nullptr) override;
    virtual void Update(class CStateMachine* pFSM, class CGameObject* pOwner, _float fTimeDelta) override;
    virtual void Exit(class CStateMachine* pFSM, class CGameObject* pOwner) override;

public:
    virtual void OnCollision(COLLISION_DESC* pDesc, _uint iCollisionLayer, class CGameObject* pOwner = nullptr) override;

private:
    void                                Set_ViperBerserkerStart_ShaderSettings();

public:
    static CAS_P2_SwingRound_Viper*     Create();
    virtual void                        Free() override;

};

NS_END