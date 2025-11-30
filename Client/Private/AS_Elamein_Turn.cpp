#include "AS_Elamein_Turn.h"

CAS_Elamein_Turn::CAS_Elamein_Turn()
{
}

void CAS_Elamein_Turn::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    if (m_pMonData == nullptr)
        m_pMonData = &static_cast<CElamein*>(pOwner)->Get_Data();
    m_eDir = m_pMonData->pOwner->Get_DIR();

    if (m_eDir == TARGET_DIR::BL)
    {
        m_pMonData->iAnimIndex = 103;
        m_fAngle = 180.f;
        m_pMonData->isAnimFinash = false;
    }
    else if (m_eDir == TARGET_DIR::BR || m_eDir == TARGET_DIR::B)
    {
        m_pMonData->iAnimIndex = 105;
        m_fAngle = 180.f;
        m_pMonData->isAnimFinash = false;
    }
    else if (m_eDir == TARGET_DIR::R )
    {
        m_pMonData->iAnimIndex = 104;
        m_fAngle = 90.f;
        m_pMonData->isAnimFinash = false;
    }
    else if (m_eDir == TARGET_DIR::L)
    {
        m_pMonData->iAnimIndex = 102;
        m_fAngle = -90.f;
        m_pMonData->isAnimFinash = false;
    }
    else
    {
        m_pMonData->isTurn = false;
        m_fAngle = 0.f;
    }
}

void CAS_Elamein_Turn::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    if (m_pMonData->isAnimFinash)
    {
        m_pMonData->fQuat = m_fAngle;
        m_pMonData->iAnimIndex = 101;
        m_pMonData->isTurn = false;
        m_pMonData->isBland = false;
    }
}

void CAS_Elamein_Turn::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
}

CAS_Elamein_Turn* CAS_Elamein_Turn::Create()
{
    return new CAS_Elamein_Turn();
}

void CAS_Elamein_Turn::Free()
{
    __super::Free();
}
