#include "Composite_Node.h"

CComposite_Node::CComposite_Node()
{

}

void CComposite_Node::Add_Child(CBTNode* pChild)
{
	if (pChild != nullptr)
		m_Children.push_back(pChild);
		
}

void CComposite_Node::Free()
{
	__super::Free();

	for (auto& child : m_Children)
		Safe_Release(child);
	m_Children.clear();

}

