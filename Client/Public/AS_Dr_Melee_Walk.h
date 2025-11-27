#pragma once
#include "AI_State.h"
#include "Dragonian_Melee.h"

class CAS_Dr_Melee_Walk : public CAI_State
{
private:
    enum FSMSTATE { STAND, WALK, END };
private:
    CAS_Dr_Melee_Walk();
    virtual ~CAS_Dr_Melee_Walk() = default;

public:
    virtual void				Enter(class CStateMachine* pFSM, class CGameObject* pOwner = nullptr) override;
    virtual void				Update(class CStateMachine* pFSM, class CGameObject* pOwner, _float fTimeDelta) override;
    virtual void				Exit(class CStateMachine* pFSM, class CGameObject* pOwner) override;
private:
    CDragonian_Melee::MONDATA* m_pMonData = { nullptr };
    FSMSTATE                    m_eState = { FSMSTATE::END };

    _bool                       m_isRange = { false };

public:
    static CAS_Dr_Melee_Walk*   Create();
    virtual void				Free() override;

};

