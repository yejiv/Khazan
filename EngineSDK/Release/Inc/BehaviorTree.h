#pragma once
#include "Base.h"

NS_BEGIN(Engine)

enum class BTEVENT { DEAD, HIT, END };

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
    void                        Set_Name(const string& strName) { m_strName = strName; }
    CBTNode*                    Get_Root() const { return m_pRoot; }

public:
    BTEVENT                     Notify_Event(BTEVENT Event);
    void                        Terminate_AllNode(CBTNode* pNode);

protected:
	class CBTNode*				m_pRoot = { nullptr };
	class CBlackBoard*			m_pBlackBoard = { nullptr };
	BTNODESTATE					m_ePrevState = { BTNODESTATE::END };
    string                      m_strName = {};


public:
	static CBehaviorTree*		Create();
	virtual void				Free() override;

};

NS_END