#pragma once
#include "BTNode.h"

NS_BEGIN(Engine)

class ENGINE_DLL CUtilityAction_Node final : public CBTNode
{
public:
    typedef function<_float(class CBlackBoard* BB)> SCORE;
    typedef function<BTNODESTATE(class CBlackBoard* BB)> ACTION;
    typedef function<void(class CBlackBoard*, BTNODESTATE)> TERMINATE;

private:
    CUtilityAction_Node(SCORE Score, ACTION Action, TERMINATE Terminate);
    virtual ~CUtilityAction_Node() = default;

public:
    // UtilitySelector가 호출하면 SCORE 계산 후 캐싱
    void UpdateScore(class CBlackBoard* BB)
    {
        if (m_Score)
            m_fCachedScore = m_Score(BB);
        else
            m_fCachedScore = 0.f;
    }

    // UtilitySelector는 캐시된 값을 사용해서 비교
    _float Get_Score() const { return m_fCachedScore; }

public:
    virtual BTNODESTATE Tick(class CBlackBoard* BB = nullptr) override;
    virtual void        Terminate(BTNODESTATE eState, class CBlackBoard* BB = nullptr) override;
    virtual void        Abort(class CBlackBoard* BB) override;

private:
    SCORE           m_Score;     // 스킬 점수 함수
    ACTION          m_Action;    // 스킬 실행 함수
    TERMINATE       m_Terminate; // 종료 함수

private:
    _float          m_fCachedScore = 0.f; // 최신 스코어 캐싱
    _bool            m_isExecuting = false; // 현재 실행 여부

public:
    static CUtilityAction_Node* Create(SCORE Score, ACTION Action, TERMINATE Terminate);
    virtual void Free() override;
};

NS_END
