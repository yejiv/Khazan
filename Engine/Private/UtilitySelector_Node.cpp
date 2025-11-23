#include "UtilitySelector_Node.h"

CUtilitySelector_Node::CUtilitySelector_Node()
{
    m_eNodeType = NODETYPE::COMPOSITE;
}

BTNODESTATE CUtilitySelector_Node::Tick(CBlackBoard* BB)
{
   
    // 현재 RUNNING 중인 노드가 있는 경우 먼저 실행하도록
    if (m_iCurrentIndex < m_UtilityChildren.size())
    {
        auto& pRunningNode = m_UtilityChildren[m_iCurrentIndex];
        BTNODESTATE eState = pRunningNode->Tick(BB);

        // RUNNIG유지
        if (eState == BTNODESTATE::RUNNING)
            return BTNODESTATE::RUNNING;

        // 끝나면 Terminate호출시키도록
        pRunningNode->Terminate(eState,BB);
    }

    // 새로 점수를 계산해서 최고 점수의 Action 을 선택하도록 한다.


    _float fBestScore = -FLT_MAX;
    _uint iBestIndex = 0;

    for (_uint i = 0; i < m_UtilityChildren.size(); ++i)
    {
        auto& pChild = m_UtilityChildren[i];
        _float fScore = pChild->Get_Score(BB);

        if (fScore > fBestScore)
        {
            fBestScore = fScore;
            iBestIndex = i;
        }
    }

    if (fBestScore <= 0.f)
    {
        m_iCurrentIndex = 0;
        return BTNODESTATE::FAILURE;
    }


    // 계산 후 해당 노드를 새 RUNNING 노드로 설정
    m_iCurrentIndex = iBestIndex;
    auto& pBestNode = m_UtilityChildren[m_iCurrentIndex];

    return pBestNode->Tick(BB);

}

void CUtilitySelector_Node::Terminate(BTNODESTATE eState, CBlackBoard* BB)
{
    if (m_iCurrentIndex < m_UtilityChildren.size())
        m_UtilityChildren[m_iCurrentIndex]->Terminate(eState, BB);

    m_iCurrentIndex = 0;
}

void CUtilitySelector_Node::Abort(CBlackBoard* BB)
{
    if (m_iCurrentIndex < m_UtilityChildren.size())
        m_UtilityChildren[m_iCurrentIndex]->Abort(BB);

    m_iCurrentIndex = 0;
}

void CUtilitySelector_Node::Add_Child(CBTNode* pChild)
{
    if (nullptr != pChild)
    {
        m_Children.push_back(pChild);
        Safe_AddRef(pChild);

        if (auto* pUtility = dynamic_cast<CUtilityAction_Node*>(pChild))
        {
            m_UtilityChildren.push_back(pUtility);
        }

    }
}

CUtilitySelector_Node* CUtilitySelector_Node::Create()
{
    return new CUtilitySelector_Node();
}

void CUtilitySelector_Node::Free()
{
    __super::Free();
}
