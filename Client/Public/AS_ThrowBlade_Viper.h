#pragma once
#include "AI_State.h"
#include "Client_Defines.h"

NS_BEGIN(Client)

class CAS_ThrowBlade_Viper final : public CAI_State
{
private:
    CAS_ThrowBlade_Viper();
    virtual ~CAS_ThrowBlade_Viper() = default;

public:
    virtual void Enter(class CStateMachine* pFSM, class CGameObject* pOwner = nullptr) override;
    virtual void Update(class CStateMachine* pFSM, class CGameObject* pOwner, _float fTimeDelta) override;
    virtual void Exit(class CStateMachine* pFSM, class CGameObject* pOwner) override;


public:
    static CAS_ThrowBlade_Viper*    Create();
    virtual void                    Free() override;

};

NS_END