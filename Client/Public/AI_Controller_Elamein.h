#pragma once
#include "AI_Controller.h"
#include "Client_Defines.h"
#include "Elamein.h"

NS_BEGIN(Client)

class CAI_Controller_Elamein final : public CAI_Controller
{
private:
    CAI_Controller_Elamein();
    virtual ~CAI_Controller_Elamein() = default;

public:
    virtual HRESULT					            Initialize(class CCreature* pOwner);
    virtual void					            Update(CGameObject* pOwner, _float fTimeDelta) override;

public:
    virtual HRESULT					            Ready_Perception(class CGameObject* pOwner, const AIPERCEPTION_DATA& Desc) override;
    virtual HRESULT					            Ready_BlackBoard(class CGameObject* pOwner) override;
    HRESULT							            Ready_BehaviorTree();

    virtual CONDITION				            GetCallbackCondition(class CGameObject* pOwner, const string& name);
    virtual ACTION					            GetCallbackAction(class CGameObject* pOwner, const string& name);
    virtual TERMINATE				            GetCallbackTeminate(class CGameObject* pOwner, const string& name);
    virtual INTERRUPTCONDITION		            GetCallbackInterruptCondition(class CGameObject* pOwner, const string& name);
    virtual PERCEPTIONCALLBACK		            GetCallBackPerception(class CGameObject* pOwner, const string& name);

private:
    CElamein::MONDATA* m_pMonData = { nullptr };

private:
    HRESULT                                     Ready_FSM(class CCreature* pOwner);

    BTNODESTATE                                 Damage_Check(CGameObject* pOwner);
    BTNODESTATE                                 Dodge(CGameObject* pOwner);
    BTNODESTATE                                 Damage(CGameObject* pOwner);

    BTNODESTATE                                 Turn_Check(CGameObject* pOwner);
    BTNODESTATE                                 Turn(CGameObject* pOwner);

    BTNODESTATE                                 Attack_Long_Check(CGameObject* pOwner);
    BTNODESTATE                                 Attack_Long(CGameObject* pOwner);

    BTNODESTATE                                 Guard_Check(CGameObject* pOwner);
    BTNODESTATE                                 Guard(CGameObject* pOwner);

    BTNODESTATE                                 Attack_Check(CGameObject* pOwner);
    BTNODESTATE                                 Attack_Enchant(CGameObject* pOwner);
    BTNODESTATE                                 Attack_Middle(CGameObject* pOwner);
    BTNODESTATE                                 Attack_Default(CGameObject* pOwner);

    BTNODESTATE                                 LockOn(CGameObject* pOwner);
    BTNODESTATE                                 Sleep(CGameObject* pOwner);

public:
    static CAI_Controller_Elamein* Create(class CCreature* pOwner);
    virtual void					            Free() override;

};

NS_END