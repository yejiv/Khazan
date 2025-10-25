#pragma once
#include "Editor_Defines.h"
#include "AI_Controller.h"

NS_BEGIN(Engine)
class CBTNode;
NS_END

NS_BEGIN(Editor)

class CAIController_Edit final : public CAI_Controller
{
private:
	CAIController_Edit();
	virtual ~CAIController_Edit() = default;
	
public:
	virtual HRESULT					Initialize(class CGameObject* pOwner);
	virtual void					Update(class CGameObject* pOwner, _float fTimeDelta) override;

private:
	HRESULT							Load_Perception(const string& FilePath);
	HRESULT							Load_BlackBoard(class CGameObject* pOwner,const string& FilePath);
	HRESULT							Load_BehaviorTree(const string& FilePath);
	HRESULT							LoadBTNode(const JSON& j, AIBTNODE_DATA& Node);
	CBTNode*						CreateBTNode(const AIBTNODE_DATA& NodeData);

	HRESULT							Ready_Perception(const AIPERCEPTION_DATA& Desc);
	HRESULT							Ready_BlackBoard(class CGameObject* pOwner);
	HRESULT							Ready_BehaviorTree();
	CONDITION						GetCallbackCondition(const string& name);
	ACTION							GetCallbackAction(const string& name);
	TERMINATE						GetCallbackTeminate(const string& name);


	AI_BTDATA						m_BTDesc{};

private:
	string							m_strMonstertag;


public:
	static CAIController_Edit*		Create(class CGameObject* pOwner);
	virtual void					Free() override;

};

NS_END