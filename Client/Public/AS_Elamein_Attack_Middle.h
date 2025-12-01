#pragma once
#include "AI_State.h"
#include "Elamein.h"

class CAS_Elamein_Attack_Middle : public CAI_State
{
private:
    enum FSMSTATE { ATTACK_1, ATTACK_2, ATTACK_3, END };

private:
    CAS_Elamein_Attack_Middle();
    virtual ~CAS_Elamein_Attack_Middle() = default;

public:
    virtual void				    Enter(class CStateMachine* pFSM, class CGameObject* pOwner = nullptr) override;
    virtual void				    Update(class CStateMachine* pFSM, class CGameObject* pOwner, _float fTimeDelta) override;
    virtual void				    Exit(class CStateMachine* pFSM, class CGameObject* pOwner) override;
    virtual void			        OnCollision(COLLISION_DESC* pDesc, _uint iCollisionLayer, class CGameObject* pOwner = nullptr) override;

private:
    CElamein::MONDATA*              m_pMonData = { nullptr };
    _bool                           m_isAttack1 = {};
    FSMSTATE                        m_eState = { FSMSTATE::END };
public:
    static CAS_Elamein_Attack_Middle*       Create();
    virtual void				    Free() override;

};

