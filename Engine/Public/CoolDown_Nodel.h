#pragma once
#include "Decorator_Node.h"

NS_BEGIN(Engine)

class CCoolDown_Node final : public CDecorator_Node
{
private:
	CCoolDown_Node(_float fCoolTime);
	virtual ~CCoolDown_Node() = default;

public:
	virtual BTNODESTATE Tick(class CBlackBoard* BB) override;
	virtual void		Terminate(BTNODESTATE eState) override;
	virtual void		Abort() override;

private:
	_float				m_fCoolTime = {};
	_float				m_fElapsed = {};
	string				m_strTag;
	string				m_strName;

public:
	static CCoolDown_Node*	Create(const string& strName, const string& strTag, _float fCoolTime);
	virtual void			Free() override;

};

NS_END