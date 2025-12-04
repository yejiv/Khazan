#include "AS_Halberd_Attack_Long.h"

CAS_Halberd_Attack_Long::CAS_Halberd_Attack_Long()
{
}

void CAS_Halberd_Attack_Long::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    if (m_pMonData == nullptr)
        m_pMonData = &static_cast<CHalberd*>(pOwner)->Get_Data();

    m_isSprint ? m_isSprint = false : m_isSprint = true;

    if(m_isSprint)
        m_pMonData->iAnimIndex = 61;
    else
        m_pMonData->iAnimIndex = 10;

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
                m_pMonData->iAnimIndex = 9;
                m_eState = RUSH;
            }
        }
        else if (m_eState == RUSH)
        {
            m_pMonData->isAttack_Collinder = true;
            m_pMonData->pOwner->LockOnLerp(fTimeDelta, 4.f);
            pOwner->Get_Transform()->Go_Straight(3.5f * fTimeDelta);
            if (m_pMonData->pOwner->Check_Ranage("AttackRange"))
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
        pTarget->Take_Damage(m_pMonData->fAttackDamage, HITREACTION::KNOCKBACK_NORMAL, nullptr);
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
