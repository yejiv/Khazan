#pragma once
#include "AI_State.h"
#include "Client_Defines.h"

NS_BEGIN(Client)

enum class DASHATTACK_STATE {START, RUNNIG, FINISHED, DODGE, END };

class CAS_DashAttack_Imp_Melee final : public CAI_State
{
private:
    CAS_DashAttack_Imp_Melee();
    virtual ~CAS_DashAttack_Imp_Melee() = default;

public:
    virtual void Enter(class CStateMachine* pFSM, class CGameObject* pOwner = nullptr) override;
    virtual void Update(class CStateMachine* pFSM, class CGameObject* pOwner, _float fTimeDelta) override;
    virtual void Exit(class CStateMachine* pFSM, class CGameObject* pOwner) override;

public:
    virtual void OnCollision(COLLISION_DESC* pDesc, _uint iCollisionLayer, class CGameObject* pOwner = nullptr) override;


private:
    _bool                               m_isEnd = { false };
    _bool						        m_isCrashed = { false };
    _float                              m_fCurrentTime = { 0.f };
    _float                              m_fLoseTime = { 3.f };
    _float                              m_fSpeed = {};

    DASHATTACK_STATE                    m_eState = { DASHATTACK_STATE::END };


public:
    static CAS_DashAttack_Imp_Melee*    Create();
    virtual void				        Free() override;

};

NS_END