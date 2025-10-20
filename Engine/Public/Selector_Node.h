#pragma once
#include "Composite_Node.h"

NS_BEGIN(Engine)

class ENGINE_DLL CSelector_Node final : public CComposite_Node<CSelector_Node>
{
private:
	CSelector_Node();
	virtual ~CSelector_Node() = default;

public:
	virtual BTNODESTATE Tick(class CBlackBoard* BB) override;
	virtual void		Terminate(BTNODESTATE eState) override;
	virtual void		Abort() override;

public:
	static	CSelector_Node* Create();
	virtual void			Free() override;

};

NS_END