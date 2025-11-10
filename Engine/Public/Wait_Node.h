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
	virtual void		Terminate(BTNODESTATE eState, class CBlackBoard* BB = nullptr) override;
    virtual void        Abort(class CBlackBoard* BB) override;


private:
	_float				m_fWaitTime = {};
	_float				m_fElapsed = {};
	_float				m_fStartTime = {};

	string				m_strTag;
	string				m_strName;

	_bool				m_isStart = { false };


public:
	static CWait_Node*	Create(const string& strName, const string& strTag, _float fWaitTime);
	virtual void		Free() override;

};

NS_END