#include "AS_Dr_Rampage_Groggy.h"
#include "AI_Controller.h"
#include "BlackBoard.h"

CAS_Dr_Rampage_Groggy::CAS_Dr_Rampage_Groggy()
{
}

void CAS_Dr_Rampage_Groggy::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    if (m_pMonData == nullptr)
        m_pMonData = &static_cast<CDragonian_Rampage*>(pOwner)->Get_Data();
    m_pMonData->pOwner->Get_Controller()->Get_BlackBoard()->Set_Value<_bool>(pOwner->Get_Name(), "isGroggy", true);

    m_pMonData->iAnimIndex = 37;
    m_eState = START;
    m_pMonData->fGloggyTime = 5.f;
    m_pMonData->pOwner->BurutalUI_On(m_pMonData->fGloggyTime);
}

void CAS_Dr_Rampage_Groggy::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    if (m_eState == START)
    {
        if (m_pMonData->isAnimFinash)
        {
            m_eState = LOOP;
            m_pMonData->iAnimIndex = 36;
        }
    }
    else if (m_eState == LOOP)
    {
        m_pMonData->fGloggyTime -= fTimeDelta;

        if (m_pMonData->fGloggyTime <= 0.f)
        {
            m_eState = END;
            m_pMonData->iAnimIndex = 35;
            m_pMonData->pOwner->BurutalUI_Off();
            m_pMonData->pOwner->Get_Controller()->Get_BlackBoard()->Set_Value<_bool>(pOwner->Get_Name(), "isGroggy", false);
            m_pMonData->eHitType = HITREACTION::END;
        }
    }
    else
    {
        if (m_pMonData->isAnimFinash)
        {
            m_pMonData->isStamina_Regen = true;
        }
    }
}

void CAS_Dr_Rampage_Groggy::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
    m_pMonData->pOwner->BurutalUI_Off();
    m_pMonData->pOwner->Get_Controller()->Get_BlackBoard()->Set_Value<_bool>(pOwner->Get_Name(), "isGroggy", false);
    m_pMonData->eHitType = HITREACTION::END;
}

CAS_Dr_Rampage_Groggy* CAS_Dr_Rampage_Groggy::Create()
{
    return new CAS_Dr_Rampage_Groggy();
}

void CAS_Dr_Rampage_Groggy::Free()
{
    __super::Free();
}
