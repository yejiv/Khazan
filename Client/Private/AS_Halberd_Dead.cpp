#include "AS_Halberd_Dead.h"
#include "GameInstance.h"
#include "Interaction_Item.h"

CAS_Halberd_Dead::CAS_Halberd_Dead()
{
}

void CAS_Halberd_Dead::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    if (m_pMonData == nullptr)
        m_pMonData = &static_cast<CHalberd*>(pOwner)->Get_Data();

    TARGET_DIR eDir = m_pMonData->pOwner->Get_DIR();

    m_pMonData->iAnimIndex = 30;
    m_eState = DIE;

}

void CAS_Halberd_Dead::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
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
        m_pMonData->fDecreaseAlpha += fTimeDelta * 0.15f;

        if (m_pMonData->fDecreaseAlpha >= 1.f)
        {
            //m_pGameInstance->Add_PoolObject(ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_GameObject_Item"), ENUM_CLASS(LEVEL::HEINMACH), TEXT("Item"), nullptr, 10);
            //CInteraction_Item* pItem = dynamic_cast<CInteraction_Item*>(m_pGameInstance->Pop_PoolObject(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Item")));
            //pItem->RandNormal_Item(pOwner->Get_Transform()->Get_State(STATE::POSITION));
            //m_pGameInstance->Push_PoolObject_ToLayer(ENUM_CLASS(LEVEL::VIPER), TEXT("Layer_Item"), pItem);

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
