#include "Khazan_Spear_Anim_Move.h"
#include "GameInstance.h"

using DIR = DIRECTION_INFO::DIR;

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

    m_FootInfo[GetBitPosition(MOV::MOVE_SPRINT)] = {
     { 6.f, 19.f, 34.f, 48.f, 62.f, 76.f, 90.f, 104.f},
     {0.f, 12.f, 26.f, 40.f, 54.f, 68.f,82.f, 96.f, 110.f},
      112.f };

    return S_OK;
}

void CKhazan_Spear_Anim_Move::Enter()
{
    m_iFootPosition[0] = m_iFootPosition[1] = 0;
}


void CKhazan_Spear_Anim_Move::Continue(_float fTimeDelta)
{
    /* Dodge �ִϸ��̼��� �������� Ȯ�� */
    if (Has_State(MOV::MOVE_DODGE))
    {
        if (m_pModel->Check_MinAnimationTime())
        {
            m_isEndAnimationFinished = true;
            m_isMoving = false;
            m_isDodging = false;
            return;
        }
    }


    /* 발 위치 계산 */

    if (Has_State(MOV::MOVE_WALK | MOV::MOVE_RUN /*| MOV::MOVE_SPRINT)*/))
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
            // Reserve�� �õ��ϵ�, �����ϸ� �׳� Clear
            _bool isTry = Try_ChangeAnimation(m_eReserveInfo);
            m_isReserve = false;

            //_bool isTry = Try_ChangeAnimation(m_eReserveInfo);
            //m_isReserve = false;
            //cout << "m_isReserve " << endl;
            //if(!isTry){
            //if (Has_State(MOV::MOVE_WALK))
            //    m_pModel->Set_Animation(m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Walk_F"));
            //else 	if (Has_State(MOV::MOVE_RUN))
            //    m_pModel->Set_Animation(m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Run_F"));
            //else 	if (Has_State(MOV::MOVE_SPRINT))
            //    m_pModel->Set_Animation(m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Sprint_F"));
            //}
        }
    }
}

void CKhazan_Spear_Anim_Move::Exit()
{
    //cout << "EXIT " << endl;
   // m_isMoving = m_isReserve = false;
}

_bool CKhazan_Spear_Anim_Move::Try_ChangeAnimation(SPEAR_MOVE moveInfo)
{
    //if (Has_State(MOV::MOVE_RUN) && moveInfo.eDir.Check_Flag(DIR::L | DIR::R) && !(moveInfo.iCycle & CYC::CYCLE_END)) {
    //    OutputDebugStringA("[MOVE_RUN] [L R SKIP]\n");

    //    return true;
    //}
    //if (Has_State(MOV::MOVE_WALK) && moveInfo.eDir.Check_Flag(DIR::L | DIR::R) && !(moveInfo.iCycle & CYC::CYCLE_END)) {
    //    OutputDebugStringA("[MOVE_WALK] [L R SKIP]\n");

    //    return true;
    //}
    //if (Has_State(MOV::MOVE_SPRINT) && moveInfo.eDir.Check_Flag(DIR::L | DIR::R) && !(moveInfo.iCycle & CYC::CYCLE_END) && !(moveInfo.iCycle & CYC::CYCLE_LOOP)) {
    //    OutputDebugStringA("[MOVE_SPRINT] [L R SKIP]\n");

    //    return true;
    //}
    //if (m_isRun_Turn) {
    //    OutputDebugStringA("[m_isRun_Turn]\n");

    //    return true;
    //}

    //_bool isOnlyLRChange = false;

    //// 이전 상태와 현재 상태가 같고, F/B는 유지되면서 L/R만 바뀐 경우
    //if (Has_State(moveInfo.iSubState))  // SubState가 같음
    //{
    //    // F나 B 방향은 유지되는지 체크
    //    _bool hasFB = moveInfo.eDir.Check_Flag(DIR::F) || moveInfo.eDir.Check_Flag(DIR::B);
    //    _bool hasLR = moveInfo.eDir.Check_Flag(DIR::L) || moveInfo.eDir.Check_Flag(DIR::R);

    //    // F/B는 있고 L/R만 변경된 경우
    //    if (hasFB && hasLR && !(moveInfo.iCycle & CYC::CYCLE_END) && !(moveInfo.iCycle & CYC::CYCLE_START))
    //    {
    //        isOnlyLRChange = true;
    //    }
    //}

    //// L/R만 변경된 경우 Skip
    //if (isOnlyLRChange)
    //{
    //    OutputDebugStringA("[SKIP] Only L/R direction changed\n");
    //    return true;  // 변경 성공으로 처리 (실제로는 변경 안함)
    //}

    //// Run Turn 중이면 Skip
    //if (m_isRun_Turn)
    //{
    //    OutputDebugStringA("[SKIP] Run Turn in progress\n");
    //    return true;
    //}


    _uint iSelectedAnimationIndex{};
    m_isDodging = false;
    Clear_State();
    Add_State(moveInfo.iSubState);
    m_isEndAnimationFinished = false;

    auto GetAnimIndexByState = [&](const std::string& bare, const std::string& spear)
        {
            return m_pModel->Get_AnimIndexByName( moveInfo.isEquipWeapon ? bare.c_str() : spear.c_str() );
        };


    if (Has_State(MOV::MOVE_WALK))
    {

        if (moveInfo.iCycle & CYC::CYCLE_END)
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
        if (moveInfo.iCycle & CYC::CYCLE_END)
		{
			if (m_curFoot == FOOT_R) iSelectedAnimationIndex = GetAnimIndexByState("CA_P_Kazan_BareHands_Run_Stop_F_RF", "CA_P_Kazan_Spear_Run_Stop_F_RF");
			else  iSelectedAnimationIndex = GetAnimIndexByState("CA_P_Kazan_BareHands_Run_Stop_F_LF", "CA_P_Kazan_Spear_Run_Stop_F_LF");
          //  m_isStopRunAnim = true;
            m_isEndAnimationFinished = true;
            m_isMoving = false;
			OutputDebugStringA("[MOVE_RUN] [CYCLE_END]\n");
		}
		else {
            iSelectedAnimationIndex = GetAnimIndexByState("CA_P_Kazan_BareHands_Run_F", "CA_P_Kazan_Spear_Run_F");
			OutputDebugStringA("[MOVE_RUN] [CYCLE_START, LOOP]\n");
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
            OutputDebugStringA("[MOVE_SPRINT] [CYCLE_START]\n");
        }
        else  if (moveInfo.iCycle & CYC::CYCLE_END)
        {
            iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Sprint_Stop_F");
            m_isEndAnimationFinished = true;
            m_isMoving = false;
            OutputDebugStringA("[MOVE_SPRINT] [CYCLE_END]\n");
        }
        else {
            iSelectedAnimationIndex = GetAnimIndexByState("CA_P_Kazan_Spear_Sprint_F", "CA_P_Kazan_Spear_Sprint_F");
            OutputDebugStringA("[MOVE_SPRINT] [CYCLE_LOOP]\n");
        }
        cout << " S P R I N T  " << endl;
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

    }

    if (m_pModel->Check_MinAnimationTime())
    {
        m_isMoving = true;

        m_iSelectedAnimationIndex = iSelectedAnimationIndex;
        m_pModel->Set_Animation(m_iSelectedAnimationIndex);

        return true;
    }
    else
        Reserve_Animation(moveInfo);
        //return false;
}

void CKhazan_Spear_Anim_Move::Reserve_Animation(SPEAR_MOVE moveInfo)
{
    //if (m_isMoving)
    //    return;
    m_isReserve = true;
    m_eReserveInfo = moveInfo;
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
