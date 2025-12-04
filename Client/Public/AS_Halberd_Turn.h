#pragma once
#include "AI_State.h"
#include "Halberd.h"

class CAS_Halberd_Turn : public CAI_State
{
private:
    CAS_Halberd_Turn();
    virtual ~CAS_Halberd_Turn() = default;

public:
    virtual void				    Enter(class CStateMachine* pFSM, class CGameObject* pOwner = nullptr) override;
    virtual void				    Update(class CStateMachine* pFSM, class CGameObject* pOwner, _float fTimeDelta) override;
    virtual void				    Exit(class CStateMachine* pFSM, class CGameObject* pOwner) override;

private:
    CHalberd::MONDATA*          m_pMonData = { nullptr };
    TARGET_DIR                  m_eDir = { TARGET_DIR::END };
    _float                      m_fAngle = {};
public:
    static CAS_Halberd_Turn* Create();
    virtual void				    Free() override;

};

