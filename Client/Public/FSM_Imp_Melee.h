#pragma once
#include "StateMachine.h"
#include "Client_Defines.h"

enum class IMPMELEE_STATE
{
    SLEEP,
    IDLE,
    MOVE,
    HIT2,
    HIT3,
    DASHATTACK,
    AMBUSH,
    DODGE,
    HIT,
    DEAD
};

NS_BEGIN(Engine)
class CGameObject;
NS_END

NS_BEGIN(Client)


class CFSM_Imp_Melee final : public CStateMachine
{
private:
    CFSM_Imp_Melee();
    virtual ~CFSM_Imp_Melee() = default;

public:
    virtual HRESULT				Initialize(CGameObject* pOwner);
    virtual void				Update(class CGameObject* pOwner, _float fTimeDelta) override;

public:
    static CFSM_Imp_Melee*      Create(CGameObject* pOwner);
    virtual void				Free() override;
};

NS_END