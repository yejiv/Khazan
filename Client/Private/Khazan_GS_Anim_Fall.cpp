#include "Khazan_GS_Anim_Fall.h"
#include "GameInstance.h"

CKhazan_GS_Anim_Fall::CKhazan_GS_Anim_Fall()
{
}

void CKhazan_GS_Anim_Fall::Enter()
{
}

void CKhazan_GS_Anim_Fall::Continue(_float fTimeDelta)
{
    /*  낙하 시작 -> 낙하 중 */
    if (m_isFallingStart && m_pModel->IsFinished())
    {
        m_isFallingStart = false;
        m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_PC_Kazan_Fall_Loop");
        m_pModel->Set_Animation(m_iSelectedAnimationIndex);
    }

    /*  낙하 공격 시작 -> 낙하 공격 중 */
    if (m_isFallAttackingStart && m_pModel->IsFinished())
    {
        m_isFallAttackingStart = false;
        m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_Com_FallAtk_Loop");
        m_pModel->Set_Animation(m_iSelectedAnimationIndex);
    }


}

void CKhazan_GS_Anim_Fall::Exit()
{
    m_isFallingStart = { false };
    m_isFalling = { false };
    m_isFallAttacking = { false };
    m_isFallAttackingStart = { false };
}

_bool CKhazan_GS_Anim_Fall::Force_StartFall()
{
    if (m_isFallingStart || m_isFalling)
        return false;

    m_isFallingStart = true;
    m_isFalling = true;
    m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_PC_Kazan_Fall_Start");
    m_pModel->Set_Animation(m_iSelectedAnimationIndex);

    return true;
}

_bool CKhazan_GS_Anim_Fall::Force_Landing()
{
    if (!m_isFalling)
        return false;

    m_isFallingStart = false;
    m_isFalling = false;

    m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_PC_Kazan_Fall_End");
    m_pModel->Set_Animation(m_iSelectedAnimationIndex);

    return true;
}

_bool CKhazan_GS_Anim_Fall::Force_StartFallAttack()
{
    if (!m_isFalling) return false;
    if (m_isFallingStart) m_isFallingStart = false;

    m_isFallAttackingStart = true;
    m_isFallAttacking = true;

    m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_Com_FallAtk_Start");
    m_pModel->Set_Animation(m_iSelectedAnimationIndex);

    return true;
}

_bool CKhazan_GS_Anim_Fall::Force_AttackLanding()
{
    if (!m_isFalling) return false;

    m_isFallingStart = false;
    m_isFalling = false;
    m_isFallAttackingStart = false;
    m_isFallAttacking = false;

    m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_Com_FallAtk_End");
    m_pModel->Set_Animation(m_iSelectedAnimationIndex);

    return true;
}

void CKhazan_GS_Anim_Fall::Clear()
{
    m_isFallingStart = { false };
    m_isFalling = { false };
    m_isFallAttacking = { false };
    m_isFallAttackingStart = { false };
}


CKhazan_GS_Anim_Fall* CKhazan_GS_Anim_Fall::Create()
{
    return new CKhazan_GS_Anim_Fall;
}

void CKhazan_GS_Anim_Fall::Free()
{
    __super::Free();

}
