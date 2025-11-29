#pragma once
#include "AI_State.h"
#include "Elamein.h"

class CAS_Elamein_Groggy : public CAI_State
{
private:
    enum FSMSTATE { START, LOOP, END };

private:
    CAS_Elamein_Groggy();
    virtual ~CAS_Elamein_Groggy() = default;

public:
    virtual void				    Enter(class CStateMachine* pFSM, class CGameObject* pOwner = nullptr) override;
    virtual void				    Update(class CStateMachine* pFSM, class CGameObject* pOwner, _float fTimeDelta) override;
    virtual void				    Exit(class CStateMachine* pFSM, class CGameObject* pOwner) override;

private:
    CElamein::MONDATA* m_pMonData = { nullptr };
    FSMSTATE                        m_eState = { FSMSTATE::END };
public:
    static CAS_Elamein_Groggy* Create();
    virtual void				    Free() override;

};

