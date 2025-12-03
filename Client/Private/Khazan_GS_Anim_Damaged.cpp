#include "Khazan_GS_Anim_Damaged.h"
#include "GameInstance.h"

CKhazan_GS_Anim_Damaged::CKhazan_GS_Anim_Damaged()
{
}

HRESULT CKhazan_GS_Anim_Damaged::Initialize_Prototype()
{
    return S_OK;
}

void CKhazan_GS_Anim_Damaged::Enter()
{
    cout << " Enter" << endl;
}

void CKhazan_GS_Anim_Damaged::Continue(_float fTimeDelta)
{

    //현재 재생중인 애니메이션이 대미지 애니메이션이 아니면 종료
    if (m_iSelectedAnimationIndex != m_pModel->Get_CurAnimIndex())
    {
        m_isDamaged = false;
        return;
    }
    //애니메이션이 끝났으면 종료
    if (/*m_pModel->Check_MinAnimationTime() ||*/ m_pModel->IsFinished())
    {
        m_isDamaged = false;
        return;
    }

}

void CKhazan_GS_Anim_Damaged::Exit()
{
    m_isDamaged = false;
}

void CKhazan_GS_Anim_Damaged::Force_DamagedNormal(_bool isWeapon, _uint iDir)
{
    if (m_isDamaged) return;

    m_eDir.iDirFlag = iDir; 
    m_isDamaged = true;

    auto GetAnimIndexByState = [&](const std::string& spear, const std::string& bare)
        {
            return m_pModel->Get_AnimIndexByName(isWeapon ? spear.c_str() : bare.c_str());
        };


    if (m_eDir.AllCheck_Flag(m_eDir.D | m_eDir.B)) {
        m_iSelectedAnimationIndex = GetAnimIndexByState("CA_P_Kazan_GSword_DamNor_D_B", "CA_P_Kazan_BareHands_Com_DamageNormal_D_B");
        cout << "D B" << endl;
    }
    else if (m_eDir.AllCheck_Flag(m_eDir.D | m_eDir.F)) {
        m_iSelectedAnimationIndex = GetAnimIndexByState("CA_P_Kazan_GSword_DamNor_D_F", "CA_P_Kazan_BareHands_Com_DamageNormal_D_F");
        cout << "D f" << endl;
    }
    else if (m_eDir.AllCheck_Flag(m_eDir.L | m_eDir.F)) {
        m_iSelectedAnimationIndex = GetAnimIndexByState("CA_P_Kazan_GSword_DamNor_L_F", "CA_P_Kazan_BareHands_Com_DamageNormal_L_F");
        cout << "l f" << endl;
    }
    else if (m_eDir.AllCheck_Flag(m_eDir.R | m_eDir.F)) {
        m_iSelectedAnimationIndex = GetAnimIndexByState("CA_P_Kazan_GSword_DamNor_R_F", "CA_P_Kazan_BareHands_Com_DamageNormal_R_F");
        cout << "r f" << endl;
    }
    else if (m_eDir.AllCheck_Flag(m_eDir.U | m_eDir.B)) {
        m_iSelectedAnimationIndex = GetAnimIndexByState("CA_P_Kazan_GSword_DamNor_U_B", "CA_P_Kazan_BareHands_Com_DamageNormal_U_B");
        cout << "u b" << endl;
    }
    else {//if (m_eDir.AllCheck_Flag(m_eDir.U | m_eDir.F))
        m_iSelectedAnimationIndex = GetAnimIndexByState("CA_P_Kazan_GSword_DamNor_U_F", "CA_P_Kazan_BareHands_Com_DamageNormal_U_F");
        cout << "u f" << endl;
    }
    //else 
    //    return;

    m_isDamaged = true;
    m_pModel->Set_Animation(m_iSelectedAnimationIndex);
}

void CKhazan_GS_Anim_Damaged::Force_DamagedStrong(_bool isWeapon, _uint iDir)
{
    if (m_isDamaged) return;

    m_eDir.iDirFlag = iDir;

 /* 이애니메이션은 맨손이 없음 */
    if (m_eDir.AllCheck_Flag(m_eDir.D | m_eDir.F))
        m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_DamStr_D_F");
    else if (m_eDir.AllCheck_Flag(m_eDir.R | m_eDir.F))
        m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_DamStr_R_F");
    else if (m_eDir.AllCheck_Flag(m_eDir.U | m_eDir.F))
        m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_DamStr_U_F");
    else
        m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_DamStr_L_F");

    m_isDamaged = true;
    m_pModel->Set_Animation(m_iSelectedAnimationIndex);
}

CKhazan_GS_Anim_Damaged* CKhazan_GS_Anim_Damaged::Create()
{

    return new CKhazan_GS_Anim_Damaged;
}

void CKhazan_GS_Anim_Damaged::Free()
{
    __super::Free();

}
