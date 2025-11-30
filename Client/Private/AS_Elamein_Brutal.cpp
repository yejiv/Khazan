#include "AS_Elamein_Brutal.h"

CAS_Elamein_Brutal::CAS_Elamein_Brutal()
{
}

void CAS_Elamein_Brutal::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    if (m_pMonData == nullptr)
        m_pMonData = &static_cast<CElamein*>(pOwner)->Get_Data();

    TARGET_DIR eDir = m_pMonData->pOwner->Get_DIR();

    if (eDir == TARGET_DIR::B || eDir == TARGET_DIR::BR || eDir == TARGET_DIR::BL)
        m_pMonData->iAnimIndex = 7;
    else
        m_pMonData->iAnimIndex = 6;

}

void CAS_Elamein_Brutal::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    if (m_pMonData->isAnimFinash)
    {
        m_pMonData->eHitType = HITREACTION::END;
        *m_pMonData->pCulStamina = *m_pMonData->pMaxStamina;
    }
}

void CAS_Elamein_Brutal::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
}

CAS_Elamein_Brutal* CAS_Elamein_Brutal::Create()
{
    return new CAS_Elamein_Brutal();
}

void CAS_Elamein_Brutal::Free()
{
    __super::Free();
}
