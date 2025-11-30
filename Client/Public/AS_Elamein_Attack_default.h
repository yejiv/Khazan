#pragma once
#include "AI_State.h"
#include "Elamein.h"

class CAS_Elamein_Attack_default : public CAI_State
{
private:
    enum FSMSTATE { STEMP, ATTACK_SWORD, ATTACK_SHILED, ATTACK_TWO, END };
private:
    CAS_Elamein_Attack_default();
    virtual ~CAS_Elamein_Attack_default() = default;

public:
    virtual void				    Enter(class CStateMachine* pFSM, class CGameObject* pOwner = nullptr) override;
    virtual void				    Update(class CStateMachine* pFSM, class CGameObject* pOwner, _float fTimeDelta) override;
    virtual void				    Exit(class CStateMachine* pFSM, class CGameObject* pOwner) override;

private:
    CElamein::MONDATA*              m_pMonData = { nullptr };
    FSMSTATE                        m_eState = { FSMSTATE::END };
    _uint                           m_iAttack = {};

public:
    static CAS_Elamein_Attack_default* Create();
    virtual void				    Free() override;

};

