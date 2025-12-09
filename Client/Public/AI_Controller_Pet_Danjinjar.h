#pragma once
#include "AI_Controller.h"
#include "Client_Defines.h"
#include "Pet_Danjinjar.h"

NS_BEGIN(Client)

class CAI_Controller_Pet_Danjinjar final : public CAI_Controller
{
private:
    CAI_Controller_Pet_Danjinjar();
    virtual ~CAI_Controller_Pet_Danjinjar() = default;

public:
    virtual HRESULT					        Initialize(class CCreature* pOwner);
    virtual void					        Update(CGameObject* pOwner, _float fTimeDelta) override;

public:
    virtual HRESULT					        Ready_Perception(class CGameObject* pOwner, const AIPERCEPTION_DATA& Desc) override;
    virtual HRESULT					        Ready_BlackBoard(class CGameObject* pOwner) override;
    HRESULT							        Ready_BehaviorTree();

    virtual CONDITION				        GetCallbackCondition(class CGameObject* pOwner, const string& name);
    virtual ACTION					        GetCallbackAction(class CGameObject* pOwner, const string& name);
    virtual TERMINATE				        GetCallbackTeminate(class CGameObject* pOwner, const string& name);
    virtual INTERRUPTCONDITION		        GetCallbackInterruptCondition(class CGameObject* pOwner, const string& name);
    virtual PERCEPTIONCALLBACK		        GetCallBackPerception(class CGameObject* pOwner, const string& name);

private:
    CPet_Danjinjar::MONDATA*              m_pMonData = { nullptr };

private:
    HRESULT                                 Ready_FSM(class CCreature* pOwner);

    BTNODESTATE                             TP(CGameObject* pOwner);
    BTNODESTATE                             Damage(CGameObject* pOwner);
    BTNODESTATE                             Move(CGameObject* pOwner);
    BTNODESTATE                             Idle(CGameObject* pOwner);

public:
    static CAI_Controller_Pet_Danjinjar*    Create(class CCreature* pOwner);
    virtual void					        Free() override;

};

NS_END