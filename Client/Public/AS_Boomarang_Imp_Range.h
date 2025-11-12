#pragma once
#include "AI_State.h"
#include "Client_Defines.h"

NS_BEGIN(Client)

class CAS_Boomarang_Imp_Range final : public CAI_State
{
private:
    CAS_Boomarang_Imp_Range();
    virtual ~CAS_Boomarang_Imp_Range() = default;

public:
    virtual void					Enter(class CStateMachine* pFSM, class CGameObject* pOwner = nullptr) override;
    virtual void					Update(class CStateMachine* pFSM, class CGameObject* pOwner, _float fTimeDelta) override;
    virtual void					Exit(class CStateMachine* pFSM, class CGameObject* pOwner) override;

public:
    virtual void					OnCollision(COLLISION_DESC* pDesc, _uint iCollisionLayer, class CGameObject* pOwner = nullptr) {};


public:
    static CAS_Boomarang_Imp_Range* Create();
    virtual void					Free() override;
};

NS_END