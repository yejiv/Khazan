#pragma once
#include "AI_State.h"
#include "Halberd.h"

class CAS_Halberd_Groggy : public CAI_State
{
private:
    enum FSMSTATE { START, LOOP, END };

private:
    CAS_Halberd_Groggy();
    virtual ~CAS_Halberd_Groggy() = default;

public:
    virtual void				    Enter(class CStateMachine* pFSM, class CGameObject* pOwner = nullptr) override;
    virtual void				    Update(class CStateMachine* pFSM, class CGameObject* pOwner, _float fTimeDelta) override;
    virtual void				    Exit(class CStateMachine* pFSM, class CGameObject* pOwner) override;

private:
    CHalberd::MONDATA*              m_pMonData = { nullptr };
    FSMSTATE                        m_eState = { FSMSTATE::END };

    _float4*                        m_pLockOnPos = { nullptr };
public:
    static CAS_Halberd_Groggy* Create();
    virtual void				    Free() override;

};

