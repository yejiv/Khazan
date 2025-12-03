#pragma once
#include "AI_State.h"
#include "Dragonian_Rampage.h"

class CAS_Dr_Rampage_Attack_Default : public CAI_State
{
private:
    enum COMBOSTATE {START, ATTACK_1, END};
    enum {ATTACK1, ATTACK2, ATTACK3, ATTACK_END};
private:
    CAS_Dr_Rampage_Attack_Default();
    virtual ~CAS_Dr_Rampage_Attack_Default() = default;

public:
    virtual void				    Enter(class CStateMachine* pFSM, class CGameObject* pOwner = nullptr) override;
    virtual void				    Update(class CStateMachine* pFSM, class CGameObject* pOwner, _float fTimeDelta) override;
    virtual void				    Exit(class CStateMachine* pFSM, class CGameObject* pOwner) override;
    virtual void			        OnCollision(COLLISION_DESC* pDesc, _uint iCollisionLayer, class CGameObject* pOwner = nullptr) override;

private:
    CDragonian_Rampage::MONDATA*    m_pMonData = { nullptr };
    COMBOSTATE                      m_eComboState = { COMBOSTATE::END };
    _int                            m_iAttackIndex = {};
public:
    static CAS_Dr_Rampage_Attack_Default*      Create();
    virtual void				    Free() override;

};

