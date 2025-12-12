#include "AS_Dr_Melee_Attack.h"
#include "GameInstance.h"

CAS_Dr_Melee_Attack::CAS_Dr_Melee_Attack()
{
}

void CAS_Dr_Melee_Attack::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    if (m_pMonData == nullptr)
        m_pMonData = &static_cast<CDragonian_Melee*>(pOwner)->Get_Data();

    if(m_isAttackChange)
        m_pMonData->iAnimIndex = 27;
    else
        m_pMonData->iAnimIndex = 20;

    m_isAttackChange ? m_isAttackChange = false : m_isAttackChange = true;

}

void CAS_Dr_Melee_Attack::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    m_pMonData->pOwner->LockOnLerp(fTimeDelta, 3.f);
    if (m_pMonData->isAnimFinash || m_pMonData->eHitType != HITREACTION::END)
    {
        m_pMonData->isAttack = false;
    }
}

void CAS_Dr_Melee_Attack::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
    m_pMonData->fAttackCool = 2.f;
}

void CAS_Dr_Melee_Attack::OnCollision(COLLISION_DESC* pDesc, _uint iCollisionLayer, CGameObject* pOwner)
{
    COLLISION_LAYER eLayer = static_cast<COLLISION_LAYER>(iCollisionLayer);
    if (COLLISION_LAYER::PLAYER == eLayer)
    {
        CCreature* pTarget = static_cast<CCreature*>(pDesc->pGameObject);
        pTarget->KnockBack(pOwner->Get_Look(), 10.f, 50.f);
        pTarget->Take_Damage(m_pGameInstance->Rand(m_pMonData->fAttackDamage * 0.8f, m_pMonData->fAttackDamage * 1.3f), HITREACTION::KNOCKBACK_STRONG, nullptr);
    }
}

CAS_Dr_Melee_Attack* CAS_Dr_Melee_Attack::Create()
{
    return new CAS_Dr_Melee_Attack();
}

void CAS_Dr_Melee_Attack::Free()
{
    __super::Free();
}
