#pragma once
#include "BTNode.h"

NS_BEGIN(Engine)

class ENGINE_DLL CDecorator_Node abstract : public CBTNode
{
protected:
	CDecorator_Node();
	virtual ~CDecorator_Node() = default;

	virtual BTNODESTATE		Tick(class CBlackBoard* BB) = 0;
	virtual void			Terminate(BTNODESTATE eState, class CBlackBoard* BB = nullptr) override;
    virtual void            Abort(class CBlackBoard* BB) { Terminate(BTNODESTATE::FAILURE, BB); }


public:
	void					Set_Child(CBTNode* pChild);
    CBTNode*                Get_Child() const { return m_pChild; }

protected:
	CBTNode*				m_pChild = { nullptr };

public:
	virtual void			Free() override;
};

NS_END