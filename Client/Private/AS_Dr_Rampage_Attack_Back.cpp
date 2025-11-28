#include "AS_Dr_Rampage_Attack_Back.h"
#include "GameInstance.h"

CAS_Dr_Rampage_Attack_Back::CAS_Dr_Rampage_Attack_Back()
{
}

void CAS_Dr_Rampage_Attack_Back::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    if (m_pMonData == nullptr)
        m_pMonData = &static_cast<CDragonian_Rampage*>(pOwner)->Get_Data();

    m_pMonData->iAnimIndex = 45;
    m_fAngle = 180.f;

}

void CAS_Dr_Rampage_Attack_Back::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    if (m_pMonData->isAnimFinash)
    {
        m_pMonData->fQuat = m_fAngle;
        m_pMonData->iAnimIndex = 44;
        m_pMonData->eAttack_State = CDragonian_Rampage::ATTACKSTATE::END;
        
        m_pMonData->fLook = 0.54f;
        m_pMonData->isBland = false;
     }
}

void CAS_Dr_Rampage_Attack_Back::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
}

void CAS_Dr_Rampage_Attack_Back::OnCollision(COLLISION_DESC* pDesc, _uint iCollisionLayer, CGameObject* pOwner)
{
    COLLISION_LAYER eLayer = static_cast<COLLISION_LAYER>(iCollisionLayer);
    if (COLLISION_LAYER::PLAYER == eLayer)
    {
        CCreature* pTarget = static_cast<CCreature*>(pDesc->pGameObject);
        pTarget->Take_Damage(m_pMonData->fAttackDamage, HITREACTION::KNOCKBACK_WEAK, nullptr);
    }
}

CAS_Dr_Rampage_Attack_Back* CAS_Dr_Rampage_Attack_Back::Create()
{
    return new CAS_Dr_Rampage_Attack_Back();
}

void CAS_Dr_Rampage_Attack_Back::Free()
{
    __super::Free();
}
