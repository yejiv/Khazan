#pragma once
#include "Base.h"
#include "BTNode_Defines.h"
#include "Perception_Defines.h"

NS_BEGIN(Engine)

class ENGINE_DLL CAI_Controller abstract : public CBase
{
protected:
	CAI_Controller();
	virtual ~CAI_Controller() = default;

public:
	class CStateMachine*		Get_State_Machine() const { return m_pFSM; }

public:
	void						AI_MoveTo(class CGameObject* pOwner, class CGameObject* pTarget, _float fSpeedPerSec ,_float fLimit, _float fTimeDelta);
	void						AI_ApplyDamage(class CGameObject* pAttacker, _float fDamage);

public:
	virtual HRESULT				Initialize(class CGameObject* pOwner, string FileName);
	virtual void				Update(class CGameObject* pOwner, _float fTimeDelta);

	HRESULT						Load_Perception(class CGameObject* pOwner, const string& FilePath);
	HRESULT						Load_BlackBoard(class CGameObject* pOwner, const string& FilePath);
	HRESULT						Load_BehaviorTree(class CGameObject* pOwner,const string& FilePath);
	HRESULT						LoadBTNode(const JSON& j, AIBTNODE_DATA& Node);
	CBTNode*					CreateBTNode(class CGameObject* pOwner, const AIBTNODE_DATA& NodeData);

	virtual HRESULT				Ready_Perception(class CGameObject* pOwner, const AIPERCEPTION_DATA& Desc);
	virtual HRESULT				Ready_BlackBoard(class CGameObject* pOwner);
	virtual HRESULT				Ready_BehaviorTree();

public:
	virtual PERCEPTIONCALLBACK	GetCallBackPerception(class CGameObject* pOwner, const string& name);


	virtual CONDITION			GetCallbackCondition(class CGameObject* pOwner,const string& name);
	virtual ACTION				GetCallbackAction(CGameObject* pOwner, const string& name);
	virtual TERMINATE			GetCallbackTeminate(CGameObject* pOwner, const string& name);
	virtual INTERRUPTCONDITION	GetCallbackInterruptCondition(CGameObject* pOwner, const string& name);


protected:
	AI_BTDATA					m_BTDesc{};
	string						m_strMonstertag;

	class CStateMachine*		m_pFSM = { nullptr };
	class CBehaviorTree*		m_pBT = { nullptr };
	class CBlackBoard*			m_pBB = { nullptr };
	class CPerception*			m_pPerception = { nullptr };
	class CGameInstance*		m_pGameInstance = { nullptr };

public:
	void						Release_BTNode(AIBTNODE_DATA* pRootNodeData);
	virtual void				Free() override;
};

NS_END