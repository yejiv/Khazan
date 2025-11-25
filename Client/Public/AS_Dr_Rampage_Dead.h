#pragma once
#include "AI_State.h"
#include "Dragonian_Rampage.h"

class CAS_Dr_Rampage_Dead : public CAI_State
{
private:
    enum FSMSTATE { DIE, RELEASSE, END };
private:
    CAS_Dr_Rampage_Dead();
    virtual ~CAS_Dr_Rampage_Dead() = default;

public:
    virtual void				    Enter(class CStateMachine* pFSM, class CGameObject* pOwner = nullptr) override;
    virtual void				    Update(class CStateMachine* pFSM, class CGameObject* pOwner, _float fTimeDelta) override;
    virtual void				    Exit(class CStateMachine* pFSM, class CGameObject* pOwner) override;

private:
    CDragonian_Rampage::MONDATA*    m_pMonData = { nullptr };
    FSMSTATE                        m_eState = { FSMSTATE::END };
public:
    static CAS_Dr_Rampage_Dead*     Create();
    virtual void				    Free() override;

};

