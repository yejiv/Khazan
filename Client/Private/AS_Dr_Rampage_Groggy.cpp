#include "AS_Dr_Rampage_Groggy.h"

CAS_Dr_Rampage_Groggy::CAS_Dr_Rampage_Groggy()
{
}

void CAS_Dr_Rampage_Groggy::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    if (m_pMonData == nullptr)
        m_pMonData = &static_cast<CDragonian_Rampage*>(pOwner)->Get_Data();
}

void CAS_Dr_Rampage_Groggy::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
}

void CAS_Dr_Rampage_Groggy::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
}

CAS_Dr_Rampage_Groggy* CAS_Dr_Rampage_Groggy::Create()
{
    return new CAS_Dr_Rampage_Groggy();
}

void CAS_Dr_Rampage_Groggy::Free()
{
    __super::Free();
}
