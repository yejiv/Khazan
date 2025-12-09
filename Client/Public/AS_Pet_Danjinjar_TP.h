#pragma once
#include "AI_State.h"
#include "Pet_Danjinjar.h"

class CAS_Pet_Danjinjar_TP : public CAI_State
{
private:
    enum STATE {READY, ATION, END};
private:
    CAS_Pet_Danjinjar_TP();
    virtual ~CAS_Pet_Danjinjar_TP() = default;

public:
    virtual void				Enter(class CStateMachine* pFSM, class CGameObject* pOwner = nullptr) override;
    virtual void				Update(class CStateMachine* pFSM, class CGameObject* pOwner, _float fTimeDelta) override;
    virtual void				Exit(class CStateMachine* pFSM, class CGameObject* pOwner) override;

private:
    CPet_Danjinjar::MONDATA*    m_pMonData = { nullptr };
    STATE                       m_eState = { END };
    _float                      m_fAcctime = {};
public:
    static CAS_Pet_Danjinjar_TP* Create();
    virtual void				Free() override;

};

