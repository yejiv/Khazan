#include "AS_Halberd_Dead.h"
#include "GameInstance.h"
#include "ClientInstance.h"
#include "Interaction_Item.h"
#include "Amount.h"

CAS_Halberd_Dead::CAS_Halberd_Dead()
{
}

void CAS_Halberd_Dead::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    if (m_pMonData == nullptr)
        m_pMonData = &static_cast<CHalberd*>(pOwner)->Get_Data();
    static_cast<CAmount*>(CClientInstance::GetInstance()->Get_RootUI(TEXT("Amount")))->Add_Value(CAmount::AMOUNT_TYPE::LACHRYMA, 1600);
    static_cast<CAmount*>(CClientInstance::GetInstance()->Get_RootUI(TEXT("Amount")))->Add_Value(CAmount::AMOUNT_TYPE::GOLD, 1600);

    TARGET_DIR eDir = m_pMonData->pOwner->Get_DIR();

    m_pMonData->iAnimIndex = 30;
    m_eState = DIE;

    m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_empirehalberd_die_01 (SFX).wav"), pOwner->Get_Position(), m_pMonData->pOwner->Get_SoundChannel(0));
    CClientInstance::GetInstance()->BGM_HeinMach_Day(3.f);
}

void CAS_Halberd_Dead::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    if (m_eState == DIE)
    {
        if (m_pMonData->isAnimFinash)
        {
            m_eState = RELEASSE;
            m_pMonData->pOwner->Hp_Dead();
            m_pGameInstance->Emit_Event< EVENT_ANNOUNCE_RESULT>(ENUM_CLASS(EVENT_TYPE::ANNOUNCE_RESULT), { true });

        }
    }
    else if (m_eState == RELEASSE)
    {
        m_pMonData->fDecreaseAlpha += fTimeDelta * 0.7f;

        if (m_pMonData->fDecreaseAlpha >= 1.f)
        {
            CInteraction_Item* pItem = dynamic_cast<CInteraction_Item*>(m_pGameInstance->Pop_PoolObject(m_pGameInstance->Get_CurrentLevelID(), TEXT("Item")));
            pItem->RandNormal_Item(pOwner->Get_Transform()->Get_State(STATE::POSITION));
            m_pGameInstance->Push_PoolObject_ToLayer(m_pGameInstance->Get_CurrentLevelID(), TEXT("Layer_Item"), pItem);

            m_pMonData->pOwner->Creature_Release();
        }
    }
}

void CAS_Halberd_Dead::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
}

CAS_Halberd_Dead* CAS_Halberd_Dead::Create()
{
    return new CAS_Halberd_Dead();
}

void CAS_Halberd_Dead::Free()
{
    __super::Free();
}
