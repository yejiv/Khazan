#include "AS_Halberd_Turn.h"

CAS_Halberd_Turn::CAS_Halberd_Turn()
{
}

void CAS_Halberd_Turn::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    if (m_pMonData == nullptr)
        m_pMonData = &static_cast<CHalberd*>(pOwner)->Get_Data();
    m_eDir = m_pMonData->pOwner->Get_DIR();

    if (m_eDir == TARGET_DIR::BL)
    {
        m_pMonData->iAnimIndex = 68;
        m_fAngle = 180.f;
        m_pMonData->isAnimFinash = false;
    }
    else if (m_eDir == TARGET_DIR::BR || m_eDir == TARGET_DIR::B)
    {
        m_pMonData->iAnimIndex = 70;
        m_fAngle = 180.f;
        m_pMonData->isAnimFinash = false;
    }
    else if (m_eDir == TARGET_DIR::R )
    {
        m_pMonData->iAnimIndex = 69;
        m_fAngle = 90.f;
        m_pMonData->isAnimFinash = false;
    }
    else if (m_eDir == TARGET_DIR::L)
    {
        m_pMonData->iAnimIndex = 67;
        m_fAngle = -90.f;
        m_pMonData->isAnimFinash = false;
    }
    else
    {
        m_pMonData->isTurn = false;
        m_fAngle = 0.f;
    }
}

void CAS_Halberd_Turn::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    if (m_pMonData->isAnimFinash)
    {
        m_pMonData->fQuat = m_fAngle;
        m_pMonData->iAnimIndex = 101;
        m_pMonData->isTurn = false;
        m_pMonData->isBland = false;
    }
}

void CAS_Halberd_Turn::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
}

CAS_Halberd_Turn* CAS_Halberd_Turn::Create()
{
    return new CAS_Halberd_Turn();
}

void CAS_Halberd_Turn::Free()
{
    __super::Free();
}
