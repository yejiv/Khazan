#include "AS_Dr_Melee_Dead.h"
#include "GameInstance.h"
#include "ClientInstance.h"
#include "Interaction_Item.h"
#include "Amount.h"
CAS_Dr_Melee_Dead::CAS_Dr_Melee_Dead()
{
}

void CAS_Dr_Melee_Dead::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    if (m_pMonData == nullptr)
        m_pMonData = &static_cast<CDragonian_Melee*>(pOwner)->Get_Data();

    m_pMonData->iAnimIndex = 19;
    m_eState = DIE;
    static_cast<CAmount*>(CClientInstance::GetInstance()->Get_RootUI(TEXT("Amount")))->Add_Value(CAmount::AMOUNT_TYPE::LACHRYMA, 600);
    static_cast<CAmount*>(CClientInstance::GetInstance()->Get_RootUI(TEXT("Amount")))->Add_Value(CAmount::AMOUNT_TYPE::GOLD, 2500);

    switch (m_pGameInstance->Rand(1, 3))
    {
    case 1:
        m_pGameInstance->PlaySoundOnce(TEXT("Mon_VO_Dragonian_A_Die_01 (SFX).wav"), pOwner->Get_Position(), m_pMonData->pOwner->Get_SoundChannel(0));
        break;
    case 2:
        m_pGameInstance->PlaySoundOnce(TEXT("Mon_VO_Dragonian_A_Die_02 (SFX).wav"), pOwner->Get_Position(), m_pMonData->pOwner->Get_SoundChannel(0));
        break;
    default:
        m_pGameInstance->PlaySoundOnce(TEXT("Mon_VO_Dragonian_A_Die_03 (SFX).wav"), pOwner->Get_Position(), m_pMonData->pOwner->Get_SoundChannel(0));
        break;
    }

}

void CAS_Dr_Melee_Dead::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    if (m_eState == DIE)
    {
        if (m_pMonData->isAnimFinash)
        {
            m_eState = RELEASSE;
            m_pMonData->pOwner->Hp_Dead();
        }
    }
    else if (m_eState == RELEASSE)
    {
        m_pMonData->fDecreaseAlpha += fTimeDelta * 0.35f;

        if (m_pMonData->fDecreaseAlpha >= 1.f)
        {
            CInteraction_Item* pItem = dynamic_cast<CInteraction_Item*>(m_pGameInstance->Pop_PoolObject(m_pGameInstance->Get_CurrentLevelID(), TEXT("Item")));
            pItem->RandNormal_Item(pOwner->Get_Transform()->Get_State(STATE::POSITION));
            m_pGameInstance->Push_PoolObject_ToLayer(m_pGameInstance->Get_CurrentLevelID(), TEXT("Layer_Item"), pItem);

            m_pMonData->pOwner->Creature_Release();
        }
    }
}
    
void CAS_Dr_Melee_Dead::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
}

CAS_Dr_Melee_Dead* CAS_Dr_Melee_Dead::Create()
{
    return new CAS_Dr_Melee_Dead();
}

void CAS_Dr_Melee_Dead::Free()
{
    __super::Free();
}
