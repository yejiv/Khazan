#include "AS_Halberd_LockOn.h"
#include "AI_Controller.h"

CAS_Halberd_LockOn::CAS_Halberd_LockOn()
{
}

void CAS_Halberd_LockOn::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    if (m_pMonData == nullptr)
        m_pMonData = &static_cast<CHalberd*>(pOwner)->Get_Data();

    m_eDir = m_pMonData->pOwner->Get_DIR();

    if (m_eDir == TARGET_DIR::FR)
        m_pMonData->iAnimIndex = 45;
    else if (m_eDir == TARGET_DIR::FL)
        m_pMonData->iAnimIndex = 46;
    else
        m_pMonData->iAnimIndex = 44;
}

void CAS_Halberd_LockOn::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    m_fAccTime -= fTimeDelta;
    if (m_fAccTime <= 0.f)
    {
        m_eDir = m_pMonData->pOwner->Get_DIR();
        m_fAccTime = 1.5f;

        _bool isMinRange = m_pMonData->pOwner->Check_Ranage("AttackRange");
        _bool isMaxRange = m_pMonData->pOwner->Check_Ranage("AttackRange_Middle");

        if (!isMaxRange || m_pMonData->fAttackCool <= 0.f)
             m_eLockOn = LOCKONSTATE::LOCKON_F;
        else if (isMinRange)
            m_eLockOn = LOCKONSTATE::LOCKON_B;
        else
        {
            if (m_eDir == TARGET_DIR::FR || m_eDir == TARGET_DIR::R || m_eDir == TARGET_DIR::BR || m_eDir == TARGET_DIR::F)    //오른쪽
                m_eLockOn = LOCKONSTATE::LOCKON_L;
            else
                m_eLockOn = LOCKONSTATE::LOCKON_R;
        }

    }

    if (LOCKONSTATE::LOCKON_F == m_eLockOn)
    {
        m_pMonData->iAnimIndex = 44;
        pOwner->Get_Transform()->Go_Straight(0.8f * fTimeDelta);
    }
    else if (LOCKONSTATE::LOCKON_L == m_eLockOn)
    {
        m_pMonData->iAnimIndex = 45;
        pOwner->Get_Transform()->Go_Left(0.55f * fTimeDelta);
    }
    else if (LOCKONSTATE::LOCKON_R == m_eLockOn)
    {
        m_pMonData->iAnimIndex = 46;
        pOwner->Get_Transform()->Go_Right(0.55f * fTimeDelta);
    }
    else if (LOCKONSTATE::LOCKON_B == m_eLockOn)
    {
        m_pMonData->iAnimIndex = 43;
        pOwner->Get_Transform()->Go_Backward(0.6f * fTimeDelta);
    }
    m_pMonData->pOwner->LockOnLerp(fTimeDelta, 5.f);
}

void CAS_Halberd_LockOn::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{

}

CAS_Halberd_LockOn* CAS_Halberd_LockOn::Create()
{
    return new CAS_Halberd_LockOn();
}

void CAS_Halberd_LockOn::Free()
{
    __super::Free();
}
