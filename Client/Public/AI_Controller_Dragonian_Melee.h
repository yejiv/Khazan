#pragma once
#include "AI_Controller.h"
#include "Client_Defines.h"

NS_BEGIN(Client)

class CAI_Controller_Dragonian_Melee final : public CAI_Controller
{
public:
    enum class STATE {  DEAD, GRORRY, BRUTAL, ATTACK, DAMAGE, LOCKON, SLEEP, WALK, END };

private:
    CAI_Controller_Dragonian_Melee();
    virtual ~CAI_Controller_Dragonian_Melee() = default;

public:
    virtual HRESULT					        Initialize(class CCreature* pOwner);
    virtual void					        Update(CGameObject* pOwner, _float fTimeDelta) override;

public:
    void                                    Update_Aggro(CGameObject* pOwner, _float fTimeDelta);

    virtual HRESULT					        Ready_Perception(class CGameObject* pOwner, const AIPERCEPTION_DATA& Desc) override;
    virtual HRESULT					        Ready_BlackBoard(class CGameObject* pOwner) override;
    HRESULT							        Ready_BehaviorTree();

    virtual CONDITION				        GetCallbackCondition(class CGameObject* pOwner, const string& name);
    virtual ACTION					        GetCallbackAction(class CGameObject* pOwner, const string& name);
    virtual TERMINATE				        GetCallbackTeminate(class CGameObject* pOwner, const string& name);
    virtual INTERRUPTCONDITION		        GetCallbackInterruptCondition(class CGameObject* pOwner, const string& name);
    virtual PERCEPTIONCALLBACK		        GetCallBackPerception(class CGameObject* pOwner, const string& name);

private:
    HRESULT                                 Ready_FSM(class CCreature* pOwner);

public:
    static CAI_Controller_Dragonian_Melee*  Create(class CCreature* pOwner);
    virtual void					        Free() override;

};

NS_END