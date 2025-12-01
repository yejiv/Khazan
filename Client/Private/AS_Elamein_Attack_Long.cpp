#include "AS_Elamein_Attack_Long.h"

CAS_Elamein_Attack_Long::CAS_Elamein_Attack_Long()
{
}

void CAS_Elamein_Attack_Long::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    if (m_pMonData == nullptr)
        m_pMonData = &static_cast<CElamein*>(pOwner)->Get_Data();

    m_isSword ? m_isSword = false : m_isSword = true;

    m_pMonData->iAnimIndex = 97;
    m_eState = ATTACK_1;

}

void CAS_Elamein_Attack_Long::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    if (m_isSword)
    {
        if (m_eState == ATTACK_1)
        {
            if (m_pMonData->isAnimFinash)
            {
                m_pMonData->iAnimIndex = 78;
                m_eState = ATTACK_2;
            }
        }
        else if (m_eState == ATTACK_2)
        {
            m_pMonData->pOwner->LockOnLerp(fTimeDelta, 10.f);
            pOwner->Get_Transform()->Go_Straight(3.5f * fTimeDelta);
            if (m_pMonData->pOwner->Check_Ranage("AttackRange"))
            {
                m_pMonData->iAnimIndex = 96;
                m_eState = ATTACK_3;
            }
        }
        else if (m_eState == ATTACK_3)
        {
            m_pMonData->pOwner->LockOnLerp(fTimeDelta, 10.f);
            if (m_pMonData->isAnimFinash)
            {
                m_pMonData->iAnimIndex = 95;
                m_eState = END;
            }
        }
        else if (m_eState == END)
        {
            m_pMonData->pOwner->LockOnLerp(fTimeDelta, 10.f);
            if (m_pMonData->isAnimFinash)
            {
                m_pMonData->eAttackState = CElamein::ATTACKSTATE::END;
                m_pMonData->fLong_AttackCool = 3.f;
            }
        }

    }
    else
    {
        if (m_eState == ATTACK_1)
        {
            if (m_pMonData->isAnimFinash)
            {
                m_pMonData->iAnimIndex = 78;
                m_eState = ATTACK_2;
            }
        }
        else if (m_eState == ATTACK_2)
        {
            m_pMonData->pOwner->LockOnLerp(fTimeDelta, 10.f);
            pOwner->Get_Transform()->Go_Straight(3.5f * fTimeDelta);
            if (m_pMonData->pOwner->Check_Ranage("AttackRange"))
            {
                m_pMonData->iAnimIndex = 77;
                m_eState = ATTACK_3;
            }
        }
        else if (m_eState == ATTACK_3)
        {
            m_pMonData->pOwner->LockOnLerp(fTimeDelta, 6.f);
            if (m_pMonData->isAnimFinash)
            {
                m_pMonData->fQuat = 180.f;
                m_pMonData->iAnimIndex = 101;
                m_pMonData->eAttackState = CElamein::ATTACKSTATE::END;
                m_pMonData->fLong_AttackCool = 3.f;
            }
        }
    }

}

void CAS_Elamein_Attack_Long::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
}

void CAS_Elamein_Attack_Long::OnCollision(COLLISION_DESC* pDesc, _uint iCollisionLayer, CGameObject* pOwner)
{
    COLLISION_LAYER eLayer = static_cast<COLLISION_LAYER>(iCollisionLayer);
    if (COLLISION_LAYER::PLAYER == eLayer)
    {
        CCreature* pTarget = static_cast<CCreature*>(pDesc->pGameObject);
        pTarget->Take_Damage(m_pMonData->fAttackDamage, HITREACTION::KNOCKBACK_NORMAL, nullptr);
    }
}

CAS_Elamein_Attack_Long* CAS_Elamein_Attack_Long::Create()
{
    return new CAS_Elamein_Attack_Long();
}

void CAS_Elamein_Attack_Long::Free()
{
    __super::Free();
}
