#include "Decorator_Node.h"

CDecorator_Node::CDecorator_Node()
{
}

void CDecorator_Node::Terminate(BTNODESTATE eState, CBlackBoard* BB)
{

}

void CDecorator_Node::Set_Child(CBTNode* pChild)
{
	m_pChild = pChild;
}

void CDecorator_Node::Free()
{
	Safe_Release(m_pChild);
	__super::Free();
}
