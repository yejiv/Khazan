#pragma once
#include "AI_State.h"
#include "Halberd.h"

class CAS_Halberd_Damage : public CAI_State
{
private:
    CAS_Halberd_Damage();
    virtual ~CAS_Halberd_Damage() = default;

public:
    virtual void				    Enter(class CStateMachine* pFSM, class CGameObject* pOwner = nullptr) override;
    virtual void				    Update(class CStateMachine* pFSM, class CGameObject* pOwner, _float fTimeDelta) override;
    virtual void				    Exit(class CStateMachine* pFSM, class CGameObject* pOwner) override;

private:
    CHalberd::MONDATA*              m_pMonData = { nullptr };
public:
    static CAS_Halberd_Damage*      Create();
    virtual void				    Free() override;

};

