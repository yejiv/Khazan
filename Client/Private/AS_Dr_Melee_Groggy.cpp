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

    m_pMonData->pOwner->Get_Controller()->Get_BlackBoard()->Set_Value<_bool>(pOwner->Get_Name(), "isCanBrutalAttack", true);

    m_pMonData->iAnimIndex = 26;
    m_eState = START;
    m_pMonData->fGloggyTime = 5.f;
    m_pMonData->pOwner->BurutalUI_On(m_pMonData->fGloggyTime);
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
            m_pMonData->pOwner->BurutalUI_Off();
            m_pMonData->pOwner->Get_Controller()->Get_BlackBoard()->Set_Value<_bool>(pOwner->Get_Name(), "isCanBrutalAttack", false);
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_dragonianwarrior_hardsmash_a_01 (SFX).wav"), pOwner->Get_Position(), m_pMonData->pOwner->Get_SoundChannel(0), 5.f);
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

void CAS_Dr_Melee_Groggy::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
    m_pMonData->pOwner->BurutalUI_Off();
    m_pMonData->pOwner->Get_Controller()->Get_BlackBoard()->Set_Value<_bool>(pOwner->Get_Name(), "isGroggy", false);
}

CAS_Dr_Melee_Groggy* CAS_Dr_Melee_Groggy::Create()
{
    return new CAS_Dr_Melee_Groggy();
}

void CAS_Dr_Melee_Groggy::Free()
{
    __super::Free();
}
