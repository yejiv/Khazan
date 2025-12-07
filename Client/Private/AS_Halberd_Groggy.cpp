#include "AS_Halberd_Groggy.h"
#include "AI_Controller.h"
#include "BlackBoard.h"

CAS_Halberd_Groggy::CAS_Halberd_Groggy()
{
}

void CAS_Halberd_Groggy::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    if (m_pMonData == nullptr)
    {
        m_pMonData = &static_cast<CHalberd*>(pOwner)->Get_Data();
        m_pLockOnPos = m_pMonData->pOwner->Get_LockOnPosition();
    }
    m_pMonData->pOwner->Get_Controller()->Get_BlackBoard()->Set_Value<_bool>(pOwner->Get_Name(), "isCanBrutalAttack", true);

    m_pMonData->iAnimIndex = 35;
    m_eState = START;
    m_pMonData->fGloggyTime = 5.f;
    m_pMonData->pOwner->BurutalUI_On(m_pMonData->fGloggyTime);
}

void CAS_Halberd_Groggy::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    if (m_eState == START)
    {
        if (m_pMonData->isAnimFinash)
        {
            m_eState = LOOP;
            m_pMonData->iAnimIndex = 34;
        }
    }
    else if (m_eState == LOOP)
    {
        m_pMonData->fGloggyTime -= fTimeDelta;

        if (m_pMonData->fGloggyTime <= 0.f)
        {
            m_eState = END;
            m_pMonData->iAnimIndex = 33;
            m_pMonData->pOwner->BurutalUI_Off();
            m_pMonData->pOwner->Get_Controller()->Get_BlackBoard()->Set_Value<_bool>(pOwner->Get_Name(), "isCanBrutalAttack", false);
        }
    }
    else
    {
        if (m_pMonData->isAnimFinash)
        {
            m_pMonData->isStamina_Regen = true;
            m_pMonData->eHitType = HITREACTION::END;
        }
    }
}

void CAS_Halberd_Groggy::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
    m_pMonData->pOwner->BurutalUI_Off();
    m_pMonData->pOwner->Get_Controller()->Get_BlackBoard()->Set_Value<_bool>(pOwner->Get_Name(), "isGroggy", false);
}

CAS_Halberd_Groggy* CAS_Halberd_Groggy::Create()
{
    return new CAS_Halberd_Groggy();
}

void CAS_Halberd_Groggy::Free()
{
    __super::Free();
}
