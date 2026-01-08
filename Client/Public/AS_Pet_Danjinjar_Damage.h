#pragma once
#include "AI_State.h"
#include "Pet_Danjinjar.h"

class CAS_Pet_Danjinjar_Damage : public CAI_State
{
private:
    CAS_Pet_Danjinjar_Damage();
    virtual ~CAS_Pet_Danjinjar_Damage() = default;

public:
    virtual void				Enter(class CStateMachine* pFSM, class CGameObject* pOwner = nullptr) override;
    virtual void				Update(class CStateMachine* pFSM, class CGameObject* pOwner, _float fTimeDelta) override;
    virtual void				Exit(class CStateMachine* pFSM, class CGameObject* pOwner) override;

private:
    CPet_Danjinjar::MONDATA* m_pMonData = { nullptr };
    _float                      m_fAccTime = {};
public:
    static CAS_Pet_Danjinjar_Damage* Create();
    virtual void				Free() override;

};

