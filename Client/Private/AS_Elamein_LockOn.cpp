#include "AS_Elamein_LockOn.h"
#include "AI_Controller.h"

CAS_Elamein_LockOn::CAS_Elamein_LockOn()
{
}

void CAS_Elamein_LockOn::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    if (m_pMonData == nullptr)
        m_pMonData = &static_cast<CElamein*>(pOwner)->Get_Data();

    m_eDir = m_pMonData->pOwner->Get_DIR();

    if (m_eDir == TARGET_DIR::FR)
        m_pMonData->iAnimIndex = 62;
    else if (m_eDir == TARGET_DIR::FL)
        m_pMonData->iAnimIndex = 61;
    else
        m_pMonData->iAnimIndex = 60;
}

void CAS_Elamein_LockOn::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    m_fAccTime -= fTimeDelta;
    if (m_fAccTime <= 0.f)
    {
        m_eDir = m_pMonData->pOwner->Get_DIR();
        m_fAccTime = 1.5f;

        _bool isMinRange = m_pMonData->pOwner->Check_Ranage(m_pMonData->pOwner->Get_Controller()->Get_BlackBoard()->Get_Value<_float>(m_pMonData->pOwner->Get_Name(), "AttackRange") * 0.5f);
        _bool isMaxRange = m_pMonData->pOwner->Check_Ranage(m_pMonData->pOwner->Get_Controller()->Get_BlackBoard()->Get_Value<_float>(m_pMonData->pOwner->Get_Name(), "AttackRange"));

        if (!isMaxRange)
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
        m_pMonData->iAnimIndex = 60;
        pOwner->Get_Transform()->Go_Straight(1.25f * fTimeDelta);
    }
    else if (LOCKONSTATE::LOCKON_L == m_eLockOn)
    {
        m_pMonData->iAnimIndex = 61;
        pOwner->Get_Transform()->Go_Left(1.1f * fTimeDelta);
    }
    else if (LOCKONSTATE::LOCKON_R == m_eLockOn)
    {
        m_pMonData->iAnimIndex = 62;
        pOwner->Get_Transform()->Go_Right(1.1f * fTimeDelta);
    }
    else if (LOCKONSTATE::LOCKON_B == m_eLockOn)
    {
        m_pMonData->iAnimIndex = 59;
        pOwner->Get_Transform()->Go_Backward(fTimeDelta);
    }
    //wasm_pMonData->pOwner->LockOnLerp(fTimeDelta);
}

void CAS_Elamein_LockOn::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{

}

CAS_Elamein_LockOn* CAS_Elamein_LockOn::Create()
{
    return new CAS_Elamein_LockOn();
}

void CAS_Elamein_LockOn::Free()
{
    __super::Free();
}
