#pragma once
#include "AI_State.h"
#include "Dragonian_Rampage.h"

class CAS_Dr_Rampage_Turn : public CAI_State
{
private:
    CAS_Dr_Rampage_Turn();
    virtual ~CAS_Dr_Rampage_Turn() = default;

public:
    virtual void				    Enter(class CStateMachine* pFSM, class CGameObject* pOwner = nullptr) override;
    virtual void				    Update(class CStateMachine* pFSM, class CGameObject* pOwner, _float fTimeDelta) override;
    virtual void				    Exit(class CStateMachine* pFSM, class CGameObject* pOwner) override;

private:
    CDragonian_Rampage::MONDATA* m_pMonData = { nullptr };
    TARGET_DIR                  m_eDir = { TARGET_DIR::END };
    _float                      m_fAngle = {};
public:
    static CAS_Dr_Rampage_Turn* Create();
    virtual void				    Free() override;

};

