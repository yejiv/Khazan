#pragma once
#include "AI_State.h"
#include "Client_Defines.h"


enum class IMP_SlEEP_STATE { SLEEP, WAKEUP, BRUTALATTACK ,END, };

NS_BEGIN(Client)

class CAS_Sleep_Imp_Melee final : public CAI_State
{
private:
    CAS_Sleep_Imp_Melee();
    virtual ~CAS_Sleep_Imp_Melee() = default;

public:
    virtual void					Enter(class CStateMachine* pFSM, class CGameObject* pOwner = nullptr) override;
    virtual void					Update(class CStateMachine* pFSM, class CGameObject* pOwner, _float fTimeDelta) override;
    virtual void					Exit(class CStateMachine* pFSM, class CGameObject* pOwner) override;

public:
    virtual void					OnCollision(COLLISION_DESC* pDesc, _uint iCollisionLayer, class CGameObject* pOwner = nullptr) {};

private:
    IMP_SlEEP_STATE				    m_eState = { IMP_SlEEP_STATE::SLEEP };
    _bool							m_isChanged = { false };

    _uint                           m_iAnimIndex = {};

public:
    static CAS_Sleep_Imp_Melee*     Create();
    virtual void					Free() override;
};

NS_END