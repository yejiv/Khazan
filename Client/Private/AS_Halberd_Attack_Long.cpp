#include "AS_Halberd_Attack_Long.h"
#include "GameInstance.h"

CAS_Halberd_Attack_Long::CAS_Halberd_Attack_Long()
{
}

void CAS_Halberd_Attack_Long::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    if (m_pMonData == nullptr)
        m_pMonData = &static_cast<CHalberd*>(pOwner)->Get_Data();

    m_isSprint ? m_isSprint = false : m_isSprint = true;

    if (m_isSprint)
        m_pMonData->iAnimIndex = 61;
    else
        m_pMonData->iAnimIndex = 10;
       

    switch (m_pGameInstance->Rand(1, 3))
    {
    case 1:
        m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_empirehalberd_charge_l_01 (SFX).wav"), pOwner->Get_Position(), m_pMonData->pOwner->Get_SoundChannel(0));
        break;
    case 2:
        m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_empirehalberd_charge_l_02 (SFX).wav"), pOwner->Get_Position(), m_pMonData->pOwner->Get_SoundChannel(0));
        break;
    default:
        m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_empirehalberd_charge_l_03 (SFX).wav"), pOwner->Get_Position(), m_pMonData->pOwner->Get_SoundChannel(0));
        break;
    }
    m_eState = START;

}

void CAS_Halberd_Attack_Long::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    if (m_isSprint)
    {
        if (m_eState == START)
        {
            if (m_pMonData->isAnimFinash)
            {
                m_pMonData->iAnimIndex = 60;
                m_eState = RUSH;
            }
        }
        else if (m_eState == RUSH)
        {
            m_pMonData->pOwner->LockOnLerp(fTimeDelta, 4.f);
            pOwner->Get_Transform()->Go_Straight(3.5f * fTimeDelta);
            if (m_pMonData->pOwner->Check_Ranage("AttackRange"))
            {
                m_pMonData->iAnimIndex = 59;
                m_eState = END;
            }
        }
        else if (m_eState == END)
        {
            m_pMonData->pOwner->LockOnLerp(fTimeDelta, 4.f);
            if (m_pMonData->isAnimFinash)
            {
                m_pMonData->eAttackState = CHalberd::ATTACKSTATE::END;
                m_pMonData->fLong_AttackCool = 3.f;
            }
        }

    }
    else
    {
        if (m_eState == START)
        {
            if (m_pMonData->isAnimFinash)
            {
                m_isTakeDamage = false;
                m_pMonData->iAnimIndex = 9;
                m_eState = RUSH;
            }
        }
        else if (m_eState == RUSH)
        {
            m_pMonData->isAttack_Collinder = true;
            m_pMonData->pOwner->LockOnLerp(fTimeDelta, 4.f);
            pOwner->Get_Transform()->Go_Straight(3.5f * fTimeDelta);
            if (m_pMonData->pOwner->Check_Ranage("AttackRange") || m_isTakeDamage)
            {
                m_pMonData->isAttack_Collinder = false;
                m_pMonData->iAnimIndex = 8;
                m_eState = END;
            }
        }
        else if (m_eState == END)
        {
            m_pMonData->pOwner->LockOnLerp(fTimeDelta, 4.f);
            if (m_pMonData->isAnimFinash)
            {
                m_pMonData->eAttackState = CHalberd::ATTACKSTATE::END;
                m_pMonData->fLong_AttackCool = 3.f;
            }
        }
    }

}

void CAS_Halberd_Attack_Long::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
}

void CAS_Halberd_Attack_Long::OnCollision(COLLISION_DESC* pDesc, _uint iCollisionLayer, CGameObject* pOwner)
{
    COLLISION_LAYER eLayer = static_cast<COLLISION_LAYER>(iCollisionLayer);
    if (COLLISION_LAYER::PLAYER == eLayer)
    {
        CCreature* pTarget = static_cast<CCreature*>(pDesc->pGameObject);
        pTarget->Take_Damage(m_pGameInstance->Rand(m_pMonData->fAttackDamage *0.8f, m_pMonData->fAttackDamage * 1.2f), HITREACTION::KNOCKBACK_NORMAL, nullptr);
        pTarget->KnockBack(pOwner->Get_Look(), 18.5f, 60.f);
        m_isTakeDamage = true;
    }
}

CAS_Halberd_Attack_Long* CAS_Halberd_Attack_Long::Create()
{
    return new CAS_Halberd_Attack_Long();
}

void CAS_Halberd_Attack_Long::Free()
{
    __super::Free();
}
