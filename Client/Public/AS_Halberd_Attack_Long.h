#pragma once
#include "AI_State.h"
#include "Halberd.h"

class CAS_Halberd_Attack_Long : public CAI_State
{
private:
    enum FSMSTATE { START, RUSH, END };

private:
    CAS_Halberd_Attack_Long();
    virtual ~CAS_Halberd_Attack_Long() = default;

public:
    virtual void				    Enter(class CStateMachine* pFSM, class CGameObject* pOwner = nullptr) override;
    virtual void				    Update(class CStateMachine* pFSM, class CGameObject* pOwner, _float fTimeDelta) override;
    virtual void				    Exit(class CStateMachine* pFSM, class CGameObject* pOwner) override;
    virtual void			        OnCollision(COLLISION_DESC* pDesc, _uint iCollisionLayer, class CGameObject* pOwner = nullptr) override;

private:
    CHalberd::MONDATA*              m_pMonData = { nullptr };
    _bool                           m_isSprint = {};
    FSMSTATE                        m_eState = { END };
public:
    static CAS_Halberd_Attack_Long* Create();
    virtual void				    Free() override;

};

