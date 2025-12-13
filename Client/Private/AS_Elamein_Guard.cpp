#include "AS_Elamein_Guard.h"
#include "GameInstance.h"

CAS_Elamein_Guard::CAS_Elamein_Guard()
{
}

void CAS_Elamein_Guard::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    if (m_pMonData == nullptr)
        m_pMonData = &static_cast<CElamein*>(pOwner)->Get_Data();

    m_pMonData->iAnimIndex = 48;
    m_eState = START;
    m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_elamein_efforts_greatguard_a_01 (Korean(KR)).wav"), pOwner->Get_Position(), m_pMonData->pOwner->Get_SoundChannel(0), 3.f);

}

void CAS_Elamein_Guard::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    m_pMonData->pOwner->LockOnLerp(fTimeDelta, 5.f);
    if (m_eState == START)
    {
        if (m_pMonData->isAnimFinash)
        {
            m_pMonData->iAnimIndex = 47;
            m_eState = LOOP;
        }

    }
    else if (m_eState == LOOP)
    {
        m_fAcctime += fTimeDelta;
        if (m_pMonData->eHitType != HITREACTION::END && m_pMonData->eHitType != HITREACTION::BRUTAL_ATTACK)
        {
            m_pMonData->iAnimIndex = 51;
            m_eState = COUNT;
            m_pMonData->pOwner->Start_DefaultRadialBlur();
            m_pMonData->pOwner->Start_DefaultVignette();
            m_pGameInstance->Start_HitStop(TIME_CHANNEL::EFFECT, 0.2f, 0.003f, 2.5f);
            m_pGameInstance->Start_HitStop(TIME_CHANNEL::ENEMY, 0.2f, 0.003f, 2.5f);
            m_pGameInstance->Start_HitStop(TIME_CHANNEL::PLAYER, 0.2f, 0.003f, 2.5f);
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_elamein_guardcounter_01 (SFX).wav"), pOwner->Get_Position(), m_pMonData->pOwner->Get_SoundChannel(1), 3.f);
        }
        else if (m_fAcctime >= 3.f)
        {
            m_pMonData->iAnimIndex = 46;
            m_eState = END;
        }
    }
    else if (m_eState == COUNT)
    {
        if (m_pMonData->isAnimFinash)
        {
            m_pMonData->isGuard = false;
        }
    }
    else if (m_eState == END)
    {
        if (m_pMonData->isAnimFinash)
        {
            m_pMonData->isGuard = false;
        }
    }
}

void CAS_Elamein_Guard::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
    m_pMonData->fGuardCool = 30.f;
    m_fAcctime = 0.f;
    m_pMonData->eHitType = HITREACTION::END;
}

void CAS_Elamein_Guard::OnCollision(COLLISION_DESC* pDesc, _uint iCollisionLayer, CGameObject* pOwner)
{
    COLLISION_LAYER eLayer = static_cast<COLLISION_LAYER>(iCollisionLayer);
    if (COLLISION_LAYER::PLAYER == eLayer)
    {
        CCreature* pTarget = static_cast<CCreature*>(pDesc->pGameObject);
        pTarget->KnockBack(pOwner->Get_Look(), 13.5f, 45.f);
        pTarget->Take_Damage(m_pGameInstance->Rand(m_pMonData->fAttackDamage * 0.8f, m_pMonData->fAttackDamage * 1.3f), HITREACTION::KNOCKBACK_STRONG, nullptr);
    }
}

CAS_Elamein_Guard* CAS_Elamein_Guard::Create()
{
    return new CAS_Elamein_Guard();
}

void CAS_Elamein_Guard::Free()
{
    __super::Free();
}
