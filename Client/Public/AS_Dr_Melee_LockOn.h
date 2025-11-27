#pragma once
#include "AI_State.h"
#include "Dragonian_Melee.h"

class CAS_Dr_Melee_LockOn : public CAI_State
{
private:
    enum AI_STATE { TURN, STAND, WALK, END};
private:
    CAS_Dr_Melee_LockOn();
    virtual ~CAS_Dr_Melee_LockOn() = default;

public:
    virtual void				Enter(class CStateMachine* pFSM, class CGameObject* pOwner = nullptr) override;
    virtual void				Update(class CStateMachine* pFSM, class CGameObject* pOwner, _float fTimeDelta) override;
    virtual void				Exit(class CStateMachine* pFSM, class CGameObject* pOwner) override;

private:
    CDragonian_Melee::MONDATA*  m_pMonData = { nullptr };
    TARGET_DIR                  m_eDir = { TARGET_DIR::END };
    _float                      m_fAngle = {};
    AI_STATE                    m_eState = { AI_STATE::END };

    _float                      m_fDeley = {};
public:
    static CAS_Dr_Melee_LockOn* Create();
    virtual void				Free() override;

};

