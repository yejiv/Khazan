#include "AS_Dr_Rampage_Dead.h"
#include "GameInstance.h"
#include "Interaction_Item.h"
CAS_Dr_Rampage_Dead::CAS_Dr_Rampage_Dead()
{
}

void CAS_Dr_Rampage_Dead::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    if (m_pMonData == nullptr)
        m_pMonData = &static_cast<CDragonian_Rampage*>(pOwner)->Get_Data();

    TARGET_DIR eDir = m_pMonData->pOwner->Get_DIR();

    if(eDir == TARGET_DIR::B || eDir == TARGET_DIR::BR || eDir == TARGET_DIR::BL)
        m_pMonData->iAnimIndex = 0;
    else
        m_pMonData->iAnimIndex = 1;
    m_eState = DIE;

    switch (m_pGameInstance->Rand(1, 3))
    {
    case 1:
        m_pGameInstance->PlaySoundOnce(TEXT("Mon_VO_Dragonian_E_Die_S_01 (SFX).wav"), pOwner->Get_Position(), m_pMonData->pOwner->Get_SoundChannel(0));
        break;
    case 2:
        m_pGameInstance->PlaySoundOnce(TEXT("Mon_VO_Dragonian_E_Die_S_02 (SFX).wav"), pOwner->Get_Position(), m_pMonData->pOwner->Get_SoundChannel(0));
        break;
    default:
        m_pGameInstance->PlaySoundOnce(TEXT("Mon_VO_Dragonian_E_Die_S_03 (SFX).wav"), pOwner->Get_Position(), m_pMonData->pOwner->Get_SoundChannel(0));
        break;
    }
}

void CAS_Dr_Rampage_Dead::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
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

void CAS_Dr_Rampage_Dead::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
}

CAS_Dr_Rampage_Dead* CAS_Dr_Rampage_Dead::Create()
{
    return new CAS_Dr_Rampage_Dead();
}

void CAS_Dr_Rampage_Dead::Free()
{
    __super::Free();
}
