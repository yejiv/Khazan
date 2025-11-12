//#include "ClientPch.h"
#include "Khazan_Spear_Anim_Guard.h"
#include "GameInstance.h"

CKhazan_Spear_Anim_Guard::CKhazan_Spear_Anim_Guard()
{
}

HRESULT CKhazan_Spear_Anim_Guard::Initialize_Prototype()
{
	return S_OK;
}

void CKhazan_Spear_Anim_Guard::Enter()
{
}

void CKhazan_Spear_Anim_Guard::Continue(_float fTimeDelta)
{
	// 가드 시작 -> 루프로 전환
	if (m_isGuardStart && m_pModel->IsFinished())
	{
		m_isGuardStart = false;
		m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Guard_Loop");
		m_pModel->Set_Animation(m_iSelectedAnimationIndex);
	}

	// 가드 성공 -> 루프로 복귀
	if (m_isGuardSuccess && m_pModel->IsFinished())
	{
		m_isGuardSuccess = false;
		m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Guard_Loop");
		m_pModel->Set_Animation(m_iSelectedAnimationIndex);
	}

	// WalkGuard 시작 -> 실제 Walk 애니메이션으로 전환
	if (m_isWalkGuardStart && m_pModel->Check_MinAnimationTime())
	{
		m_isWalkGuardStart = false;
		Try_WalkGuard(m_iMoveDir);
	}

	if (m_isFinishedGuard && m_pModel->Check_MinAnimationTime())
	{
		Exit();
	}
}

void CKhazan_Spear_Anim_Guard::Exit()
{
	m_isGuarding = false;
	m_isWalkGuarding = false;
	m_isJustGuarding = false;
	m_isGuardStart = false;
	m_isWalkGuardStart = false;
	m_isGuardSuccess = false;
	m_isFinishedGuard = false;
	m_iHitDir = 0;
	m_iMoveDir = 0;
}

_bool CKhazan_Spear_Anim_Guard::Try_Guard()
{
	if (!m_pModel->Check_MinAnimationTime()/* || !m_pModel->IsFinished()*/)
		return false;

    /*  걷는 가드 -> 제자리 가드*/
	if (m_isWalkGuarding)
	{
		m_isWalkGuarding = false;
		m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Guard_Loop");
		m_pModel->Set_Animation(m_iSelectedAnimationIndex);
	}

    /* 가드 시작 */
	if (!m_isGuarding ) {
		m_isGuarding = true;
		m_isGuardStart = true;
		m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Guard_Start");
		m_pModel->Set_Animation(m_iSelectedAnimationIndex);
		return true;
	}

	return false;
}

_bool CKhazan_Spear_Anim_Guard::Try_SuccessGuard(_uint iHitDir)
{
	if (!m_isGuarding)
		return false;

	m_isWalkGuarding = m_isJustGuarding = false;


	if (m_iHitDir & DIRECTION_INFO::DIR::U) 
		m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Com_Guard_Suc_U");
	else if (m_iHitDir & DIRECTION_INFO::DIR::L) 
		m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Com_Guard_Suc_L");	
	else if (m_iHitDir & DIRECTION_INFO::DIR::R) 
		m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Com_Guard_Suc_R");
	else 
		m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Com_Guard_Suc_D");


	m_iHitDir = iHitDir;
	m_isGuardSuccess = true;
	m_pModel->Set_Animation(m_iSelectedAnimationIndex);

	return true;
}

_bool CKhazan_Spear_Anim_Guard::Try_WalkGuard(_uint iMoveDir)
{
	//if (!m_pModel->Check_MinAnimationTime() /*|| !m_pModel->IsFinished()*/)
	//	return false;

	   // 가드 중이 아니면 가드 시작
	if (!m_isGuarding)
	{
		m_iMoveDir = iMoveDir;
		m_isGuarding = true;
		m_isWalkGuardStart = true;
		m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Guard_Start");
		m_pModel->Set_Animation(m_iSelectedAnimationIndex);
		return true;
	}

	// 방향 변경 감지
	//if (m_iMoveDir != iMoveDir)
	//{
	//	m_iMoveDir = iMoveDir;
	//	m_isWalkGuarding = false;  // 새로운 방향으로 전환 필요
	//}

	//// 이미 해당 방향의 WalkGuard 중이면 변경 불필요
	//if (m_isWalkGuarding)
	//	return false;

	// 최소 애니메이션 시간 체크
	if (!m_pModel->Check_MinAnimationTime())
		return false;

	// 방향에 따른 애니메이션 선택
	if ((iMoveDir & DIRECTION_INFO::DIR::F) && (iMoveDir & DIRECTION_INFO::DIR::R))
		m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Guard_Walk_FR");
	else if ((iMoveDir & DIRECTION_INFO::DIR::F) && (iMoveDir & DIRECTION_INFO::DIR::L))
		m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Guard_Walk_FL");
	else if (iMoveDir & DIRECTION_INFO::DIR::F)
		m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Guard_Walk_F");
	else if ((iMoveDir & DIRECTION_INFO::DIR::B) && (iMoveDir & DIRECTION_INFO::DIR::R))
		m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Guard_Walk_BR");
	else if ((iMoveDir & DIRECTION_INFO::DIR::B) && (iMoveDir & DIRECTION_INFO::DIR::L))
		m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Guard_Walk_BL");
	else if (iMoveDir & DIRECTION_INFO::DIR::B)
		m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Guard_Walk_B");
	else if (iMoveDir & DIRECTION_INFO::DIR::R)
		m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Guard_Walk_R");
	else if (iMoveDir & DIRECTION_INFO::DIR::L)
		m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Guard_Walk_L");
	else
		return false;  // 유효하지 않은 방향

	m_isWalkGuarding = true;
	m_pModel->Set_Animation(m_iSelectedAnimationIndex);
	return true;
}

_bool CKhazan_Spear_Anim_Guard::Try_JustGuard(_uint m_iHitDir)
{
	if (!m_pModel->Check_MinAnimationTime() /*|| !m_pModel->IsFinished()*/)
		return false;

	if (m_iHitDir & DIRECTION_INFO::DIR::U) {
		m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Com_JustGuard_U");
	}
	else if (m_iHitDir & DIRECTION_INFO::DIR::L) {
		m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Com_JustGuard_L");
	}
	else if (m_iHitDir & DIRECTION_INFO::DIR::R) {
		m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Com_JustGuard_R");
	}
	else {
		m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Com_JustGuard_D");
	}

	m_pModel->Set_Animation(m_iSelectedAnimationIndex);

	return true;
}

_bool CKhazan_Spear_Anim_Guard::Play_FinishGuard()
{
	if (!m_isGuarding)
		return false;

	m_isFinishedGuard = true;
	m_iSelectedAnimationIndex = m_pModel->Get_AnimIndexByName("CA_P_Kazan_Spear_Guard_End");
	m_pModel->Set_Animation(m_iSelectedAnimationIndex);

	return true;
}

CKhazan_Spear_Anim_Guard* CKhazan_Spear_Anim_Guard::Create()
{
	return new CKhazan_Spear_Anim_Guard;

}

void CKhazan_Spear_Anim_Guard::Free()
{
	__super::Free();
}
