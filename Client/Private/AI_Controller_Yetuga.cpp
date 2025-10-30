#include "AI_Controller_Yetuga.h"
#include "Yetuga.h"
#include "FSM_Yetuga.h"
#include "BlackBoard.h"
#include "BehaviorTree.h"
#include "GameInstance.h"
#include "Perception.h"


CAI_Controller_Yetuga::CAI_Controller_Yetuga()
{
}

HRESULT CAI_Controller_Yetuga::Initialize(CCreature* pOwner)
{

	CYetuga* pYetuga = static_cast<CYetuga*>(pOwner);

	if (FAILED(__super::Initialize(pYetuga, pYetuga->Get_Name())))
		return E_FAIL;

	m_pFSM = CFSM_Yetuga::Create();
	if (nullptr == m_pFSM)
		return E_FAIL;

	return S_OK;
}

void CAI_Controller_Yetuga::Update(CGameObject* pOwner, _float fTimeDelta)
{
	m_pPerception->Update(pOwner, fTimeDelta);

	_float fPrevTime = m_pBB->Get_Value<_float>(m_strMonstertag, "CurrentTime");
	m_pBB->Set_Value(m_strMonstertag, "CurrentTime", fPrevTime + fTimeDelta);

	_uint iDirFlag = m_pBB->Get_Value<_uint>("Yetuga", "TargetDirection");
	cout << "DirFlag : " << iDirFlag << "(";

	if (iDirFlag == 5) cout << "FL";
	if (iDirFlag == 9) cout << "FR";
	if (iDirFlag == 6) cout << "BL";
	if (iDirFlag == 10) cout << "BR";
	if (iDirFlag == 1) cout << "F";
	if (iDirFlag == 2) cout << "B";
	if (iDirFlag == 4) cout << "L";
	if (iDirFlag == 8) cout << "R";
	
	cout << ")" << endl;
	
	m_pBT->Update();

	m_pFSM->Update(pOwner,fTimeDelta);

}


HRESULT CAI_Controller_Yetuga::Ready_Perception(const AIPERCEPTION_DATA& Desc)
{
	m_pPerception = CPerception::Create(Desc, ENUM_CLASS(TEAM::YETI));
	if (nullptr == m_pPerception)
		return E_FAIL;

	m_pPerception->Set_PerceptionCallBack([this](CGameObject* pTarget, const STIMULUS& Stim)
		{
			if (Stim.eType == SENSETYPE::SIGHT)
			{
				if (Stim.bSensed)
				{
					m_pBB->Set_Value("Yetuga", "Target", pTarget);
					m_pBB->Set_Value("Yetuga", "isDetected", true);
				}
				else
				{
					m_pBB->Set_Value("Yetuga", "isDetected", false);
				}
			}
		});

	return S_OK;

}

HRESULT CAI_Controller_Yetuga::Ready_BlackBoard(CGameObject* pOwner)
{
	m_pBB = m_pGameInstance->Get_BlackBoard();
	if (nullptr == m_pBB)
		return E_FAIL;

	m_pBB->Set_Value(m_strMonstertag, "Owner", pOwner);

	return S_OK;
}


HRESULT CAI_Controller_Yetuga::Ready_BehaviorTree()
{
	if (nullptr == m_pBB)
		return E_FAIL;

	m_pBT->Set_BlackBoard(m_pBB);

	if (nullptr == m_pBT)
		return E_FAIL;

	return S_OK;
}

CONDITION CAI_Controller_Yetuga::GetCallbackCondition(CGameObject* pOwner, const string& name)
{
	CYetuga* pYetuga = static_cast<CYetuga*>(pOwner);
	if (nullptr == pYetuga)
		return nullptr;

	/*if ("ThrowBall" == name)
	{
		return [pYetuga](CBlackBoard* BB)->_bool
			{

				_float fDist = BB->Get_Value<_float>(pYetuga->Get_Name(), "TargetDist");
				_float fAttackRanage = BB->Get_Value<_float>(pYetuga->Get_Name(), "ThrowBallRange");
				if (fDist != 0 && fDist <= fAttackRanage && !BB->Get_Value<_bool>(pYetuga->Get_Name(), "IsThrowBall"))
				{
					cout << "IsThrowBall Condition TRUE!!!!!!!!!!!!" << endl;
					return true;
				}
				else
					return false;
			};
	}*/

	if ("LieDown" == name)
	{
		return [pYetuga](CBlackBoard* BB)->_bool
			{

				_float fDist = BB->Get_Value<_float>(pYetuga->Get_Name(), "TargetDist");
				_float fAttackRanage = BB->Get_Value<_float>(pYetuga->Get_Name(), "AttackRange");
				
				if (fDist != 0 && fDist <= fAttackRanage && !BB->Get_Value<_bool>(pYetuga->Get_Name(), "IsAttack3"))
				{
					cout << "LieDownCondition" << endl;

					DIRECTION_INFO Info = {};
					Info.iDirFlag = BB->Get_Value<_uint>("Yetuga", "TargetDirection");
					if (Info.Check_Flag(DIRECTION_INFO::DIR::B) && Info.Check_Flag(DIRECTION_INFO::DIR::L))
						return true;
					else if (Info.Check_Flag(DIRECTION_INFO::DIR::B))
						return true;
					else if (Info.Check_Flag(DIRECTION_INFO::DIR::B) && Info.Check_Flag(DIRECTION_INFO::DIR::R))
						return true;
				}
				else
					return false;
			};
	}

	if ("RightHand_2Hit" == name)
	{
		return [pYetuga](CBlackBoard* BB)->_bool
			{			

				_float fDist = BB->Get_Value<_float>(pYetuga->Get_Name(), "TargetDist");
				_float fAttackRanage = BB->Get_Value<_float>(pYetuga->Get_Name(), "AttackRange");
				if (fDist != 0 && fDist <= fAttackRanage && !BB->Get_Value<_bool>(pYetuga->Get_Name(), "IsAttack"))
				{
					//cout << "RightHand_2Hit Condition TRUE!!!!!!!!!!!!" << endl;
					return true;
				}
				else
					return false;
			};
	}

	if ("RightHand_5Hit" == name)
	{
		return [pYetuga](CBlackBoard* BB)->_bool
			{

				_float fDist = BB->Get_Value<_float>(pYetuga->Get_Name(), "TargetDist");
				_float fAttackRanage = BB->Get_Value<_float>(pYetuga->Get_Name(), "AttackRange");
				if (fDist != 0 && fDist <= fAttackRanage && !BB->Get_Value<_bool>(pYetuga->Get_Name(), "IsAttack2"))
				{
					//cout << "RightHand_5Hit Condition TRUE!!!!!!!!!!!!" << endl;
					return true;
				}
				else
					return false;
			};
	}

	if ("Turn" == name)
	{
		return [pYetuga](CBlackBoard* BB)->_bool
			{
				if (BB->Get_Value<_bool>(pYetuga->Get_Name(), "isDead")) return false;

				_float fDot = BB->Get_Value<_float>(pYetuga->Get_Name(), "fDot");
				fDot = clamp(fDot, -1.f, 1.f);
				_float fAngle = XMConvertToDegrees(acos(fDot));
				_float fLimit = BB->Get_Value<_float>(pYetuga->Get_Name(), "TurnChangeRange");

				if (fAngle > 15.f)
				{
					//cout << "Turn Condition" << endl;
					return true;
				}
				else
					return false;
			};
	}

	if ("MoveCondition" == name)
	{
		return [pYetuga](CBlackBoard* BB)->_bool
			{
				if (BB->Get_Value<_bool>(pYetuga->Get_Name(), "isDead"))
					return false;
				_float fDist = BB->Get_Value<_float>(pYetuga->Get_Name(), "TargetDist");
				_float fChaseRange = BB->Get_Value<_float>(pYetuga->Get_Name(), "ChaseRange");
				_float fSprintRange = BB->Get_Value<_float>(pYetuga->Get_Name(), "SprintRange");
				_float fRunRange = BB->Get_Value<_float>(pYetuga->Get_Name(), "RunRange");

				CYetuga::MONSTER_INFO Info{};

				Info.Clear_State();

				cout << "Dist" << fDist << endl;
				cout << "ChaseRange" << fChaseRange << endl;

				if (fDist != 0 && fDist <= fChaseRange)
				{
					cout << "MoveCondition" << endl;

					if (fDist <= fRunRange)
						Info.Add_State(Info.WALK);
					else if (fDist > fRunRange && fDist <= fSprintRange)
						Info.Add_State(Info.RUN);
					else if (fDist > fSprintRange)
						Info.Add_State(Info.SPRINT);
						
					BB->Set_Value<_uint>(pYetuga->Get_Name(), "iMovementFlag", Info.iStateFlag);

					return true;
				}
				else
					return false;
			};
	}


	return nullptr;
}

ACTION CAI_Controller_Yetuga::GetCallbackAction(CGameObject* pOwner, const string& name)
{
	CYetuga* pYetuga = static_cast<CYetuga*>(pOwner);
	if (nullptr == pYetuga)
		return nullptr;

	//if ("ThrowBall" == name)
	//{
	//	return [pYetuga](CBlackBoard* BB)-> BTNODESTATE
	//		{

	//			if (BB->Get_Value<_bool>(pYetuga->Get_Name(), "isThrowBallFinished"))
	//			{
	//				//cout << "ThrowBall Action SUCESSSS!!!!!!!!!!!!" << endl;
	//				return BTNODESTATE::SUCCESS;
	//			}
	//			//cout << "ThrowBall Action Running" << endl;

	//			BB->Set_Value(pYetuga->Get_Name(), "isisThrowBall", true);
	//			BB->Set_Value(pYetuga->Get_Name(), "isThrowBallFinished", false);


	//			pYetuga->Get_Controller()->Get_State_Machine()->
	//				Change_State(ENUM_CLASS(YETUGA_STATE::THROWBALL), pYetuga);
	//			return BTNODESTATE::RUNNING;

	//		};
	//}



	if ("LieDown" == name)
	{
		return [pYetuga](CBlackBoard* BB)-> BTNODESTATE
			{

				if (BB->Get_Value<_bool>(pYetuga->Get_Name(), "isAttackFinished3"))
				{

					return BTNODESTATE::SUCCESS;
				}

				BB->Set_Value(pYetuga->Get_Name(), "isAttack3", true);
				BB->Set_Value(pYetuga->Get_Name(), "isAttackFinished3", false);


				pYetuga->Get_Controller()->Get_State_Machine()->
					Change_State(ENUM_CLASS(YETUGA_STATE::LIE_DOWN), pYetuga);
				return BTNODESTATE::RUNNING;


			};
	}

	if ("Turn" == name)
	{
		return [pYetuga](CBlackBoard* BB)->BTNODESTATE
			{
				if (BB->Get_Value<_bool>(pYetuga->Get_Name(), "isTurnFinished"))
				{
					cout << "isTrurnSucess!!!!!!!!!!!!" << endl;
					return BTNODESTATE::SUCCESS;

				}
				pYetuga->Get_Controller()->Get_State_Machine()->
					Change_State(ENUM_CLASS(YETUGA_STATE::TURN), pYetuga);
				return BTNODESTATE::RUNNING;
			};
	}

	if ("RightHand_2Hit" == name)
	{
		return [pYetuga](CBlackBoard* BB)-> BTNODESTATE
			{

				if (BB->Get_Value<_bool>(pYetuga->Get_Name(), "isAttackFinished"))
				{
					//cout << "RightHand_2Hit Action SUCESSSS!!!!!!!!!!!!" << endl;
					return BTNODESTATE::SUCCESS;
				}
				//cout << "RightHand_2Hit Action Running" << endl;

				BB->Set_Value(pYetuga->Get_Name(), "isAttack", true);
				BB->Set_Value(pYetuga->Get_Name(), "isAttackFinished", false);

				
				pYetuga->Get_Controller()->Get_State_Machine()->
					Change_State(ENUM_CLASS(YETUGA_STATE::ATTACK),pYetuga);
				return BTNODESTATE::RUNNING;

			
			};
	}

	if ("RightHand_5Hit" == name)
	{
		return [pYetuga](CBlackBoard* BB)-> BTNODESTATE
			{
				if (BB->Get_Value<_bool>(pYetuga->Get_Name(), "isAttack2Finished"))
				{
					//cout << "RightHand_5Hit Action Suceess" << endl;
					return BTNODESTATE::SUCCESS;
				}
				//cout << "RightHand_5Hit Action Running" << endl;
				BB->Set_Value(pYetuga->Get_Name(), "isAttack2", true);
				BB->Set_Value(pYetuga->Get_Name(), "isAttack2Finished", false);

				pYetuga->Get_Controller()->Get_State_Machine()->
					Change_State(ENUM_CLASS(YETUGA_STATE::RIGHTHAND_5HIT), pYetuga);
				return BTNODESTATE::RUNNING;

			};
	}

	if ("MoveAction" == name)
	{
		CYetuga* pYetuga = static_cast<CYetuga*>(pOwner);
		if (nullptr == pYetuga)
			return nullptr;

		return [pYetuga](CBlackBoard* BB) ->BTNODESTATE
			{
				//cout << "MoveAction" << endl;
				if (BB->Get_Value<_float>(pYetuga->Get_Name(), "TargetDist") <= BB->Get_Value<_float>("Yetuga", "AttackRange"))
					return BTNODESTATE::SUCCESS;

				cout << "MoveRunning" << endl;
				pYetuga->Get_Controller()->Get_State_Machine()->Change_State(ENUM_CLASS(YETUGA_STATE::MOVE), pYetuga);

				return BTNODESTATE::RUNNING;
			};
	}

	if ("IdleAction" == name)
	{
		return [pYetuga](CBlackBoard* BB)->BTNODESTATE
			{
				if (BB->Get_Value<_bool>(pYetuga->Get_Name(), "isDead")) return BTNODESTATE::FAILURE;
				if (BB->Get_Value<_bool>(pYetuga->Get_Name(), "isDetected")) return BTNODESTATE::FAILURE;


				//cout << "Idle" << endl;
				pYetuga->Get_Controller()->Get_State_Machine()->Change_State(ENUM_CLASS(YETUGA_STATE::IDLE), pYetuga);
				return BTNODESTATE::RUNNING;
			};
	}

	return nullptr;
}

TERMINATE CAI_Controller_Yetuga::GetCallbackTeminate(CGameObject* pOwner, const string& name)
{
	CYetuga* pYetuga = static_cast<CYetuga*>(pOwner);
	if (nullptr == pYetuga)
		return nullptr;

	/*if ("ThrowBall" == name)
	{
		return [pYetuga](CBlackBoard* BB, BTNODESTATE eState)
			{
				if (nullptr == BB)
					return;

				if (eState == BTNODESTATE::SUCCESS || eState == BTNODESTATE::FAILURE)
				{
					cout << "Attack Turminate " << endl;

					BB->Set_Value<_bool>(pYetuga->Get_Name(), "isThrowBall", false);
					BB->Set_Value<_bool>(pYetuga->Get_Name(), "isisThrowBallFinished", false);
					pYetuga->Get_Controller()->Get_State_Machine()->Change_State(ENUM_CLASS(YETUGA_STATE::IDLE), pYetuga);
				}
			};
	}*/



	if ("LieDown" == name)
	{
		return [pYetuga](CBlackBoard* BB, BTNODESTATE eState)
			{
				if (nullptr == BB)
					return;

				if (eState == BTNODESTATE::SUCCESS || eState == BTNODESTATE::FAILURE)
				{
					cout << "Attack Turminate " << endl;

					BB->Set_Value<_bool>(pYetuga->Get_Name(), "IsAttack3", false);
					BB->Set_Value<_bool>(pYetuga->Get_Name(), "isAttackFinished3", false);
					pYetuga->Get_Controller()->Get_State_Machine()->Change_State(ENUM_CLASS(YETUGA_STATE::IDLE), pYetuga);
				}
			};
	}

	if ("Turn" == name)
	{
		return [pYetuga](CBlackBoard* BB, BTNODESTATE eState)
			{
				if (nullptr == BB)
					return;

				if (eState == BTNODESTATE::SUCCESS || eState == BTNODESTATE::FAILURE)
				{
					//cout << "Attack Turminate " << endl;
					BB->Set_Value<_bool>("Yetuga", "isTurnFinished", false);
					//pYetuga->Get_Controller()->Get_State_Machine()->Change_State(ENUM_CLASS(YETUGA_STATE::IDLE), pYetuga);
				}
			};
	}

	if ("RightHand_2Hit" == name)
	{
		return [pYetuga](CBlackBoard* BB, BTNODESTATE eState)
			{
				if (nullptr == BB)
					return;

				if (eState == BTNODESTATE::SUCCESS || eState == BTNODESTATE::FAILURE)
				{
					//cout << "Attack Turminate " << endl;

					BB->Set_Value<_bool>(pYetuga->Get_Name(), "IsAttack", false);
					BB->Set_Value<_bool>(pYetuga->Get_Name(), "isAttackFinished", false);
					pYetuga->Get_Controller()->Get_State_Machine()->Change_State(ENUM_CLASS(YETUGA_STATE::IDLE), pYetuga);
				}
			};
	}

	if ("RightHand_5Hit" == name)
	{
		return [pYetuga](CBlackBoard* BB, BTNODESTATE eState)
			{
				if (nullptr == BB)
					return;

				if (eState == BTNODESTATE::SUCCESS || eState == BTNODESTATE::FAILURE)
				{
					BB->Set_Value<_bool>(pYetuga->Get_Name(), "IsAttack2", false);
					BB->Set_Value<_bool>(pYetuga->Get_Name(), "isAttack2Finished", false);
					pYetuga->Get_Controller()->Get_State_Machine()->Change_State(ENUM_CLASS(YETUGA_STATE::IDLE), pYetuga);
				}
			};
	}

	if ("MoveAction" == name)
	{
		return [pYetuga](CBlackBoard* BB, BTNODESTATE eState)
			{
				if (nullptr == BB)
					return;

				if (eState == BTNODESTATE::SUCCESS || eState == BTNODESTATE::FAILURE)
				{
				}
			};
	}

	return nullptr;
}

CAI_Controller_Yetuga* CAI_Controller_Yetuga::Create(CCreature* pOwner)
{
	CAI_Controller_Yetuga* pInstance = new CAI_Controller_Yetuga();
	if (FAILED(pInstance->Initialize(pOwner)))
	{
		Safe_Release(pInstance);
		MSG_BOX(TEXT("Failed Create : CAI_Controller_Yetuga"));
	}

	return pInstance;
}

void CAI_Controller_Yetuga::Free()
{
	__super::Free();
}
