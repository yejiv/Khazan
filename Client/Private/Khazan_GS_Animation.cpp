#include "Khazan_GS_Animation.h"

CKhazan_GS_Animation::CKhazan_GS_Animation()
{
}

void CKhazan_GS_Animation::Enter()
{
}

void CKhazan_GS_Animation::Continue(_float fTimeDelta)
{
}

void CKhazan_GS_Animation::Exit()
{
}

inline _bool CKhazan_GS_Animation::Has_States(_uint iMaxState)
{
    for (_uint i = 0; i < iMaxState; ++i)
    {
        if (Has_State(1 << i))
            return true;
    }

    return false;
}

void CKhazan_GS_Animation::Free()
{
    __super::Free();
    Safe_Release(m_pModel);
}
