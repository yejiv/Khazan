#include "Khazan_GS_Anim_Move.h"
#include "GameInstance.h"


using DIR = DIRECTION_INFO::DIR;

const string CKhazan_GS_Anim_Move::s_strSpearSprinStartAnims[] =
{
        "CA_P_Kazan_Spear_Sprint_F_Start",
        "CA_P_Kazan_Spear_Sprint_FL_Start",
        "CA_P_Kazan_Spear_Sprint_FR_Start",
        "CA_P_Kazan_Spear_Sprint_L_Start",
        "CA_P_Kazan_Spear_Sprint_R_Start",
        "CA_P_Kazan_Spear_Sprint_B_Start",
        "CA_P_Kazan_Spear_Sprint_BL_Start",
        "CA_P_Kazan_Spear_Sprint_BR_Start"
};
const string CKhazan_GS_Anim_Move::s_strGSSprinStartAnims[] =
{
        "CA_P_Kazan_GSword_Sprint_F_Start",
        "CA_P_Kazan_GSword_Sprint_FL_Start",
        "CA_P_Kazan_GSword_Sprint_FR_Start",
        "CA_P_Kazan_GSword_Sprint_L_Start",
        "CA_P_Kazan_GSword_Sprint_R_Start",
        "CA_P_Kazan_GSword_Sprint_B_Start",
        "CA_P_Kazan_GSword_Sprint_BL_Start",
        "CA_P_Kazan_GSword_Sprint_BR_Start"
};
const string CKhazan_GS_Anim_Move::s_strSpearDodgeAnims[] =
{
        "CA_P_Kazan_Spear_Com_Dodge_F",
        "CA_P_Kazan_Spear_Com_Dodge_B",
        "CA_P_Kazan_Spear_Com_Dodge_L",
        "CA_P_Kazan_Spear_Com_Dodge_R",
        "CA_P_Kazan_Spear_Com_Dodge_FL",
        "CA_P_Kazan_Spear_Com_Dodge_FR",
        "CA_P_Kazan_Spear_Com_Dodge_BL",
        "CA_P_Kazan_Spear_Com_Dodge_BR"
};
const string CKhazan_GS_Anim_Move::s_strGSDodgeAnims[] =
{
        "CA_P_Kazan_GSword_Dodge_F",
        "CA_P_Kazan_GSword_Dodge_B",
        "CA_P_Kazan_GSword_Dodge_L",
        "CA_P_Kazan_GSword_Dodge_R",
        "CA_P_Kazan_GSword_Dodge_FL",
        "CA_P_Kazan_GSword_Dodge_FR",
        "CA_P_Kazan_GSword_Dodge_BL",
        "CA_P_Kazan_GSword_Dodge_BR"
};

CKhazan_GS_Anim_Move::CKhazan_GS_Anim_Move()
{
}

void CKhazan_GS_Anim_Move::Initialize()
{
    /* walk */
    m_FootInfo[0] = {
    {7.7f},
    {22.f},
     30.f };

    /*run */
    m_FootInfo[1] = {
        { 7.f, 26.f, 46.f,  66.f, 86.f, 106.f},
        {16.f, 36.f, 56.f, 76.f, 96.f, 116.f},
         120.f };

    m_iStopAnimIndex[0] = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Sprint_Stop_F");
    m_iStopAnimIndex[1] = m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_LockOn_Sprint_Stop_F");
    m_iStopAnimIndex[2] = m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_Sprint_Stop_F");
    m_iStopAnimIndex[3] = m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_LockOn_Sprint_Stop_F");

}

void CKhazan_GS_Anim_Move::Enter()
{
    m_iFootPosition[0] = m_iFootPosition[1] = 0;
}

void CKhazan_GS_Anim_Move::Continue(_float fTimeDelta)
{
    /* 180 turn 처리 */
    if (m_isTurning180)
    {
        if (m_pModel->Check_MinAnimationTime())
        {
            m_isTurning180 = false;
            m_isMoving = false;
            return;
        }
    }
   
    if (m_isMoving)
    { 
        /* Fall 처리 */
        if(m_pModel->Get_CurAnimIndex() == m_pModel->Get_AnimIndexByName("CA_PC_Kazan_Fall_End"))  m_pModel->Set_Animation(m_iSelectedAnimationIndex);
        /* Armed 처리 */
        else if (m_pModel->Get_CurAnimIndex() == m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_Armed") && m_pModel->IsFinished() )  m_pModel->Set_Animation(m_iSelectedAnimationIndex);
        else if (m_pModel->Get_CurAnimIndex() == m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Armed") && m_pModel->IsFinished())  m_pModel->Set_Animation(m_iSelectedAnimationIndex);
        /* UnArmed 처리 */
        else if (m_pModel->Get_CurAnimIndex() == m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_UnArmed") && m_pModel->IsFinished())  m_pModel->Set_Animation(m_iSelectedAnimationIndex);
        else if (m_pModel->Get_CurAnimIndex() == m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_UnArmed") && m_pModel->IsFinished())  m_pModel->Set_Animation(m_iSelectedAnimationIndex);
    }


    Check_SprintStart();

    if (Check_Dodge())
        return;

    Update_FootPosition();

    Check_Reserve();
}

void CKhazan_GS_Anim_Move::Exit()
{
    m_isMoving = false;
    m_isReserve = false;
    m_isTurning180 = false;
    m_isDodging = false;
    //Clear();
}

_bool CKhazan_GS_Anim_Move::Try_ChangeAnimation(GS_MOVEINFO moveInfo)
{
    // 180도 회전 중에는 애니메이션 변경 차단
     if (m_isTurning180)
    {
        return false;
    }

    _uint currentAnimIndex = m_pModel->Get_CurAnimIndex();
    if (currentAnimIndex == m_iStopAnimIndex[0] || currentAnimIndex == m_iStopAnimIndex[1] || currentAnimIndex == m_iStopAnimIndex[2] || currentAnimIndex == m_iStopAnimIndex[3])
    {
        if (!m_pModel->Check_MinAnimationTime())
            return false;
    }

    _uint iSelectedAnimationIndex{};
    m_isDodging = false;
    Clear_State();
    Add_State(moveInfo.iState);
    m_isStopAnimationFinished = false;
    m_iCurWeapon = moveInfo.iWeapon;
    _bool   isLockOn = moveInfo.isLockOn;

    if (Has_State(MOV::MOVE_WALK))
    {
        if (isLockOn)
        {
            iSelectedAnimationIndex = GetLockOnWalkAnimation(moveInfo.eDir);
        }
        else if (moveInfo.iCycle & CYC::CYCLE_END)
        {
            if (m_curFoot == FOOT_R) iSelectedAnimationIndex = Get_AnimIndexByName("CA_P_Kazan_Gsword_Walk_Stop_F_RF", "CA_P_Kazan_Spear_Walk_Stop_F_RF", "CA_P_Kazan_Gsword_Walk_Stop_F_RF");
            else  iSelectedAnimationIndex = Get_AnimIndexByName("CA_P_Kazan_Gsword_Walk_Stop_F_LF", "CA_P_Kazan_Spear_Walk_Stop_F_LF", "CA_P_Kazan_Gsword_Walk_Stop_F_LF");

            m_isStopAnimationFinished = true;
            m_isMoving = false;
        }
        else
            iSelectedAnimationIndex = Get_AnimIndexByName("CA_P_Kazan_GSword_Walk_F", "CA_P_Kazan_Spear_Walk_F", "CA_P_Kazan_GSword_Walk_F");


    }

    if (Has_State(MOV::MOVE_RUN))
    {

        if (isLockOn)
        {
            iSelectedAnimationIndex = GetLockOnRunAnimation(moveInfo.eDir);
        }
        else if (moveInfo.iCycle & CYC::CYCLE_END)
        {
            if (m_curFoot == FOOT_R) iSelectedAnimationIndex = Get_AnimIndexByName("CA_P_Kazan_Spear_Run_Stop_F_RF", "CA_P_Kazan_Spear_Run_Stop_F_RF", "CA_P_Kazan_GSword_Run_Stop_F_RF");
            else  iSelectedAnimationIndex = Get_AnimIndexByName("CA_P_Kazan_Spear_Run_Stop_F_LF", "CA_P_Kazan_Spear_Run_Stop_F_LF", "CA_P_Kazan_GSword_Run_Stop_F_LF");

            m_isStopAnimationFinished = true;
            m_isMoving = false;
        }
        else {
             iSelectedAnimationIndex = Get_AnimIndexByName("CA_P_Kazan_Spear_Run_F", "CA_P_Kazan_Spear_Run_F", "CA_P_Kazan_GSword_Run_F");
        }

    }

    if (Has_State(MOV::MOVE_SPRINT))
    {
        if (moveInfo.iCycle & CYC::CYCLE_START)
        {
            if (moveInfo.eDir.AllCheck_Flag(DIR::B | DIR::L)) iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_Sprint_BL_Start");
            else if (moveInfo.eDir.AllCheck_Flag(DIR::B | DIR::R)) iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_Sprint_BR_Start");
            else if (moveInfo.eDir.AllCheck_Flag(DIR::B)) iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_Sprint_B_Start");
            else if (moveInfo.eDir.AllCheck_Flag(DIR::F | DIR::L)) iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_Sprint_FR_Start");
            else if (moveInfo.eDir.AllCheck_Flag(DIR::F | DIR::R)) iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_Sprint_FR_Start");
            else if (moveInfo.eDir.AllCheck_Flag(DIR::F)) iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_Sprint_F_Start");
            else if (moveInfo.eDir.AllCheck_Flag(DIR::L)) iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_Sprint_L_Start");
            else if (moveInfo.eDir.AllCheck_Flag(DIR::R)) iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_Sprint_R_Start");
        }
        else  if (moveInfo.iCycle & CYC::CYCLE_END)
        {
            if (isLockOn)
                iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_LockOn_Sprint_Stop_F");
            else
                iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_Sprint_Stop_F");
            m_isStopAnimationFinished = true;
            m_isMoving = false;
        }
        else {
            iSelectedAnimationIndex = Get_AnimIndexByName("CA_P_Kazan_GSword_Sprint_F","CA_P_Kazan_Spear_Sprint_F", "CA_P_Kazan_GSword_Sprint_F");

        }
    }

    if (Has_State(MOV::MOVE_DODGE))
    {
        if (moveInfo.eDir.AllCheck_Flag(DIR::B | DIR::L)) iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_Dodge_BL");
        else if (moveInfo.eDir.AllCheck_Flag(DIR::B | DIR::R)) iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSwordDodge_BR");
        //if (moveInfo.eDir.AllCheck_Flag(DIR::B | DIR::L)) iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_Dodge_B");
        //else if (moveInfo.eDir.AllCheck_Flag(DIR::B | DIR::R)) iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_Dodge_B");
        else if (moveInfo.eDir.AllCheck_Flag(DIR::B)) iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_Dodge_B");
        else if (moveInfo.eDir.AllCheck_Flag(DIR::F | DIR::L)) iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_Dodge_FL");
        else if (moveInfo.eDir.AllCheck_Flag(DIR::F | DIR::R)) iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_Dodge_FR");
        else if (moveInfo.eDir.AllCheck_Flag(DIR::F)) iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_Dodge_F");
        else if (moveInfo.eDir.AllCheck_Flag(DIR::L)) iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_Dodge_L");
        else if (moveInfo.eDir.AllCheck_Flag(DIR::R)) iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_Dodge_R");

        m_isDodging = true;
        m_isReserve = { false };
        m_isStopAnimationFinished = true;
    }



    _bool isCheckMin = Has_State(MOV::MOVE_DODGE) ? m_pModel->Check_CanDodgeTime() : m_pModel->Check_MinAnimationTime();
    if (m_iPrevSelectedAnimationIndex == iSelectedAnimationIndex && !isCheckMin)
        return false;
    else if (isCheckMin || Has_State(MOV::MOVE_DODGE))
    {
        if (m_isStopAnimationFinished) m_isMoving = false;
        else m_isMoving = true;
        m_iPrevSelectedAnimationIndex = m_iSelectedAnimationIndex;
        m_iSelectedAnimationIndex = iSelectedAnimationIndex;
        if (m_iSelectedAnimationIndex == 0)
            return false;

        m_pModel->Set_Animation(m_iSelectedAnimationIndex);
        
        return true;
    }
    else {
        Reserve_Animation(moveInfo);
        return false;
    }
}


_bool CKhazan_GS_Anim_Move::Force_DodgeAnimation(GS_MOVEINFO moveInfo)
{

    Clear_State();
    Add_State(moveInfo.iState);
    m_isStopAnimationFinished = false;
    m_isReserve = { false };
    m_isTurning180 = { false };

    _uint iSelectedAnimationIndex{};

    if (moveInfo.eDir.AllCheck_Flag(DIR::B | DIR::L)) iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_Dodge_BL");
    else if (moveInfo.eDir.AllCheck_Flag(DIR::B | DIR::R)) iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSwordDodge_BR");
    //if (moveInfo.eDir.AllCheck_Flag(DIR::B | DIR::L)) iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_Dodge_B");
    //else if (moveInfo.eDir.AllCheck_Flag(DIR::B | DIR::R)) iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_Dodge_B");
    else if (moveInfo.eDir.AllCheck_Flag(DIR::B)) iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_Dodge_B");
    else if (moveInfo.eDir.AllCheck_Flag(DIR::F | DIR::L)) iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_Dodge_FL");
    else if (moveInfo.eDir.AllCheck_Flag(DIR::F | DIR::R)) iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_Dodge_FR");
    else if (moveInfo.eDir.AllCheck_Flag(DIR::F)) iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_Dodge_F");
    else if (moveInfo.eDir.AllCheck_Flag(DIR::L)) iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_Dodge_L");
    else if (moveInfo.eDir.AllCheck_Flag(DIR::R)) iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_Dodge_R");


    m_isDodging = true;
    m_isStopAnimationFinished = true;
    m_isMoving = true;
    m_iPrevSelectedAnimationIndex = m_iSelectedAnimationIndex;
    m_iSelectedAnimationIndex = iSelectedAnimationIndex;

    if (m_iSelectedAnimationIndex == 0)
        return false;

    m_pModel->Set_Animation(m_iSelectedAnimationIndex);

    return S_OK;
}

void CKhazan_GS_Anim_Move::Reserve_Animation(GS_MOVEINFO moveInfo)
{
    m_isReserve = true;
    m_eReserveInfo = moveInfo;
}

_bool CKhazan_GS_Anim_Move::Try_Turn180Animation(GS_MOVEINFO moveInfo)
{
    return true;
}

void CKhazan_GS_Anim_Move::Update_FootPosition()
{
    if(!Has_State(MOV::MOVE_WALK | MOV::MOVE_RUN))
        return ;

    if (m_iFootPosition[0] >= m_FootInfo[GetBitPosition(m_iState)].vLeftFootFrames.size() ||
        m_iFootPosition[1] >= m_FootInfo[GetBitPosition(m_iState)].vRightFootFrames.size())
        m_iFootPosition[0] = m_iFootPosition[1] = 0;

    _float fLeftTime  = m_FootInfo[GetBitPosition(m_iState)].vLeftFootFrames[m_iFootPosition[0]];
    _float fRightTime = m_FootInfo[GetBitPosition(m_iState)].vRightFootFrames[m_iFootPosition[1]];

    _bool isLeft = fLeftTime < fRightTime;

    if (isLeft && *m_pModel->Get_CurTrackPosition() >= fLeftTime)
    {
        m_curFoot = FOOT_R;
        ++m_iFootPosition[0];
    }

    if (!isLeft && *m_pModel->Get_CurTrackPosition() >= fRightTime)
    {
        m_curFoot = FOOT_L;
        ++m_iFootPosition[1];
    }
}

_bool CKhazan_GS_Anim_Move::Check_SprintStart()
{
    /* sprint start -> loop 못가는 상황 배제 */
    if (!Has_State(MOV::MOVE_SPRINT))
        return false;

    //gsword
    if (m_iCurWeapon == 4)
    {
        _uint iIndex = m_pModel->Get_CurAnimIndex();
        for (size_t i = 0; i < 8; i++)
        {
            if (m_pModel->Get_AnimIndexByName(s_strGSSprinStartAnims[i]) == iIndex) {
                if (m_pModel->IsFinished())
                {
                    m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_Sprint_F");
                    m_pModel->Set_Animation(m_iSelectedAnimationIndex);
                    return true;
                }
            }
        }

    }
    //spear , barehand
    else
    {
        _uint iIndex = m_pModel->Get_CurAnimIndex();
        for (size_t i = 0; i < 8; i++)
        {
            if (m_pModel->Get_AnimIndexByName(s_strSpearSprinStartAnims[i]) == iIndex) {
                if (m_pModel->IsFinished())
                {
                    m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Sprint_F");
                    if (m_iSelectedAnimationIndex == 0)
                        int  a = 0;
                    m_pModel->Set_Animation(m_iSelectedAnimationIndex);
                    return true;
                }
            }
        }
    }

  

    return false;
}

_bool CKhazan_GS_Anim_Move::Check_Dodge()
{
    if (!Has_State(MOV::MOVE_DODGE))
        return false;

    _uint curAnimIndex = m_pModel->Get_CurAnimIndex();

    _bool isDodgeAnim = false;



    //gsword{
    if (m_iCurWeapon == 4)
    {
        for (const auto& animName : s_strGSDodgeAnims)
        {
            if (curAnimIndex == m_pModel->Get_AnimIndexByName(animName))
            {
                isDodgeAnim = true;
                break;
            }
        }
    }
    //spear + bareHand
    else
    {
        for (const auto& animName : s_strSpearDodgeAnims)  //todo 이상하면 스피어 dodge 추가 
        {
            if (curAnimIndex == m_pModel->Get_AnimIndexByName(animName))
            {
                isDodgeAnim = true;
                break;
            }
        }
    }

    // Dodge 애니메이션이고 최소 시간이 지났으면 종료
    if (isDodgeAnim && m_pModel->Check_MinAnimationTime())
    {
        m_isStopAnimationFinished = true;
        m_isMoving = false;
        m_isDodging = false;
        return true;
    }


    return false;

}

void CKhazan_GS_Anim_Move::Check_Reserve()
{
    if (m_isReserve)
    {
        if (m_pModel->Check_MinAnimationTime() || m_pModel->IsFinished()) {
            Try_ChangeAnimation(m_eReserveInfo);
            m_isReserve = false;
        }
    }

}

void CKhazan_GS_Anim_Move::Clear()
{
    //_bool		    m_isStopAnimationFinished = { false };
    _bool		    m_isMoving = { false };
    //_bool		    m_isWalk = { false };
    //_bool		    m_isRun = { false };
    //_bool		    m_isSprint = { false };
    _bool		    m_isDodge = { false };
    _bool		    m_isReserve = { false };
    _bool           m_isTurning180 = { false };
}

_uint CKhazan_GS_Anim_Move::GetLockOnWalkAnimation(DIRECTION_INFO eDir)
{
    if (eDir.AllCheck_Flag(DIR::F | DIR::L)) return m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_LockOn_Walk_FL");
    else if (eDir.AllCheck_Flag(DIR::F | DIR::R)) return m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_LockOn_Walk_FR");
    else if (eDir.AllCheck_Flag(DIR::B | DIR::L)) return m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_LockOn_Walk_BL");
    else if (eDir.AllCheck_Flag(DIR::B | DIR::R)) return m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_LockOn_Walk_BR");
    else if (eDir.AllCheck_Flag(DIR::F)) return m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_LockOn_Walk_F");
    else if (eDir.AllCheck_Flag(DIR::L)) return m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_LockOn_Walk_L");
    else if (eDir.AllCheck_Flag(DIR::B)) return m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_LockOn_Walk_B");
    else if (eDir.AllCheck_Flag(DIR::R)) return m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_LockOn_Walk_R");

    return m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_LockOn_Walk_F");
}

_uint CKhazan_GS_Anim_Move::GetLockOnRunAnimation(DIRECTION_INFO eDir)
{
    if (eDir.AllCheck_Flag(DIR::F | DIR::L)) return m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_LockOn_Run_FL");
    else if (eDir.AllCheck_Flag(DIR::F | DIR::R)) return m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_LockOn_Run_FR");
    else if (eDir.AllCheck_Flag(DIR::B | DIR::R)) return m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_LockOn_Run_BR");
    else if (eDir.AllCheck_Flag(DIR::B | DIR::L)) return m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_LockOn_Run_BL");
    else  if (eDir.AllCheck_Flag(DIR::F)) return m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_LockOn_Run_F");
    else if (eDir.AllCheck_Flag(DIR::L)) return m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_LockOn_Run_L");
    else if (eDir.AllCheck_Flag(DIR::B)) return m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_LockOn_Run_B");
    else if (eDir.AllCheck_Flag(DIR::R)) return m_pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_LockOn_Run_R");

    return m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_LockOn_Run_F");
}

inline _uint CKhazan_GS_Anim_Move::Get_AnimIndexByName(const string& bare, const string& spear, const string& gs)
{
    if (m_iCurWeapon == 1)
        return  m_pModel->Get_AnimIndexByName(bare);
    if (m_iCurWeapon == 2)
        return  m_pModel->Get_AnimIndexByName(spear);
    if (m_iCurWeapon == 4)
        return  m_pModel->Get_AnimIndexByName(gs);

    return m_pModel->Get_AnimIndexByName(gs);
}

CKhazan_GS_Anim_Move* CKhazan_GS_Anim_Move::Create()
{

    return  new CKhazan_GS_Anim_Move;
}

void CKhazan_GS_Anim_Move::Free()
{
    __super::Free();

}
