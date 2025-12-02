#include "AS_Dr_Rampage_Attack_Default.h"
#include "GameInstance.h"
CAS_Dr_Rampage_Attack_Default::CAS_Dr_Rampage_Attack_Default()
{
}

void CAS_Dr_Rampage_Attack_Default::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    if (m_pMonData == nullptr)
        m_pMonData = &static_cast<CDragonian_Rampage*>(pOwner)->Get_Data();

    ++m_iAttackIndex;
    if (m_iAttackIndex >= ATTACK_END)
        m_iAttackIndex = 0;

    if(m_iAttackIndex == ATTACK1)
        m_pMonData->iAnimIndex = 31;
    else if (m_iAttackIndex == ATTACK2)
    {
        m_pMonData->iAnimIndex = 34;
        m_eComboState = COMBOSTATE::START;
    }
    else if (m_iAttackIndex == ATTACK3)
    {
        m_pMonData->iAnimIndex = 14;
        m_eComboState = COMBOSTATE::START;
    }
}

void CAS_Dr_Rampage_Attack_Default::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    if (m_iAttackIndex == ATTACK1)
    {
        if (m_pMonData->isAnimFinash)
        {
            m_pMonData->eAttack_State = CDragonian_Rampage::ATTACKSTATE::END;
        }
    }
    else if (m_iAttackIndex == ATTACK2)
    {
        if (m_eComboState == COMBOSTATE::START)
        {
            if (m_pMonData->isAnimFinash)
            {
                m_pMonData->iAnimIndex = 30;
                m_eComboState = COMBOSTATE::ATTACK_1;
                m_pMonData->isBland = false;
            }
        }
        else if (m_eComboState == COMBOSTATE::ATTACK_1)
        {
            if (m_pMonData->isAnimFinash)
            {
                m_pMonData->iAnimIndex = 31;
                m_eComboState = COMBOSTATE::END;
                m_pMonData->isBland = false;
            }
        }
        else if (m_eComboState == COMBOSTATE::END)
        {
            m_pMonData->pOwner->LockOnLerp(fTimeDelta, 5.f);
            if (m_pMonData->isAnimFinash)
            {
                m_pMonData->eAttack_State = CDragonian_Rampage::ATTACKSTATE::END;
            }
        }
    }
    else if (m_iAttackIndex == ATTACK3)
    {
        m_pMonData->pOwner->LockOnLerp(fTimeDelta, 5.f);
        if (m_pMonData->isAnimFinash)
            m_pMonData->eAttack_State = CDragonian_Rampage::ATTACKSTATE::END;
    }
}

void CAS_Dr_Rampage_Attack_Default::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
    m_pMonData->fAttackCool = 3.5f;
}

void CAS_Dr_Rampage_Attack_Default::OnCollision(COLLISION_DESC* pDesc, _uint iCollisionLayer, CGameObject* pOwner)
{
    COLLISION_LAYER eLayer = static_cast<COLLISION_LAYER>(iCollisionLayer);
    if (COLLISION_LAYER::PLAYER == eLayer)
    {
        CCreature* pTarget = static_cast<CCreature*>(pDesc->pGameObject);
        pTarget->Take_Damage(m_pMonData->fAttackDamage, HITREACTION::KNOCKBACK_WEAK, nullptr);
    }
}

CAS_Dr_Rampage_Attack_Default* CAS_Dr_Rampage_Attack_Default::Create()
{
    return new CAS_Dr_Rampage_Attack_Default();
}

void CAS_Dr_Rampage_Attack_Default::Free()
{
    __super::Free();
}
