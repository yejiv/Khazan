#include "AS_Elamein_Dead.h"
#include "GameInstance.h"
#include "ClientInstance.h"
#include "Interaction_Item.h"
#include "Amount.h"

CAS_Elamein_Dead::CAS_Elamein_Dead()
{
}

void CAS_Elamein_Dead::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    if (m_pMonData == nullptr)
        m_pMonData = &static_cast<CElamein*>(pOwner)->Get_Data();

    static_cast<CAmount*>(CClientInstance::GetInstance()->Get_RootUI(TEXT("Amount")))->Add_Value(CAmount::AMOUNT_TYPE::LACHRYMA, 2800);
    static_cast<CAmount*>(CClientInstance::GetInstance()->Get_RootUI(TEXT("Amount")))->Add_Value(CAmount::AMOUNT_TYPE::GOLD, 7200);

    TARGET_DIR eDir = m_pMonData->pOwner->Get_DIR();

    m_pMonData->iAnimIndex = 29;
    m_eState = DIE;

    m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_elamein_die_01 (SFX).wav"), pOwner->Get_Position(), m_pMonData->pOwner->Get_SoundChannel(1));
    m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_elamein_efforts_die_a_01 (Korean(KR)).wav"), pOwner->Get_Position(), m_pMonData->pOwner->Get_SoundChannel(0));
    CClientInstance::GetInstance()->BGM_Embars_B1(3.f);
}

void CAS_Elamein_Dead::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
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

void CAS_Elamein_Dead::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
}

CAS_Elamein_Dead* CAS_Elamein_Dead::Create()
{
    return new CAS_Elamein_Dead();
}

void CAS_Elamein_Dead::Free()
{
    __super::Free();
}
