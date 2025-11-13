#include "Khazan_Spear_Anim_Move.h"
#include "GameInstance.h"

using DIR = DIRECTION_INFO::DIR;

const string CKhazan_Spear_Anim_Move::s_strSprinStartAnims[] =
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
const string CKhazan_Spear_Anim_Move::s_strDodgeAnims[] =
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

CKhazan_Spear_Anim_Move::CKhazan_Spear_Anim_Move()
{
}

HRESULT CKhazan_Spear_Anim_Move::Initialize_Prototype()
{
    m_FootInfo[GetBitPosition(MOV::MOVE_WALK)] = {
        {7.7f},
        {22.f},
         30.f };

    m_FootInfo[GetBitPosition(MOV::MOVE_RUN)] = {
        { 7.f, 26.f, 46.f,  66.f, 86.f, 106.f},
        {16.f, 36.f, 56.f, 76.f, 96.f, 116.f},
         120.f };

    //m_FootInfo[GetBitPosition(MOV::MOVE_SPRINT)] = {
    // { 6.f, 19.f, 34.f, 48.f, 62.f, 76.f, 90.f, 104.f},
    // {0.f, 12.f, 26.f, 40.f, 54.f, 68.f,82.f, 96.f, 110.f},
    //  112.f };

    m_FootInfo[GetBitPosition(MOV::MOVE_INJURED)] = {
        { 0.f, 23.f, 53.f, 85.f, 114.f, 146.f, 179.f, 211.f,246.f,278.f},
        {10.f, 42.f, 68.f, 100.f, 130.f,164.f,196.f, 230.f, 264.f},
        284.f };
    return S_OK;
}

void CKhazan_Spear_Anim_Move::Enter()
{
    m_iFootPosition[0] = m_iFootPosition[1] = 0;
}


void CKhazan_Spear_Anim_Move::Continue(_float fTimeDelta)
{
    /* sprint start -> loop 못가는 상황 배제 */
    if (Has_State(MOV::MOVE_SPRINT)) {
        _uint iIndex = m_pModel->Get_CurAnimIndex();
        for (size_t i = 0; i < 8; i++)
        {
            if (m_pModel->Get_AnimIndexByName(s_strSprinStartAnims[i]) == iIndex) {
                if (m_pModel->IsFinished())
                {
                    m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Sprint_F");
                    m_pModel->Set_Animation(m_iSelectedAnimationIndex);
                    break;
                }
            }
        }

    }


    //if (Has_State(MOV::MOVE_DODGE))
    //{
    //    if (m_pModel->Check_MinAnimationTime())
    //    {
    //        m_isEndAnimationFinished = true;
    //        m_isMoving = false;
    //        m_isDodging = false;
    //        return;
    //    }
    //}

    if (Has_State(MOV::MOVE_DODGE))
    {
        _uint curAnimIndex = m_pModel->Get_CurAnimIndex();

        _bool isDodgeAnim = false;
        for (const auto& animName : s_strDodgeAnims)
        {
            if (curAnimIndex == m_pModel->Get_AnimIndexByName(animName))
            {
                isDodgeAnim = true;
                break;
            }
        }

        // Dodge 애니메이션이고 최소 시간이 지났으면 종료
        if (isDodgeAnim && m_pModel->Check_MinAnimationTime())
        {
            m_isEndAnimationFinished = true;
            m_isMoving = false;
            m_isDodging = false;
            return;
        }
    }


    /* 발 위치 계산 */

    if (Has_State(MOV::MOVE_WALK | MOV::MOVE_RUN | MOV::MOVE_INJURED))
    {
        if (m_iFootPosition[0] >= m_FootInfo[GetBitPosition(m_iState)].vLeftFootFrames.size() ||
            m_iFootPosition[1] >= m_FootInfo[GetBitPosition(m_iState)].vRightFootFrames.size())
            m_iFootPosition[0] = m_iFootPosition[1] = 0;

        _float fLeftTime = m_FootInfo[GetBitPosition(m_iState)].vLeftFootFrames[m_iFootPosition[0]];
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

    if (m_isReserve)
    {

        if (m_pModel->Check_MinAnimationTime()|| m_pModel->IsFinished()) {
            //todo : Reserve Clear
            _bool isTry = Try_ChangeAnimation(m_eReserveInfo);
            m_isReserve = false;

        }
    }
}

void CKhazan_Spear_Anim_Move::Exit()
{
    //cout << "EXIT " << endl;
   // m_isMoving = m_isReserve = false;
}

//_bool CKhazan_Spear_Anim_Move::Try_ChangeAnimation(SPEAR_MOVE moveInfo)
//{
//    _uint currentAnimIndex = m_pModel->Get_CurAnimIndex();
//
//    // Sprint Stop 애니메이션 체크
//    if (currentAnimIndex == m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Sprint_Stop_F") ||
//        currentAnimIndex == m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_LockOn_Sprint_Stop_F"))
//    {
//        if (!m_pModel->Check_MinAnimationTime())
//        {
//            cout << "Sprint Stop in progress, cannot change" << endl;
//            return false;
//        }
//    }
//
//    // Dodge 애니메이션 체크 - 진행 중이면 변경 불가
//    static const string dodgeAnims[] = {
//        "CA_P_Kazan_Spear_Com_Dodge_F", "CA_P_Kazan_Spear_Com_Dodge_B",
//        "CA_P_Kazan_Spear_Com_Dodge_L", "CA_P_Kazan_Spear_Com_Dodge_R",
//        "CA_P_Kazan_Spear_Com_Dodge_FL", "CA_P_Kazan_Spear_Com_Dodge_FR",
//        "CA_P_Kazan_Spear_Com_Dodge_BL", "CA_P_Kazan_Spear_Com_Dodge_BR"
//    };
//
//    for (const auto& animName : dodgeAnims)
//    {
//        if (currentAnimIndex == m_pModel->Get_AnimIndexByName(animName))
//        {
//            if (!m_pModel->Check_MinAnimationTime())
//            {
//                cout << "Dodge in progress, cannot change to: " << moveInfo.iSubState << endl;
//                return false;
//            }
//        }
//    }
//
//    _uint iSelectedAnimationIndex{};
//    m_isDodging = false;
//
//    // 이전 상태 저장
//    _uint prevState = m_iState;
//
//    Clear_State();
//    Add_State(moveInfo.iSubState);
//    m_isEndAnimationFinished = false;
//    _bool isLockOn = moveInfo.isLockOn;
//
//    auto GetAnimIndexByState = [&](const std::string& bare, const std::string& spear)
//        {
//            return m_pModel->Get_AnimIndexByName(moveInfo.isEquipWeapon ? bare.c_str() : spear.c_str());
//        };
//
//    if (Has_State(MOV::MOVE_WALK))
//    {
//        if (isLockOn)
//        {
//            iSelectedAnimationIndex = GetLockOnWalkAnimation(moveInfo.eDir);
//        }
//        else if (moveInfo.iCycle & CYC::CYCLE_END)
//        {
//            if (m_curFoot == FOOT_R)
//                iSelectedAnimationIndex = GetAnimIndexByState("CA_P_Kazan_BareHands_Walk_Stop_F_RF", "CA_P_Kazan_Spear_Walk_Stop_F_RF");
//            else
//                iSelectedAnimationIndex = GetAnimIndexByState("CA_P_Kazan_BareHands_Walk_Stop_F_LF", "CA_P_Kazan_Spear_Walk_Stop_F_LF");
//
//            m_isEndAnimationFinished = true;
//            m_isMoving = false;
//        }
//        else
//            iSelectedAnimationIndex = GetAnimIndexByState("CA_P_Kazan_BareHands_Walk_F", "CA_P_Kazan_Spear_Walk_F");
//    }
//
//    if (Has_State(MOV::MOVE_RUN))
//    {
//        if (isLockOn)
//        {
//            iSelectedAnimationIndex = GetLockOnRunAnimation(moveInfo.eDir);
//        }
//        else if (moveInfo.iCycle & CYC::CYCLE_END)
//        {
//            if (m_curFoot == FOOT_R)
//                iSelectedAnimationIndex = GetAnimIndexByState("CA_P_Kazan_BareHands_Run_Stop_F_RF", "CA_P_Kazan_Spear_Run_Stop_F_RF");
//            else
//                iSelectedAnimationIndex = GetAnimIndexByState("CA_P_Kazan_BareHands_Run_Stop_F_LF", "CA_P_Kazan_Spear_Run_Stop_F_LF");
//
//            m_isEndAnimationFinished = true;
//            m_isMoving = false;
//        }
//        else
//        {
//            iSelectedAnimationIndex = GetAnimIndexByState("CA_P_Kazan_BareHands_Run_F", "CA_P_Kazan_Spear_Run_F");
//        }
//    }
//
//    if (Has_State(MOV::MOVE_SPRINT))
//    {
//        if (moveInfo.iCycle & CYC::CYCLE_START)
//        {
//            if (moveInfo.eDir.AllCheck_Flag(DIR::B | DIR::L))
//                iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Sprint_BL_Start");
//            else if (moveInfo.eDir.AllCheck_Flag(DIR::B | DIR::R))
//                iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Sprint_BR_Start");
//            else if (moveInfo.eDir.AllCheck_Flag(DIR::B))
//                iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Sprint_B_Start");
//            else if (moveInfo.eDir.AllCheck_Flag(DIR::F | DIR::L))
//                iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Sprint_FL_Start");
//            else if (moveInfo.eDir.AllCheck_Flag(DIR::F | DIR::R))
//                iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Sprint_FR_Start");
//            else if (moveInfo.eDir.AllCheck_Flag(DIR::F))
//                iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Sprint_F_Start");
//            else if (moveInfo.eDir.AllCheck_Flag(DIR::L))
//                iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Sprint_L_Start");
//            else if (moveInfo.eDir.AllCheck_Flag(DIR::R))
//                iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Sprint_R_Start");
//        }
//        else if (moveInfo.iCycle & CYC::CYCLE_END)
//        {
//            if (isLockOn)
//                iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_LockOn_Sprint_Stop_F");
//            else
//                iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Sprint_Stop_F");
//
//            m_isEndAnimationFinished = true;
//            m_isMoving = false;
//        }
//        else
//        {
//            iSelectedAnimationIndex = GetAnimIndexByState("CA_P_Kazan_Spear_Sprint_F", "CA_P_Kazan_Spear_Sprint_F");
//        }
//    }
//
//    if (Has_State(MOV::MOVE_DODGE))
//    {
//        if (moveInfo.eDir.AllCheck_Flag(DIR::B | DIR::L))
//            iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Com_Dodge_BR");
//        else if (moveInfo.eDir.AllCheck_Flag(DIR::B | DIR::R))
//            iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Com_Dodge_BL");
//        else if (moveInfo.eDir.AllCheck_Flag(DIR::B))
//            iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Com_Dodge_B");
//        else if (moveInfo.eDir.AllCheck_Flag(DIR::F | DIR::L))
//            iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Com_Dodge_FR");
//        else if (moveInfo.eDir.AllCheck_Flag(DIR::F | DIR::R))
//            iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Com_Dodge_FL");
//        else if (moveInfo.eDir.AllCheck_Flag(DIR::F))
//            iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Com_Dodge_F");
//        else if (moveInfo.eDir.AllCheck_Flag(DIR::L))
//            iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Com_Dodge_L");
//        else if (moveInfo.eDir.AllCheck_Flag(DIR::R))
//            iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Com_Dodge_R");
//
//        m_isDodging = true;
//        m_isEndAnimationFinished = false;  // Dodge는 진행 중
//       // cout << "Dodge animation set: " << iSelectedAnimationIndex << endl;
//    }
//
//    _bool isCheckMin = m_pModel->Check_MinAnimationTime();
//
//    // 같은 애니메이션이고 최소 시간이 안지났으면 변경 안함
//    if (m_iPrevSelectedAnimationIndex == iSelectedAnimationIndex && !isCheckMin)
//    {
//        //cout << "Same animation, not changed" << endl;
//        return false;
//    }
//
//    if (isCheckMin || Has_State(MOV::MOVE_DODGE))  // Dodge는 즉시 실행
//    {
//        m_isMoving = true;
//        m_iPrevSelectedAnimationIndex = m_iSelectedAnimationIndex;
//        m_iSelectedAnimationIndex = iSelectedAnimationIndex;
//        m_pModel->Set_Animation(m_iSelectedAnimationIndex);
//
//       // cout << "Animation changed to: " << iSelectedAnimationIndex << endl;
//        return true;
//    }
//    else
//    {
//        Reserve_Animation(moveInfo);
//        return false;
//    }
//}

_bool CKhazan_Spear_Anim_Move::Try_ChangeAnimation(SPEAR_MOVE moveInfo)
{
    _uint currentAnimIndex = m_pModel->Get_CurAnimIndex();
    if (currentAnimIndex == m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Sprint_Stop_F") ||
        currentAnimIndex == m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_LockOn_Sprint_Stop_F"))
    {
        //_float progress = *m_pModel->Get_CurTrackPosition() / m_pModel->Get_CurDuration();

        //// Stop 애니메이션이 60% 이상 진행되기 전에는 변경 차단
        //if (progress < 0.6f)
        //{
        //    return false;
        //}

        if (!m_pModel->Check_MinAnimationTime())
            return false;
    }

    //for (const auto& animName : s_strDodgeAnims)
    //{
    //    if (currentAnimIndex == m_pModel->Get_AnimIndexByName(animName))
    //    {
    //        if (!m_pModel->Check_MinAnimationTime())
    //        {
    //            //cout << "Dodge in progress, cannot change to: " << moveInfo.iSubState << endl;
    //            return false;
    //        }
    //    }
    //}



    _uint iSelectedAnimationIndex{};
    m_isDodging = false;
    Clear_State();
    Add_State(moveInfo.iSubState);
    m_isEndAnimationFinished = false;
    _bool   isLockOn = moveInfo.isLockOn;

    auto GetAnimIndexByState = [&](const std::string& bare, const std::string& spear)
        {
            return m_pModel->Get_AnimIndexByName( moveInfo.isEquipWeapon ? bare.c_str() : spear.c_str() );
        };


    if (Has_State(MOV::MOVE_WALK))
    {
        if (isLockOn)
        {
            iSelectedAnimationIndex = GetLockOnWalkAnimation(moveInfo.eDir);
            cout << "Lock on   Walkkkk" << endl;
        }
        else if (moveInfo.iCycle & CYC::CYCLE_END)
        {
            if (m_curFoot == FOOT_R) iSelectedAnimationIndex = GetAnimIndexByState("CA_P_Kazan_BareHands_Walk_Stop_F_RF", "CA_P_Kazan_Spear_Walk_Stop_F_RF");
            else  iSelectedAnimationIndex = GetAnimIndexByState("CA_P_Kazan_BareHands_Walk_Stop_F_LF", "CA_P_Kazan_Spear_Walk_Stop_F_LF");

            m_isEndAnimationFinished = true;
            m_isMoving = false;
        }
        else 
            iSelectedAnimationIndex = GetAnimIndexByState("CA_P_Kazan_BareHands_Walk_F", "CA_P_Kazan_Spear_Walk_F");

	}


	if (Has_State(MOV::MOVE_RUN))
	{
  
		//if (moveInfo.eDir.AllCheck_Flag(DIR::B))
		//{    /* turn */
		//	m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_BareHands_Run_MovingTurn_R_180");
  //          m_isRun_Turn = true;
  //          m_isEquipSpear = moveInfo.isEquipWeapon;
		//}
		//else 
        if (isLockOn)
        {
            iSelectedAnimationIndex = GetLockOnRunAnimation(moveInfo.eDir);
            cout << "Lock on   Run" << endl;
        }
        else if (moveInfo.iCycle & CYC::CYCLE_END)
		{   
		    if (m_curFoot == FOOT_R)  iSelectedAnimationIndex = GetAnimIndexByState("CA_P_Kazan_BareHands_Run_Stop_F_RF", "CA_P_Kazan_Spear_Run_Stop_F_RF");
			else  iSelectedAnimationIndex = GetAnimIndexByState("CA_P_Kazan_BareHands_Run_Stop_F_LF", "CA_P_Kazan_Spear_Run_Stop_F_LF");
          //  m_isStopRunAnim = true;
            m_isEndAnimationFinished = true;
            m_isMoving = false;
		}
		else {
            iSelectedAnimationIndex = GetAnimIndexByState("CA_P_Kazan_BareHands_Run_F", "CA_P_Kazan_Spear_Run_F");
		}

    }

    if (Has_State(MOV::MOVE_SPRINT))
    {
        if (moveInfo.iCycle & CYC::CYCLE_START)
        {
            if (moveInfo.eDir.AllCheck_Flag(DIR::B | DIR::L)) iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Sprint_BL_Start");
            else if (moveInfo.eDir.AllCheck_Flag(DIR::B | DIR::R)) iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Sprint_BR_Start");
            else if (moveInfo.eDir.AllCheck_Flag(DIR::B)) iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Sprint_B_Start");
            else if (moveInfo.eDir.AllCheck_Flag(DIR::F | DIR::L)) iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Sprint_FR_Start");
            else if (moveInfo.eDir.AllCheck_Flag(DIR::F | DIR::R)) iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Sprint_FR_Start");
            else if (moveInfo.eDir.AllCheck_Flag(DIR::F)) iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Sprint_F_Start");
            else if (moveInfo.eDir.AllCheck_Flag(DIR::L)) iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Sprint_L_Start");
            else if (moveInfo.eDir.AllCheck_Flag(DIR::R)) iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Sprint_R_Start");
        }
        else  if (moveInfo.iCycle & CYC::CYCLE_END)
        {
            if (isLockOn)
                iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_LockOn_Sprint_Stop_F");
            else
                iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Sprint_Stop_F");
            m_isEndAnimationFinished = true;
            m_isMoving = false;
        }
        else {
            iSelectedAnimationIndex = GetAnimIndexByState("CA_P_Kazan_Spear_Sprint_F", "CA_P_Kazan_Spear_Sprint_F");
        }
    }

    if (Has_State(MOV::MOVE_DODGE))
    {
        if (moveInfo.eDir.AllCheck_Flag(DIR::B | DIR::L)) iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Com_Dodge_BR");
        else if (moveInfo.eDir.AllCheck_Flag(DIR::B | DIR::R)) iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Com_Dodge_BL");
        else if (moveInfo.eDir.AllCheck_Flag(DIR::B)) iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Com_Dodge_B");
        else if (moveInfo.eDir.AllCheck_Flag(DIR::F | DIR::L)) iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Com_Dodge_FR");
        else if (moveInfo.eDir.AllCheck_Flag(DIR::F | DIR::R)) iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Com_Dodge_FL");
        else if (moveInfo.eDir.AllCheck_Flag(DIR::F)) iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Com_Dodge_F");
        else if (moveInfo.eDir.AllCheck_Flag(DIR::L)) iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Com_Dodge_L");
        else if (moveInfo.eDir.AllCheck_Flag(DIR::R)) iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Com_Dodge_R");
		m_isDodging = true;
        m_isEndAnimationFinished = true;
    }



    _bool isCheckMin = m_pModel->Check_MinAnimationTime();
    if (m_iPrevSelectedAnimationIndex == iSelectedAnimationIndex && !isCheckMin)
       return false;
    else if (isCheckMin || Has_State(MOV::MOVE_DODGE))
    {
        m_isMoving = true;
        m_iPrevSelectedAnimationIndex = m_iSelectedAnimationIndex;
        m_iSelectedAnimationIndex = iSelectedAnimationIndex;
        m_pModel->Set_Animation(m_iSelectedAnimationIndex);

        return true;
    }
    else {
        Reserve_Animation(moveInfo);
        return false;
    }
}

_bool CKhazan_Spear_Anim_Move::Try_InjuredAnimaition(SPEAR_MOVE moveInfo)
{
    _uint iSelectedAnimationIndex{};

    // 상태 초기화 후 설정
    Clear_State();
    Add_State(MOV::MOVE_INJURED);
    m_isEndAnimationFinished = false;

    // CYCLE_START 추가
    if (moveInfo.iCycle & CYC::CYCLE_START)
    {
        iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Injured_Walk");
        cout << "CYCLE_START" << endl;
        m_isMoving = true;
    }
    else if (moveInfo.iCycle & CYC::CYCLE_LOOP)
    {
        // Loop 중에는 현재 애니메이션 유지
        _uint currentAnim = m_pModel->Get_CurAnimIndex();
        _uint injuredWalkAnim = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Injured_Walk");

        // 이미 Walk 애니메이션이면 변경하지 않음
        if (currentAnim == injuredWalkAnim)
        {
            return false;  //애니메이션 재설정 방지
        }

        iSelectedAnimationIndex = injuredWalkAnim;
        m_isMoving = true;
        cout << "CYCLE_LOOP" << endl;
    }
    else if (moveInfo.iCycle & CYC::CYCLE_END)
    {
        iSelectedAnimationIndex = (m_curFoot == FOOT_R)
            ? m_pModel->Get_AnimIndexByName("CA_P_Kazan_Injured_Walk_Stop_F_LF")
            : m_pModel->Get_AnimIndexByName("CA_P_Kazan_Injured_Walk_Stop_F_RF");

        m_isEndAnimationFinished = true;
        m_isMoving = false;
        cout << "CYCLE_END" << endl;
    }

    // 최소 애니메이션 시간 체크
    if ( moveInfo.iCycle & CYC::CYCLE_START || m_pModel->Check_MinAnimationTime() )
    {
        m_iSelectedAnimationIndex = iSelectedAnimationIndex;
        m_pModel->Set_Animation(m_iSelectedAnimationIndex);
        cout << "Set_Animation" << endl;
        return true;
    }

    return false;
}

void CKhazan_Spear_Anim_Move::Reserve_Animation(SPEAR_MOVE moveInfo)
{
    m_isReserve = true;
    m_eReserveInfo = moveInfo;
}

_uint CKhazan_Spear_Anim_Move::GetLockOnWalkAnimation(DIRECTION_INFO eDir)
{
    if (eDir.AllCheck_Flag(DIR::F | DIR::L)) return m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_LockOn_Walk_FL");
    else if (eDir.AllCheck_Flag(DIR::F | DIR::R)) return m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_LockOn_Walk_FR");
    else if (eDir.AllCheck_Flag(DIR::B | DIR::L)) return m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_LockOn_Walk_BL");
    else if (eDir.AllCheck_Flag(DIR::B | DIR::R)) return m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_LockOn_Walk_BR");
    else if (eDir.AllCheck_Flag(DIR::F)) return m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_LockOn_Walk_F");
    else if (eDir.AllCheck_Flag(DIR::L)) return m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_LockOn_Walk_L");
    else if (eDir.AllCheck_Flag(DIR::B)) return m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_LockOn_Walk_B");
    else if (eDir.AllCheck_Flag(DIR::R)) return m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_LockOn_Walk_R");

    return m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_LockOn_Walk_F");
}

_uint CKhazan_Spear_Anim_Move::GetLockOnRunAnimation(DIRECTION_INFO eDir)
{
    if (eDir.AllCheck_Flag(DIR::F | DIR::L)) return m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_LockOn_Run_FL");
    else if (eDir.AllCheck_Flag(DIR::F | DIR::R)) return m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_LockOn_Run_FR");
    else if (eDir.AllCheck_Flag(DIR::B | DIR::R)) return m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_LockOn_Run_BR");
    else if (eDir.AllCheck_Flag(DIR::B | DIR::L)) return m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_LockOn_Run_BL");
    else  if (eDir.AllCheck_Flag(DIR::F)) return m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_LockOn_Run_F");
    else if (eDir.AllCheck_Flag(DIR::L)) return m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_LockOn_Run_L");
    else if (eDir.AllCheck_Flag(DIR::B)) return m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_LockOn_Run_B");
    else if (eDir.AllCheck_Flag(DIR::R)) return m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_LockOn_Run_R");

    return m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_LockOn_Run_F");
}

CKhazan_Spear_Anim_Move* CKhazan_Spear_Anim_Move::Create()
{
    CKhazan_Spear_Anim_Move* pInstance = new CKhazan_Spear_Anim_Move();

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CKhazan_Spear_Anim_Move"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CKhazan_Spear_Anim_Move::Free()
{
	__super::Free();


}
