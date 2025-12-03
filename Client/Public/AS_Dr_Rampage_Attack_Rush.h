#pragma once
#include "AI_State.h"
#include "Dragonian_Rampage.h"

class CAS_Dr_Rampage_Attack_Rush : public CAI_State
{
private:
    enum COMBOSTATE { START, LOOP, END,};

private:
    CAS_Dr_Rampage_Attack_Rush();
    virtual ~CAS_Dr_Rampage_Attack_Rush() = default;

public:
    virtual void				    Enter(class CStateMachine* pFSM, class CGameObject* pOwner = nullptr) override;
    virtual void				    Update(class CStateMachine* pFSM, class CGameObject* pOwner, _float fTimeDelta) override;
    virtual void				    Exit(class CStateMachine* pFSM, class CGameObject* pOwner) override;
    virtual void			        OnCollision(COLLISION_DESC* pDesc, _uint iCollisionLayer, class CGameObject* pOwner = nullptr) override;

private:
    CDragonian_Rampage::MONDATA*    m_pMonData = { nullptr };
    _bool                           m_isCombo = { false };
    COMBOSTATE                      m_eComboState = { COMBOSTATE::END };
    _float                          m_fAccTime = {};
public:
    static CAS_Dr_Rampage_Attack_Rush*      Create();
    virtual void				    Free() override;

};

