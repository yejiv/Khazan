#pragma once
#include "AI_State.h"
#include "Halberd.h"

class CAS_Halberd_Sleep : public CAI_State
{
private:
    enum FSMSTATE { SLEEP, GETUP, END };
private:
    CAS_Halberd_Sleep();
    virtual ~CAS_Halberd_Sleep() = default;

public:
    virtual void				    Enter(class CStateMachine* pFSM, class CGameObject* pOwner = nullptr) override;
    virtual void				    Update(class CStateMachine* pFSM, class CGameObject* pOwner, _float fTimeDelta) override;
    virtual void				    Exit(class CStateMachine* pFSM, class CGameObject* pOwner) override;

private:
    CHalberd::MONDATA*              m_pMonData = { nullptr };
    FSMSTATE                        m_eState = { FSMSTATE::END };
    _bool                           m_isChange = {};
public:
    static CAS_Halberd_Sleep*       Create();
    virtual void				    Free() override;

};

