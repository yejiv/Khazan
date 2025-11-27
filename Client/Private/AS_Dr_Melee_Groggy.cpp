#include "AS_Dr_Melee_Groggy.h"
#include "AI_Controller.h"
#include "GameInstance.h"

CAS_Dr_Melee_Groggy::CAS_Dr_Melee_Groggy()
{
}

void CAS_Dr_Melee_Groggy::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    if (m_pMonData == nullptr)
        m_pMonData = &static_cast<CDragonian_Melee*>(pOwner)->Get_Data();

    m_pMonData->pOwner->Get_Controller()->Get_BlackBoard()->Set_Value<_bool>(pOwner->Get_Name(), "isGroggy", true);

    m_pMonData->iAnimIndex = 26;
    m_eState = START;
}

void CAS_Dr_Melee_Groggy::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    if (m_eState == START)
    {
        if (m_pMonData->isAnimFinash)
        {
            m_eState = LOOP;
            m_pMonData->iAnimIndex = 25;
        }
    }
    else if (m_eState == LOOP)
    {
        m_pMonData->fGloggyTime -= fTimeDelta;

        if (m_pMonData->fGloggyTime <= 0.f)
        {
            m_eState = STAMIN;
            m_pMonData->iAnimIndex = 48;
        }

        else if (m_pMonData->eHitType != HITREACTION::END)
        {
            m_eState = DAMAGE;
            m_pMonData->iAnimIndex = 17;
        }
    }
    else if (m_eState == DAMAGE)
    {
        m_pMonData->fGloggyTime -= fTimeDelta;

        if (m_pMonData->fGloggyTime <= 0.f)
        {
            m_eState = STAMIN;
            m_pMonData->iAnimIndex = 48;
        }
        else if (m_pMonData->isAnimFinash)
        {
            m_eState = LOOP;
            m_pMonData->iAnimIndex = 25;
            m_pMonData->eHitType = HITREACTION::END;
        }
    }
    else if (m_eState == STAMIN)
    {
        if (m_pMonData->isAnimFinash)
        {
            m_eState = END;
            m_pMonData->iAnimIndex = 33;
        }
    }
    else
    {
        if (m_pMonData->isAnimFinash)
        {
            *m_pMonData->pCulStamina = *m_pMonData->pMaxStamina;
        }
    }
}

void CAS_Dr_Melee_Groggy::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
    m_pMonData->pOwner->Get_Controller()->Get_BlackBoard()->Set_Value<_bool>(pOwner->Get_Name(), "isGroggy", false);

    m_pMonData->fGloggyTime = 10.f;
}

CAS_Dr_Melee_Groggy* CAS_Dr_Melee_Groggy::Create()
{
    return new CAS_Dr_Melee_Groggy();
}

void CAS_Dr_Melee_Groggy::Free()
{
    __super::Free();
}
