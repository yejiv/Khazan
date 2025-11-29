#pragma once
#include "AI_State.h"
#include "Elamein.h"

class CAS_Elamein_Sleep : public CAI_State
{
private:
    CAS_Elamein_Sleep();
    virtual ~CAS_Elamein_Sleep() = default;

public:
    virtual void				    Enter(class CStateMachine* pFSM, class CGameObject* pOwner = nullptr) override;
    virtual void				    Update(class CStateMachine* pFSM, class CGameObject* pOwner, _float fTimeDelta) override;
    virtual void				    Exit(class CStateMachine* pFSM, class CGameObject* pOwner) override;

private:
    CElamein::MONDATA*              m_pMonData = { nullptr };

public:
    static CAS_Elamein_Sleep*       Create();
    virtual void				    Free() override;

};

