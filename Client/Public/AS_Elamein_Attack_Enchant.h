#pragma once
#include "AI_State.h"
#include "Elamein.h"

class CAS_Elamein_Attack_Enchant : public CAI_State
{
private:
    enum FSMSTATE { CHARGE, START, LOOP, COUNT, END };
private:
    CAS_Elamein_Attack_Enchant();
    virtual ~CAS_Elamein_Attack_Enchant() = default;

public:
    virtual void				    Enter(class CStateMachine* pFSM, class CGameObject* pOwner = nullptr) override;
    virtual void				    Update(class CStateMachine* pFSM, class CGameObject* pOwner, _float fTimeDelta) override;
    virtual void				    Exit(class CStateMachine* pFSM, class CGameObject* pOwner) override;
    virtual void			        OnCollision(COLLISION_DESC* pDesc, _uint iCollisionLayer, class CGameObject* pOwner = nullptr) override;

private:
    CElamein::MONDATA*              m_pMonData = { nullptr };
    FSMSTATE                        m_eState = { END };
public:
    static CAS_Elamein_Attack_Enchant* Create();
    virtual void				    Free() override;

};

