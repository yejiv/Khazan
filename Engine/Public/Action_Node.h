#pragma once
#include "BTNode.h"

NS_BEGIN(Engine)

class ENGINE_DLL CAction_Node final : public CBTNode
{
public:
	typedef function<BTNODESTATE(CBlackBoard* BB)> ACTION;
	typedef	function<void(CBlackBoard*, BTNODESTATE)> TERMINATE;

private:
	CAction_Node(ACTION Action, TERMINATE Terminate);
	virtual ~CAction_Node() = default;

public:
	virtual BTNODESTATE		Tick(class CBlackBoard* BB = nullptr) override;
	virtual void			Terminate(BTNODESTATE eState, class CBlackBoard* BB = nullptr) override;
    virtual void            Abort(class CBlackBoard* BB) override;

private:
	ACTION					m_Action;
	TERMINATE				m_Terminate;

public:
	static CAction_Node*	Create(ACTION Action, TERMINATE Terminate);
	virtual void			Free() override;

};

NS_END