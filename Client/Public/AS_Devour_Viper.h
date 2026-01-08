#pragma once
#include "AI_State.h"
#include "Client_Defines.h"

NS_BEGIN(Client)

class CAS_Devour_Viper final : public CAI_State
{
private:
    CAS_Devour_Viper();
    virtual ~CAS_Devour_Viper() = default;

public:
    virtual void Enter(class CStateMachine* pFSM, class CGameObject* pOwner = nullptr) override;
    virtual void Update(class CStateMachine* pFSM, class CGameObject* pOwner, _float fTimeDelta) override;
    virtual void Exit(class CStateMachine* pFSM, class CGameObject* pOwner) override;

public:
    virtual void OnCollision(COLLISION_DESC* pDesc, _uint iCollisionLayer, class CGameObject* pOwner = nullptr) override;



private:
    _float                          MakeDevourSpeed(_float fDist);
    
private:
    _float                          m_fMinSpeed = {};
    _float                          m_fMaxSpeed = {};
    _float                          m_fMinRange = {};
    _float                          m_fMaxRange = {};
    _float                          m_fDevourAcc = {};

public:
    static CAS_Devour_Viper*        Create();
    virtual void                    Free() override;

};

NS_END