#pragma once
#include "AI_State.h"
#include "Dragonian_Rampage.h"

class CAS_Dr_Rampage_Sleep : public CAI_State
{
private:
    enum FSMSTATE { SLEEP, GETUP, END };
private:
    CAS_Dr_Rampage_Sleep();
    virtual ~CAS_Dr_Rampage_Sleep() = default;

public:
    virtual void				    Enter(class CStateMachine* pFSM, class CGameObject* pOwner = nullptr) override;
    virtual void				    Update(class CStateMachine* pFSM, class CGameObject* pOwner, _float fTimeDelta) override;
    virtual void				    Exit(class CStateMachine* pFSM, class CGameObject* pOwner) override;

private:
    CDragonian_Rampage::MONDATA*    m_pMonData = { nullptr };
    FSMSTATE                        m_eState = { FSMSTATE::END };
    _bool                           m_isChange = {};

public:
    static CAS_Dr_Rampage_Sleep*    Create();
    virtual void				    Free() override;

};

