#pragma once
#include "AI_State.h"
#include "Elamein.h"

class CAS_Elamein_Attack_Long : public CAI_State
{
private:
    enum FSMSTATE { ATTACK_1, ATTACK_2, ATTACK_3, END };

private:
    CAS_Elamein_Attack_Long();
    virtual ~CAS_Elamein_Attack_Long() = default;

public:
    virtual void				    Enter(class CStateMachine* pFSM, class CGameObject* pOwner = nullptr) override;
    virtual void				    Update(class CStateMachine* pFSM, class CGameObject* pOwner, _float fTimeDelta) override;
    virtual void				    Exit(class CStateMachine* pFSM, class CGameObject* pOwner) override;
    virtual void			        OnCollision(COLLISION_DESC* pDesc, _uint iCollisionLayer, class CGameObject* pOwner = nullptr) override;

private:
    CElamein::MONDATA*              m_pMonData = { nullptr };
    _bool                           m_isSword = {};
    FSMSTATE                        m_eState = { END };
public:
    static CAS_Elamein_Attack_Long* Create();
    virtual void				    Free() override;

};

