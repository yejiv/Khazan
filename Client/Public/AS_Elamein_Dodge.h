#pragma once
#include "AI_State.h"
#include "Elamein.h"

class CAS_Elamein_Dodge : public CAI_State
{
private:
    enum FSMSTATE { DODGE, ATTACK, END };
private:
    CAS_Elamein_Dodge();
    virtual ~CAS_Elamein_Dodge() = default;

public:
    virtual void				    Enter(class CStateMachine* pFSM, class CGameObject* pOwner = nullptr) override;
    virtual void				    Update(class CStateMachine* pFSM, class CGameObject* pOwner, _float fTimeDelta) override;
    virtual void				    Exit(class CStateMachine* pFSM, class CGameObject* pOwner) override;

private:
    CElamein::MONDATA*              m_pMonData = { nullptr };
    _bool                           m_isAttack = {};
    FSMSTATE                        m_eState = {};
public:
    static CAS_Elamein_Dodge* Create();
    virtual void				    Free() override;

};

