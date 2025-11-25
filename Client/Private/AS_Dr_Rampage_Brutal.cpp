#include "AS_Dr_Rampage_Brutal.h"

CAS_Dr_Rampage_Brutal::CAS_Dr_Rampage_Brutal()
{
}

void CAS_Dr_Rampage_Brutal::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    if (m_pMonData == nullptr)
        m_pMonData = &static_cast<CDragonian_Rampage*>(pOwner)->Get_Data();

    m_pMonData->iAnimIndex = 28;
}

void CAS_Dr_Rampage_Brutal::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    if (m_pMonData->isAnimFinash)
    {
        m_pMonData->eHitType = HITREACTION::END;
        *m_pMonData->pCulStamina = *m_pMonData->pMaxStamina;
    }
}

void CAS_Dr_Rampage_Brutal::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
}

CAS_Dr_Rampage_Brutal* CAS_Dr_Rampage_Brutal::Create()
{
    return new CAS_Dr_Rampage_Brutal();
}

void CAS_Dr_Rampage_Brutal::Free()
{
    __super::Free();
}
