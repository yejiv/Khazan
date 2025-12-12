#include "AS_Elamein_Dodge.h"
#include "GameInstance.h"
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

        m_pGameInstance->Start_HitStop(TIME_CHANNEL::EFFECT, 1.f, 0.003f, 2.1f);
        m_pGameInstance->Start_HitStop(TIME_CHANNEL::ENEMY, 1.f, 0.003f, 2.1f);
        m_pGameInstance->Start_HitStop(TIME_CHANNEL::PLAYER, 1.f, 0.003f, 2.1f);
        m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_elamein_dodgel_01 (SFX).wav"), pOwner->Get_Position(), m_pMonData->pOwner->Get_SoundChannel(1), 3.f);
        m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_elamein_efforts_dodge_a_01 (Korean(KR)).wav"), pOwner->Get_Position(), m_pMonData->pOwner->Get_SoundChannel(0), 3.f);
    }
    else
    {
        m_pMonData->iAnimIndex = 35;
        m_pGameInstance->Start_HitStop(TIME_CHANNEL::EFFECT, 0.8f, 0.003f, 2.5f);
        m_pGameInstance->Start_HitStop(TIME_CHANNEL::ENEMY, 0.8f, 0.003f, 2.5f);
        m_pGameInstance->Start_HitStop(TIME_CHANNEL::PLAYER, 0.8f, 0.003f, 2.5f);
        m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_elamein_greatguard_01 (SFX).wav"), pOwner->Get_Position(), m_pMonData->pOwner->Get_SoundChannel(1), 3.f);
        m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_elamein_efforts_dodge_a_01 (Korean(KR)).wav"), pOwner->Get_Position(), m_pMonData->pOwner->Get_SoundChannel(0), 3.f);
    }
    m_pMonData->fDodgeCool = 10.5f;
    m_pMonData->isAnimFinash = false;
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
            m_pMonData->pOwner->LockOnLerp(fTimeDelta, 3.5f);
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
    m_pMonData->eAttackState = CElamein::ATTACKSTATE::END;
}

void CAS_Elamein_Dodge::OnCollision(COLLISION_DESC* pDesc, _uint iCollisionLayer, CGameObject* pOwner)
{
    COLLISION_LAYER eLayer = static_cast<COLLISION_LAYER>(iCollisionLayer);
    if (COLLISION_LAYER::PLAYER == eLayer)
    {
        CCreature* pTarget = static_cast<CCreature*>(pDesc->pGameObject);
        pTarget->KnockBack(pOwner->Get_Look(), 12.5f, 45.f);

        pTarget->Take_Damage(m_pGameInstance->Rand(m_pMonData->fAttackDamage * 0.8f, m_pMonData->fAttackDamage * 1.3f), HITREACTION::KNOCKBACK_STRONG, nullptr);
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
