#pragma once
#include "AI_State.h"
#include "Dragonian_Rampage.h"

class CAS_Dr_Rampage_Attack_Back : public CAI_State
{
private:
    CAS_Dr_Rampage_Attack_Back();
    virtual ~CAS_Dr_Rampage_Attack_Back() = default;

public:
    virtual void				        Enter(class CStateMachine* pFSM, class CGameObject* pOwner = nullptr) override;
    virtual void				        Update(class CStateMachine* pFSM, class CGameObject* pOwner, _float fTimeDelta) override;
    virtual void				        Exit(class CStateMachine* pFSM, class CGameObject* pOwner) override;
    virtual void			            OnCollision(COLLISION_DESC* pDesc, _uint iCollisionLayer, class CGameObject* pOwner = nullptr) override;

private:
    CDragonian_Rampage::MONDATA*        m_pMonData = { nullptr };
    _float                              m_fAngle = {};
public:
    static CAS_Dr_Rampage_Attack_Back*  Create();
    virtual void				        Free() override;

};

