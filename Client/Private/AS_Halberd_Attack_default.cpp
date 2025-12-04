#include "AS_Halberd_Attack_default.h"

CAS_Halberd_Attack_default::CAS_Halberd_Attack_default()
{
}

void CAS_Halberd_Attack_default::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    if (m_pMonData == nullptr)
        m_pMonData = &static_cast<CHalberd*>(pOwner)->Get_Data();

    ++m_iAttack;
    if (m_iAttack >= END)
        m_iAttack = ATTACK_1;

    if (m_iAttack == ATTACK_1)
    {
        m_pMonData->iAnimIndex = 74;
    }
    else if (m_iAttack == ATTACK_2)
    {
        m_pMonData->iAnimIndex = 64;
    }
    else if (m_iAttack == ATTACK_3)
    {
        m_pMonData->iAnimIndex = 51;
    }
    else if (m_iAttack == ATTACK_4)
    {
        m_pMonData->iAnimIndex = 4;
    }

}

void CAS_Halberd_Attack_default::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    if (m_pMonData->isAnimFinash)
    {
        m_pMonData->eAttackState = CHalberd::ATTACKSTATE::END;
        m_pMonData->fAttackCool = 2.f;
    }
}

void CAS_Halberd_Attack_default::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
}

void CAS_Halberd_Attack_default::OnCollision(COLLISION_DESC* pDesc, _uint iCollisionLayer, CGameObject* pOwner)
{
    COLLISION_LAYER eLayer = static_cast<COLLISION_LAYER>(iCollisionLayer);
    if (COLLISION_LAYER::PLAYER == eLayer)
    {
        CCreature* pTarget = static_cast<CCreature*>(pDesc->pGameObject);
        pTarget->Take_Damage(m_pMonData->fAttackDamage, HITREACTION::KNOCKBACK_NORMAL, nullptr);

        if(m_iAttack == ATTACK_1)
            pTarget->KnockBack(pOwner->Get_Look(), 15.5f, 60.f);
        else if (m_iAttack == ATTACK_2)
            pTarget->KnockBack(pOwner->Get_Look(), 10.5f, 45.f);
        else if (m_iAttack == ATTACK_3)
            pTarget->KnockBack(pOwner->Get_Look(), 18.5f, 60.f);
        else
            pTarget->KnockBack(pOwner->Get_Look(), 13.5f, 50.f);

    }
}

CAS_Halberd_Attack_default* CAS_Halberd_Attack_default::Create()
{
    return new CAS_Halberd_Attack_default();
}

void CAS_Halberd_Attack_default::Free()
{
    __super::Free();
}
