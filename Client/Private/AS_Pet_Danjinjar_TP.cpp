#include "AS_Pet_Danjinjar_TP.h"
#include "GameInstance.h"

CAS_Pet_Danjinjar_TP::CAS_Pet_Danjinjar_TP()
{
}

void CAS_Pet_Danjinjar_TP::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    if (m_pMonData == nullptr)
        m_pMonData = &static_cast<CPet_Danjinjar*>(pOwner)->Get_Data();
    m_pMonData->iAnimIndex = 18;
    m_eState = READY;
    m_pMonData->isTP = true;
    m_pMonData->isAnimFinash = false;
    m_pMonData->pOwner->isTalk(true, 1002);
}

void CAS_Pet_Danjinjar_TP::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    if (m_eState == READY)
    {
        if (m_pMonData->isAnimFinash)
        {
            m_pMonData->pOwner->Player_TP();
            m_eState = ATION;
            m_fAcctime = 0.f;
            m_pMonData->isTPDanjin = true;
            m_pMonData->iAnimIndex = 19;
        }
    }
    else if (m_eState == ATION)
    {
        m_pMonData->iAnimIndex = 18;
        m_fAcctime += fTimeDelta;
        m_pMonData->pOwner->LockOnLerp(fTimeDelta, 3.5f);
        if (m_fAcctime >= 2.f)
        {
            m_pMonData->isTP = false;
            m_pMonData->isTPDanjin = false;
        }
    }

}

void CAS_Pet_Danjinjar_TP::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
}

CAS_Pet_Danjinjar_TP* CAS_Pet_Danjinjar_TP::Create()
{
    return new CAS_Pet_Danjinjar_TP();
}

void CAS_Pet_Danjinjar_TP::Free()
{
    __super::Free();
}
