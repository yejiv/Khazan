#include "Khazan_Spear_Anim_Attack.h"
#include "GameInstance.h"
#include "ClientInstance.h"


CKhazan_Spear_Anim_Attack::CKhazan_Spear_Anim_Attack()
    : m_pClientInstance{ CClientInstance::GetInstance() }
{
    Safe_AddRef(m_pClientInstance);
}

HRESULT CKhazan_Spear_Anim_Attack::Initialize_Prototype()
{
    return S_OK;
}

void CKhazan_Spear_Anim_Attack::Enter()
{
    m_isAttacking = true;
    m_isCanNextCombo = false;

}

void CKhazan_Spear_Anim_Attack::Continue(_float fTimeDelta)
{

    if (m_isFastCombo)
    {
        /* 콤보공격에서 다음 공격이 가능 한 구간이라면  */
        if (m_pModel->Check_MinAnimationTime() && (*m_pModel->Get_CurTrackPosition() <= m_fFastAttackComboPossibleMaxFrame)) {
            m_isCanNextCombo = true;
        }
        /* 공격 중 */
        else if (!m_pModel->Check_MinAnimationTime())
        {
            m_isAttacking = true;
            m_isCanNextCombo = false;
        }
        /* 다음 공격 가능 한 구간까지 지나감 */
        else
        {
            m_isAttacking = false;
            m_isCanNextCombo = false;
            //if (m_iCurrentCombo >= 3)
            //{
                m_iCurrentCombo = 0;
                m_isFastCombo = false;
            //}
        }
    }
    else if (m_isStrongCombo)
    {
        /* 콤보공격에서 다음 공격이 가능 한 구간이라면  */
        if (m_pModel->Check_MinAnimationTime() && *m_pModel->Get_CurTrackPosition() <= m_fStrongAttackComboPossibleMaxFrame) {
            m_isCanNextCombo = true;
        }
        /* 공격 중 */
        else if (!m_pModel->Check_MinAnimationTime())
        {
            m_isAttacking = true;
            m_isCanNextCombo = false;
        }
        /* 다음 공격 가능 한 구간까지 지나감 */
        else
        {
            m_isAttacking = false;
            m_isCanNextCombo = false;
            //if (m_iCurrentCombo >= 3)
            //{
                m_iCurrentCombo = 0;
                m_isStrongCombo = false;
            //}
        }
    }
    else if (m_isStrongCharge)
    {

        if (m_iSelectedAnimationIndex == m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Com_StrongAtk_Charge")  && m_pModel->IsFinished())
        {
            m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Com_StrongAtk_ChargeAtk");
            m_pModel->Set_Animation(m_iSelectedAnimationIndex);
        }
        // Charge Attack 애니메이션 종료
        else if (m_iSelectedAnimationIndex == m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Com_StrongAtk_ChargeAtk") && m_pModel->IsFinished())
        {
            m_isAttacking = false;
            m_isStrongCharge = false;
        }
    }


    else  if (m_isAttacking)
    {
        if (m_pModel->Check_MinAnimationTime())
        {
            m_isAttacking = false;
        }


    }

}

void CKhazan_Spear_Anim_Attack::Exit()
{
    m_isAttacking = false;
    m_isCanNextCombo = false;
    m_iCurrentCombo = 0;
    m_isFastCombo = false;
    m_isStrongCombo = false;
    m_isStrongCharge = false;

}

_bool CKhazan_Spear_Anim_Attack::Try_FallAttack()
{
    //if (!m_isAttacking)
    //    return false;
    //m_isAttacking = true;

    //m_pModel->Set_Animation(m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Sprint_BL_Start"))

    return true;
}

_bool CKhazan_Spear_Anim_Attack::Try_FastAttack()
{
    if (m_isAttacking && !m_isCanNextCombo)
    {
        OutputDebugStringA("[Attack] Cannot attack yet\n");
        return false;
    }
    // 최소 애니메이션 시간 체크
    if (!m_pModel->Check_MinAnimationTime() && m_iCurrentCombo > 0)
    {
        OutputDebugStringA("[Attack] Min time not passed\n");
        return false;
    }

    if (m_isStrongCombo)
    {
        m_isStrongCombo = false;
        m_iCurrentCombo = 0;
    }


    m_isAttacking = true;
    m_isFastCombo = true;
    m_isCanNextCombo = false;

    if (m_iCurrentCombo == 0) {
        m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Com_FastAtk01");
        m_pModel->Set_Animation(m_iSelectedAnimationIndex);
        m_iCurrentCombo = 1;
        m_isCanNextCombo = false;
        m_isFastCombo = true;
    }
    else if (m_iCurrentCombo == 1) {
        m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Com_FastAtk02");
        m_pModel->Set_Animation(m_iSelectedAnimationIndex);
        m_iCurrentCombo = 2;
        m_isCanNextCombo = false;
    }
    else if (m_iCurrentCombo == 2) {
        if (m_pClientInstance->Check_SpearSkill(SPEARSKILL::FULL_MOON))  m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_LightningSpear_Advanced");
        else  m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Com_FastAtk03_02");

        m_pModel->Set_Animation(m_iSelectedAnimationIndex);
        m_iCurrentCombo = 3;
        m_isFastCombo = false;
    }

    return true;

}

_bool CKhazan_Spear_Anim_Attack::Try_GrappleAttack()
{
    if (m_isAttacking)
        return false;
    m_isAttacking = true;

    return _bool();
}

_bool CKhazan_Spear_Anim_Attack::Try_SingleSkillAttack(_uint iSkill)
{
    if (m_isAttacking)
        return false;
    m_isAttacking = true;


    if (m_pClientInstance->Check_SpearSkill(SPEARSKILL::MOONLIGHT_SLASH))  m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_SpaceTimeCutter03");
    else  if (m_pClientInstance->Check_SpearSkill(SPEARSKILL::SPIRAL_THRUST))  m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Tempest_SpiralSpear");
    else  if (m_pClientInstance->Check_SpearSkill(SPEARSKILL::SPIRAL_THRUST_WHIRLWIND))  m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Tempest_TwisterSpear");
    else  if (m_pClientInstance->Check_SpearSkill(SPEARSKILL::ASSAULT))  m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_PureMind_SeismicKick");
    else  if (m_pClientInstance->Check_SpearSkill(SPEARSKILL::MOMENT_SLASH))  m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName(" CA_P_Kazan_Spear_LowFlying_F");
    else  if (m_pClientInstance->Check_SpearSkill(SPEARSKILL::CRITICAL_STRIKE))  m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Crescent");
    else  if (m_pClientInstance->Check_SpearSkill(SPEARSKILL::SHADOW_CLEAVE))  m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Tempest_MoonVeil");
    else
    {
        m_isAttacking = false;
        return false;
    }

    m_pModel->Set_Animation(m_iSelectedAnimationIndex);


    return true;
}

_bool CKhazan_Spear_Anim_Attack::Try_CounterAttack()
{
    if (m_isAttacking)
        return false;
    m_isAttacking = true;

    m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Com_CounterAction");
    m_pModel->Set_Animation(m_iSelectedAnimationIndex);

    return true;
}

_bool CKhazan_Spear_Anim_Attack::Try_DodgeAttack(_uint iDir)
{
    if (m_isAttacking)
        return false;
    m_isAttacking = true;

    if (iDir & ENUM_CLASS(DIRECTION::F)) m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Com_DodgeAtk_F");
    else if (iDir & ENUM_CLASS(DIRECTION::B)) m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Com_DodgeAtk_B");
    else if (iDir & ENUM_CLASS(DIRECTION::L)) m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Com_DodgeAtk_L");
    else if (iDir & ENUM_CLASS(DIRECTION::R)) m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Com_DodgeAtk_R");

    m_pModel->Set_Animation(m_iSelectedAnimationIndex);

    return true;
}

_bool CKhazan_Spear_Anim_Attack::Try_SprintFastAttack()
{
    if (m_isAttacking)
        return false;
    m_isAttacking = true;

    m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_SprintAtk_Fast");
    m_pModel->Set_Animation(m_iSelectedAnimationIndex);

    return true;
}

_bool CKhazan_Spear_Anim_Attack::Try_SprintStrongAttack()
{
    if (m_isAttacking)
        return false;
    m_isAttacking = true;

    m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_SprintAtk_Strong");
    m_pModel->Set_Animation(m_iSelectedAnimationIndex);


    return true;
}

_bool CKhazan_Spear_Anim_Attack::Try_StrongAttack()
{

    // 첫 공격이거나, 콤보 가능 상태일 때만 실행
    if (m_isAttacking && !m_isCanNextCombo)
    {
        OutputDebugStringA("[Attack] Cannot attack yet\n");
        return false;
    }

    // 최소 애니메이션 시간 체크
    if (!m_pModel->Check_MinAnimationTime() && m_iCurrentCombo > 0)
    {
        OutputDebugStringA("[Attack] Min time not passed\n");
        return false;
    }

    if (m_isFastCombo)
    {
        m_isFastCombo = false;
        m_iCurrentCombo = 0;
    }


    m_isAttacking = true;
    m_isStrongCombo = true;
    m_isCanNextCombo = false;

    if (m_iCurrentCombo == 0)
    {
        m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Com_StrongAtk01");
        m_pModel->Set_Animation(m_iSelectedAnimationIndex);
        m_iCurrentCombo = 1;
        OutputDebugStringA("[Attack] Strong Attack 1\n");
    }
    else if (m_iCurrentCombo == 1)
    {
        m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Com_StrongAtk02");
        m_pModel->Set_Animation(m_iSelectedAnimationIndex);
        m_iCurrentCombo = 2;
        OutputDebugStringA("[Attack] Strong Attack 2\n");
    }
    else if (m_iCurrentCombo == 2)
    {
        m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Com_StrongAtk03");
        m_pModel->Set_Animation(m_iSelectedAnimationIndex);
        m_iCurrentCombo = 3; // 마지막 콤보
        OutputDebugStringA("[Attack] Strong Attack 3 (Final)\n");
    }

    return true;

}

_bool CKhazan_Spear_Anim_Attack::Try_ChageStrongAttack()
{
    if (m_isStrongCharge)
    {
        OutputDebugStringA("[Attack] Already Charging\n");
        return false;
    }

    m_isAttacking = true;
    m_isStrongCharge = true;

    m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Com_StrongAtk_Charge");
    m_pModel->Set_Animation(m_iSelectedAnimationIndex);

    OutputDebugStringA("[Attack] Charge Strong Attack Started\n");
    return true;
}



CKhazan_Spear_Anim_Attack* CKhazan_Spear_Anim_Attack::Create()
{
    return new CKhazan_Spear_Anim_Attack;
}

void CKhazan_Spear_Anim_Attack::Free()
{
    __super::Free();
    Safe_Release(m_pClientInstance);


}
