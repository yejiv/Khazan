#include "Inverter_Node.h"

CInverter_Node::CInverter_Node()
{
    m_eNodeType = NODETYPE::DECORATOR;
}

BTNODESTATE CInverter_Node::Tick(CBlackBoard* BB)
{
    if (nullptr == m_pChild)
        return BTNODESTATE::FAILURE;

    BTNODESTATE eState = m_pChild->Tick(BB);
    switch (eState)
    {
    case BTNODESTATE::SUCCESS:
        return BTNODESTATE::FAILURE;
    case BTNODESTATE::FAILURE:
        return BTNODESTATE::SUCCESS;
    case BTNODESTATE::RUNNING:
        return BTNODESTATE::RUNNING;
    }


    return BTNODESTATE::FAILURE;
}

void CInverter_Node::Terminate(BTNODESTATE eState, CBlackBoard* BB)
{
    if (m_pChild)
        m_pChild->Terminate(eState);
}

void CInverter_Node::Abort(CBlackBoard* BB)
{
    if (m_pChild)
         m_pChild->Abort(BB);
}


CInverter_Node* CInverter_Node::Create()
{
    return new CInverter_Node();
}

void CInverter_Node::Free()
{
    __super::Free();
}
