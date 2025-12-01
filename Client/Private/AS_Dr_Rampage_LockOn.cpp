#include "AS_Dr_Rampage_LockOn.h"
#include "AI_Controller.h"

CAS_Dr_Rampage_LockOn::CAS_Dr_Rampage_LockOn()
{
}

void CAS_Dr_Rampage_LockOn::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    if (m_pMonData == nullptr)
        m_pMonData = &static_cast<CDragonian_Rampage*>(pOwner)->Get_Data();

    m_eDir = m_pMonData->pOwner->Get_DIR();

    if (m_eDir == TARGET_DIR::FR)
        m_pMonData->iAnimIndex = 41;
    else if (m_eDir == TARGET_DIR::FL)
        m_pMonData->iAnimIndex = 42;
    else
        m_pMonData->iAnimIndex = 40;
}

void CAS_Dr_Rampage_LockOn::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    m_fAccTime -= fTimeDelta;
    if (m_fAccTime <= 0.f)
    {
        m_eDir = m_pMonData->pOwner->Get_DIR();
        m_fAccTime = 1.f;
    
        _bool isMinRange = m_pMonData->pOwner->Check_Ranage(m_pMonData->pOwner->Get_Controller()->Get_BlackBoard()->Get_Value<_float>(m_pMonData->pOwner->Get_Name(), "AttackRange") + 2.f);
        _bool isMaxRange = m_pMonData->pOwner->Check_Ranage(m_pMonData->pOwner->Get_Controller()->Get_BlackBoard()->Get_Value<_float>(m_pMonData->pOwner->Get_Name(), "AttackRange_Middle") + 2.f);
        
        if (!isMaxRange)
            m_eLockOn = LOCKONSTATE::LOCKON_F;
        else if (isMinRange)
            m_eLockOn = LOCKONSTATE::LOCKON_B;
        else
        {
            if (m_eLockOn != LOCKONSTATE::LOCKON_L && m_eLockOn != LOCKONSTATE::LOCKON_R)
            {
                if (m_eDir == TARGET_DIR::FR || m_eDir == TARGET_DIR::R || m_eDir == TARGET_DIR::BR || m_eDir == TARGET_DIR::F)    //¿À¸¥ÂÊ
                    m_eLockOn = LOCKONSTATE::LOCKON_L;
                else
                    m_eLockOn = LOCKONSTATE::LOCKON_R;
            }
        }
    }

    if (LOCKONSTATE::LOCKON_F == m_eLockOn)
    {
        m_pMonData->iAnimIndex = 40;
        pOwner->Get_Transform()->Go_Straight(1.25f * fTimeDelta);
    }
    else if (LOCKONSTATE::LOCKON_L == m_eLockOn)
    {
        m_pMonData->iAnimIndex = 41;
        pOwner->Get_Transform()->Go_Left(fTimeDelta * 0.7f);
    }
    else if (LOCKONSTATE::LOCKON_R == m_eLockOn)
    {
        m_pMonData->iAnimIndex = 42;
        pOwner->Get_Transform()->Go_Right(fTimeDelta * 0.7f);
    }
    else if (LOCKONSTATE::LOCKON_B == m_eLockOn)
    {
        m_pMonData->iAnimIndex = 39;
        pOwner->Get_Transform()->Go_Backward(fTimeDelta);
    }
    m_pMonData->pOwner->LockOnLerp(fTimeDelta, 4.f);

}

void CAS_Dr_Rampage_LockOn::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
    m_pMonData->isLockOn = false;
}

CAS_Dr_Rampage_LockOn* CAS_Dr_Rampage_LockOn::Create()
{
    return new CAS_Dr_Rampage_LockOn();
}

void CAS_Dr_Rampage_LockOn::Free()
{
    __super::Free();
}
