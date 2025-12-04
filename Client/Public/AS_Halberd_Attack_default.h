#pragma once
#include "AI_State.h"
#include "Halberd.h"

class CAS_Halberd_Attack_default : public CAI_State
{
private:
    enum FSMSTATE { ATTACK_1, ATTACK_2, ATTACK_3, ATTACK_4, END };
private:
    CAS_Halberd_Attack_default();
    virtual ~CAS_Halberd_Attack_default() = default;

public:
    virtual void				    Enter(class CStateMachine* pFSM, class CGameObject* pOwner = nullptr) override;
    virtual void				    Update(class CStateMachine* pFSM, class CGameObject* pOwner, _float fTimeDelta) override;
    virtual void				    Exit(class CStateMachine* pFSM, class CGameObject* pOwner) override;
    virtual void			        OnCollision(COLLISION_DESC* pDesc, _uint iCollisionLayer, class CGameObject* pOwner = nullptr) override;

private:
    CHalberd::MONDATA*              m_pMonData = { nullptr };
    FSMSTATE                        m_eState = { FSMSTATE::END };
    _uint                           m_iAttack = {};
public:
    static CAS_Halberd_Attack_default* Create();
    virtual void				    Free() override;

};

