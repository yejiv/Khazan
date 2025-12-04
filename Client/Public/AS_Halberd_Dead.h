#pragma once
#include "AI_State.h"
#include "Halberd.h"

class CAS_Halberd_Dead : public CAI_State
{
private:
    enum FSMSTATE { DIE, RELEASSE, END };
private:
    CAS_Halberd_Dead();
    virtual ~CAS_Halberd_Dead() = default;

public:
    virtual void				    Enter(class CStateMachine* pFSM, class CGameObject* pOwner = nullptr) override;
    virtual void				    Update(class CStateMachine* pFSM, class CGameObject* pOwner, _float fTimeDelta) override;
    virtual void				    Exit(class CStateMachine* pFSM, class CGameObject* pOwner) override;

private:
    CHalberd::MONDATA*              m_pMonData = { nullptr };
    FSMSTATE                        m_eState = { FSMSTATE::END };
public:
    static CAS_Halberd_Dead*        Create();
    virtual void				    Free() override;

};

