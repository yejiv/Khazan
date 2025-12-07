#include "AS_Elamein_Groggy.h"
#include "AI_Controller.h"
#include "BlackBoard.h"

CAS_Elamein_Groggy::CAS_Elamein_Groggy()
{
}

void CAS_Elamein_Groggy::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    if (m_pMonData == nullptr)
        m_pMonData = &static_cast<CElamein*>(pOwner)->Get_Data();
    m_pMonData->pOwner->Get_Controller()->Get_BlackBoard()->Set_Value<_bool>(pOwner->Get_Name(), "isCanBrutalAttack", true);

    m_pMonData->iAnimIndex = 53;
    m_eState = START;
    m_pMonData->fGloggyTime = 5.f;
    m_pMonData->pOwner->BurutalUI_On(m_pMonData->fGloggyTime);

}

void CAS_Elamein_Groggy::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    if (m_eState == START)
    {
        if (m_pMonData->isAnimFinash)
        {
            m_eState = LOOP;
            m_pMonData->iAnimIndex = 54;
        }
    }
    else if (m_eState == LOOP)
    {
        m_pMonData->fGloggyTime -= fTimeDelta;

        if (m_pMonData->fGloggyTime <= 0.f)
        {
            m_eState = END;
            m_pMonData->iAnimIndex = 52;
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

void CAS_Elamein_Groggy::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
    m_pMonData->pOwner->BurutalUI_Off();
    m_pMonData->pOwner->Get_Controller()->Get_BlackBoard()->Set_Value<_bool>(pOwner->Get_Name(), "isGroggy", false);
}

CAS_Elamein_Groggy* CAS_Elamein_Groggy::Create()
{
    return new CAS_Elamein_Groggy();
}

void CAS_Elamein_Groggy::Free()
{
    __super::Free();
}
