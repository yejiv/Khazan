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

    if (m_eDir == TARGET_DIR::FR)
        m_pMonData->iAnimIndex = 30;
    else if (m_eDir == TARGET_DIR::FL)
        m_pMonData->iAnimIndex = 31;
    else
        m_pMonData->iAnimIndex = 29;
}

void CAS_Dr_Melee_LockOn::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    m_fAccTime -= fTimeDelta;
    if (m_fAccTime <= 0.f)
    {
        m_eDir = m_pMonData->pOwner->Get_DIR();
        m_fAccTime = 1.f;

        _bool isMinRange = m_pMonData->pOwner->Check_Ranage(m_pMonData->pOwner->Get_Controller()->Get_BlackBoard()->Get_Value<_float>(m_pMonData->pOwner->Get_Name(), "AttackRange"));
        _bool isMaxRange = m_pMonData->pOwner->Check_Ranage(m_pMonData->pOwner->Get_Controller()->Get_BlackBoard()->Get_Value<_float>(m_pMonData->pOwner->Get_Name(), "AttackRange_Middle") + 2.f);

        if (!isMaxRange || m_pMonData->fAttackCool <= 0.f)
            m_eLockOn = LOCKONSTATE::LOCKON_F;
        else if (isMinRange)
            m_eLockOn = LOCKONSTATE::LOCKON_B;
        else
        {
            if (m_eLockOn != LOCKONSTATE::LOCKON_L && m_eLockOn != LOCKONSTATE::LOCKON_R)
            {
                if (m_eDir == TARGET_DIR::FR || m_eDir == TARGET_DIR::R || m_eDir == TARGET_DIR::BR || m_eDir == TARGET_DIR::F)    //오른쪽
                    m_eLockOn = LOCKONSTATE::LOCKON_L;
                else
                    m_eLockOn = LOCKONSTATE::LOCKON_R;
            }
        }
    }

    if (LOCKONSTATE::LOCKON_F == m_eLockOn)
    {
        m_pMonData->iAnimIndex = 29;
        pOwner->Get_Transform()->Go_Straight(1.1f * fTimeDelta);
    }
    else if (LOCKONSTATE::LOCKON_L == m_eLockOn)
    {
        m_pMonData->iAnimIndex = 30;
        pOwner->Get_Transform()->Go_Left(fTimeDelta * 0.5f);
    }
    else if (LOCKONSTATE::LOCKON_R == m_eLockOn)
    {
        m_pMonData->iAnimIndex = 31;
        pOwner->Get_Transform()->Go_Right(fTimeDelta * 0.5);
    }
    else if (LOCKONSTATE::LOCKON_B == m_eLockOn)
    {
        m_pMonData->iAnimIndex = 28;
        pOwner->Get_Transform()->Go_Backward(fTimeDelta * 0.7f);
    }
    m_pMonData->pOwner->LockOnLerp(fTimeDelta, 4.f);
}

void CAS_Dr_Melee_LockOn::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
    m_pMonData->isLockOn = false;
}

CAS_Dr_Melee_LockOn* CAS_Dr_Melee_LockOn::Create()
{
    return new CAS_Dr_Melee_LockOn();
}

void CAS_Dr_Melee_LockOn::Free()
{
    __super::Free();
}
