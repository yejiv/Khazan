#pragma once
#include "BTNode.h"

NS_BEGIN(Engine)

class ENGINE_DLL CUtilityAction_Node final : public CBTNode
{
public:
    typedef function<_float(class CBlackBoard* BB)> SCORE;
    typedef function<BTNODESTATE(class CBlackBoard* BB)> ACTION;
    typedef	function<void(class CBlackBoard*, BTNODESTATE)> TERMINATE;

private:
    CUtilityAction_Node(SCORE Score,ACTION Action, TERMINATE Terminate);
    virtual ~CUtilityAction_Node() = default;

public:
    _float Get_Score(class CBlackBoard* BB) const
    {
        if (m_Score)
            return m_Score(BB);

        return 0.f;
    }


public:
    virtual BTNODESTATE		Tick(class CBlackBoard* BB = nullptr) override;
    virtual void			Terminate(BTNODESTATE eState, class CBlackBoard* BB = nullptr) override;
    virtual void            Abort(class CBlackBoard* BB) override;

private:
    SCORE                       m_Score;
    ACTION					    m_Action;
    TERMINATE				    m_Terminate;

public:
    static CUtilityAction_Node* Create(SCORE Score,ACTION Action, TERMINATE Terminate);
    virtual void			    Free() override;

};

NS_END