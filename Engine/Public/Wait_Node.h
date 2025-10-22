#pragma once
#include "Decorator_Node.h"

NS_BEGIN(Engine)

class ENGINE_DLL CWait_Node final : public CBTNode
{
private:
	CWait_Node(_float fWaitTime);
	virtual ~CWait_Node() = default;

public:
	virtual BTNODESTATE Tick(class CBlackBoard* BB) override;
	virtual void		Terminate(BTNODESTATE eState) override;
	virtual void		Abort() override;

private:
	_float				m_fWaitTime = {};
	_float				m_fElapsed = {};
	string				m_strTag;
	string				m_strName;


public:
	static CWait_Node*	Create(const string& strTag, const string& strName,_float fWaitTime);
	virtual void		Free() override;

};

NS_END