#pragma once
#include "AI_State.h"
#include "Dragonian_Rampage.h"

class CAS_Dr_Rampage_Brutal : public CAI_State
{
private:
    enum FSMSTATE { START, END };
private:
    CAS_Dr_Rampage_Brutal();
    virtual ~CAS_Dr_Rampage_Brutal() = default;

public:
    virtual void				    Enter(class CStateMachine* pFSM, class CGameObject* pOwner = nullptr) override;
    virtual void				    Update(class CStateMachine* pFSM, class CGameObject* pOwner, _float fTimeDelta) override;
    virtual void				    Exit(class CStateMachine* pFSM, class CGameObject* pOwner) override;

private:
    CDragonian_Rampage::MONDATA*    m_pMonData = { nullptr };
    FSMSTATE                        m_eState = { END };
    _float                          m_fAccTime = {};
    _bool                           m_isF = {};
public:
    static CAS_Dr_Rampage_Brutal*      Create();
    virtual void				    Free() override;

};

