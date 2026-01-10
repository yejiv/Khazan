#include "AI_Controller_Yetuga.h"
#include "Yetuga.h"
#include "FSM_Yetuga.h"
#include "BlackBoard.h"
#include "BehaviorTree.h"
#include "GameInstance.h"
#include "Perception.h"
#include "ClientInstance.h"



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
     
    if (m_pGameInstance->Key_Down(DIK_Z))
    {

        CYetuga* pYetuga = static_cast<CYetuga*>(pOwner);
        CGameObject* pTarget = m_pBB->Get_Value<CGameObject*>(m_strMonstertag, "Target");
        pYetuga->Take_Damage(100.f, HITREACTION::KNOCKBACK_STRONG, pTarget);
        //m_pFSM->Change_State(ENUM_CLASS(YETUGA_STATE::DEAD), pYetuga);
        //pViper->Consume_Stamina(10.f);

    }


    if (m_pGameInstance->Key_Pressing(DIK_RSHIFT, fTimeDelta))
    {

        if (m_pGameInstance->Key_Down(DIK_B))
        {
            CYetuga* pYetuga = static_cast<CYetuga*>(pOwner);

            m_pFSM->Change_State(ENUM_CLASS(YETUGA_STATE::JUMPGRAB), pYetuga);

        }

        if (m_pGameInstance->Key_Down(DIK_N))
        {
            CYetuga* pYetuga = static_cast<CYetuga*>(pOwner);
            m_pFSM->Change_State(ENUM_CLASS(YETUGA_STATE::AMAGEDDON), pYetuga);

        }
        if (m_pGameInstance->Key_Down(DIK_M))
        {
            CYetuga* pYetuga = static_cast<CYetuga*>(pOwner);
            m_pFSM->Change_State(ENUM_CLASS(YETUGA_STATE::ICEBREATH), pYetuga);
        }

    }


    if (m_pGameInstance->Key_Down(DIK_J))
    {
        m_isActiveController = true;
    }
    

    if (m_isActiveController)
    {
        m_pPerception->Update(pOwner, m_pBB, fTimeDelta);
        _float fPrevTime = m_pBB->Get_Value<_float>(m_strMonstertag, "CurrentTime");

        if (m_pBB->Get_Value<_bool>(m_strMonstertag, "isDetected"))
        {
            m_pBB->Set_Value(m_strMonstertag, "CurrentTime", fPrevTime + fTimeDelta);
        }
        else
            m_pBB->Set_Value(m_strMonstertag, "CurrentTime", 0.f);

       if (!m_pBB->Get_Value<_bool>(m_strMonstertag, "isDeadFinished"))
            m_pBT->Update();
    }

	
    m_pFSM->Update(pOwner, fTimeDelta * 1.2f);

}


HRESULT CAI_Controller_Yetuga::Ready_Perception(CGameObject* pOwner, const AIPERCEPTION_DATA& Desc)
{
	m_pPerception = CPerception::Create(Desc, ENUM_CLASS(TEAM::YETI));
	if (nullptr == m_pPerception)
		return E_FAIL;

	m_pPerception->Set_PerceptionCallBack([this, pOwner, Desc](CGameObject* pTarget, const STIMULUS& Stim)
		{
			for (_uint i = 0; i < Desc.CallbackTags.size(); i++)
			{
				string strCallbackTag = Desc.CallbackTags[i];
				auto cb = GetCallBackPerception(pOwner, strCallbackTag);
				cb(pTarget, Stim);
			}
		});


	return S_OK;
}

HRESULT CAI_Controller_Yetuga::Ready_BlackBoard(CGameObject* pOwner)
{
	m_pBB = CBlackBoard::Create();
	if (nullptr == m_pBB)
		return E_FAIL;

	return S_OK;
}

HRESULT CAI_Controller_Yetuga::Ready_BehaviorTree(CGameObject* pOwner)
{
    if (nullptr == m_pBB)
        return E_FAIL;

    if (nullptr == m_pBT)
        return E_FAIL;

    m_pBT->Set_BlackBoard(m_pBB);
    CYetuga* pYetuga = static_cast<CYetuga*>(pOwner);
    m_pBT->Set_Name(pYetuga->Get_Name());
       
    return S_OK;

}


CONDITION CAI_Controller_Yetuga::GetCallbackCondition(CGameObject* pOwner, const string& name)
{
	CYetuga* pYetuga = static_cast<CYetuga*>(pOwner);
	if (nullptr == pYetuga)
		return nullptr;

#pragma region HIT SELECTOR

	if ("Dead" == name)
	{
		return [pYetuga](CBlackBoard* BB)->_bool
			{

                BB->Set_Value<_bool>(pYetuga->Get_Name(), "DamageInterrupt", false);


                if (pYetuga->Get_CurrentHP() <= 0.f)
                {
                   
                    return true;

                }
				else
					return false;
			};
	}

	else if ("Groggy" == name)
	{
		return [pYetuga](CBlackBoard* BB)->_bool
			{
				HITREACTION eHitRection = static_cast<HITREACTION>(BB->Get_Value<_uint>(pYetuga->Get_Name(), "DamageType"));
				_float fCurrentStamina = pYetuga->Get_CurrentStamina();
			
				if (fCurrentStamina <= 0.1f || BB->Get_Value<_bool>(pYetuga->Get_Name(), "isJustGuard"))
				{
					eHitRection = HITREACTION::GROGGY;
					return true;
				}
				else
					return false;
			};
	}

	/*else if ("Dodge" == name)
	{
		return [pYetuga](CBlackBoard* BB)->_bool
			{
				_float fDamageAcc = BB->Get_Value<_float>(pYetuga->Get_Name(), "DamageACC");
				_float fMaxHp = pYetuga->Get_MaxHP();
				_float fRatio = fDamageAcc / fMaxHp;

				if (fRatio >= 0.15f && !BB->Get_Value<_bool>(pYetuga->Get_Name(),"isDodge"))
					return true;
				else
					return false;
			};
	}*/

	else if ("Hit" == name)
	{
		return [pYetuga](CBlackBoard* BB) -> _bool
			{
                if (pYetuga->Get_CurrentHP() <= 0)
                    return false;

				HITREACTION eHitRection = static_cast<HITREACTION>(
					BB->Get_Value<_uint>(pYetuga->Get_Name(), "DamageType"));

				if (!BB->Get_Value<_bool>(pYetuga->Get_Name(), "isHit") &&
					(eHitRection == HITREACTION::KNOCKBACK_WEAK ||
						eHitRection == HITREACTION::KNOCKBACK_NORMAL ||
						eHitRection == HITREACTION::KNOCKBACK_STRONG))
				{
					return true;
				}

				return false;
			};
	}

#pragma endregion

#pragma region READY ATTACK SELECTOR

	

#pragma endregion


#pragma region ATTACK SELECTOR

#pragma region ETC
	
	

#pragma endregion

	else if ("IceBreath" == name)
	{
		return [pYetuga](CBlackBoard* BB)->_bool
			{

				_float fDist = BB->Get_Value<_float>(pYetuga->Get_Name(), "TargetDist");
				_float fAttackRanage = BB->Get_Value<_float>(pYetuga->Get_Name(), "AttackRange");

				_float fMaxHp = pYetuga->Get_MaxHP();
				_float fCurrentHp = pYetuga->Get_CurrentHP();

				_float fRate = fCurrentHp / fMaxHp;

				if(fRate <= 0.6f)
				{
					if (fDist != 0 && fDist <= fAttackRanage && !BB->Get_Value<_bool>(pYetuga->Get_Name(), "isIceBreath"))
					{
						return true;
					}
					else
						return false;
				}
				return false;
			};
	}


	else if ("Armageddon" == name)
	{
		return [pYetuga](CBlackBoard* BB)->_bool
			{

				_float fDist = BB->Get_Value<_float>(pYetuga->Get_Name(), "TargetDist");
				_float fAttackRanage = BB->Get_Value<_float>(pYetuga->Get_Name(), "ThrowBallRange");
				_float fChaseRange = BB->Get_Value<_float>(pYetuga->Get_Name(), "ChaseRange");

				_float fMaxHp = pYetuga->Get_MaxHP();
				_float fCurrentHp = pYetuga->Get_CurrentHP();
				_float fRate = fCurrentHp / fMaxHp;

				if (fRate <= 0.6f)
				{
					if (fDist != 0 && fDist >= fAttackRanage && fDist <= fChaseRange &&
						!BB->Get_Value<_bool>(pYetuga->Get_Name(), "IsAmageddon"))
					{
                      
						return true;
					}
					else
						return false;
				}
				return false;
				
			};
	}


	else if ("JumpGrab" == name)
	{
		return [pYetuga](CBlackBoard* BB)
			{
				_float fDist = BB->Get_Value<_float>(pYetuga->Get_Name(), "TargetDist");
				_float fAttackRanage = BB->Get_Value<_float>(pYetuga->Get_Name(), "JumpAttackRange");

				if (fDist != 0 && fDist < fAttackRanage && !BB->Get_Value<_bool>(pYetuga->Get_Name(), "isJumpGrab"))
				{
            
                
					return true;
				}
				else
					return false;
			};
	}

	else if ("Rush" == name)
	{
		return [pYetuga](CBlackBoard* BB)
			{
                _float fDist = BB->Get_Value<_float>(pYetuga->Get_Name(), "TargetDist");
                _float fJumpRange = BB->Get_Value<_float>(pYetuga->Get_Name(), "JumpAttackRange");
                _float fChasRange = BB->Get_Value<_float>(pYetuga->Get_Name(), "ChaseRange");

   				if (fDist > fJumpRange && fDist <= fChasRange)
				{
					return true;
				}
				else
					return false;
			};
	}

    else if ("LockOn" == name)
    {
        return [pYetuga](CBlackBoard* BB)->_bool
            {

                _float fDist = BB->Get_Value<_float>(pYetuga->Get_Name(), "TargetDist");
                _float fJumpRange = BB->Get_Value<_float>(pYetuga->Get_Name(), "JumpAttackRange");
                _float fChaseRange = BB->Get_Value<_float>(pYetuga->Get_Name(), "ChaseRange");

                if (fDist > fJumpRange && fDist <= fChaseRange 
                    && !BB->Get_Value<_bool>(pYetuga->Get_Name(), "isLockOn"))
                {
                    return true;
                }
                else
                    return false;
            };
    }


    else if ("StepBack" == name)
    {
        return [pYetuga](CBlackBoard* BB)->_bool
            {
                _float fDist = BB->Get_Value<_float>(pYetuga->Get_Name(), "TargetDist");
                _float fAttackRanage = BB->Get_Value<_float>(pYetuga->Get_Name(), "AttackRange");

                DIRECTION_INFO Info = {};
                Info.iDirFlag = BB->Get_Value<_uint>("Yetuga", "TargetDirection");

                if (fDist <= fAttackRanage && !BB->Get_Value<_bool>(pYetuga->Get_Name(), "isDodge"))
                {
                    DIRECTION_INFO Info = {};
                    Info.iDirFlag = BB->Get_Value<_uint>("Yetuga", "TargetDirection");
                    if (Info.Check_Flag(DIRECTION_INFO::DIR::F))
                        return true;
                    else
                        return false;
                }
                else
                    return false;

                return false;

            };
    }

	else if ("JumpAttack" == name)
	{
		return [pYetuga](CBlackBoard* BB)->_bool
			{

				_float fDist = BB->Get_Value<_float>(pYetuga->Get_Name(), "TargetDist");
				_float fAttackRanage = BB->Get_Value<_float>(pYetuga->Get_Name(), "JumpAttackRange");


				if (fDist != 0 && fDist < fAttackRanage && !BB->Get_Value<_bool>(pYetuga->Get_Name(), "isJumpAttack"))
				{
					return true;
				}
				else
					return false;
			};
	}

	else if ("ThrowBall" == name)
	{
		return [pYetuga](CBlackBoard* BB)->_bool
			{

				_float fDist = BB->Get_Value<_float>(pYetuga->Get_Name(), "TargetDist");
				_float fAttackRanage = BB->Get_Value<_float>(pYetuga->Get_Name(), "ThrowBallRange");
				_float fChasRange = BB->Get_Value<_float>(pYetuga->Get_Name(), "ChaseRange");

				if (fDist != 0 && 
					fDist <= fChasRange && fDist > fAttackRanage &&
					!BB->Get_Value<_bool>(pYetuga->Get_Name(), "IsThrowBall"))
				{
           
					return true;
				}
				else
					return false;
			};
	}


	else if ("RightHand_5Hit" == name)
		{
			return [pYetuga](CBlackBoard* BB)->_bool
				{

                    _float fDist = BB->Get_Value<_float>(pYetuga->Get_Name(), "TargetDist");
                    _float fAttackRanage = BB->Get_Value<_float>(pYetuga->Get_Name(), "AttackRange");

					if (fDist <= fAttackRanage)
					{
						return true;
					}
					else
						return false;
				};
		}


    else if ("LieDown" == name)
    {
        return [pYetuga](CBlackBoard* BB)->_bool
            {

                _float fDist = BB->Get_Value<_float>(pYetuga->Get_Name(), "TargetDist");
                _float fAttackRanage = BB->Get_Value<_float>(pYetuga->Get_Name(), "AttackRange");

                if (fDist != 0 && fDist <= fAttackRanage)
                {

                    DIRECTION_INFO Info = {};
                    Info.iDirFlag = BB->Get_Value<_uint>("Yetuga", "TargetDirection");

                    if (Info.Check_Flag(DIRECTION_INFO::DIR::B) && Info.Check_Flag(DIRECTION_INFO::DIR::L))
                        return true;
                    else if (Info.Check_Flag(DIRECTION_INFO::DIR::B))
                        return true;
                    else if (Info.Check_Flag(DIRECTION_INFO::DIR::B) && Info.Check_Flag(DIRECTION_INFO::DIR::R))
                        return true;
                }
                return false;
            };
    }


	else if ("NormalSmash" == name)
	{
		return [pYetuga](CBlackBoard* BB)->_bool
			{
				_float fDist = BB->Get_Value<_float>(pYetuga->Get_Name(), "TargetDist");
				_float fAttackRanage = BB->Get_Value<_float>(pYetuga->Get_Name(), "AttackRange");

				if (fDist != 0 && fDist < fAttackRanage /*&& !BB->Get_Value<_bool>(pYetuga->Get_Name(), "isNormalSmash")*/)
				{
					return true;
				}
				else
					return false;
			};
	}


	else if ("RightHand_2Hit" == name)
	{
		return [pYetuga](CBlackBoard* BB)->_bool
			{			

				_float fDist = BB->Get_Value<_float>(pYetuga->Get_Name(), "TargetDist");
				_float fAttackRanage = BB->Get_Value<_float>(pYetuga->Get_Name(), "AttackRange");

         
                _bool isAttackFinished = BB->Get_Value<_bool>(pYetuga->Get_Name(), "is2HitFinished");
				if ( fDist <= fAttackRanage + 5.f)
				{
					return true;
				}
				else
					return false;
			};
	}

#pragma endregion

#pragma region NONATTACK SELECTOR

	//if ("Turn" == name)
	//{
	//	return [pYetuga](CBlackBoard* BB)->_bool

	//		{
	//			if (BB->Get_Value<_bool>(pYetuga->Get_Name(), "isDead")) return false;

 //               //cout << "TURN CONDITION " << endl;

	//			_float fDot = BB->Get_Value<_float>(pYetuga->Get_Name(), "fDot");
	//			fDot = clamp(fDot, -1.f, 1.f);
	//			_float fAngle = XMConvertToDegrees(acos(fDot));
	//			_float fLimit = BB->Get_Value<_float>(pYetuga->Get_Name(), "TurnChangeRange");
 //               _float fDist = BB->Get_Value<_float>(pYetuga->Get_Name(), "TargetDist");
 //               _float fRunRange = BB->Get_Value<_float>(pYetuga->Get_Name(), "RunRange");

 //               if (fDist > fRunRange) return false;
 //               if (fDot > 0.98) return false;

	//			if (fAngle > 30.f)
	//				return true;
	//			else
	//				return false;
	//		};
	//}


    if ("Chase" == name)
    {
        return [pYetuga](CBlackBoard* BB)->_bool
            {
                _float fDist = BB->Get_Value<float>("Yetuga", "TargetDist");
                _float fChaseRange = BB->Get_Value<float>("Yetuga", "ChaseRange");
                _float fStopRange = BB->Get_Value<float>("Yetuga", "MoveStopRange");

                // 타겟 없다면 이동 불가
                if (!BB->Get_Value<_bool>(pYetuga->Get_Name(), "isDetected"))
                    return false;


                //  너무 가까우면 "이동할 필요 없음"이지만 여기서는 false를 반환해야 Attack이나 Idle로 넘어감
                if (fDist <= fStopRange)
                     return false;


                // Chase 범위 안이면 이동 OK
                return fDist <= fChaseRange;
            };
    }

#pragma endregion

	return nullptr;
}

ACTION CAI_Controller_Yetuga::GetCallbackAction(CGameObject* pOwner, const string& name)
{
	CYetuga* pYetuga = static_cast<CYetuga*>(pOwner);
	if (nullptr == pYetuga)
		return nullptr;


    if ("Dead" == name)
    {

        return [pYetuga](CBlackBoard* BB)-> BTNODESTATE
            {
                if (BB->Get_Value<_bool>(pYetuga->Get_Name(), "isDeadFinished"))
                {
                    return BTNODESTATE::SUCCESS;
                }

                BB->Set_Value(pYetuga->Get_Name(), "isDead", true);

                pYetuga->Get_Controller()->Get_State_Machine()->
                    Change_State(ENUM_CLASS(YETUGA_STATE::DEAD), pYetuga);
                return BTNODESTATE::RUNNING;
            };
    }


#pragma region HIT SELECTOR

	

	else if ("Groggy" == name)
	{
		return [pYetuga](CBlackBoard* BB)-> BTNODESTATE
			{

				if (BB->Get_Value<_bool>(pYetuga->Get_Name(), "isGroggyFinished"))
				{

                    BB->Set_Value<_bool>(pYetuga->Get_Name(), "isJustGuard", false);
					return BTNODESTATE::SUCCESS;
				}

				pYetuga->Get_Controller()->Get_State_Machine()->
					Change_State(ENUM_CLASS(YETUGA_STATE::GROGGY), pYetuga);
				return BTNODESTATE::RUNNING;

			};
	}

	//else if ("Dodge" == name)
	//{
	//	return [pYetuga](CBlackBoard* BB) -> BTNODESTATE
	//		{

	//			// 애니 종료 플래그가 true면 SUCCESS
	//			if (true == BB->Get_Value<_bool>(pYetuga->Get_Name(), "isDodgeFinished"))
	//			{
	//				return BTNODESTATE::SUCCESS;
	//			}
	//			
	//			BB->Set_Value(pYetuga->Get_Name(), "isDodge", true);
	//			BB->Set_Value(pYetuga->Get_Name(), "isDodgeFinished", false);
 //               BB->Set_Value<_bool>(pYetuga->Get_Name(), "DamageInterrupt", false);

	//			pYetuga->Get_Controller()->Get_State_Machine()->Change_State(
	//				ENUM_CLASS(YETUGA_STATE::DODGE), pYetuga);

	//			return BTNODESTATE::RUNNING;
	//		};
	//}

	else if ("Hit" == name)
	{
		return [pYetuga](CBlackBoard* BB) -> BTNODESTATE
			{

				// 애니 종료 플래그가 true면 SUCCESS
				if (true == BB->Get_Value<_bool>(pYetuga->Get_Name(), "isHitFinished"))
				{
                    BB->Set_Value(pYetuga->Get_Name(), "DamageInterrupt", false);
                    BB->Set_Value<_uint>(pYetuga->Get_Name(), "DamageType", ENUM_CLASS(HITREACTION::NONE));
					return BTNODESTATE::SUCCESS;
				}

				BB->Set_Value(pYetuga->Get_Name(), "isHit", true);
				BB->Set_Value(pYetuga->Get_Name(), "isHitFinished", false);


				pYetuga->Get_Controller()->Get_State_Machine()->Change_State(
					ENUM_CLASS(YETUGA_STATE::HIT), pYetuga);

				return BTNODESTATE::RUNNING;
			};
	}

#pragma endregion


#pragma region ATTACK READY SELECTOR

	else if ("LockOn" == name)
	{
		return [pYetuga](CBlackBoard* BB) -> BTNODESTATE
			{
				if (BB->Get_Value<_bool>(pYetuga->Get_Name(), "isLockOnFinished"))
				{
					return BTNODESTATE::SUCCESS;
				}

				BB->Set_Value(pYetuga->Get_Name(), "isLockOn", true);

				pYetuga->Get_Controller()->Get_State_Machine()->
					Change_State(ENUM_CLASS(YETUGA_STATE::LOCKON), pYetuga);

				return BTNODESTATE::RUNNING;
			};
	}


#pragma endregion

#pragma region ATTACK SELECTOR

#pragma region ETC

	

#pragma endregion

	else if ("IceBreath" == name)
	{
		return [pYetuga](CBlackBoard* BB)-> BTNODESTATE
			{

				if (BB->Get_Value<_bool>(pYetuga->Get_Name(), "isIceBreathFinished"))
				{
                    BB->Set_Value<_bool>(pYetuga->Get_Name(), "AttackInterrupt", true);

					return BTNODESTATE::SUCCESS;
				}

				BB->Set_Value(pYetuga->Get_Name(), "isIceBreath", true);
				BB->Set_Value(pYetuga->Get_Name(), "isIceBreathFinished", false);
				BB->Set_Value(pYetuga->Get_Name(), "isSuperArmor", true);
                BB->Set_Value<_bool>(pYetuga->Get_Name(), "AttackInterrupt", true);


				pYetuga->Get_Controller()->Get_State_Machine()->
					Change_State(ENUM_CLASS(YETUGA_STATE::ICEBREATH), pYetuga);
				return BTNODESTATE::RUNNING;

			};
	}


	else if ("Armageddon" == name)
	{
		return [pYetuga](CBlackBoard* BB)-> BTNODESTATE
			{

				if (BB->Get_Value<_bool>(pYetuga->Get_Name(), "isAmageddonFinished"))
				{

                    BB->Set_Value<_bool>(pYetuga->Get_Name(), "AttackInterrupt", true);

					return BTNODESTATE::SUCCESS;
				}

				BB->Set_Value(pYetuga->Get_Name(), "isAmageddon", true);
				BB->Set_Value(pYetuga->Get_Name(), "isAmageddonFinished", false);
                BB->Set_Value(pYetuga->Get_Name(), "isSuperArmor", true);
                BB->Set_Value<_bool>(pYetuga->Get_Name(), "AttackInterrupt", true);


				pYetuga->Get_Controller()->Get_State_Machine()->
					Change_State(ENUM_CLASS(YETUGA_STATE::AMAGEDDON), pYetuga);
				return BTNODESTATE::RUNNING;

			};
	}

	else if ("JumpGrab" == name)
	{
		return [pYetuga](CBlackBoard* BB) -> BTNODESTATE
			{
				if (BB->Get_Value<_bool>(pYetuga->Get_Name(), "isJumpGrabFinished"))
				{
					return BTNODESTATE::SUCCESS;
				}

				BB->Set_Value(pYetuga->Get_Name(), "isJumpGrabFinished", false);
				BB->Set_Value(pYetuga->Get_Name(), "isJumpGrab", true);
                BB->Set_Value(pYetuga->Get_Name(), "isSuperArmor", true);


				pYetuga->Get_Controller()->Get_State_Machine()->
					Change_State(ENUM_CLASS(YETUGA_STATE::JUMPGRAB), pYetuga);

				return BTNODESTATE::RUNNING;
			};
	}

	else if ("Rush" == name)
	{
		return [pYetuga](CBlackBoard* BB)
			{
				if (BB->Get_Value<_bool>(pYetuga->Get_Name(), "isRushFinished"))
				{
                    BB->Set_Value<_bool>(pYetuga->Get_Name(), "AttackInterrupt", false);
					return BTNODESTATE::SUCCESS;
				}

                BB->Set_Value<_bool>(pYetuga->Get_Name(), "AttackInterrupt", true);
                BB->Set_Value(pYetuga->Get_Name(), "isSuperArmor", true);


				pYetuga->Get_Controller()->Get_State_Machine()->
					Change_State(ENUM_CLASS(YETUGA_STATE::RUSH), pYetuga);

				return BTNODESTATE::RUNNING;
			};
	}


    else if ("StepBack" == name)
    {
        return [pYetuga](CBlackBoard* BB) -> BTNODESTATE
            {
                if (BB->Get_Value<_bool>(pYetuga->Get_Name(), "isDodgeFinished"))
                {
                    return BTNODESTATE::SUCCESS;
                }

                BB->Set_Value(pYetuga->Get_Name(), "isDodgeFinished", false);
                BB->Set_Value(pYetuga->Get_Name(), "isDodge", true);
                BB->Set_Value(pYetuga->Get_Name(), "isSuperArmor", true);

                pYetuga->Get_Controller()->Get_State_Machine()->
                    Change_State(ENUM_CLASS(YETUGA_STATE::DODGE), pYetuga);

                return BTNODESTATE::RUNNING;
            };
            }


	else if ("JumpAttack" == name)
	{
		return [pYetuga](CBlackBoard* BB) -> BTNODESTATE
			{

				if (BB->Get_Value<_bool>(pYetuga->Get_Name(), "isJumpAttackFinished"))
				{
                    BB->Set_Value<_bool>(pYetuga->Get_Name(), "AttackInterrupt", false);
					return BTNODESTATE::SUCCESS;
				}

                BB->Set_Value<_bool>(pYetuga->Get_Name(), "AttackInterrupt", true);
				BB->Set_Value(pYetuga->Get_Name(), "isJumpAttackFinished", false);
				BB->Set_Value(pYetuga->Get_Name(), "isJumpAttack", true);
                BB->Set_Value(pYetuga->Get_Name(), "isSuperArmor", true);



				pYetuga->Get_Controller()->Get_State_Machine()->
					Change_State(ENUM_CLASS(YETUGA_STATE::JUMPATTACK), pYetuga);

				return BTNODESTATE::RUNNING;
			};
	}


	else if ("ThrowBall" == name)
	{
		return [pYetuga](CBlackBoard* BB)-> BTNODESTATE
			{

				if (BB->Get_Value<_bool>(pYetuga->Get_Name(), "isThrowBallFinished"))
				{
                    BB->Set_Value<_bool>(pYetuga->Get_Name(), "AttackInterrupt", false);
					return BTNODESTATE::SUCCESS;
				}

				BB->Set_Value(pYetuga->Get_Name(), "isThrowBall", true);
				BB->Set_Value(pYetuga->Get_Name(), "isThrowBallFinished", false);
                BB->Set_Value(pYetuga->Get_Name(), "isSuperArmor", true);
                BB->Set_Value<_bool>(pYetuga->Get_Name(), "AttackInterrupt", true);


				pYetuga->Get_Controller()->Get_State_Machine()->
					Change_State(ENUM_CLASS(YETUGA_STATE::THROWBALL), pYetuga);
				return BTNODESTATE::RUNNING;

			};
	}


	
	else if ("RightHand_5Hit" == name)
	{
		return [pYetuga](CBlackBoard* BB)-> BTNODESTATE
			{
				if (BB->Get_Value<_bool>(pYetuga->Get_Name(), "isAttack2Finished"))
				{
					return BTNODESTATE::SUCCESS;
				}

               
                _bool isAbort = BB->Get_Value<_bool>(pYetuga->Get_Name(), "isAbort");
                if (isAbort)
                {
                    BB->Set_Value(pYetuga->Get_Name(), "isSuperArmor", false);
                    BB->Set_Value(pYetuga->Get_Name(), "isAttack2Finished", false);
                    BB->Set_Value(pYetuga->Get_Name(), "isAttack2", false);

                    return BTNODESTATE::SUCCESS;

                }


				BB->Set_Value(pYetuga->Get_Name(), "isAttack2", true);
				BB->Set_Value(pYetuga->Get_Name(), "isAttack2Finished", false);
                BB->Set_Value(pYetuga->Get_Name(), "isSuperArmor", true);

				pYetuga->Get_Controller()->Get_State_Machine()->
					Change_State(ENUM_CLASS(YETUGA_STATE::RIGHTHAND_5HIT), pYetuga);
				return BTNODESTATE::RUNNING;

			};
		}

    else if ("LieDown" == name)
    {
        return [pYetuga](CBlackBoard* BB)-> BTNODESTATE
            {

                if (BB->Get_Value<_bool>(pYetuga->Get_Name(), "isAttackFinished3"))
                {
                    return BTNODESTATE::SUCCESS;
                }

                BB->Set_Value(pYetuga->Get_Name(), "isAttack3", true);
                BB->Set_Value(pYetuga->Get_Name(), "isAttackFinished3", false);
                BB->Set_Value(pYetuga->Get_Name(), "isSuperArmor", true);

                pYetuga->Get_Controller()->Get_State_Machine()->
                    Change_State(ENUM_CLASS(YETUGA_STATE::LIE_DOWN), pYetuga);
                return BTNODESTATE::RUNNING;
            };
        }


	else if ("NormalSmash" == name)
	{
		return [pYetuga](CBlackBoard* BB) -> BTNODESTATE
			{
				if (BB->Get_Value<_bool>(pYetuga->Get_Name(), "isNormalSmashFinished"))
				{
					return BTNODESTATE::SUCCESS;
				}

				BB->Set_Value(pYetuga->Get_Name(), "isNormalSmashFinished", false);
				BB->Set_Value(pYetuga->Get_Name(), "isNormalSmash", true);
                BB->Set_Value(pYetuga->Get_Name(), "isSuperArmor", true);

				pYetuga->Get_Controller()->Get_State_Machine()->
					Change_State(ENUM_CLASS(YETUGA_STATE::SMASH), pYetuga);

				return BTNODESTATE::RUNNING;
			};
	}



	else if ("RightHand_2Hit" == name)
	{
		return [pYetuga](CBlackBoard* BB)-> BTNODESTATE
			{
				if (BB->Get_Value<_bool>(pYetuga->Get_Name(), "is2HitFinished"))
				{
					return BTNODESTATE::SUCCESS;
				}

                BB->Set_Value(pYetuga->Get_Name(), "is2Hit", true);
                BB->Set_Value(pYetuga->Get_Name(), "isSuperArmor", true);

				pYetuga->Get_Controller()->Get_State_Machine()->
					Change_State(ENUM_CLASS(YETUGA_STATE::ATTACK), pYetuga);

				return BTNODESTATE::RUNNING;
			};
	}


#pragma endregion

#pragma region NONATTACK SELECTOR

	//if ("Turn" == name)
	//{
	//	return [pYetuga](CBlackBoard* BB)->BTNODESTATE
	//		{

	//			_bool isDamaged = BB->Get_Value<_bool>(pYetuga->Get_Name(), "DamageInterrupt");
	//			if (isDamaged)
	//				return BTNODESTATE::SUCCESS;
	//			
	//			if (BB->Get_Value<_bool>(pYetuga->Get_Name(), "isTurnFinished"))
	//			{
	//				return BTNODESTATE::SUCCESS;

	//			}
	//			pYetuga->Get_Controller()->Get_State_Machine()->
	//				Change_State(ENUM_CLASS(YETUGA_STATE::TURN), pYetuga);

 //               //cout << "TURN ACTION " << endl;


	//			return BTNODESTATE::RUNNING;
	//		};
	//}


    if ("Chase" == name)
    {
        return [pYetuga](CBlackBoard* BB) -> BTNODESTATE
            {

                _float fDist = BB->Get_Value<_float>(pYetuga->Get_Name(), "TargetDist");
                _float fStopRange = BB->Get_Value<_float>(pYetuga->Get_Name(), "MoveStopRange");
                _float fChaseRange = BB->Get_Value<_float>(pYetuga->Get_Name(), "ChaseRange");
                _float fRunRange = BB->Get_Value<_float>(pYetuga->Get_Name(), "RunRange");
                _float fSprintRange = BB->Get_Value<_float>(pYetuga->Get_Name(), "SprintRange");

                BB->Set_Value(pYetuga->Get_Name(), "isSuperArmor", false);


                if (BB->Get_Value<_bool>(pYetuga->Get_Name(), "AttackInterrupt"))
                    return BTNODESTATE::SUCCESS;
            

                if (fDist <= fStopRange)
                    return BTNODESTATE::SUCCESS;

                if (!BB->Get_Value<_bool>(pYetuga->Get_Name(), "isDetected"))
                    return BTNODESTATE::FAILURE;


                if (fDist > fChaseRange)
                    return BTNODESTATE::FAILURE;

               // 이동 타입 결정
                CYetuga::MONSTER_INFO Info{};
                Info.Clear_State();

                if (fDist <= fRunRange)
                    Info.Add_State(Info.WALK);
                else if (fDist <= fSprintRange)
                    Info.Add_State(Info.RUN);
                else
                    Info.Add_State(Info.SPRINT);

                BB->Set_Value<uint>(pYetuga->Get_Name(), "iMovementFlag", Info.iStateFlag);

                
                pYetuga->Get_Controller()->Get_State_Machine()->Change_State(
                    ENUM_CLASS(YETUGA_STATE::MOVE), pYetuga);

                return BTNODESTATE::RUNNING;
            };
    }


	if ("Idle" == name)
	{
		return [pYetuga](CBlackBoard* BB)->BTNODESTATE
			{

				if (BB->Get_Value<_bool>(pYetuga->Get_Name(), "isDead")) return BTNODESTATE::FAILURE;
				if (BB->Get_Value<_bool>(pYetuga->Get_Name(), "isDetected")) return BTNODESTATE::FAILURE;
				_bool isDamaged = BB->Get_Value<_bool>(pYetuga->Get_Name(), "DamageInterrupt");
                _float fDist = BB->Get_Value<_float>(pYetuga->Get_Name(), "TargetDist");
                _float fStopRange = BB->Get_Value<_float>(pYetuga->Get_Name(), "MoveStopRange");
                
                if (isDamaged)
                    return BTNODESTATE::FAILURE;

                if (fDist >= fStopRange)
                    return BTNODESTATE::FAILURE;

				pYetuga->Get_Controller()->Get_State_Machine()->Change_State(ENUM_CLASS(YETUGA_STATE::IDLE), pYetuga);
				return BTNODESTATE::RUNNING;
			};
	}
#pragma endregion

	return nullptr;
}

TERMINATE CAI_Controller_Yetuga::GetCallbackTeminate(CGameObject* pOwner, const string& name)
{
	CYetuga* pYetuga = static_cast<CYetuga*>(pOwner);
	if (nullptr == pYetuga)
		return nullptr;

    if ("Dead" == name)
    {
        return [pYetuga](CBlackBoard* BB, BTNODESTATE eState)
            {
                if (nullptr == BB)
                    return;



                if (eState == BTNODESTATE::SUCCESS || eState == BTNODESTATE::FAILURE)
                {
                    
                    //BB->Set_Value<_bool>(pYetuga->Get_Name(), "isDeadFinished", false);
                    BB->Set_Value<_bool>(pYetuga->Get_Name(), "DamageInterrupt", false);
                    BB->Set_Value<_bool>(pYetuga->Get_Name(), "isDead", false);
                }
            };
    }

#pragma region HIT SELECTOR

	else if ("Groggy" == name)
	{
		return [pYetuga](CBlackBoard* BB, BTNODESTATE eState)
			{
				if (nullptr == BB)
					return;

				if (eState == BTNODESTATE::SUCCESS || eState == BTNODESTATE::FAILURE)
				{
                    BB->Set_Value<_bool>(pYetuga->Get_Name(), "isGroggy", false);
					BB->Set_Value<_bool>(pYetuga->Get_Name(), "isGroggyFinished", false);
				}
			};
	}
	//else if ("Dodge" == name)
	//{
	//	return [pYetuga](CBlackBoard* BB, BTNODESTATE eState)
	//		{
	//			if (nullptr == BB)
	//				return;

	//			if (eState == BTNODESTATE::SUCCESS || eState == BTNODESTATE::FAILURE)
	//			{

	//				BB->Set_Value<_bool>(pYetuga->Get_Name(), "isDodge", false);
	//			
	//			}
	//		};
	//}



	else if ("Hit" == name)
	{
		return [pYetuga](CBlackBoard* BB, BTNODESTATE eState)
			{
				if (nullptr == BB)
					return;

				if (eState == BTNODESTATE::SUCCESS || eState == BTNODESTATE::FAILURE)
				{
					BB->Set_Value<_bool>(pYetuga->Get_Name(), "isHit", false);
					BB->Set_Value<_bool>(pYetuga->Get_Name(), "isHitFinished", false);
					BB->Set_Value<_bool>(pYetuga->Get_Name(), "DamageInterrupt", false);
				}
			};
	}
#pragma endregion

#pragma region ATTACK SELECTOR

	else if ("IceBreath" == name)
	{
		return [pYetuga](CBlackBoard* BB, BTNODESTATE eState)
			{
				if (nullptr == BB)
					return;

				if (eState == BTNODESTATE::SUCCESS || eState == BTNODESTATE::FAILURE)
				{
					BB->Set_Value<_bool>(pYetuga->Get_Name(), "isIceBreath", false);
					BB->Set_Value<_bool>(pYetuga->Get_Name(), "isIceBreathFinished", false);
					BB->Set_Value<_bool>(pYetuga->Get_Name(), "AttackInterrupt", false);
                    BB->Set_Value(pYetuga->Get_Name(), "isSuperArmor", false);
                    BB->Set_Value<_bool>(pYetuga->Get_Name(), "isHit", false);
                    BB->Set_Value<_bool>(pYetuga->Get_Name(), "isHitFinished", false);
				}
			};
	}


	else if ("Armageddon" == name)
	{
		return [pYetuga](CBlackBoard* BB, BTNODESTATE eState)
			{
				if (nullptr == BB)
					return;

				if (eState == BTNODESTATE::SUCCESS || eState == BTNODESTATE::FAILURE)
				{

					BB->Set_Value<_bool>(pYetuga->Get_Name(), "isAmageddon", false);
					BB->Set_Value<_bool>(pYetuga->Get_Name(), "isAmageddonFinished", false);
                    BB->Set_Value(pYetuga->Get_Name(), "isSuperArmor", false);
                    BB->Set_Value<_bool>(pYetuga->Get_Name(), "isHit", false);
                    BB->Set_Value<_bool>(pYetuga->Get_Name(), "isHitFinished", false);
				}
			};
	}

	else if ("JumpGrab" == name)
	{
		return [pYetuga](CBlackBoard* BB, BTNODESTATE eState)
			{
				if (nullptr == BB)
					return;

				if (eState == BTNODESTATE::SUCCESS || eState == BTNODESTATE::FAILURE)
				{
					BB->Set_Value<_bool>(pYetuga->Get_Name(), "isJumpGrab", false);
					BB->Set_Value<_bool>(pYetuga->Get_Name(), "isJumpGrabFinished", false);
                    BB->Set_Value(pYetuga->Get_Name(), "isSuperArmor", false);
                    BB->Set_Value<_bool>(pYetuga->Get_Name(), "isHit", false);
                    BB->Set_Value<_bool>(pYetuga->Get_Name(), "isHitFinished", false);
				}
			};
	}

	else if ("Rush" == name)
	{
		return [pYetuga](CBlackBoard* BB, BTNODESTATE eState)
			{
				if (nullptr == BB)
					return;

				if (eState == BTNODESTATE::SUCCESS || eState == BTNODESTATE::FAILURE)
				{
					BB->Set_Value<_bool>(pYetuga->Get_Name(), "isRush", false);
					BB->Set_Value<_bool>(pYetuga->Get_Name(), "isRushFinished", false);
                    BB->Set_Value(pYetuga->Get_Name(), "isSuperArmor", false);
                    BB->Set_Value<_bool>(pYetuga->Get_Name(), "isHit", false);
                    BB->Set_Value<_bool>(pYetuga->Get_Name(), "isHitFinished", false);
				}
			};
	}

    else if ("LockOn" == name)
    {
    	return [pYetuga](CBlackBoard* BB, BTNODESTATE eState)
    		{
    			if (nullptr == BB)
    				return;

    			if (eState == BTNODESTATE::SUCCESS || eState == BTNODESTATE::FAILURE)
    			{

    				BB->Set_Value<_bool>(pYetuga->Get_Name(), "isLockOn", false);
    				BB->Set_Value<_bool>(pYetuga->Get_Name(), "isLockOnFinished", false);
                    BB->Set_Value<_bool>(pYetuga->Get_Name(), "isHit", false);
                    BB->Set_Value<_bool>(pYetuga->Get_Name(), "isHitFinished", false);
    			}
    	};
    }

    else if ("StepBack" == name)
    {
        return [pYetuga](CBlackBoard* BB, BTNODESTATE eState)
            {
                if (nullptr == BB)
                    return;

                if (eState == BTNODESTATE::SUCCESS || eState == BTNODESTATE::FAILURE)
                {

                    BB->Set_Value<_bool>(pYetuga->Get_Name(), "isDodge", false);
                    BB->Set_Value<_bool>(pYetuga->Get_Name(), "isDodgeFinished", false);
                    BB->Set_Value<_bool>(pYetuga->Get_Name(), "isHit", false);
                    BB->Set_Value<_bool>(pYetuga->Get_Name(), "isHitFinished", false);
                    BB->Set_Value(pYetuga->Get_Name(), "isSuperArmor", false);

                }
            };
    }


	else if ("JumpAttack" == name)
	{
		return [pYetuga](CBlackBoard* BB, BTNODESTATE eState)
			{
				if (nullptr == BB)
					return;

				if (eState == BTNODESTATE::SUCCESS || eState == BTNODESTATE::FAILURE)
				{
					BB->Set_Value<_bool>(pYetuga->Get_Name(), "isJumpAttack", false);
					BB->Set_Value<_bool>(pYetuga->Get_Name(), "isJumpAttackFinished", false);
                    BB->Set_Value<_bool>(pYetuga->Get_Name(), "isHit", false);
                    BB->Set_Value<_bool>(pYetuga->Get_Name(), "isHitFinished", false);
                    BB->Set_Value<_uint>(pYetuga->Get_Name(), "DamageType", ENUM_CLASS(HITREACTION::NONE));
                    BB->Set_Value(pYetuga->Get_Name(), "isSuperArmor", false);

				}
			};
	}


	else if ("ThrowBall" == name)
	{
		return [pYetuga](CBlackBoard* BB, BTNODESTATE eState)
			{
				if (nullptr == BB)
					return;

				if (eState == BTNODESTATE::SUCCESS || eState == BTNODESTATE::FAILURE)
				{
					BB->Set_Value<_bool>(pYetuga->Get_Name(), "isThrowBall", false);
					BB->Set_Value<_bool>(pYetuga->Get_Name(), "isThrowBallFinished", false);
                    BB->Set_Value<_bool>(pYetuga->Get_Name(), "isHit", false);
                    BB->Set_Value<_bool>(pYetuga->Get_Name(), "isHitFinished", false);
                    BB->Set_Value(pYetuga->Get_Name(), "isSuperArmor", false);


				}
			};
	}

	else if ("LieDown" == name)
	{
		return [pYetuga](CBlackBoard* BB, BTNODESTATE eState)
			{
				if (nullptr == BB)
					return;

				if (eState == BTNODESTATE::SUCCESS || eState == BTNODESTATE::FAILURE)
				{
					BB->Set_Value<_bool>(pYetuga->Get_Name(), "IsAttack3", false);
					BB->Set_Value<_bool>(pYetuga->Get_Name(), "isAttackFinished3", false);
                    BB->Set_Value<_bool>(pYetuga->Get_Name(), "isHit", false);
                    BB->Set_Value<_bool>(pYetuga->Get_Name(), "isHitFinished", false);
                    BB->Set_Value(pYetuga->Get_Name(), "isSuperArmor", false);

				}
			};
	}


	else if ("RightHand_5Hit" == name)
	{
		return [pYetuga](CBlackBoard* BB, BTNODESTATE eState)
			{
				if (nullptr == BB)
					return;

				if (eState == BTNODESTATE::SUCCESS || eState == BTNODESTATE::FAILURE)
				{
					BB->Set_Value<_bool>(pYetuga->Get_Name(), "IsAttack2", false);
					BB->Set_Value<_bool>(pYetuga->Get_Name(), "isAttack2Finished", false);
                    BB->Set_Value<_bool>(pYetuga->Get_Name(), "isHit", false);
                    BB->Set_Value<_bool>(pYetuga->Get_Name(), "isHitFinished", false);
                    BB->Set_Value(pYetuga->Get_Name(), "isSuperArmor", false);


				}
			};
		}

	else if ("NormalSmash" == name)
	{
		return [pYetuga](CBlackBoard* BB, BTNODESTATE eState)
			{
				if (nullptr == BB)
					return;

			if (eState == BTNODESTATE::SUCCESS || eState == BTNODESTATE::FAILURE)
			{
				BB->Set_Value<_bool>(pYetuga->Get_Name(), "isNormalSmash", false);
				BB->Set_Value<_bool>(pYetuga->Get_Name(), "isNormalSmashFinished", false);
                BB->Set_Value<_bool>(pYetuga->Get_Name(), "isHit", false);
                BB->Set_Value<_bool>(pYetuga->Get_Name(), "isHitFinished", false);
                BB->Set_Value(pYetuga->Get_Name(), "isSuperArmor", false);


			}
		};
	}

	else if ("RightHand_2Hit" == name)
	{
		return [pYetuga](CBlackBoard* BB, BTNODESTATE eState)
			{
				if (nullptr == BB)
					return;

				if (eState == BTNODESTATE::SUCCESS || eState == BTNODESTATE::FAILURE)
				{
					BB->Set_Value<_bool>(pYetuga->Get_Name(), "is2Hit", false);
					BB->Set_Value<_bool>(pYetuga->Get_Name(), "is2HitFinished", false);
                    BB->Set_Value<_bool>(pYetuga->Get_Name(), "isHit", false);
                    BB->Set_Value<_bool>(pYetuga->Get_Name(), "isHitFinished", false);
                    BB->Set_Value(pYetuga->Get_Name(), "isSuperArmor", false);

				}
			};
	}


#pragma endregion

#pragma region NONATTACK SELECTOR

	//else if ("Turn" == name)
	//{
	//	return [pYetuga](CBlackBoard* BB, BTNODESTATE eState)
	//		{

 //               //cout << "TURN TERMINATE " << endl;


	//			if (nullptr == BB)
	//				return;

	//			if (eState == BTNODESTATE::SUCCESS || eState == BTNODESTATE::FAILURE)
	//			{
	//				BB->Set_Value<_bool>("Yetuga", "isTurnFinished", false);
	//			}
	//		};
	//		}


	else if ("Chase" == name)
	{
		return [pYetuga](CBlackBoard* BB, BTNODESTATE eState)
			{

              //  cout << "MOVE TERMINATE " << endl;

				if (nullptr == BB)
					return;

				if (eState == BTNODESTATE::SUCCESS || eState == BTNODESTATE::FAILURE)
				{
                    BB->Set_Value<_uint>(pYetuga->Get_Name(), "iMovementFlag", 0);
                    BB->Set_Value<_bool>(pYetuga->Get_Name(), "isHit", false);
                    BB->Set_Value<_bool>(pYetuga->Get_Name(), "isHitFinished", false);
				}
			};
	}

#pragma endregion

	return nullptr;
}

INTERRUPTCONDITION CAI_Controller_Yetuga::GetCallbackInterruptCondition(CGameObject* pOwner, const string& name)
{
	CYetuga* pYetuga = static_cast<CYetuga*>(pOwner);

	if ("Root_Interrupt" == name)
	{
		return [pYetuga](CBlackBoard* BB)
			{
                if (BB->Get_Value<_bool>(pYetuga->Get_Name(), "isDead"))
                    return true;
                if (BB->Get_Value<_bool>(pYetuga->Get_Name(), "isGroggy"))
                    return true;
                if (BB->Get_Value<_bool>(pYetuga->Get_Name(), "DamageInterrupt"))
                {
                    _bool isSuperArmor = BB->Get_Value<_bool>(pYetuga->Get_Name(), "isSuperArmor");
                    if (isSuperArmor)
                        return false;

                    return true;

                }
                return false;
			};
	}

	return nullptr;
}

PERCEPTIONCALLBACK CAI_Controller_Yetuga::GetCallBackPerception(CGameObject* pOwner, const string& name)
{
	
	if (name == "Target")
	{
		return [this](CGameObject* pTarget, const STIMULUS& Stim)
			{
				if (Stim.eType == SENSETYPE::SIGHT)
				{
					if (Stim.bSensed)
					{
						m_pBB->Set_Value("Yetuga", "Target", pTarget);
						m_pBB->Set_Value("Yetuga", "isDetected", true);
					}
				}
			};
	}

	else if (name == "DamageInterrupt")
	{
		return[this](CGameObject* pTarget, const STIMULUS& Stim) 
		{
			if (Stim.eType == SENSETYPE::DAMAGE)
			{
				
				if (Stim.bSensed)
				{
    				m_pBB->Set_Value<_uint>("Yetuga", "DamageType", Stim.iDamageType);
					m_pBB->Set_Value("Yetuga", "DamageInterrupt", true);
					m_pBB->Set_Value("Yetuga", "DamageACC", m_pPerception->Get_DamageAcc());
				}
				else
				{
					m_pBB->Set_Value("Yetuga", "DamageACC", m_pPerception->Get_DamageAcc());
				}
			}
		};
	};
    
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
