#include "AS_Elamein_Attack_Enchant.h"
#include "GameInstance.h"

CAS_Elamein_Attack_Enchant::CAS_Elamein_Attack_Enchant()
{
}

void CAS_Elamein_Attack_Enchant::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    if (m_pMonData == nullptr)
        m_pMonData = &static_cast<CElamein*>(pOwner)->Get_Data();

    m_pMonData->iAnimIndex = 89;
    m_eState = CHARGE;
    m_pMonData->isAnimFinash = false;
    m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_elamein_enchant_01 (SFX).wav"), pOwner->Get_Position(), m_pMonData->pOwner->Get_SoundChannel(1), 3.f);

}

void CAS_Elamein_Attack_Enchant::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    if (m_eState == CHARGE)
    {
        m_eState = START;
    }
    if (m_eState == START)
    {
        m_pMonData->pOwner->Add_Charge((m_pMonData->pOwner->Get_TrackPotion() / 45.f) * 5.f);
        if (m_pMonData->isAnimFinash)
        {
            m_pMonData->iAnimIndex = 88;
            m_eState = LOOP;
        }
    }
    else if (m_eState == LOOP)
    {
        m_pMonData->pOwner->LockOnLerp(fTimeDelta, 4.f);
        pOwner->Get_Transform()->Go_Straight(3.5f * fTimeDelta);
        if (m_pMonData->pOwner->Check_Ranage("AttackRange"))
        {
            m_pMonData->iAnimIndex = 83;
            m_eState = END;
        }
    }
    else if (m_eState == END)
    {
        m_pMonData->pOwner->LockOnLerp(fTimeDelta, 4.f);
        if (m_pMonData->isAnimFinash)
        {
            m_pMonData->fSpecial_AttackCool = 30.f;
            m_pMonData->eAttackState = CElamein::ATTACKSTATE::END;
            m_pMonData->pOwner->Reset_Charge();
            m_pMonData->eHitType = HITREACTION::END;
        }
    }

}

void CAS_Elamein_Attack_Enchant::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
    m_pMonData->fSpecial_AttackCool = 30.f;
    m_pMonData->pOwner->Reset_Charge();
}

void CAS_Elamein_Attack_Enchant::OnCollision(COLLISION_DESC* pDesc, _uint iCollisionLayer, CGameObject* pOwner)
{
    COLLISION_LAYER eLayer = static_cast<COLLISION_LAYER>(iCollisionLayer);
    if (COLLISION_LAYER::PLAYER == eLayer)
    {
        CCreature* pTarget = static_cast<CCreature*>(pDesc->pGameObject);
        pTarget->KnockBack(pOwner->Get_Look(), 10.5f, 40.f);
        pTarget->Take_Damage(m_pMonData->fAttackDamage, HITREACTION::KNOCKBACK_STRONG, nullptr);

    }
}

CAS_Elamein_Attack_Enchant* CAS_Elamein_Attack_Enchant::Create()
{
    return new CAS_Elamein_Attack_Enchant();
}

void CAS_Elamein_Attack_Enchant::Free()
{
    __super::Free();
}
