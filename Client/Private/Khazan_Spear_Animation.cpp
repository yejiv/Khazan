#include "Khazan_Spear_Animation.h"

CKhazan_Spear_Animation::CKhazan_Spear_Animation()
{
}

void CKhazan_Spear_Animation::Enter()
{
}

void CKhazan_Spear_Animation::Continue(_float fTimeDelta)
{
}

void CKhazan_Spear_Animation::Exit()
{
}



inline _bool CKhazan_Spear_Animation::Has_States(_uint iMaxState)
{
	for (_uint i = 0; i < iMaxState; ++i)
	{
		if (Has_State(1 << i))
			return true;
	}

	return false;
}

void CKhazan_Spear_Animation::Free()
{
	__super::Free();
	Safe_Release(m_pModel);
}
