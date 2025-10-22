#pragma once
#include "Decorator_Node.h"

NS_BEGIN(Engine)

class ENGINE_DLL CRepeater_Node final : public CDecorator_Node
{
private:
	CRepeater_Node(_int iRepeatCount);
	virtual ~CRepeater_Node() = default;

public:
	virtual BTNODESTATE Tick(class CBlackBoard* BB) override;
	virtual void		Terminate(BTNODESTATE eState) override;
	virtual void		Abort() override;

private:
	_int				m_iRepeatCount = {};
	_int				m_iCurrentCount = {};

public:
	static CRepeater_Node*	Create(_int iRepeatCount);
	virtual void			Free() override;

};

NS_END