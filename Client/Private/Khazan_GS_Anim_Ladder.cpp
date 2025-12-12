#include "Khazan_GS_Anim_Ladder.h"
#include "GameInstance.h"

CKhazan_GS_Anim_Ladder::CKhazan_GS_Anim_Ladder()
{
}

void CKhazan_GS_Anim_Ladder::Initialize()
{
    m_iLadderStartEndAnimIndex[0] = m_pModel->Get_AnimIndexByName("CA_P_Kazan_ClimbUp_U_Strart");
    m_iLadderStartEndAnimIndex[1] = m_pModel->Get_AnimIndexByName("CA_P_Kazan_ClimbDn_D_Start_End");
    m_iLadderStartEndAnimIndex[2] = m_pModel->Get_AnimIndexByName("CA_P_Kazan_ClimbDn_D_Start_End2");
    m_iLadderStartEndAnimIndex[3] = m_pModel->Get_AnimIndexByName("CA_P_Kazan_ClimbDn_D_Start_End3");
    m_iLadderStartEndAnimIndex[4] = m_pModel->Get_AnimIndexByName("CA_P_Kazan_ClimbDn_D_Start_End4");

}

void CKhazan_GS_Anim_Ladder::Enter()
{
}

void CKhazan_GS_Anim_Ladder::Continue(_float fTimeDelta)
{
}

void CKhazan_GS_Anim_Ladder::Exit()
{
}

_bool CKhazan_GS_Anim_Ladder::Try_Start_Up_Ladder()
{
    if (!m_pModel->Check_MinAnimationTime())
        return false;


    m_pModel->Set_AnimationSet("Set_ClimbUp_Start");
    m_pModel->AnimationSetIndexIncrease();

    m_isCurFootLeft = false;
    m_isStartClimb = true;
    m_isClimbing = false;

    return true;
}

_bool CKhazan_GS_Anim_Ladder::Try_Start_Down_Ladder(_uint iType)
{
    //if (!m_pModel->Check_MinAnimationTime())
      //  return false;


    if (iType == 0) m_pModel->Set_AnimationSet("Set_ClimbDown_Start1");
    else if (iType == 1) m_pModel->Set_AnimationSet("Set_ClimbDown_Start2");
    else if (iType == 2) m_pModel->Set_AnimationSet("Set_ClimbDown_Start3");
    else if (iType == 3) m_pModel->Set_AnimationSet("Set_ClimbDown_Start4");
    else if (iType == 4) m_pModel->Set_AnimationSet("Set_ClimbDown_Start5");
    m_pModel->AnimationSetIndexIncrease();

    m_isCurFootLeft = false;
    m_isStartClimb = true;
    m_isClimbing = false;
    return true;
}

_bool CKhazan_GS_Anim_Ladder::Force_End_Up_Ladder()
{
    if (m_isCurFootLeft)
        m_pModel->Set_AnimationSet("Set_ClimbUp_RU_End");
    else
        m_pModel->Set_AnimationSet("Set_ClimbUp_LU_End");

    m_pModel->Set_AnimationBlend(true);

    return true;
}

_bool CKhazan_GS_Anim_Ladder::Force_End_Down_Ladder()
{
   // m_pModel->Set_AnimationSet("Set_ClimbDown_End");
    m_pModel->Set_Animation(m_pModel->Get_AnimIndexByName("CA_P_Kazan_ClimbDn_D_End_Start"));
    m_pModel->Set_AnimationBlend(true);

    return true;
}

_bool CKhazan_GS_Anim_Ladder::Try_PlayLadder(GS_LADDERINFO info)
{  

    /* Jolt 캐릭터버츄얼떄문에 흔들리는것처럼 보임 사용 x - 기본 움직임 빠르게하기로 적용 */
    /*스프린트 루프 유지 */
    //if (m_isSprint && info.isSprint) {
    //    return true;
    //}

    ///*  스프린트 끝 */
    //if (m_isSprint && !info.isSprint) {
    //    m_pModel->AnimationSetIndexIncrease();
    //    m_isSprint = false;
    //    return true;
    //}

    ///* 다운 스프린트 시작 */
    //if (!info.eDir.Check_Flag(DIRECTION_INFO::DIR::U) && !m_isSprint && info.isSprint)
    //{
    //    m_pModel->Set_AnimationSet("Set_LadderDownSprint");
    //    m_isSprint = true;
    //    m_isStanding = false;
    //   // m_isClimbing = true;
    //    return true;
    //}


    /* 한 동작이 다 끝났는지 */
    if (!m_pModel->Check_MinAnimationTime())
        return false;

    m_pModel->Set_AnimationBlend(true);

    /* 사다리 타기 구분 동작이 다 끝났는지  */
    if (m_isStartClimb)
    {
        _uint iCurAnimIndex = m_pModel->Get_CurAnimIndex();
        for (size_t i = 0; i < 5; i++)
        {
            if (iCurAnimIndex == m_iLadderStartEndAnimIndex[i])
            {
                if (m_pModel->IsFinished())
                {
                    m_isStartClimb = false;
                    m_isClimbing = true;

                    break;
                }
                else
                    return true;
            }
        }
        return false;
    }

    else if (!m_isStanding && info.eDir.iDirFlag == 0)
    {
        if (!m_isCurFootLeft)  m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_ClimbStand_L_Loop");
        else  m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_ClimbStand_R_Loop");
        m_pModel->Set_AnimationBlend(false);

        m_isStanding = true; 
    }

    else if (info.eDir.Check_Flag(DIRECTION_INFO::DIR::U))
    {
        if (m_isCurFootLeft)  m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_ClimbUp_UR_Loop");
        else  m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_ClimbUp_UL_Loop");

        m_isCurFootLeft = !m_isCurFootLeft;
        m_isStanding = false;
    }

    else if (info.eDir.Check_Flag(DIRECTION_INFO::DIR::D))
    {
        if (m_isCurFootLeft)  m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_ClimbDn_DR_Loop");
        else  m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_ClimbDn_DL_Loop");
        m_isCurFootLeft = !m_isCurFootLeft;
        m_isStanding = false;

    }

    m_pModel->Set_Animation(m_iSelectedAnimationIndex);

    return true;
}

CKhazan_GS_Anim_Ladder* CKhazan_GS_Anim_Ladder::Create()
{
    return new CKhazan_GS_Anim_Ladder();
}

void CKhazan_GS_Anim_Ladder::Free()
{
    __super::Free();
}
