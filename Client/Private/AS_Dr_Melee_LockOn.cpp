#include "AS_Dr_Melee_LockOn.h"
#include "AI_Controller.h"
#include "BlackBoard.h"

CAS_Dr_Melee_LockOn::CAS_Dr_Melee_LockOn()
{
}

void CAS_Dr_Melee_LockOn::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    if (m_pMonData == nullptr)
        m_pMonData = &static_cast<CDragonian_Melee*>(pOwner)->Get_Data();

    m_eDir = m_pMonData->pOwner->Get_DIR();

    if (m_eDir == TARGET_DIR::FR || m_eDir == TARGET_DIR::F)
        m_pMonData->iAnimIndex = 30;
    else if (m_eDir == TARGET_DIR::FL)
        m_pMonData->iAnimIndex = 31;
    else if (m_eDir == TARGET_DIR::F)
        m_pMonData->iAnimIndex = 29;
    //턴
    else if (m_eDir == TARGET_DIR::BL)
    {
        m_pMonData->isTurn = true;
        m_eState = AI_STATE::TURN;
        m_pMonData->iAnimIndex = 53;
        m_fAngle = 180.f;
    }
    else if (m_eDir == TARGET_DIR::BR || m_eDir == TARGET_DIR::B)
    {
        m_pMonData->isTurn = true;
        m_eState = AI_STATE::TURN;
        m_pMonData->iAnimIndex = 55;
        m_fAngle = 180.f;
    }
    else if (m_eDir == TARGET_DIR::R)
    {
        m_pMonData->isTurn = true;
        m_eState = AI_STATE::TURN;
        m_pMonData->iAnimIndex = 54;
        m_fAngle = 90.f;
    }
    else if (m_eDir == TARGET_DIR::L)
    {
        m_pMonData->isTurn = true;
        m_eState = AI_STATE::TURN;
        m_pMonData->iAnimIndex = 52;
        m_fAngle = -90.f;
    }
    m_fDeley = 3.f;
}

void CAS_Dr_Melee_LockOn::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{

    if (m_eState == AI_STATE::TURN)
    {
        if (m_pMonData->isAnimFinash)
        {
            m_pMonData->fQuat = m_fAngle;
                
            m_eState = AI_STATE::STAND;

            m_pMonData->iAnimIndex = 35;
        }
    }
    else if (m_eState == AI_STATE::STAND)
    {
        m_pMonData->isTurn = false;
        m_eDir = m_pMonData->pOwner->Get_DIR();
        if (m_eDir == TARGET_DIR::FR || m_eDir == TARGET_DIR::R || m_eDir == TARGET_DIR::BR || m_eDir == TARGET_DIR::F)
            m_pMonData->iAnimIndex = 30;
        else if (m_eDir == TARGET_DIR::FL || m_eDir == TARGET_DIR::L || m_eDir == TARGET_DIR::BL || m_eDir == TARGET_DIR::B)
            m_pMonData->iAnimIndex = 31;

        m_eState = AI_STATE::WALK;
    }
    else
    {
        m_pMonData->isLockOn = true;
        if(m_fDeley > 0.f)
            m_fDeley -= fTimeDelta;
        else
            m_pMonData->isLockOn = false;
        
        m_eDir = m_pMonData->pOwner->Get_DIR();

        if (m_eDir == TARGET_DIR::FR)
            m_pMonData->iAnimIndex = 30;
        else if (m_eDir == TARGET_DIR::FL)
            m_pMonData->iAnimIndex = 31;
        
        m_pMonData->pOwner->LockOnLerp(fTimeDelta);

        if (m_pMonData->pOwner->Check_Ranage(m_pMonData->pOwner->Get_Controller()->Get_BlackBoard()->Get_Value<_float>(m_pMonData->pOwner->Get_Name(), "AttackRange") * 0.5f))
        {
            pOwner->Get_Transform()->Go_Backward(fTimeDelta * 0.5f);

        }
        
        if (m_eDir == TARGET_DIR::FR || m_eDir == TARGET_DIR::R || m_eDir == TARGET_DIR::BR || m_eDir == TARGET_DIR::F)    //오른쪽
            pOwner->Get_Transform()->Go_Left(fTimeDelta);
        else
            pOwner->Get_Transform()->Go_Right(fTimeDelta);
    }
}

void CAS_Dr_Melee_LockOn::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
    m_eState = AI_STATE::WALK;
}

CAS_Dr_Melee_LockOn* CAS_Dr_Melee_LockOn::Create()
{
    return new CAS_Dr_Melee_LockOn();
}

void CAS_Dr_Melee_LockOn::Free()
{
    __super::Free();
}
