#include "Khazan_Spear_Anim_Fall.h"
#include "GameInstance.h"
#include "ClientInstance.h"

CKhazan_Spear_Anim_Fall::CKhazan_Spear_Anim_Fall()
    : m_pClientInstance{ CClientInstance::GetInstance() }
{
    Safe_AddRef(m_pClientInstance);
}

HRESULT CKhazan_Spear_Anim_Fall::Initialize()
{
    m_pPlayerData = m_pClientInstance->Get_pInitailizePlayerData();
    return S_OK;
}

void CKhazan_Spear_Anim_Fall::Enter()
{
}

void CKhazan_Spear_Anim_Fall::Continue(_float fTimeDelta)
{
    /*  낙하 시작 -> 낙하 중 */
    if (m_isFallingStart && m_pModel->IsFinished())
    {
        m_isFallingStart = false;
        m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Fall_Loop");
        m_pModel->Set_Animation(m_iSelectedAnimationIndex);
    }

    /*  낙하 공격 시작 -> 낙하 공격 중 */
    if (m_isFallAttackingStart && m_pModel->IsFinished())
    {
        m_isFallAttackingStart = false;
        m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Com_FallAtk_Loop");
        m_pModel->Set_Animation(m_iSelectedAnimationIndex);
    }


}

void CKhazan_Spear_Anim_Fall::Exit()
{
    m_isFallingStart = { false };
    m_isFalling = { false };
    m_isFallAttacking = { false };
    m_isFallAttackingStart = { false };
}

_bool CKhazan_Spear_Anim_Fall::Force_StartFall()
{
    if (m_isFallingStart || m_isFalling)
        return false;

    m_isFallingStart = true;
    m_isFalling = true;
    m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Fall_Start");
    m_pModel->Set_Animation(m_iSelectedAnimationIndex);

    return true;
}

_bool CKhazan_Spear_Anim_Fall::Force_Landing()
{
    if (!m_isFalling)
        return false;

    m_isFallingStart = false;
    m_isFalling = false;

    m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Fall_End");
    m_pModel->Set_Animation(m_iSelectedAnimationIndex);

    return true;
}

_bool CKhazan_Spear_Anim_Fall::Force_StartFallAttack()
{
    if (!m_isFalling) return false;
    if (m_isFallingStart) m_isFallingStart = false;

    m_isFallAttackingStart = true;
    m_isFallAttacking = true;

    m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Com_FallAtk_Start");
    m_pModel->Set_Animation(m_iSelectedAnimationIndex);

    return true;
}

_bool CKhazan_Spear_Anim_Fall::Force_AttackLanding()
{
    if (!m_isFalling) return false;

    m_isFallingStart = false;
    m_isFalling = false;
    m_isFallAttackingStart = false;
    m_isFallAttacking = false;

    m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Com_FallAtk_End");
    m_pModel->Set_Animation(m_iSelectedAnimationIndex);

    m_pPlayerData->fBonusDamage = m_pPlayerData->fDamage * 20.f;

    return true;
}

void CKhazan_Spear_Anim_Fall::Clear()
{
    m_isFallingStart = { false };
    m_isFalling = { false };
    m_isFallAttacking = { false };
    m_isFallAttackingStart = { false };
}


CKhazan_Spear_Anim_Fall* CKhazan_Spear_Anim_Fall::Create()
{
    CKhazan_Spear_Anim_Fall* pInstance = new CKhazan_Spear_Anim_Fall();

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX(TEXT("Failed to Created : CKhazan_Spear_Anim_Fall"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CKhazan_Spear_Anim_Fall::Free()
{
    __super::Free();
    Safe_Release(m_pClientInstance);

}
