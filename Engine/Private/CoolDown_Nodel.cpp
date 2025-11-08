#include "CoolDown_Nodel.h"
#include "BlackBoard.h"

CCoolDown_Node::CCoolDown_Node(_float fCoolTime)
    : m_fCoolTime{fCoolTime}
    , m_fElapsed {0.f}
{
}

BTNODESTATE CCoolDown_Node::Tick(CBlackBoard* BB)
{
    _float fCurrentTime = BB->Get_Value<_float>(m_strName, m_strTag);
 
    // ��Ÿ�� ���̸� �ڽ� ������ϵ��� ���´�.
    if (m_isCooling)
    {
        m_fElapsed = fCurrentTime - m_fStartTime;
        if (m_fElapsed >= m_fCoolTime)
        {
            m_isCooling = false;
        }
        else
        {
            return BTNODESTATE::FAILURE;
        }
    }
   
    if (m_pChild && !m_isCooling)
    {
        BTNODESTATE eState = m_pChild->Tick(BB);
        // 자식이 성공하면 다시 쿨타임 돌리기
        if (BTNODESTATE::SUCCESS == eState)
        {
            m_isCooling = true;
            m_fStartTime = fCurrentTime;
            m_fElapsed = 0.f;
        }
        return eState;

    }
    return BTNODESTATE::FAILURE;
}

void CCoolDown_Node::Terminate(BTNODESTATE eState, CBlackBoard* BB)
{
    if (nullptr == m_pChild)
        return;
    
    m_pChild->Terminate(eState,BB);

}

void CCoolDown_Node::Abort()
{
    m_pChild->Abort();

}

CCoolDown_Node* CCoolDown_Node::Create(const string& strName, const string& strTag, _float fCoolTime)
{
    CCoolDown_Node* pInstance = new CCoolDown_Node(fCoolTime);
    pInstance->m_strName = strName;
    pInstance->m_strTag = strTag;
    return pInstance;
}

void CCoolDown_Node::Free()
{
    __super::Free();
}


