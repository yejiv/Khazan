#pragma once
#include "AI_State.h"
#include "Halberd.h"

class CAS_Halberd_LockOn : public CAI_State
{
private:
    enum class LOCKONSTATE { LOCKON_F, LOCKON_B, LOCKON_L, LOCKON_R, END };
private:
    CAS_Halberd_LockOn();
    virtual ~CAS_Halberd_LockOn() = default;

public:
    virtual void				    Enter(class CStateMachine* pFSM, class CGameObject* pOwner = nullptr) override;
    virtual void				    Update(class CStateMachine* pFSM, class CGameObject* pOwner, _float fTimeDelta) override;
    virtual void				    Exit(class CStateMachine* pFSM, class CGameObject* pOwner) override;

private:
    CHalberd::MONDATA*              m_pMonData = { nullptr };
    TARGET_DIR                      m_eDir = { TARGET_DIR::END };
    _float                          m_fAccTime = {};
    LOCKONSTATE                     m_eLockOn = { LOCKONSTATE::END };

public:
    static CAS_Halberd_LockOn*      Create();
    virtual void				    Free() override;

};

