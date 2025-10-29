#pragma once
#include "Decorator_Node.h"

NS_BEGIN(Engine)

class ENGINE_DLL CInverter_Node final : public CDecorator_Node
{
private:
	CInverter_Node();
	virtual ~CInverter_Node() = default;

public:
	virtual BTNODESTATE		Tick(class CBlackBoard* BB) override;
	virtual void			Terminate(BTNODESTATE eState, class CBlackBoard* BB = nullptr) override;
	virtual void			Abort() override;

public:
	static CInverter_Node*	Create();
	virtual void			Free() override;


};


NS_END
