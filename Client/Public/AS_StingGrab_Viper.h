#pragma once
#include "AI_State.h"
#include "Client_Defines.h"

NS_BEGIN(Client)

enum class  VIPERGRAB_STATE { SUCCESS, FAIL, NONE };

class CAS_StingGrab_Viper final : public CAI_State
{
private:
    CAS_StingGrab_Viper();
    virtual ~CAS_StingGrab_Viper() = default;

public:
    virtual void Enter(class CStateMachine* pFSM, class CGameObject* pOwner = nullptr) override;
    virtual void Update(class CStateMachine* pFSM, class CGameObject* pOwner, _float fTimeDelta) override;
    virtual void Exit(class CStateMachine* pFSM, class CGameObject* pOwner) override;

public:
    virtual void OnCollision(COLLISION_DESC* pDesc, _uint iCollisionLayer, class CGameObject* pOwner = nullptr) override;



private:
    _bool						m_isGrabbed = { false };
    _float3						m_vGrabPoint = {};


    VIPERGRAB_STATE             m_eState = { VIPERGRAB_STATE::NONE };

public:
    static CAS_StingGrab_Viper* Create();
    virtual void				Free() override;

};

NS_END