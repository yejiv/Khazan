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
//void CKhazan_Spear_Anim_Attack::Continue(_float fTimeDelta)
//{
//    if (m_isReserve)
//    {
//        if (m_iReserveSkillIndex == 0)
//        {
//            m_isReserve = false;
//            return;
//        }
//
//        if (Try_SkillAttack(m_iReserveSkillIndex))
//            m_isReserve = false;
//
//        return;
//    }
//
//    if (m_isFastCombo)
//    {
//        if (m_pModel->Check_MinAnimationTime() && (*m_pModel->Get_CurTrackPosition() <= m_fFastAttackComboPossibleMaxFrame))
//        {
//            m_isCanNextCombo = true;
//        }
//        else if (!m_pModel->Check_MinAnimationTime())
//        {
//            m_isAttacking = true;
//            m_isCanNextCombo = false;
//        }
//        else if (m_iCurrentCombo == 3 && m_pModel->IsFinished())
//        {
//            m_isAttacking = false;
//            m_isCanNextCombo = false;
//            m_iCurrentCombo = 0;
//            m_isFastCombo = false;
//        }
//        else if (m_pModel->IsFinished())
//        {
//            m_isAttacking = false;
//            m_isCanNextCombo = false;
//            m_iCurrentCombo = 0;
//            m_isFastCombo = false;
//        }
//        return;
//    }
//
//    if (m_isStrongCombo)
//    {
//        if (m_pModel->Check_MinAnimationTime() && *m_pModel->Get_CurTrackPosition() <= m_fStrongAttackComboPossibleMaxFrame)
//        {
//            m_isCanNextCombo = true;
//        }
//        else if (!m_pModel->Check_MinAnimationTime())
//        {
//            m_isAttacking = true;
//            m_isCanNextCombo = false;
//        }
//        else if (m_iCurrentCombo == 3 && m_pModel->IsFinished())
//        {
//            m_isAttacking = false;
//            m_isCanNextCombo = false;
//            m_iCurrentCombo = 0;
//            m_isStrongCombo = false;
//        }
//        else if (m_pModel->IsFinished())
//        {
//            m_isAttacking = false;
//            m_isCanNextCombo = false;
//            m_iCurrentCombo = 0;
//            m_isStrongCombo = false;
//        }
//        return;
//    }
//
//    if (m_isStrongCharge)
//    {
//        _uint curAnimIndex = m_pModel->Get_CurAnimIndex();
//        _uint chargeAnimIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Com_StrongAtk_Charge");
//        _uint chargeAtkAnimIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Com_StrongAtk_ChargeAtk");
//
//        // Charge -> ChargeAtk 전환
//        if (curAnimIndex == chargeAnimIndex && m_pModel->IsFinished())
//        {
//            m_iSelectedAnimationIndex = chargeAtkAnimIndex;
//            m_pModel->Set_Animation(m_iSelectedAnimationIndex);
//            cout << "Charge -> ChargeAtk transition" << endl;
//        }
//        // ChargeAtk 종료
//        else if (curAnimIndex == chargeAtkAnimIndex && m_pModel->IsFinished())
//        {
//            m_isAttacking = false;
//            m_isStrongCharge = false;
//            cout << "ChargeAtk finished" << endl;
//        }
//        return;
//    }
//
//    _bool isFinish = m_pModel->IsFinished();
//    _bool isMinTime = m_pModel->Check_MinAnimationTime();
//
//    if (m_isSkilling)
//    {
//        if (m_iCurSkillIndex == m_pModel->Get_CurAnimIndex() && (isFinish || isMinTime))
//        {
//            m_pClientInstance->Set_UsedSkill(m_iCurSkillIndex, false);
//            m_iCurSkillIndex = 0;
//            m_isSkilling = false;
//        }
//    }
//
//    if (m_isAttacking)
//    {
//        if (isFinish || isMinTime)
//        {
//            m_isAttacking = false;
//        }
//    }
//}
void CKhazan_Spear_Anim_Attack::Continue(_float fTimeDelta)
{
    if (m_isReserve)
    {
        /* 기다리는 중 다른 공격이 나오면 예약 취소 */
        if (m_iReserveSkillIndex == 0)
        {
            m_isReserve = false;
            return; 
        }

        if (Try_SkillAttack(m_iReserveSkillIndex))
            m_isReserve = false;

        return;
    }

    if (m_isFastCombo)
    {
        /* 콤보공격에서 다음 공격이 가능한 구간 */
        if (m_pModel->Check_MinAnimationTime() && (*m_pModel->Get_CurTrackPosition() <= m_fFastAttackComboPossibleMaxFrame))
        {
            m_isCanNextCombo = true;
        }
        /* 공격 중 */
        else if (!m_pModel->Check_MinAnimationTime())
        {
            m_isAttacking = true;
            m_isCanNextCombo = false;
        }
        /* 콤보 3 완료 - 애니메이션이 끝났을 때 */
        else if (m_iCurrentCombo == 3 && (m_pModel->Check_MinAnimationTime() || m_pModel->IsFinished()))  // IsFinished() 체크 추가
        {
            m_isAttacking = false;
            m_isCanNextCombo = false;
            m_iCurrentCombo = 0;  // 리셋
            m_isFastCombo = false;
        }
        /* 콤보 중간에 입력이 끊긴 경우 */
        else if (m_pModel->IsFinished())  // 애니메이션 완료 체크
        {
            m_isAttacking = false;
            m_isCanNextCombo = false;
            m_iCurrentCombo = 0;  // 리셋
            m_isFastCombo = false;
        }
        /* 다음 공격 가능 한 구간까지 지나감  */
        else
        {
            m_isAttacking = false;
            m_isCanNextCombo = false;
            m_iCurrentCombo = 0;
            m_isFastCombo = false;
        }
        return;
    }

    if (m_isStrongCombo)
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
        /* 콤보 3 완료 - 애니메이션이 끝났을 때 */
        else if (m_iCurrentCombo == 3 && (m_pModel->Check_MinAnimationTime() || m_pModel->IsFinished()))  // IsFinished() 체크 추가
        {
            m_isAttacking = false;
            m_isCanNextCombo = false;
            m_iCurrentCombo = 0;  // 리셋
            m_isStrongCombo = false;
        }
        /* 콤보 중간에 입력이 끊긴 경우 */
        else if (m_pModel->IsFinished())  // 애니메이션 완료 체크
        {
            m_isAttacking = false;
            m_isCanNextCombo = false;
            m_iCurrentCombo = 0;  // 리셋
            m_isStrongCombo = false;
        }
        /* 다음 공격 가능 한 구간까지 지나감 */
        else
        {
            m_isAttacking = false;
            m_isCanNextCombo = false;
            m_iCurrentCombo = 0;
            m_isStrongCombo = false;

        }
        return;
    }

    //if (m_isStrongCharge)
    //{

    //    if (m_iSelectedAnimationIndex == m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Com_StrongAtk_Charge") && m_pModel->IsFinished())
    //    {
    //        m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Com_StrongAtk_ChargeAtk");
    //        m_pModel->Set_Animation(m_iSelectedAnimationIndex);
    //    }
    //    // Charge Attack 애니메이션 종료
    //    else if (m_iSelectedAnimationIndex == m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Com_StrongAtk_ChargeAtk")&& m_pModel->Check_MinAnimationTime())
    //    {
    //        m_isAttacking = false;
    //        m_isStrongCharge = false;
    //    }
    //    return;
    //}
    if (m_isStrongCharge)
    {
        _uint curAnimIndex = m_pModel->Get_CurAnimIndex();
        _uint chargeAnimIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Com_StrongAtk_Charge");
        _uint chargeAtkAnimIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Com_StrongAtk_ChargeAtk");

        // Charge -> ChargeAtk 전환
        if (curAnimIndex == chargeAnimIndex && m_pModel->IsFinished())
        {
            m_iSelectedAnimationIndex = chargeAtkAnimIndex;
            m_pModel->Set_Animation(m_iSelectedAnimationIndex);
            cout << "Charge -> ChargeAtk transition" << endl;
        }
        // ChargeAtk 종료
        else if (curAnimIndex == chargeAtkAnimIndex && m_pModel->IsFinished())
        {
            m_isAttacking = false;
            m_isStrongCharge = false;
            cout << "ChargeAtk finished" << endl;
        }
        return;
    }
    _bool isFinish = m_pModel->IsFinished();
    _bool isMinTime = m_pModel->Check_MinAnimationTime();

    if (m_isSkilling)
    {
        if (m_iCurSkillIndex == m_pModel->Get_CurAnimIndex() && isFinish || isMinTime)
        {
            m_pClientInstance->Set_UsedSkill(m_iCurSkillIndex, false);

            m_iCurSkillIndex = 0;
            m_isSkilling = false;
        }
    }
    if (m_isAttacking)
    {
        if (isFinish || isMinTime)
        {
            m_isAttacking = false;
        }
    }

    
}

void CKhazan_Spear_Anim_Attack::Exit()
{
    m_isSkilling = false;
    m_isAttacking = false;
    m_isCanNextCombo = false;
    m_iCurrentCombo = 0;
    m_isFastCombo = false;
    m_isStrongCombo = false;
    m_isStrongCharge = false;
    m_isReserve = false;
    m_iReserveSkillIndex = 0;

    cout << "Attack Exit " << endl;
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
    if (m_iCurrentCombo == 3 &&  !m_pModel->Check_MinAnimationTime())
    {
        m_iCurrentCombo = 0;
        //cout << "Try_FastAttack blocked - Combo 3 in progress" << endl;
        //return false;
    }

    if (m_isAttacking && !m_isCanNextCombo)
        return false;

    if (m_isStrongCombo)
    {
        m_isStrongCombo = false;
        m_iCurrentCombo = 0;
    }

    _uint iAnimIndex{};
    if (m_iCurrentCombo == 0)iAnimIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Com_FastAtk01");
    if (m_iCurrentCombo == 1)iAnimIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Com_FastAtk02");
    if (m_iCurrentCombo == 2)
        iAnimIndex = (m_pClientInstance->Check_SpearSkill(SPEARSKILL::MOONLIGHT_SLASH)) 
       ? m_pModel->Get_AnimIndexByName("CA_P_Kazan_LightningSpear_Advanced")
       : m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Com_FastAtk03_02");

    // 최소 애니메이션 시간 체크
    if (!m_pModel->Check_MinAnimationTime() && m_iCurrentCombo >= 0 && m_iSelectedAnimationIndex == iAnimIndex)
        return false;



    m_isAttacking = true;
    m_isFastCombo = true;
    m_isCanNextCombo = false;
    m_iReserveSkillIndex = 0;

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
        if (m_pClientInstance->Check_SpearSkill(SPEARSKILL::MOONLIGHT_SLASH))  m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_LightningSpear_Advanced");
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

_bool CKhazan_Spear_Anim_Attack::Try_SkillAttack(_uint iSkill)
{
    if (!m_pModel->Check_MinAnimationTime())
        return false;

    m_isAttacking = true;
    m_isSkilling = true;

    m_isCanNextCombo = false;
    m_iCurrentCombo = 0;
    m_isFastCombo = false;
    m_isStrongCombo = false;
    m_isStrongCharge = false;
    m_isReserve = false;
    m_iReserveSkillIndex = 0;

    if (m_pClientInstance->Check_SpearSkill(SPEARSKILL::FULL_MOON)&& iSkill & SPEARSKILL::FULL_MOON)  m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_SpaceTimeCutter03");
    else  if (m_pClientInstance->Check_SpearSkill(SPEARSKILL::SPIRAL_THRUST) && iSkill & SPEARSKILL::SPIRAL_THRUST) 
        m_iSelectedAnimationIndex = m_pClientInstance->Check_SpearSkill(SPEARSKILL::SPIRAL_THRUST_WHIRLWIND)
        ? m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Tempest_TwisterSpear")
        :  m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Tempest_SpiralSpear");
    else  if (m_pClientInstance->Check_SpearSkill(SPEARSKILL::ASSAULT) && iSkill & SPEARSKILL::ASSAULT)  m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_PureMind_SeismicKick");
    else  if (m_pClientInstance->Check_SpearSkill(SPEARSKILL::MOMENT_SLASH) && iSkill & SPEARSKILL::MOMENT_SLASH)  m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName(" CA_P_Kazan_Spear_LowFlying_F");
    else  if (m_pClientInstance->Check_SpearSkill(SPEARSKILL::CRITICAL_STRIKE) && iSkill & SPEARSKILL::CRITICAL_STRIKE)  m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Crescent");
    else  if (m_pClientInstance->Check_SpearSkill(SPEARSKILL::SHADOW_CLEAVE) && iSkill & SPEARSKILL::SHADOW_CLEAVE)  m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Tempest_MoonVeil");
    else
    {
        m_isAttacking = false;
        m_isSkilling = false;
        return false;
    }

    m_iCurSkillIndex = iSkill;
    m_pClientInstance->Set_UsedSkill(iSkill,true);
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
    m_iReserveSkillIndex = 0;

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
    m_iReserveSkillIndex = 0;
    m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_SprintAtk_Fast");
    m_pModel->Set_Animation(m_iSelectedAnimationIndex);

    return true;
}

_bool CKhazan_Spear_Anim_Attack::Try_SprintStrongAttack()
{
    if (m_isAttacking)
        return false;
    m_isAttacking = true;
    m_iReserveSkillIndex = 0;

    m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_SprintAtk_Strong");
    m_pModel->Set_Animation(m_iSelectedAnimationIndex);


    return true;
}

_bool CKhazan_Spear_Anim_Attack::Try_StrongAttack()
{
    // 첫 공격이거나, 콤보 가능 상태일 때만 실행
    if (m_isAttacking && !m_isCanNextCombo)
    {
        return false;
    }

    if (m_isFastCombo)
    {
        m_isFastCombo = false;
        m_iCurrentCombo = 0;
    }

    _uint iAnimIndex{};
    if (m_iCurrentCombo == 0)iAnimIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Com_StrongAtk01");
    if (m_iCurrentCombo == 1)iAnimIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Com_StrongAtk02");
    if (m_iCurrentCombo == 2)iAnimIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Com_StrongAtk03");

    // 최소 애니메이션 시간 체크 및 최소시간전에 최소시간 전에 같은 애니메이션 들어오면 
    if (!m_pModel->Check_MinAnimationTime() && m_iCurrentCombo >= 0 && m_iSelectedAnimationIndex == iAnimIndex)
    {
        return false;
    }


  


    m_isAttacking = true;
    m_isStrongCombo = true;
    m_isCanNextCombo = false;
    m_iReserveSkillIndex = 0;

    cout << m_iCurrentCombo << endl;

    if (m_iCurrentCombo == 0)
    {
        cout << "m_iCurrentCombo = 0" << endl;
        m_iSelectedAnimationIndex = iAnimIndex;
        m_pModel->Set_Animation(m_iSelectedAnimationIndex);
        m_iCurrentCombo = 1;
    }
    else if (m_iCurrentCombo == 1)
    {
        cout << "m_iCurrentCombo =1" << endl;
        m_iSelectedAnimationIndex = iAnimIndex;
        m_pModel->Set_Animation(m_iSelectedAnimationIndex);
        m_iCurrentCombo = 2;
    }
    else if (m_iCurrentCombo == 2)
    {
        cout << "m_iCurrentCombo = 2" << endl;
        m_iSelectedAnimationIndex = iAnimIndex;
        m_pModel->Set_Animation(m_iSelectedAnimationIndex);
        m_iCurrentCombo = 3; // 마지막 콤보
    }

    return true;

}

_bool CKhazan_Spear_Anim_Attack::Try_ChageStrongAttack()
{
    // 이미 차징 공격 중이면 중복 실행 방지
    if (m_isStrongCharge)
    {
        cout << "Already charging" << endl;
        return false;
    }

    // 다른 공격 중이면 실행 불가
    if (m_isAttacking && !m_isCanNextCombo)
    {
        cout << "Cannot charge - attacking" << endl;
        return false;
    }

    m_isAttacking = true;
    m_isStrongCharge = true;
    m_isFastCombo = false;
    m_isStrongCombo = false;
    m_iCurrentCombo = 0;
    m_isReserve = false;
    m_iReserveSkillIndex = 0;

    m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Com_StrongAtk_Charge");
    m_pModel->Set_Animation(m_iSelectedAnimationIndex);

    cout << "Charge attack started" << endl;
    return true;
}

//_bool CKhazan_Spear_Anim_Attack::Try_ChageStrongAttack()
//{
//    if (m_isStrongCharge)
//    {
//        return false;
//    }
//
//    m_isAttacking = true;
//    m_isStrongCharge = true;
//    m_iReserveSkillIndex = 0;
//
//    m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Com_StrongAtk_Charge");
//    m_pModel->Set_Animation(m_iSelectedAnimationIndex);
//
//    return true;
//}

void CKhazan_Spear_Anim_Attack::Reserve_SkillAttack(_uint iSkill)
{
    m_isReserve = true;
    m_iReserveSkillIndex = iSkill;
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
