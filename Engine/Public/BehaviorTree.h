#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class ENGINE_DLL CBehaviorTree final : public CBase
{
protected:
	CBehaviorTree();
	virtual ~CBehaviorTree() = default;

public:
	virtual HRESULT				Initialize(void* pArg = nullptr);
	BTNODESTATE					Update();
	void						Set_Root(class CBTNode* pRoot);
	void						Set_BlackBoard(class CBlackBoard* BB);

protected:
	class CBTNode*				m_pRoot = { nullptr };
	class CBlackBoard*			m_pBlackBoard = { nullptr };
	BTNODESTATE					m_ePrevState = { BTNODESTATE::END };



public:
	static CBehaviorTree*		Create();
	virtual void				Free() override;

};

NS_END