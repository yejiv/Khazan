#include "Khazan_GS_Anim_Attack.h"
#include "GameInstance.h"
#include "ClientInstance.h"

using GS_SKILL = CPlayerData_Manager::GSWORDSKILL;

CKhazan_GS_Anim_Attack::CKhazan_GS_Anim_Attack()
    : m_pClientInstance{ CClientInstance::GetInstance() }
{
    Safe_AddRef(m_pClientInstance);
}

HRESULT CKhazan_GS_Anim_Attack::Initialize()
{
    m_iCachedFastAttackAnimIndices[0] = m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_WeakAtk01");
    m_iCachedFastAttackAnimIndices[1] = m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_WeakAtk02");
    m_iCachedFastAttackAnimIndices[2] = m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_WeakAtk03");

    m_iCachedFastChargeAnimIndices[0] = m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_WeakAtk01_Charge");
    m_iCachedFastChargeAnimIndices[1] = m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_WeakAtk02_Charge");
    m_iCachedFastChargeAnimIndices[2] = m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_WeakAtk03_Charge");

    m_iCachedFastChargeAttackAnimIndices[0] = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Gsword_WeakAtk01_ChargeAtk");
    m_iCachedFastChargeAttackAnimIndices[1] = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Gsword_WeakAtk02_ChargeAtk");
    m_iCachedFastChargeAttackAnimIndices[2] = m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_WeakAtk03_ChargeAtk");

    m_pPlayerData = m_pClientInstance->Get_pInitailizePlayerData();

    return S_OK;
}

void CKhazan_GS_Anim_Attack::Enter()
{
    //cout << "Attack Enter - Combo: " << m_iCurrentCombo << endl;
}

void CKhazan_GS_Anim_Attack::Continue(_float fTimeDelta)
{
    if (m_isReserveSkill)  
    {
        /* 기다리는 중 다른 공격이 나오면 예약 취소 */
        if (m_iReserveSkillIndex == 0)
        {
            m_isReserveSkill = false;
            return;
        }
        if (Try_SkillAttack(m_iReserveSkillIndex))
            m_isReserveSkill = false;
        return;
    }

    Update_FastAttackCharge();

    Update_StrongAttack();

    Update_StrongAttackCharge();

    Update_SprintAttackCharge();

    Update_DodgeAttackCharge();

    Update_BrutalAttack();

    if (!m_isFastChargeStart) Update_FastAttackCombo();

    if (m_isSkillSingle) Update_Skill_Single();

    if (m_isBreathaking)Update_Skill_Breathaking();

    if (m_isBreathaking_Embryonic)Update_Skill_Breathaking_Embryonic();

    if (m_isBreathaking_Bloodshed)Update_Skill_Breathaking_Bloodshed();

    if (m_isGiantHunt) Update_Skill_GiantHunt();

    if (m_isPhantom) Update_Skill_Phantom();

    if (m_isBreakThrough) Update_Skill_BreakThrough();

    //if (m_isWarCry) Update_Skill_WarCry();

    if (m_isInnerFury) Update_Skill_InnerFury();
}

void CKhazan_GS_Anim_Attack::Exit()
{
    Clear_All();

    cout << "Attack Exit " << endl;
}

_bool CKhazan_GS_Anim_Attack::Try_FastAttack()
{
    // 콤보 3 완료 후 최소 시간이 안 지났으면 리셋
    if (m_iCurrentCombo == 3 && m_pModel->Check_MinAnimationTime() /*&&  *m_pModel->Get_CurTrackPosition() < m_fFastAttackComboPossibleMaxFrame*/)
    {
        m_iCurrentCombo = 0;
        m_isFastCombo = false;
        m_isCanNextCombo = false;
    }

    // 공격 중이고 다음 콤보 입력이 불가능하면 실패
    if (m_iCurrentCombo > 0 &&m_isAttacking && !m_isCanNextCombo)
        return false;

    //// 최소 애니메이션 시간 체크
    //if (!m_pModel->Check_MinAnimationTime() && m_iCurrentCombo >= 0 && m_iSelectedAnimationIndex == m_iCachedFastAttackAnimIndices[m_iCurrentCombo])
    //    return false;

    // 이미 콤보 진행 중이고 최소 시간이 안 지났으면 실패
    if (!m_pModel->Check_MinAnimationTime())
        return false;
    


    if (m_iCurrentCombo == 0) {
        if (m_pPlayerData->fCulStamina == 0.f)
            return false;

        m_iSelectedAnimationIndex = m_iCachedFastAttackAnimIndices[m_iCurrentCombo];
        m_pModel->Set_Animation(m_iSelectedAnimationIndex);
        m_iCurrentCombo = 1;

        m_pPlayerData->fCulStamina -= m_pPlayerData->fUsedStamina;
        m_pPlayerData->fBonusDamage = m_pPlayerData->fDamage;
    }
    else if (m_iCurrentCombo == 1) {
        if (m_pPlayerData->fCulStamina == 0.f)
            return false;

        m_iSelectedAnimationIndex = m_iCachedFastAttackAnimIndices[m_iCurrentCombo];
        m_pModel->Set_Animation(m_iSelectedAnimationIndex);
        m_iCurrentCombo = 2;

        m_pPlayerData->fCulStamina -= m_pPlayerData->fUsedStamina ;
        m_pPlayerData->fBonusDamage = m_pPlayerData->fDamage * 1.4f;
    }
    else if (m_iCurrentCombo == 2) {
        if (m_pPlayerData->fCulStamina == 0.f)
            return false;

        m_iSelectedAnimationIndex = m_iCachedFastAttackAnimIndices[m_iCurrentCombo];
        m_pModel->Set_Animation(m_iSelectedAnimationIndex);
        m_iCurrentCombo = 3;

        m_pPlayerData->fCulStamina -= m_pPlayerData->fUsedStamina ;
        m_pPlayerData->fBonusDamage = m_pPlayerData->fDamage * 2.f;
    }

    m_pPlayerData->fCulStamina = m_pPlayerData->fCulStamina < 0.f ? 0.f : m_pPlayerData->fCulStamina;

    m_isAttacking = true;
    m_isFastCombo = true;
    m_isCanNextCombo = false;
    m_iReserveSkillIndex = 0;

    return true;
}

_bool CKhazan_GS_Anim_Attack::Try_ChageFastAttack()
{
    if (!m_pModel->Check_MinAnimationTime())
        return false;

    if (m_isFastChargeStart)
        return false;

    /* 스킬 해금됐는지 검사 */
    if (!m_pClientInstance->Check_Skill(GS_SKILL::MOMENTUM))
        return false ;
    /* 스태미나 검사 */
    if (m_pPlayerData->fCulStamina == 0.f)
        return false;

    /* 차징 스타트  */
    m_isAttacking = true;
    m_isFastChargeStart = true;
    m_isFastChargeEnd = false;
    m_iReserveSkillIndex = 0;

    /* 콤보 리셋*/
  /*  m_iCurrentCombo = 0;
    m_isFastCombo = false;
    m_isCanNextCombo = false;*/

    m_isInFastCombo = m_isFastCombo;
    if (m_isInFastCombo)
    {
        m_iLastFastComboIndex = (m_iCurrentCombo > 0) ? (m_iCurrentCombo - 1) : 0;
       // m_iSelectedAnimationIndex = m_iCachedFastChargeAnimIndices[m_iLastFastComboIndex];
       // m_iSelectedAnimationIndex = m_iCachedFastChargeAnimIndices[m_iCurrentCombo];
        if (m_iCurrentCombo == 1)
            m_iSelectedAnimationIndex = m_iCachedFastChargeAnimIndices[1];
        if (m_iCurrentCombo == 2)
            m_iSelectedAnimationIndex = m_iCachedFastChargeAnimIndices[2];

        m_pModel->Set_Animation(m_iSelectedAnimationIndex);
        m_pClientInstance->Set_UsedSkill(GS_SKILL::MOMENTUM, true);
    

    }
    else
    {
        m_iLastFastComboIndex = 0;
        m_iSelectedAnimationIndex = m_iCachedFastChargeAnimIndices[0];
        m_pModel->Set_Animation(m_iSelectedAnimationIndex);
        m_pClientInstance->Set_UsedSkill(GS_SKILL::MOMENTUM, true);
    }

    /* Fast Combo Clear */
    m_iCurrentCombo = 0;
    m_isCanNextCombo = false;
    m_isFastCombo = false;

    return true;

}

void CKhazan_GS_Anim_Attack::Execute_ChargedFastAttack()
{
    // 차징 공격 (내부에서만 호출함)
    m_isFastChargedAttack = true;
    m_iSelectedAnimationIndex = m_iCachedFastChargeAttackAnimIndices[m_iLastFastComboIndex];
    m_pModel->Set_Animation(m_iSelectedAnimationIndex);

    m_pPlayerData->fCulStamina = max(0.f, m_pPlayerData->fCulStamina - m_pPlayerData->fUsedStamina * 2.f);
    m_pPlayerData->fBonusDamage = m_pPlayerData->fDamage * (2.5f + m_iLastFastComboIndex * 0.4f);
}

_bool CKhazan_GS_Anim_Attack::Try_StrongAttack()
{
    if (!m_pModel->Check_MinAnimationTime())
        return false;

    if (m_pPlayerData->fCulStamina == 0.f)
        return false;

    m_isStrongAttack = true;
    m_isAttacking = true;
    m_iReserveSkillIndex = 0;

    /* 콤보 리셋*/
    m_iCurrentCombo = 0;
    m_isFastCombo = false;
    m_isCanNextCombo = false;

    m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_StrongAtk03_Level0");
    m_pModel->Set_Animation(m_iSelectedAnimationIndex);

    m_pPlayerData->fCulStamina = max(0.f, m_pPlayerData->fCulStamina - m_pPlayerData->fUsedStamina * 1.4f);
    m_pPlayerData->fBonusDamage = m_pPlayerData->fDamage * 1.5f;

    return true;
}

_bool CKhazan_GS_Anim_Attack::Try_ChageStrongAttack()
{
    if (!m_pModel->Check_MinAnimationTime())
        return false;

    _bool isManifestStrength = m_pClientInstance->Check_Skill(GS_SKILL::MANIFESTSTRENGTH);
    _bool isLimitBreak = m_pClientInstance->Check_Skill(GS_SKILL::LIMIT_BREAK);

    /* 스킬 해금 후 투지 없으면  */
    if (isLimitBreak && m_pPlayerData->fCulDoggedness < 1.f)
    {
        return Try_StrongAttack();
    }

    /* 스태미나 검사  */
    if (m_pPlayerData->fCulStamina == 0.f)
        return false;

    /* 차징 스타트  */
    if (!m_isStrongChargeStart)
    {
        m_isAttacking = true;
        m_isStrongChargeStart = true;
        m_isFastChargeEnd = false;
        m_iReserveSkillIndex = 0;

        /* 콤보 리셋*/
        m_iCurrentCombo = 0;
        m_isFastCombo = false;
        m_isCanNextCombo = false;

        if (isLimitBreak) //한계극복
        {
            m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_ChargeMaster_Apocalypse_Charge_02");
            m_pClientInstance->Set_UsedSkill(GS_SKILL::LIMIT_BREAK, true);
        }
        else if (!isLimitBreak && isManifestStrength) //강기발현
        {
            m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_Com_StrongAtk01_Charge_2");
            m_pClientInstance->Set_UsedSkill(GS_SKILL::MANIFESTSTRENGTH, true);

        }
        else  //기본 
        {
            m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_Com_StrongAtk01_Charge");
        }
  

        m_pModel->Set_Animation(m_iSelectedAnimationIndex);

        return true;
    }

    return false;
}

void CKhazan_GS_Anim_Attack::Execute_ChargedStrongAttack()
{
    m_isStrongChargedAttack = true;
    _bool isLimitBreak = m_pClientInstance->Check_Skill(GS_SKILL::LIMIT_BREAK);


    if(!isLimitBreak)
        m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_StrongAtk03_Level0");
    else
        m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_ChargeMaster_Apocalypse_Atk");

    m_pModel->Set_Animation(m_iSelectedAnimationIndex);

    if (isLimitBreak)
        m_pPlayerData->fCulDoggedness -= 1.f;

    m_pPlayerData->fCulStamina = max(0.f, m_pPlayerData->fCulStamina - m_pPlayerData->fUsedStamina * 2.f);
    m_pPlayerData->fBonusDamage = m_pPlayerData->fDamage * 3.f;

}

_bool CKhazan_GS_Anim_Attack::Try_SprintAttack()
{
    if (m_isAttacking && m_isSprintChargeStart)
        return false;

    /* 스태미나 검사  */
    if (m_pPlayerData->fCulStamina == 0.f)
        return false;

    /* 차징 스타트  */
    m_isAttacking = true;
    m_isSprintChargeStart = true;
    m_isSprintChargeEnd = false;
    m_iReserveSkillIndex = 0;

    /* 콤보 리셋*/
    m_iCurrentCombo = 0;
    m_isFastCombo = false;
    m_isCanNextCombo = false;

    m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_Com_SprintAtk_Charge");
    m_pModel->Set_Animation(m_iSelectedAnimationIndex);

    return true;
}

void CKhazan_GS_Anim_Attack::Execute_SprintAttack()
{
    m_isSprintChargedAttack = true;
    m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_Com_SprintAtk");
    m_pModel->Set_Animation(m_iSelectedAnimationIndex);

    m_pPlayerData->fCulStamina = max(0.f, m_pPlayerData->fCulStamina - m_pPlayerData->fUsedStamina );
    m_pPlayerData->fBonusDamage = m_pPlayerData->fDamage * 1.5f;
}

_bool CKhazan_GS_Anim_Attack::Try_DodgeAttack(_uint iDir)
{
    if (m_isAttacking && m_isDodgeChargeStart)
        return false;

    /* 스태미나 검사  */
    if (m_pPlayerData->fCulStamina == 0.f)
        return false;

    /* 차징 스타트  */
    m_isAttacking = true;
    m_isDodgeChargeStart = true;
    m_isDodgeChargeEnd = false;
    m_iReserveSkillIndex = 0;
    m_iDodgeDirection = iDir;

    /* 콤보 리셋*/
    m_iCurrentCombo = 0;
    m_isFastCombo = false;
    m_isCanNextCombo = false;

    m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_DodgeAtk_F_Charge");
    m_pModel->Set_Animation(m_iSelectedAnimationIndex);

    return true;

}

void CKhazan_GS_Anim_Attack::Execute_DodgeAttack()
{
    m_isDodgeChargedAttack = true;

    if (m_iDodgeDirection & ENUM_CLASS(DIRECTION::B)) m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_DodgeAtk_B");
    else if (m_iDodgeDirection & ENUM_CLASS(DIRECTION::L)) m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_DodgeAtk_L");
    else if (m_iDodgeDirection & ENUM_CLASS(DIRECTION::R)) m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_DodgeAtk_R");
    else m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_DodgeAtk_F");

    m_pModel->Set_Animation(m_iSelectedAnimationIndex);

    m_pPlayerData->fCulStamina = max(0.f, m_pPlayerData->fCulStamina - m_pPlayerData->fUsedStamina );
    m_pPlayerData->fBonusDamage = m_pPlayerData->fDamage * 1.5f;
}

_bool CKhazan_GS_Anim_Attack::Try_GrappleAttack()
{
    if (m_isAttacking)
        return false;

    m_isAttacking = true;
    m_isBrutalStart = true;
    m_isBrutalEnd = false;
    m_iReserveSkillIndex = 0;

    /* 콤보 리셋*/
    m_iCurrentCombo = 0;
    m_isFastCombo = false;
    m_isCanNextCombo = false;

    m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_Com_Grapple_Atk_01");
    m_pModel->Set_Animation(m_iSelectedAnimationIndex);

    m_pPlayerData->fBonusDamage = m_pPlayerData->fDamage * 6.f;

    return true;
}

void CKhazan_GS_Anim_Attack::Execute_GappleAttack()
{
    m_isBrutalSecondAttack = true;

    m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_Com_Grapple_Atk_02");
    m_pModel->Set_Animation(m_iSelectedAnimationIndex);

    m_pPlayerData->fBonusDamage = m_pPlayerData->fDamage * 9.99f;
}

_bool CKhazan_GS_Anim_Attack::Try_SkillAttack(_uint iSkill)
{
    if (!m_pModel->Check_MinAnimationTime())
        return false;

    /* 임시 */
    m_pPlayerData->fCulDoggedness = 100.f;
   // m_pPlayerData->fCulStamina = 10000.f;

    auto checkSkill = [&](const _uint whatSkill) {return  m_pClientInstance->Check_Skill(whatSkill) && (iSkill & whatSkill); };

    /* 숨통끊기 */
    if (checkSkill(GS_SKILL::BREATHTAKING))
    {
        if (m_pPlayerData->fCulStamina == 0.f)
            return false;

        Clear_All();
        m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_GhostSlash_Charge01");
        m_isSkillChargeEnd = false;
        m_isBreathaking = true;

        m_pPlayerData->fCulStamina = max(0.f, m_pPlayerData->fCulStamina - m_pPlayerData->fUsedStamina);
        m_pPlayerData->fBonusDamage = m_pPlayerData->fDamage * 1.8f;

    }
    /* 숨통 끊기 :  태동 */
    else if (checkSkill(GS_SKILL::BREATHTAKING_EMBRYONIC))
    {
        if (m_pPlayerData->fCulStamina == 0.f)
            return false;

        Clear_All();
        m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_GhostSlash_Charge03_Turn");
        m_isSkillChargeEnd = false;
        m_isBreathaking_Embryonic = true;

        m_pPlayerData->fCulStamina = max(0.f, m_pPlayerData->fCulStamina - m_pPlayerData->fUsedStamina * 1.5f);
        m_pPlayerData->fBonusDamage = m_pPlayerData->fDamage * 2.3f;
    }

    /* 숨통끊기 : 선혈 */
    else if (checkSkill(GS_SKILL::BREATHTAKING_BLOODSHED))
    {
        if (m_pPlayerData->fCulStamina == 0.f)
            return false;

        Clear_All();
        m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_GhostSlash_Charge03");
        m_isSkillChargeEnd = false;
        m_isBreathaking_Bloodshed = true;

        m_pPlayerData->fCulStamina = max(0.f, m_pPlayerData->fCulStamina - m_pPlayerData->fUsedStamina * 2.4f);
        m_pPlayerData->fBonusDamage = m_pPlayerData->fDamage * 3.f;
    }


    /* 거인 사냥 */
    else if (checkSkill(GS_SKILL::GIANTHUNT))
    {
        if (m_pPlayerData->fCulDoggedness < 1.f || m_pPlayerData->fCulStamina == 0.f)
            return false;

        Clear_All();
        m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_AsheFork_Charge");
        m_isSkillChargeEnd = false;
        m_isGiantHunt = true;

        m_pPlayerData->fCulStamina = max(0.f, m_pPlayerData->fCulStamina - m_pPlayerData->fUsedStamina * 2.f);
        m_pPlayerData->fCulDoggedness -= 1.f;
        m_pPlayerData->fBonusDamage = m_pPlayerData->fDamage * 3.3f;
    }

    /* 귀신 : 어둠의 그림자 */
    else if (checkSkill(GS_SKILL::PHANTOM_SHADOWOFDARKNESS))
    {
        if (m_pPlayerData->fCulDoggedness < 2.f || m_pPlayerData->fCulStamina == 0.f)
            return false;

        Clear_All();
        m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_SoulbringerGhostLiberation_Charge");
        m_isSkillChargeEnd = false;
        m_isPhantom = true;
        m_pPlayerData->fCulStamina = max(0.f, m_pPlayerData->fCulStamina - m_pPlayerData->fUsedStamina * 2.5f);
        m_pPlayerData->fCulDoggedness -= 2.f;
        m_pPlayerData->fBonusDamage = m_pPlayerData->fDamage * 4.5f;
    }

    /* 정면 돌파  */
    else if (checkSkill(GS_SKILL::BREAK_THROUGH))
    {
        if (m_pPlayerData->fCulDoggedness < 1.f || m_pPlayerData->fCulStamina == 0.f)
            return false;

        Clear_All();
        m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_ChargeCrash_Charge");
        m_isSkillChargeEnd = false;
        m_isBreakThrough = true;

        m_pPlayerData->fCulStamina = max(0.f, m_pPlayerData->fCulStamina - m_pPlayerData->fUsedStamina * 1.5f);
        m_pPlayerData->fCulDoggedness -= 1.f;
        m_pPlayerData->fBonusDamage = m_pPlayerData->fDamage * 2.f;
    }

    /* 거대한 포효 */
    else if (checkSkill(GS_SKILL::WARCRY))
    {
        if (m_pPlayerData->fCulDoggedness < 1.f || m_pPlayerData->fCulStamina == 0.f)
            return false;

        Clear_All();
        m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_WarDeclaration_Atk");
        m_isSkillSingle = true;
        //m_isSkillChargeEnd = false;
        //m_isWarCry = true;

        m_pPlayerData->fCulStamina = max(0.f, m_pPlayerData->fCulStamina - m_pPlayerData->fUsedStamina * 1.5f);
        m_pPlayerData->fCulDoggedness -= 1.f;
        m_pPlayerData->fBonusDamage = m_pPlayerData->fDamage * 1.5f;
    }

    /* 내재된 분노  */
    else if (checkSkill(GS_SKILL::INNER_FURY))
    {
        if (m_pPlayerData->fCulDoggedness < 1.f || m_pPlayerData->fCulStamina == 0.f)
            return false;

        Clear_All();
        m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_RasingFurry_Charge");
        m_isSkillChargeEnd = false;
        m_isInnerFury = true;

        m_pPlayerData->fCulStamina = max(0.f, m_pPlayerData->fCulStamina - m_pPlayerData->fUsedStamina * 1.5f);
        m_pPlayerData->fCulDoggedness -= 1.f;
        m_pPlayerData->fBonusDamage = m_pPlayerData->fDamage * 1.5f;
    }

    m_isAttacking = true;
    m_isSkilling = true;

    /* 콤보 리셋*/
    m_iCurrentCombo = 0;
    m_isFastCombo = false;
    m_isCanNextCombo = false;

    m_iCurSkillIndex = iSkill;
    m_pClientInstance->Set_UsedSkill(iSkill, true);

    m_pModel->Set_Animation(m_iSelectedAnimationIndex);

    return true;
}

_bool CKhazan_GS_Anim_Attack::Reserve_SkillAttack(_uint iSkill)
{
    /* 스태미나 또는 투지 부족하면 예약 x  */
    auto checkSkill = [&](const _uint whatSkill) {return  m_pClientInstance->Check_Skill(whatSkill) && (iSkill & whatSkill); };

    /* 거센 기세 */
    if (checkSkill(GS_SKILL::BREATHTAKING))
    {
        if (m_pPlayerData->fCulStamina == 0.f)return false;
    }

    /* 숨통 끊기 :  태동 */
    else if (checkSkill(GS_SKILL::BREATHTAKING_EMBRYONIC))
    {
        if (m_pPlayerData->fCulStamina == 0.f)   return  false;
    }

    /* 숨통끊기 : 선혈 */
    else if (checkSkill(GS_SKILL::BREATHTAKING_BLOODSHED))
    {
        if (m_pPlayerData->fCulStamina == 0.f) return false;
    }

    /* 거인 사냥 */
    else if (checkSkill(GS_SKILL::GIANTHUNT))
    {
        if (m_pPlayerData->fCulDoggedness < 1.f || m_pPlayerData->fCulStamina == 0.f)   return false;
    }

    /* 귀신 : 어둠의 그림자 */
    else if (checkSkill(GS_SKILL::PHANTOM_SHADOWOFDARKNESS))
    {
        if (m_pPlayerData->fCulDoggedness < 2.f || m_pPlayerData->fCulStamina == 0.f)  return false;
    }

    /* 정면 돌파  */
    else if (checkSkill(GS_SKILL::BREAK_THROUGH))
    {
        if (m_pPlayerData->fCulDoggedness < 1.f || m_pPlayerData->fCulStamina == 0.f) return false;
    }

    /* 거대한 포효 */
    else if (checkSkill(GS_SKILL::WARCRY))
    {
        if (m_pPlayerData->fCulDoggedness < 1.f || m_pPlayerData->fCulStamina == 0.f)  return false;
    }

    /* 내재된 분노  */
    else if (checkSkill(GS_SKILL::INNER_FURY))
    {
        if (m_pPlayerData->fCulDoggedness < 1.f || m_pPlayerData->fCulStamina == 0.f)  return false;
    }

    m_isReserveSkill = true;
    m_iReserveSkillIndex = iSkill;

    return true;
}

void CKhazan_GS_Anim_Attack::Clear_Skill()
{
    m_isSkilling = { false };
    m_isAttacking = { false };
    m_isReserveSkill = { false };
    m_iReserveSkillIndex = { 0 };

    m_iCurSkillIndex = { 0 };
    m_isSkillChargeEnd = { false };
    m_isSkillChargeAttack = { false };
    m_isSkillSingle = { false };
    m_isBreathaking = { false };
    m_isBreathaking_Embryonic = { false };
    m_isBreathaking_Bloodshed = { false };
    m_isGiantHunt = { false };
    m_isPhantom = { false };
    m_isBreakThrough = { false };
    m_isWarCry = { false };
    m_isInnerFury = { false };
    m_isPossibleBreathtaking = { false };

    //m_pClientInstance->Set_UnUsedAllSkills();
    m_pClientInstance->Set_UsedSkills(
        GS_SKILL::BREATHTAKING 
        | GS_SKILL::BREATHTAKING_BLOODSHED
        | GS_SKILL::BREATHTAKING_EMBRYONIC
        | GS_SKILL::MANIFESTSTRENGTH
        | GS_SKILL::GIANTHUNT
        | GS_SKILL::PHANTOM_SHADOWOFDARKNESS 
        | GS_SKILL::LIMIT_BREAK
        | GS_SKILL::BREAK_THROUGH
        | GS_SKILL::WARCRY 
        | GS_SKILL::INNER_FURY

        , false
    );
}

void CKhazan_GS_Anim_Attack::Clear_All()
{
    //cout << "CKhazan_GS_Anim_Attack::Clear_All() " << endl;

    m_isSkilling = { false };
    m_isAttacking = { false };
    m_isReserveSkill = { false };
    m_iReserveSkillIndex = { 0 };
    m_iCurrentCombo = { 0 };
    m_isFastCombo = { false };
    m_isCanNextCombo = { false };
    m_iLastFastComboIndex = { 0 };
    m_isInFastCombo = { false };
    m_isFastChargeStart = { false };
    m_isFastChargeEnd = { false };
    m_isFastChargedAttack = { false };
    m_isStrongAttack = { false };
    m_isStrongChargeStart = { false }; 
    m_isStrongChargeEnd = { false };
    m_isStrongChargedAttack = { false };
    m_isSprintChargeStart = { false };
    m_isSprintChargeEnd = { false };
    m_isSprintChargedAttack = { false };
    m_iDodgeDirection = { 0 };
    m_isDodgeChargeStart = { false };
    m_isDodgeChargeEnd = { false };
    m_isDodgeChargedAttack = { false };
    m_isBrutalStart = { false };
    m_isBrutalEnd = { false };
    m_isBrutalSecondAttack = { false };

    m_iCurSkillIndex = { 0 };
    m_isSkillChargeEnd = { false };
    m_isSkillChargeAttack = { false };
    m_isSkillSingle = { false };
    m_isBreathaking = { false };
    m_isBreathaking_Embryonic = { false };
    m_isBreathaking_Bloodshed = { false };
    m_isGiantHunt = { false };
    m_isPhantom = { false };
    m_isBreakThrough = { false };
    m_isWarCry = { false };
    m_isInnerFury = { false };
    m_isPossibleBreathtaking = { false };

    //m_pClientInstance->Set_UnUsedAllSkills();
    m_pClientInstance->Set_UsedSkills(
        GS_SKILL::BREATHTAKING 
        | GS_SKILL::BREATHTAKING_BLOODSHED
        | GS_SKILL::BREATHTAKING_EMBRYONIC
        | GS_SKILL::MANIFESTSTRENGTH
        | GS_SKILL::GIANTHUNT 
        | GS_SKILL::PHANTOM_SHADOWOFDARKNESS
        | GS_SKILL::LIMIT_BREAK
        | GS_SKILL::BREAK_THROUGH 
        | GS_SKILL::WARCRY 
        | GS_SKILL::INNER_FURY
        
        , false
    );
}

_bool CKhazan_GS_Anim_Attack::Is_PossibleBreathtaking()
{
    /* 리턴값으로 판단하고 스킬예약걸자 */
    if(!m_isReserveSkill && m_isFastCombo && m_iCurrentCombo == 1)
        return true;

    return false;
}

_bool CKhazan_GS_Anim_Attack::Is_PossibleBreathtaking_Embryonic()
{
    /* 리턴값으로 판단하고 스킬예약걸자 */
    if (!m_isReserveSkill && m_isDodgeChargeStart)
        return true;

    return false;
}

_bool CKhazan_GS_Anim_Attack::Is_PossibleBreathtaking_BloodShed()
{
    /* 리턴값으로 판단하고 스킬예약걸자 */
    if (!m_isReserveSkill && m_isFastCombo && m_iCurrentCombo == 1)
        return true;

    return false;
}

void CKhazan_GS_Anim_Attack::Update_FastAttackCombo()
{
    if (!m_isFastCombo) return;

    _float trackPos = *m_pModel->Get_CurTrackPosition();
    _bool isMinTimePassed = m_pModel->Check_MinAnimationTime();
    _bool isFinished = m_pModel->IsFinished();

    /* 콤보공격에서 다음 공격이 가능한 구간 */
    if (isMinTimePassed && trackPos <= m_fFastAttackComboPossibleMaxFrame && m_iCurrentCombo < 3)
    {
        m_isCanNextCombo = true;
    }
    ///* 공격 중 */
    else if (!isMinTimePassed)
    {
        m_isAttacking = true;
        m_isCanNextCombo = false;
    }
    /* 콤보 3 완료 - 애니메이션이 끝났을 때 */
    else if (m_iCurrentCombo == 3 && (isMinTimePassed || isFinished))
    {
        m_isAttacking = false;
        m_isCanNextCombo = false;
        m_isFastCombo = false;
        m_iCurrentCombo = 0;
    }
    /* 콤보 중간에 입력이 끊긴 경우 */
    else if (isFinished && m_iCurrentCombo > 0 && m_iCurrentCombo < 3)  // 애니메이션 완료 체크
    {
        m_isAttacking = false;
        m_isCanNextCombo = false;
        m_iCurrentCombo = 0; 
        m_isFastCombo = false;
    }
    /* 다음 공격 가능 한 구간까지 지나감  */
    else if(m_pModel->IsAnimationStart(m_iSelectedAnimationIndex) &&  trackPos > m_fFastAttackComboPossibleMaxFrame)
    {
        m_isAttacking = false;
        m_isCanNextCombo = false;
        m_isFastCombo = false;
        m_iCurrentCombo = 0;
    }
}

void CKhazan_GS_Anim_Attack::Update_FastAttackCharge()
{
    /* 차징 공격이 아니면  리턴 */
    if (!m_isFastChargeStart)
        return;

    _bool isCurAnimStart = m_pModel->IsAnimationStart(m_iSelectedAnimationIndex);
    _bool isMinTime = m_pModel->Check_MinAnimationTime();
    /* 현재 애니메이션이 시작안했으면 리턴 */
    if (!isCurAnimStart)
             return;

    /* 현재 애니메이션이 최소 보장 시간이 안지났으면 리턴  */
    if (isCurAnimStart && !isMinTime)
        return;
   

    /* 차징 공격을 아직 안했으면 해라 */
    if (!m_isFastChargeEnd)
    {
        m_isFastChargeEnd = true;
        Execute_ChargedFastAttack();
        return;
    }

    /* 차징공격까지 다 끝마쳣으면 Clear */
    if (m_isFastChargedAttack)
    {
        m_isAttacking = { false };
        m_isInFastCombo = { false };
        m_isFastChargeStart = { false };
        m_isFastChargeEnd = { false };
        m_isFastChargedAttack = { false };
        m_iLastFastComboIndex = { 0 };

        m_pClientInstance->Set_UsedSkill(GS_SKILL::MOMENTUM, false);
    }

}

void CKhazan_GS_Anim_Attack::Update_StrongAttack()
{
    if (!m_isStrongAttack) return;

    if (m_pModel->IsAnimationStart(m_iSelectedAnimationIndex) && m_pModel->Check_MinAnimationTime())
    {
        m_isAttacking = { false };
        m_isStrongAttack = { false };
    }

}

void CKhazan_GS_Anim_Attack::Update_StrongAttackCharge()
{
    /* 차징 공격이 아니면  리턴 */
    if (!m_isStrongChargeStart)
        return;


    _bool isCurAnimStart = m_pModel->IsAnimationStart(m_iSelectedAnimationIndex);
    _bool isMinTime = m_pModel->Check_MinAnimationTime();
    /* 현재 애니메이션이 시작안했으면 리턴 */
    if (!isCurAnimStart)
        return;

    /* 현재 애니메이션이 최소 보장 시간이 안지났으면 리턴  */
    if (isCurAnimStart && !isMinTime)
        return;


    /* 차징 공격을 아직 안했으면 해라 */
    if (!m_isStrongChargeEnd)
    {
        m_isStrongChargeEnd = true;
        Execute_ChargedStrongAttack();
        return;
    }

    /* 차징공격까지 다 끝마쳣으면 Clear */
    if (m_isStrongChargedAttack && isMinTime)
    {
        m_isAttacking = { false };
        m_isStrongChargeStart = { false };
        m_isStrongChargeEnd = { false };
        m_isStrongChargedAttack = { false };
    }

}

void CKhazan_GS_Anim_Attack::Update_SprintAttackCharge()
{
    /* 차징 공격이 아니면  리턴 */
    if (!m_isSprintChargeStart)
        return;

    _bool isCurAnimStart = m_pModel->IsAnimationStart(m_iSelectedAnimationIndex);
    _bool isMinTime = m_pModel->Check_MinAnimationTime();
    /* 현재 애니메이션이 시작안했으면 리턴 */
    if (!isCurAnimStart)
        return;

    /* 현재 애니메이션이 최소 보장 시간이 안지났으면 리턴  */
    if (isCurAnimStart && !isMinTime)
        return;


    /* 차징 공격을 아직 안했으면 해라 */
    if (!m_isSprintChargeEnd)
    {
        m_isSprintChargeEnd = true;
        Execute_SprintAttack();
        return;
    }

    /* 차징공격까지 다 끝마쳣으면 Clear */
    if (m_isSprintChargedAttack && isMinTime)
    {
        m_isAttacking = { false };
        m_isSprintChargeStart = { false };
        m_isSprintChargeEnd = { false };
        m_isSprintChargedAttack = { false };
    }

}

void CKhazan_GS_Anim_Attack::Update_DodgeAttackCharge()
{
    /* 차징 공격이 아니면  리턴 */
    if (!m_isDodgeChargeStart)
        return;


    _bool isCurAnimStart = m_pModel->IsAnimationStart(m_iSelectedAnimationIndex);
    _bool isMinTime = m_pModel->Check_MinAnimationTime();
    /* 현재 애니메이션이 시작안했으면 리턴 */
    if (!isCurAnimStart)
        return;

    /* 현재 애니메이션이 최소 보장 시간이 안지났으면 리턴  */
    if (isCurAnimStart && !isMinTime)
        return;


    /* 차징 공격을 아직 안했으면 해라 */
    if (!m_isDodgeChargeEnd)
    {
        m_isDodgeChargeEnd = true;
        Execute_DodgeAttack();
        return;
    }

    /* 차징공격까지 다 끝마쳣으면 Clear */
    if (m_isDodgeChargedAttack && isMinTime)
    {
        m_isAttacking = { false };
        m_isDodgeChargeStart = { false };
        m_isDodgeChargeEnd = { false };
        m_isDodgeChargedAttack = { false };
    }

}

void CKhazan_GS_Anim_Attack::Update_BrutalAttack()
{
    /* 브루탈 어택이 아니면 리턴 */
    if (!m_isBrutalStart)
        return;

    _bool isCurAnimStart = m_pModel->IsAnimationStart(m_iSelectedAnimationIndex);
    _bool isMinTime = m_pModel->Check_MinAnimationTime();

    /* 현재 애니메이션이 시작안했으면 리턴 */
    if (!isCurAnimStart)
        return;

    /* 현재 애니메이션이 최소 보장 시간이 안지났으면 리턴  */
    if (isCurAnimStart && !isMinTime)
        return;

    /* 브루탈 2번쨰 공격을 아직 안했으면 해라 */
    if (!m_isBrutalEnd)
    {
        m_isBrutalEnd = true;
        Execute_GappleAttack();
        return;
    }

    /* 차징공격까지 다 끝마쳣으면 Clear */
    if (m_isBrutalSecondAttack && isMinTime)
    {
        m_isAttacking = { false };
        m_isBrutalStart = { false };
        m_isBrutalEnd = { false };
        m_isBrutalSecondAttack = { false };
    }

}

void CKhazan_GS_Anim_Attack::Update_Skill_Single()
{
    if (m_pModel->IsAnimationStart(m_iSelectedAnimationIndex) && m_pModel->Check_MinAnimationTime())
    {
        m_isSkilling = { false };
        m_isAttacking = { false };
        m_isReserveSkill = { false };
        m_iReserveSkillIndex = { 0 };

        m_iCurSkillIndex = { 0 };
        m_isSkillChargeEnd = { false };
        m_isSkillChargeAttack = { false };
        m_isSkillSingle = { false };

        m_pClientInstance->Set_UsedSkills(GS_SKILL::WARCRY, false);
    }

}

void CKhazan_GS_Anim_Attack::Update_Skill_Breathaking()
{
    /* 차징 공격이 아니면  리턴 */
    if (!m_isBreathaking)
        return;

    _bool isCurAnimStart = m_pModel->IsAnimationStart(m_iSelectedAnimationIndex);
    _bool isMinTime = m_pModel->Check_MinAnimationTime();
    /* 현재 애니메이션이 시작안했으면 리턴 */
    if (!isCurAnimStart)
        return;

    cout << *m_pModel->Get_CurTrackPosition() << endl;

    /* 현재 애니메이션이 최소 보장 시간이 안지났으면 리턴  */
    if (isCurAnimStart && !isMinTime)
        return;


    /* 차징 공격을 아직 안했으면 해라 */
    if (!m_isSkillChargeEnd)
    {
        m_isSkillChargeEnd = true;
        Execute_Skill_Breathaking();
        return;
    }

    /* 차징공격까지 다 끝마쳣으면 Clear */
    if (m_isSkillChargeAttack && isMinTime)
    {
        m_isSkilling = { false };
        m_isAttacking = { false };
        m_isReserveSkill = { false };
        m_iReserveSkillIndex = { 0 };

        m_iCurSkillIndex = { 0 };
        m_isSkillChargeEnd = { false };
        m_isSkillChargeAttack = { false };
        m_isBreathaking = { false };

        m_iCurrentCombo = { 0 };
        m_isFastCombo = { false };
        m_isCanNextCombo = { false };
        m_pClientInstance->Set_UsedSkill(GS_SKILL::BREATHTAKING, false);

    }
}

void CKhazan_GS_Anim_Attack::Update_Skill_Breathaking_Embryonic()
{
    /* 차징 공격이 아니면  리턴 */
    if (!m_isBreathaking_Embryonic)
        return;

    _bool isCurAnimStart = m_pModel->IsAnimationStart(m_iSelectedAnimationIndex);
    _bool isMinTime = m_pModel->Check_MinAnimationTime();
    /* 현재 애니메이션이 시작안했으면 리턴 */
    if (!isCurAnimStart)
        return;

    /* 현재 애니메이션이 최소 보장 시간이 안지났으면 리턴  */
    if (isCurAnimStart && !isMinTime)
        return;


    /* 차징 공격을 아직 안했으면 해라 */
    if (!m_isSkillChargeEnd)
    {
        m_isSkillChargeEnd = true;
        Execute_Skill_Breathaking_Embryonic();
        return;
    }

    /* 차징공격까지 다 끝마쳣으면 Clear */
    if (m_isSkillChargeAttack && isMinTime)
    {
        m_isSkilling = { false };
        m_isAttacking = { false };
        m_isReserveSkill = { false };
        m_iReserveSkillIndex = { 0 };

        m_iCurSkillIndex = { 0 };
        m_isSkillChargeEnd = { false };
        m_isSkillChargeAttack = { false };
        m_isBreathaking_Embryonic = { false };
        m_pClientInstance->Set_UsedSkill(GS_SKILL::BREATHTAKING_EMBRYONIC, false);

    }
}

void CKhazan_GS_Anim_Attack::Update_Skill_Breathaking_Bloodshed()
{
    /* 차징 공격이 아니면  리턴 */
    if (!m_isBreathaking_Bloodshed)
        return;

    _bool isCurAnimStart = m_pModel->IsAnimationStart(m_iSelectedAnimationIndex);
    _bool isMinTime = m_pModel->Check_MinAnimationTime();
    /* 현재 애니메이션이 시작안했으면 리턴 */
    if (!isCurAnimStart)
        return;

    /* 현재 애니메이션이 최소 보장 시간이 안지났으면 리턴  */
    if (isCurAnimStart && !isMinTime)
        return;


    /* 차징 공격을 아직 안했으면 해라 */
    if (!m_isSkillChargeEnd)
    {
        m_isSkillChargeEnd = true;
        Execute_Skill_Breathaking_Bloodshed();
        return;
    }

    /* 차징공격까지 다 끝마쳣으면 Clear */
    if (m_isSkillChargeAttack && isMinTime)
    {
        m_isSkilling = { false };
        m_isAttacking = { false };
        m_isReserveSkill = { false };
        m_iReserveSkillIndex = { 0 };

        m_iCurSkillIndex = { 0 };
        m_isSkillChargeEnd = { false };
        m_isSkillChargeAttack = { false };
        m_isBreathaking_Bloodshed = { false };

        m_iCurrentCombo = { 0 };
        m_isFastCombo = { false };
        m_isCanNextCombo = { false };

        m_pClientInstance->Set_UsedSkill(GS_SKILL::BREATHTAKING_BLOODSHED, false);

    }

}

void CKhazan_GS_Anim_Attack::Update_Skill_GiantHunt()
{
    /* 차징 공격이 아니면  리턴 */
    if (!m_isGiantHunt)
        return;

    _bool isCurAnimStart = m_pModel->IsAnimationStart(m_iSelectedAnimationIndex);
    _bool isMinTime = m_pModel->Check_MinAnimationTime();
    /* 현재 애니메이션이 시작안했으면 리턴 */
    if (!isCurAnimStart)
        return;

    /* 현재 애니메이션이 최소 보장 시간이 안지났으면 리턴  */
    if (isCurAnimStart && !isMinTime)
        return;


    /* 차징 공격을 아직 안했으면 해라 */
    if (!m_isSkillChargeEnd)
    {
        m_isSkillChargeEnd = true;
        Execute_Skill_GiantHunt();
        return;
    }

    /* 차징공격까지 다 끝마쳣으면 Clear */
    if (m_isSkillChargeAttack && isMinTime)
    {
        m_isSkilling = { false };
        m_isAttacking = { false };
        m_isReserveSkill = { false };
        m_iReserveSkillIndex = { 0 };

        m_iCurSkillIndex = { 0 };
        m_isSkillChargeEnd = { false };
        m_isSkillChargeAttack = { false };
        m_isGiantHunt = { false };
        m_pClientInstance->Set_UsedSkill(GS_SKILL::GIANTHUNT, false);

    }
}

void CKhazan_GS_Anim_Attack::Update_Skill_Phantom()
{
    /* 차징 공격이 아니면  리턴 */
    if (!m_isPhantom)
        return;

    _bool isCurAnimStart = m_pModel->IsAnimationStart(m_iSelectedAnimationIndex);
    _bool isMinTime = m_pModel->Check_MinAnimationTime();
    /* 현재 애니메이션이 시작안했으면 리턴 */
    if (!isCurAnimStart)
        return;

    /* 현재 애니메이션이 최소 보장 시간이 안지났으면 리턴  */
    if (isCurAnimStart && !isMinTime)
        return;


    /* 차징 공격을 아직 안했으면 해라 */
    if (!m_isSkillChargeEnd)
    {
        m_isSkillChargeEnd = true;
        Execute_Skill_Phantom();
        return;
    }

    /* 차징공격까지 다 끝마쳣으면 Clear */
    if (m_isSkillChargeAttack && isMinTime)
    {
        m_isSkilling = { false };
        m_isAttacking = { false };
        m_isReserveSkill = { false };
        m_iReserveSkillIndex = { 0 };

        m_iCurSkillIndex = { 0 };
        m_isSkillChargeEnd = { false };
        m_isSkillChargeAttack = { false };
        m_isPhantom = { false };
        m_pClientInstance->Set_UsedSkill(GS_SKILL::PHANTOM_SHADOWOFDARKNESS, false);

    }
}

void CKhazan_GS_Anim_Attack::Update_Skill_BreakThrough()
{
    /* 차징 공격이 아니면  리턴 */
    if (!m_isBreakThrough)
        return;

    _bool isCurAnimStart = m_pModel->IsAnimationStart(m_iSelectedAnimationIndex);
    _bool isMinTime = m_pModel->Check_MinAnimationTime();
    /* 현재 애니메이션이 시작안했으면 리턴 */
    if (!isCurAnimStart)
        return;

    /* 현재 애니메이션이 최소 보장 시간이 안지났으면 리턴  */
    if (isCurAnimStart && !isMinTime)
        return;


    /* 차징 공격을 아직 안했으면 해라 */
    if (!m_isSkillChargeEnd)
    {
        m_isSkillChargeEnd = true;
        Execute_Skill_BreakThrough();
        return;
    }

    /* 차징공격까지 다 끝마쳣으면 Clear */
    if (m_isSkillChargeAttack && isMinTime)
    {
        m_isSkilling = { false };
        m_isAttacking = { false };
        m_isReserveSkill = { false };
        m_iReserveSkillIndex = { 0 };

        m_iCurSkillIndex = { 0 };
        m_isSkillChargeEnd = { false };
        m_isSkillChargeAttack = { false };
        m_isBreakThrough = { false };
        m_pClientInstance->Set_UsedSkill(GS_SKILL::BREAK_THROUGH, false);
    }
}

//void CKhazan_GS_Anim_Attack::Update_Skill_WarCry()
//{
//    /* 차징 공격이 아니면  리턴 */
//    if (!m_isWarCry)
//        return;
//
//    _bool isCurAnimStart = m_pModel->IsAnimationStart(m_iSelectedAnimationIndex);
//    _bool isMinTime = m_pModel->Check_MinAnimationTime();
//    /* 현재 애니메이션이 시작안했으면 리턴 */
//    if (!isCurAnimStart)
//        return;
//
//    /* 현재 애니메이션이 최소 보장 시간이 안지났으면 리턴  */
//    if (isCurAnimStart && !isMinTime)
//        return;
//
//
//    /* 차징 공격을 아직 안했으면 해라 */
//    if (!m_isSkillChargeEnd)
//    {
//        m_isSkillChargeEnd = true;
//        Execute_Skill_WarCry();
//        return;
//    }
//
//    /* 차징공격까지 다 끝마쳣으면 Clear */
//    if (m_isSkillChargeAttack && isMinTime)
//    {
//        m_isSkilling = { false };
//        m_isAttacking = { false };
//        m_isReserveSkill = { false };
//        m_iReserveSkillIndex = { 0 };
//
//        m_iCurSkillIndex = { 0 };
//        m_isSkillChargeEnd = { false };
//        m_isSkillChargeAttack = { false };
//        m_isWarCry = { false };
//        m_pClientInstance->Set_UsedSkill(GS_SKILL::WARCRY, false);
//    }
//}

void CKhazan_GS_Anim_Attack::Update_Skill_InnerFury()
{
    /* 차징 공격이 아니면  리턴 */
    if (!m_isInnerFury)
        return;

    _bool isCurAnimStart = m_pModel->IsAnimationStart(m_iSelectedAnimationIndex);
    _bool isMinTime = m_pModel->Check_MinAnimationTime();
    /* 현재 애니메이션이 시작안했으면 리턴 */
    if (!isCurAnimStart)
        return;

    /* 현재 애니메이션이 최소 보장 시간이 안지났으면 리턴  */
    if (isCurAnimStart && !isMinTime)
        return;


    /* 차징 공격을 아직 안했으면 해라 */
    if (!m_isSkillChargeEnd)
    {
        m_isSkillChargeEnd = true;
        Execute_Skill_InnerFury();
        return;
    }

    /* 차징공격까지 다 끝마쳣으면 Clear */
    if (m_isSkillChargeAttack && isMinTime)
    {
        m_isSkilling = { false };
        m_isAttacking = { false };
        m_isReserveSkill = { false };
        m_iReserveSkillIndex = { 0 };

        m_iCurSkillIndex = { 0 };
        m_isSkillChargeEnd = { false };
        m_isSkillChargeAttack = { false };
        m_isInnerFury = { false };
        m_pClientInstance->Set_UsedSkill(GS_SKILL::INNER_FURY, false);
    }
}

void CKhazan_GS_Anim_Attack::Execute_Skill_Breathaking()
{
    m_isSkillChargeAttack = true;
    m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_GhostSlash_Atk");
    m_pModel->Set_Animation(m_iSelectedAnimationIndex);
}

void CKhazan_GS_Anim_Attack::Execute_Skill_Breathaking_Embryonic()
{
    m_isSkillChargeAttack = true;
    m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_GhostSlash_Atk");
    m_pModel->Set_Animation(m_iSelectedAnimationIndex);
}

void CKhazan_GS_Anim_Attack::Execute_Skill_Breathaking_Bloodshed()
{
    m_isSkillChargeAttack = true;
    m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_GhostSlash_ChargeAtk");
    m_pModel->Set_Animation(m_iSelectedAnimationIndex);
}

void CKhazan_GS_Anim_Attack::Execute_Skill_GiantHunt()
{
    m_isSkillChargeAttack = true;
    m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_AsheFork_Atk");
    m_pModel->Set_Animation(m_iSelectedAnimationIndex);
}

void CKhazan_GS_Anim_Attack::Execute_Skill_Phantom()
{
    m_isSkillChargeAttack = true;
    m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_SoulbringerGhostLiberation_ChargeAtk");
    m_pModel->Set_Animation(m_iSelectedAnimationIndex);
}

void CKhazan_GS_Anim_Attack::Execute_Skill_BreakThrough()
{
    m_isSkillChargeAttack = true;
    m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_ChargeCrash_Atk");
    m_pModel->Set_Animation(m_iSelectedAnimationIndex);
}

//void CKhazan_GS_Anim_Attack::Execute_Skill_WarCry()
//{
//    m_isSkillChargeAttack = true;
//    m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_WarDeclaration_Atk");
//    m_pModel->Set_Animation(m_iSelectedAnimationIndex);
//}

void CKhazan_GS_Anim_Attack::Execute_Skill_InnerFury()
{
    m_isSkillChargeAttack = true;
    m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_RasingFurry");
    m_pModel->Set_Animation(m_iSelectedAnimationIndex);
}

CKhazan_GS_Anim_Attack* CKhazan_GS_Anim_Attack::Create()
{
    return new CKhazan_GS_Anim_Attack;
}

void CKhazan_GS_Anim_Attack::Free()
{
    __super::Free();

    Safe_Release(m_pClientInstance);

}
