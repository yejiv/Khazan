#pragma once
#include "AI_Controller.h"
#include "Client_Defines.h"

NS_BEGIN(Engine)
class CGameObject;
NS_END


NS_BEGIN(Client)

class CAI_Controller_Yetuga final : public CAI_Controller
{
private:
	CAI_Controller_Yetuga();
	virtual ~CAI_Controller_Yetuga() = default;

public:
	virtual HRESULT				Initialize(class CCreature* pOwner);
	virtual void				Update(CGameObject* pOwner, _float fTimeDelta) override;

	virtual HRESULT				Ready_Perception(class CGameObject* pOwner, const AIPERCEPTION_DATA& Desc) override;
	virtual HRESULT				Ready_BlackBoard(class CGameObject* pOwner) override;
	HRESULT						Ready_BehaviorTree();

	virtual CONDITION			GetCallbackCondition(class CGameObject* pOwner, const string& name);
	virtual ACTION				GetCallbackAction(CGameObject* pOwner, const string& name);
	virtual TERMINATE			GetCallbackTeminate(CGameObject* pOwner, const string& name);
	virtual INTERRUPTCONDITION	GetCallbackInterruptCondition(CGameObject* pOwner, const string& name);
	virtual PERCEPTIONCALLBACK	GetCallBackPerception(class CGameObject* pOwner, const string& name);



public:
	static CAI_Controller_Yetuga*	Create(class CCreature* pOwner);
	virtual void					Free() override;
};

NS_END