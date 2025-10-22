#include "Wait_Node.h"
#include "BlackBoard.h"

CWait_Node::CWait_Node(_float fWaitTime)
    : m_fWaitTime{fWaitTime}
    , m_fElapsed{ 0.f }
{

}

BTNODESTATE CWait_Node::Tick(CBlackBoard* BB)
{
    m_fElapsed += BB->Get_Value<_float>(m_strName, m_strTag);
    if (m_fElapsed >= m_fWaitTime)
    {
        m_fElapsed = 0.f;
        return BTNODESTATE::SUCCESS;
    }

    return BTNODESTATE::RUNNING;
}

void CWait_Node::Terminate(BTNODESTATE eState)
{
    m_fElapsed = 0.f;
}

void CWait_Node::Abort()
{
    m_fElapsed = 0.f;
}

CWait_Node* CWait_Node::Create(const string& strTag, const string& strName, _float fWaitTime)
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
