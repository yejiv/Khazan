#pragma once
#include "AI_State.h"
#include "Pet_Danjinjar.h"

class CAS_Pet_Danjinjar_Move : public CAI_State
{

private:
    CAS_Pet_Danjinjar_Move();
    virtual ~CAS_Pet_Danjinjar_Move() = default;

public:
    virtual void				Enter(class CStateMachine* pFSM, class CGameObject* pOwner = nullptr) override;
    virtual void				Update(class CStateMachine* pFSM, class CGameObject* pOwner, _float fTimeDelta) override;
    virtual void				Exit(class CStateMachine* pFSM, class CGameObject* pOwner) override;

private:
    CPet_Danjinjar::MONDATA*    m_pMonData = { nullptr };
    _int                        m_iTalkCount = {};
public:
    static CAS_Pet_Danjinjar_Move* Create();
    virtual void				Free() override;

};

