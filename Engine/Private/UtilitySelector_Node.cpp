#include "UtilitySelector_Node.h"

CUtilitySelector_Node::CUtilitySelector_Node()
{
    m_eNodeType = NODETYPE::COMPOSITE;
}

BTNODESTATE CUtilitySelector_Node::Tick(CBlackBoard* BB)
{
 
    //if (m_iCurrentIndex < m_UtilityChildren.size())
    //{
    //    auto& pRunningNode = m_UtilityChildren[m_iCurrentIndex];
    //    BTNODESTATE eState = pRunningNode->Tick(BB);

    //    // RUNNIG
    //    if (eState == BTNODESTATE::RUNNING)
    //        return BTNODESTATE::RUNNING;

    //    pRunningNode->Terminate(eState,BB);
    //}



    // 모든 Action Score 재계산

    for (auto& fAction : m_UtilityChildren)
    {
        fAction->UpdateScore(BB);
    }

    
    // 최고 점수 Action 찾기

    CUtilityAction_Node* pBestAction = nullptr;
    _float fBestScore = -FLT_MAX;

    for (auto& fAction : m_UtilityChildren)
    {
        _float score = fAction->Get_Score();
        if (score > fBestScore)
        {
            fBestScore = score;
            pBestAction = fAction;
        }
    }

    // 모든 스킬 점수가 0 이하면선택 불가
    if (fBestScore <= 0.f)
    {
        m_pRunningAction = nullptr;
        return BTNODESTATE::FAILURE;
    }

    // 현재 실행 중인 Action 비교

    //m_iCurrentIndex = iBestIndex;
    //auto& pBestNode = m_UtilityChildren[m_iCurrentIndex];
    if (nullptr != m_pRunningAction && m_pRunningAction != pBestAction)
    {
        //더 높은 점수 Action이 등장하면 교체
        m_pRunningAction->Abort(BB);
        Safe_Release(m_pRunningAction);
    }


    // 새 Action 실행 혹은 기존 유지
    m_pRunningAction = pBestAction;

    return m_pRunningAction->Tick(BB);
}

void CUtilitySelector_Node::Terminate(BTNODESTATE eState, CBlackBoard* BB)
{
    if (m_pRunningAction)
    {
        m_pRunningAction->Terminate(eState, BB);
        m_pRunningAction = nullptr;
    }
}

void CUtilitySelector_Node::Abort(CBlackBoard* BB)
{
    if (m_pRunningAction)
    {
        m_pRunningAction->Abort(BB);
        m_pRunningAction = nullptr;
    }
}

void CUtilitySelector_Node::Add_Child(CBTNode* pNode)
{
    if (auto* pAction = dynamic_cast<CUtilityAction_Node*>(pNode))
    {
        m_UtilityChildren.push_back(pAction);
    }

    m_Children.push_back(pNode);
    Safe_AddRef(pNode);
}

CUtilitySelector_Node* CUtilitySelector_Node::Create()
{
    return new CUtilitySelector_Node();
}

void CUtilitySelector_Node::Free()
{
    __super::Free();
}
