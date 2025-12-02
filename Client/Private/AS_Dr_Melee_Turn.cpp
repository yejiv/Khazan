#include "AS_Dr_Melee_Turn.h"
#include "AI_Controller.h"
#include "BlackBoard.h"

CAS_Dr_Melee_Turn::CAS_Dr_Melee_Turn()
{
}

void CAS_Dr_Melee_Turn::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    if (m_pMonData == nullptr)
        m_pMonData = &static_cast<CDragonian_Melee*>(pOwner)->Get_Data();
    m_eDir = m_pMonData->pOwner->Get_DIR();

    if (m_eDir == TARGET_DIR::BL)
    {
        m_pMonData->iAnimIndex = 53;
        m_fAngle = 180.f;
    }
    else if (m_eDir == TARGET_DIR::BR || m_eDir == TARGET_DIR::B)
    {
        m_pMonData->iAnimIndex = 55;
        m_fAngle = 180.f;
    }
    else if (m_eDir == TARGET_DIR::R)
    {
        m_pMonData->iAnimIndex = 54;
        m_fAngle = 90.f;
    }
    else if (m_eDir == TARGET_DIR::L)
    {
        m_pMonData->iAnimIndex = 52;
        m_fAngle = -90.f;
    }
    else
        m_pMonData->isTurn = false;

    m_pMonData->isAnimFinash = false;
}

void CAS_Dr_Melee_Turn::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    if (m_pMonData->isAnimFinash)
    {
        m_pMonData->fQuat = m_fAngle;
        m_pMonData->iAnimIndex = 51;
        m_pMonData->isTurn = false;
        m_pMonData->isBland = false;
    }
}

void CAS_Dr_Melee_Turn::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
    m_pMonData->isLockOn = false;
}

CAS_Dr_Melee_Turn* CAS_Dr_Melee_Turn::Create()
{
    return new CAS_Dr_Melee_Turn();
}

void CAS_Dr_Melee_Turn::Free()
{
    __super::Free();
}
