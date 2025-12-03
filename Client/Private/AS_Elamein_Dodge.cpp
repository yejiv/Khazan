#include "AS_Elamein_Dodge.h"

CAS_Elamein_Dodge::CAS_Elamein_Dodge()
{
}

void CAS_Elamein_Dodge::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    if (m_pMonData == nullptr)
        m_pMonData = &static_cast<CElamein*>(pOwner)->Get_Data();

    m_isAttack ? m_isAttack = false : m_isAttack = true;

    if (m_isAttack)
    {
        m_pMonData->iAnimIndex = 31;
        m_eState = DODGE;
    }
    else
    {
        m_pMonData->iAnimIndex = 35;
    }
}

void CAS_Elamein_Dodge::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    if (m_isAttack)
    {
        if (m_eState == DODGE)
        {
            if (m_pMonData->isAnimFinash)
            {
                m_pMonData->iAnimIndex = 96;
                m_eState = ATTACK;
            }
        }
        else if (m_eState == ATTACK)
        {
            if (m_pMonData->isAnimFinash)
            {
                m_pMonData->iAnimIndex = 95;
                m_eState = END;
            }
        }
        else if (m_eState == END)
        {
            if (m_pMonData->isAnimFinash)
            {
                m_pMonData->isDodge = false;
                m_pMonData->eHitType = HITREACTION::END;
            }
        }
    }
    else
    {
        if (m_pMonData->isAnimFinash)
        {
            m_pMonData->isDodge = false;
            m_pMonData->eHitType = HITREACTION::END;
        }
    }
}

void CAS_Elamein_Dodge::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
}

void CAS_Elamein_Dodge::OnCollision(COLLISION_DESC* pDesc, _uint iCollisionLayer, CGameObject* pOwner)
{
    COLLISION_LAYER eLayer = static_cast<COLLISION_LAYER>(iCollisionLayer);
    if (COLLISION_LAYER::PLAYER == eLayer)
    {
        CCreature* pTarget = static_cast<CCreature*>(pDesc->pGameObject);
        pTarget->Take_Damage(m_pMonData->fAttackDamage, HITREACTION::KNOCKBACK_STRONG, nullptr);
    }
}

CAS_Elamein_Dodge* CAS_Elamein_Dodge::Create()
{
    return new CAS_Elamein_Dodge();
}

void CAS_Elamein_Dodge::Free()
{
    __super::Free();
}
