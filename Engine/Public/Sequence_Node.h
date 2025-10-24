#pragma once
#include "Composite_Node.h"

NS_BEGIN(Engine)

class ENGINE_DLL CSequence_Node final : public CComposite_Node
{
private:
	CSequence_Node();
	virtual ~CSequence_Node() = default;

public:
	virtual BTNODESTATE		Tick(class CBlackBoard* BB) override;
	virtual void			Terminate(BTNODESTATE eState) override;
	virtual void			Abort() override;

public:
	static CSequence_Node*	Create();
	virtual void			Free() override;

};

NS_END