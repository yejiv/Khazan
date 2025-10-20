#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class ENGINE_DLL CBTNode abstract : public CBase
{
protected:
	CBTNode();
	virtual ~CBTNode() = default;

public:
	// 노드를 실행하고 실행 상태를 반환
	virtual BTNODESTATE Tick(class CBlackBoard* BB) = 0;

	// 노드 종료 처리 : 성공 / 실패 후 정리기능
	virtual void Terminate(BTNODESTATE eState) {};
	// 노드 강제 중단 : 외부 이벤트나 상위 노드 요청시 노드들을 강제 중단
	virtual void Abort() { Terminate(BTNODESTATE::FAILURE); }

public:
	virtual void Free() override;

};

NS_END