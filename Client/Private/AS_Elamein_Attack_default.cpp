#include "AS_Elamein_Attack_default.h"

CAS_Elamein_Attack_default::CAS_Elamein_Attack_default()
{
}

void CAS_Elamein_Attack_default::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    if (m_pMonData == nullptr)
        m_pMonData = &static_cast<CElamein*>(pOwner)->Get_Data();

    ++m_iAttack;

    if (m_iAttack >= END)
        m_iAttack = STEMP;

    if (m_iAttack == STEMP)
    {
        m_pMonData->iAnimIndex = 93;
    }
    else if (m_iAttack == ATTACK_SWORD)
    {
        m_pMonData->iAnimIndex = 64;
    }
    else if (m_iAttack == ATTACK_SHILED)
    {
        m_pMonData->iAnimIndex = 67;
    }
    else if (m_iAttack == ATTACK_TWO)
    {
        m_pMonData->iAnimIndex = 70;
    }

}

void CAS_Elamein_Attack_default::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    if (m_pMonData->isAnimFinash)
    {
        m_pMonData->eAttackState = CElamein::ATTACKSTATE::END;
        m_pMonData->fAttackCool = 2.f;
        m_pMonData->eHitType = HITREACTION::END;
    }
}

void CAS_Elamein_Attack_default::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
}

void CAS_Elamein_Attack_default::OnCollision(COLLISION_DESC* pDesc, _uint iCollisionLayer, CGameObject* pOwner)
{
    COLLISION_LAYER eLayer = static_cast<COLLISION_LAYER>(iCollisionLayer);
    if (COLLISION_LAYER::PLAYER == eLayer)
    {
        CCreature* pTarget = static_cast<CCreature*>(pDesc->pGameObject);
        pTarget->Take_Damage(m_pMonData->fAttackDamage, HITREACTION::KNOCKBACK_WEAK, nullptr);
        pTarget->KnockBack(pOwner->Get_Look(), 13.5f, 45.f);
    }
}

CAS_Elamein_Attack_default* CAS_Elamein_Attack_default::Create()
{
    return new CAS_Elamein_Attack_default();
}

void CAS_Elamein_Attack_default::Free()
{
    __super::Free();
}
