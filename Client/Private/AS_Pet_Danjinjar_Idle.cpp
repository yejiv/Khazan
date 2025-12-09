#include "AS_Pet_Danjinjar_Idle.h"
#include "GameInstance.h"

CAS_Pet_Danjinjar_Idle::CAS_Pet_Danjinjar_Idle()
{
}

void CAS_Pet_Danjinjar_Idle::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    if (m_pMonData == nullptr)
        m_pMonData = &static_cast<CPet_Danjinjar*>(pOwner)->Get_Data();
    
    switch (m_pGameInstance->Rand(1, 5))
    {
    case 1: m_pMonData->iAnimIndex = 0; break;
    case 2: m_pMonData->iAnimIndex = 2; break;
    case 3: m_pMonData->iAnimIndex = 4; break;
    case 4: m_pMonData->iAnimIndex = 10; break;
    case 5: m_pMonData->iAnimIndex = 12; break;
    }

    m_fAccTime = 2.f;
    m_pMonData->isIdle = true;
    m_isAnimEnd = true;
}

void CAS_Pet_Danjinjar_Idle::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    m_fAccTime -= fTimeDelta;
    if (m_pMonData->isAnimFinash)
    {
        if (m_isAnimEnd)
        {
            switch (m_pMonData->iAnimIndex)
            {
            case 0: m_pMonData->iAnimIndex = 1; break;
            case 2: m_pMonData->iAnimIndex = 3; break;
            case 4: m_pMonData->iAnimIndex = 5; break;
            case 10: m_pMonData->iAnimIndex = 11; break;
            case 12: m_pMonData->iAnimIndex = 13; break;
            }
            m_isAnimEnd = false;
        }
        else
        {
            switch (m_pGameInstance->Rand(1, 5))
            {
            case 1: m_pMonData->iAnimIndex = 0; break;
            case 2: m_pMonData->iAnimIndex = 2; break;
            case 3: m_pMonData->iAnimIndex = 4; break;
            case 4: m_pMonData->iAnimIndex = 10; break;
            case 5: m_pMonData->iAnimIndex = 12; break;
            }
            m_isAnimEnd = true;
        }
    }

    if (m_fAccTime <= 0.f)
        m_pMonData->isIdle = false;

    m_pMonData->pOwner->LockOnLerp(fTimeDelta, 1.5f);
}

void CAS_Pet_Danjinjar_Idle::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
    m_pMonData->isIdle = false;
}

CAS_Pet_Danjinjar_Idle* CAS_Pet_Danjinjar_Idle::Create()
{
    return new CAS_Pet_Danjinjar_Idle();
}

void CAS_Pet_Danjinjar_Idle::Free()
{
    __super::Free();
}
