#pragma once
#include "Engine_Defines.h"
#include "Composite_Node.h"

NS_BEGIN(Engine)

class ENGINE_DLL CInterruptibleSelector_Node final : public CComposite_Node
{
public:
	typedef function<_bool(CBlackBoard* BB)> INTERRUPTCONDITION;

private:
	CInterruptibleSelector_Node(INTERRUPTCONDITION Func);
	virtual ~CInterruptibleSelector_Node() = default;

public:
	virtual BTNODESTATE					Tick(class CBlackBoard* BB) override;
	virtual void						Terminate(BTNODESTATE eState, class CBlackBoard* BB = nullptr) override;
	virtual void						Abort(CBlackBoard* BB);

private:
	INTERRUPTCONDITION					m_pInterruptCondition;

public:
	static CInterruptibleSelector_Node* Create(INTERRUPTCONDITION Func);
	virtual void						Free();

};

NS_END