#include "AS_Dr_Rampage_Page2.h"

CAS_Dr_Rampage_Page2::CAS_Dr_Rampage_Page2()
{
}

void CAS_Dr_Rampage_Page2::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    if (m_pMonData == nullptr)
        m_pMonData = &static_cast<CDragonian_Rampage*>(pOwner)->Get_Data();

    m_pMonData->iAnimIndex = 13;
}

void CAS_Dr_Rampage_Page2::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    if (m_pMonData->isAnimFinash)
    {
        m_pMonData->is2Page = true;
        m_pMonData->isLockOn = true;
    }
}

void CAS_Dr_Rampage_Page2::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
}

CAS_Dr_Rampage_Page2* CAS_Dr_Rampage_Page2::Create()
{
    return new CAS_Dr_Rampage_Page2();
}

void CAS_Dr_Rampage_Page2::Free()
{
    __super::Free();
}
