#pragma once
#include "BTNode.h"

NS_BEGIN(Engine)

class ENGINE_DLL CCondition_Node final : public CBTNode
{
public:
	typedef function<_bool(class CBlackBoard*)> CONDITION;

private:
	CCondition_Node(CONDITION Condition);
	virtual ~CCondition_Node() = default;

public:
	virtual BTNODESTATE			Tick(class CBlackBoard* BB) override;

private:
	CONDITION					m_Condition;

public:
	static CCondition_Node*		Create(CONDITION Condition);
	virtual void				Free() override;

};

NS_END