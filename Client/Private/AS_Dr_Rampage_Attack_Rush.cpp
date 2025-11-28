#include "AS_Dr_Rampage_Attack_Rush.h"
#include "GameInstance.h"

CAS_Dr_Rampage_Attack_Rush::CAS_Dr_Rampage_Attack_Rush()
{
}

void CAS_Dr_Rampage_Attack_Rush::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    if (m_pMonData == nullptr)
        m_pMonData = &static_cast<CDragonian_Rampage*>(pOwner)->Get_Data();
    m_isCombo ? m_isCombo = false : m_isCombo = true;
    if (!m_isCombo)
        m_pMonData->iAnimIndex = 38;
    else
    {
        m_pMonData->iAnimIndex = 34;
        m_eComboState = COMBOSTATE::START;
    }

}

void CAS_Dr_Rampage_Attack_Rush::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    if (!m_isCombo)
    {
        if (m_pMonData->isAnimFinash)
        {
            m_pMonData->eAttack_State = CDragonian_Rampage::ATTACKSTATE::END;
        }
    }
    else
    {
        if (m_eComboState == COMBOSTATE::START)
        {
            if (m_pMonData->isAnimFinash)
            {
                m_pMonData->iAnimIndex = 33;
                m_eComboState = COMBOSTATE::LOOP;
                m_pMonData->isBland = false;
                m_fAccTime = 3.f;
                m_pMonData->iAttackBody_State = (_uint)CDragonian_Rampage::ATTACK_BODY::HAND_L | (_uint)CDragonian_Rampage::ATTACK_BODY::HAND_R;
            }
        }
        else if (m_eComboState == COMBOSTATE::LOOP)
        {
            m_fAccTime -= fTimeDelta;
            pOwner->Get_Transform()->Go_Straight(7.f * fTimeDelta);
            if (m_pMonData->pOwner->Check_Ranage("AttackRange") || m_fAccTime <= 0.f || m_pMonData->isWallCrushed)
            {
                m_pMonData->iAnimIndex = 32;
                m_eComboState = COMBOSTATE::END;
                m_pMonData->isBland = false;
                m_pMonData->iAttackBody_State = 0;
                m_pMonData->isWallCrushed = false;
            }
        }
        else if (m_eComboState == COMBOSTATE::END)
        {
            if (m_pMonData->isAnimFinash)
            {
                m_pMonData->eAttack_State = CDragonian_Rampage::ATTACKSTATE::END;
            }
        }
    }
}

void CAS_Dr_Rampage_Attack_Rush::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
}

void CAS_Dr_Rampage_Attack_Rush::OnCollision(COLLISION_DESC* pDesc, _uint iCollisionLayer, CGameObject* pOwner)
{
    COLLISION_LAYER eLayer = static_cast<COLLISION_LAYER>(iCollisionLayer);
    if (COLLISION_LAYER::PLAYER == eLayer)
    {
        CCreature* pTarget = static_cast<CCreature*>(pDesc->pGameObject);
        pTarget->Take_Damage(m_pMonData->fAttackDamage, HITREACTION::KNOCKBACK_WEAK, nullptr);
    }
}

CAS_Dr_Rampage_Attack_Rush* CAS_Dr_Rampage_Attack_Rush::Create()
{
    return new CAS_Dr_Rampage_Attack_Rush();
}

void CAS_Dr_Rampage_Attack_Rush::Free()
{
    __super::Free();
}
