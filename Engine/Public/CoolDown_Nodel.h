#pragma once
#include "Decorator_Node.h"

NS_BEGIN(Engine)

class ENGINE_DLL CCoolDown_Node final : public CDecorator_Node
{
private:
	CCoolDown_Node(_float fCoolTime);
	virtual ~CCoolDown_Node() = default;

public:
	virtual BTNODESTATE Tick(class CBlackBoard* BB) override;
	virtual void		Terminate(BTNODESTATE eState, class CBlackBoard* BB = nullptr) override;
	virtual void		Abort() override;

private:
	_float				m_fCoolTime = {};
	_float				m_fElapsed = {};
	string				m_strTag;
	string				m_strName;
	
	_bool				m_isCooling = { true };

public:
	static CCoolDown_Node*	Create(const string& strName, const string& strTag, _float fCoolTime);
	virtual void			Free() override;

};

NS_END