#pragma once
#include "Base.h"

NS_BEGIN(Engine)

enum class NODETYPE { COMPOSITE, DECORATOR, LEAF, END };

class ENGINE_DLL CBTNode abstract : public CBase
{
protected:
	CBTNode();
	virtual ~CBTNode() = default;

public:
	BTNODESTATE			Get_CurrentNodeState() const { return m_eCurrentNodeState; }
    NODETYPE            Get_NodeType() const { return m_eNodeType; }
public:
	// 노드를 실행하고 실행 상태를 반환
	virtual BTNODESTATE Tick(class CBlackBoard* BB) = 0;

	// 노드 종료 처리 : 성공 / 실패 후 정리기능
	virtual void Terminate(BTNODESTATE eState, class CBlackBoard* BB = nullptr) {};
	// 노드 강제 중단 : 외부 이벤트나 상위 노드 요청시 노드들을 강제 중단
	virtual void Abort(class CBlackBoard* BB ) { Terminate(BTNODESTATE::FAILURE, BB); }

protected:
	BTNODESTATE				m_eCurrentNodeState = { BTNODESTATE::END };
    NODETYPE                m_eNodeType = { NODETYPE::END };
public:
	virtual void Free() override;

};

NS_END