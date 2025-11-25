#pragma once
#include "AI_State.h"
#include "Dragonian_Rampage.h"

class CAS_Dr_Rampage_LockOn : public CAI_State
{
private:
    CAS_Dr_Rampage_LockOn();
    virtual ~CAS_Dr_Rampage_LockOn() = default;

public:
    virtual void				    Enter(class CStateMachine* pFSM, class CGameObject* pOwner = nullptr) override;
    virtual void				    Update(class CStateMachine* pFSM, class CGameObject* pOwner, _float fTimeDelta) override;
    virtual void				    Exit(class CStateMachine* pFSM, class CGameObject* pOwner) override;

private:
    CDragonian_Rampage::MONDATA*    m_pMonData = { nullptr };

public:
    static CAS_Dr_Rampage_LockOn*      Create();
    virtual void				    Free() override;

};

