#include "Wait_Node.h"
#include "BlackBoard.h"

CWait_Node::CWait_Node(_float fWaitTime)
    : m_fWaitTime{fWaitTime}
    , m_fElapsed{ 0.f }
{
    m_eNodeType = NODETYPE::LEAF;
}

BTNODESTATE CWait_Node::Tick(CBlackBoard* BB)
{

    _float fCurrentTime = BB->Get_Value<_float>(m_strName, m_strTag);

    if (!m_isStart)
    {
        m_fStartTime = fCurrentTime;
        m_isStart = true;
        m_fElapsed = 0.f;
    }

    m_fElapsed = fCurrentTime - m_fStartTime;

    if (m_fElapsed >= m_fWaitTime)
    {
        m_isStart = false;
        m_fElapsed = 0.f;
        return BTNODESTATE::SUCCESS;
    }

    return BTNODESTATE::RUNNING;
}

void CWait_Node::Terminate(BTNODESTATE eState, CBlackBoard* BB)
{

}

void CWait_Node::Abort(CBlackBoard* BB)
{
}


CWait_Node* CWait_Node::Create(const string& strName, const string& strTag, _float fWaitTime)
{
    CWait_Node* pInstance = new CWait_Node(fWaitTime);
    pInstance->m_strName = strName;
    pInstance->m_strTag = strTag;
   
    return pInstance;
}

void CWait_Node::Free()
{
    __super::Free();
}
