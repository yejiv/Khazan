#include "CoolDown_Nodel.h"
#include "BlackBoard.h"

CCoolDown_Node::CCoolDown_Node(_float fCoolTime)
    : m_fCoolTime{fCoolTime}
    , m_fElapsed {0.f}
{
}

BTNODESTATE CCoolDown_Node::Tick(CBlackBoard* BB)
{
    _float fTimeDelta = BB->Get_Value<_float>(m_strName, m_strTag);
    m_fElapsed += fTimeDelta;
    if (m_fElapsed < m_fCoolTime)
        return BTNODESTATE::FAILURE; // ÄðÅ¸ÀÓ Áß

    if (m_pChild)
    {
        BTNODESTATE eState = m_pChild->Tick(BB);

        if (BTNODESTATE::SUCCESS == eState)
        {
            m_fElapsed = 0.f;
        }

        return eState;
    }

 
    return BTNODESTATE::FAILURE;
}

void CCoolDown_Node::Terminate(BTNODESTATE eState)
{
    m_pChild->Terminate(eState);
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


