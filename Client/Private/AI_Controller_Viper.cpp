#include "AI_Controller_Viper.h"
#include "Viper.h"
#include "Perception.h"
#include "BlackBoard.h"
#include "BehaviorTree.h"
#include "FSM_Viper.h"
#include "UtilityScore.h"
#include "GameInstance.h"


CAI_Controller_Viper::CAI_Controller_Viper()
{
}

HRESULT CAI_Controller_Viper::Initialize(CCreature* pOwner)
{
    CViper* pViper = static_cast<CViper*>(pOwner);

    if (FAILED(__super::Initialize(pViper, pViper->Get_Name())))
        return E_FAIL;

    m_pFSM = CFSM_Viper::Create();
    if (nullptr == m_pFSM)
        return E_FAIL;

    return S_OK;
}

void CAI_Controller_Viper::Update(CGameObject* pOwner, _float fTimeDelta)
{
    // 누르면 컷신 모드 AS_CutScene_Start
    if (m_pGameInstance->Key_Down(DIK_T))
    {
        CViper* pViper = static_cast<CViper*>(pOwner);
        //CGameObject* pTarget = m_pBB->Get_Value<CGameObject*>(m_strMonstertag, "Target");
        ////pViper->Take_Damage(10.f,HITREACTION::KNOCKBACK_WEAK,pTarget);
        //pViper->Consume_Stamina(10.f);
        
        m_pFSM->Change_State(ENUM_CLASS(VIPER_STATE_P1::CUTSCENE_START),pViper);

    }

    if (m_pGameInstance->Key_Down(DIK_Y))
    {

        CViper* pViper = static_cast<CViper*>(pOwner);
        m_pFSM->Change_State(ENUM_CLASS(VIPER_STATE_P1::CUTSCENE_PHASE2), pViper);

    }

    if (m_pGameInstance->Key_Down(DIK_Z))
    {

        CViper* pViper = static_cast<CViper*>(pOwner);
        CGameObject* pTarget = m_pBB->Get_Value<CGameObject*>(m_strMonstertag, "Target");
        //pViper->Take_Damage(10.f,HITREACTION::KNOCKBACK_WEAK,pTarget);
        //pViper->Take_Damage(10.f,HITREACTION::KNOCKBACK_WEAK,pTarget);
         pViper->Consume_Stamina(10.f);

    }




    if(m_pGameInstance->Key_Down(DIK_J))
        Set_ActiveAIController(true); // 이거 하면 실행됩니다.

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

        m_pBT->Update();
    }

    m_pFSM->Update(pOwner, fTimeDelta);
}

HRESULT CAI_Controller_Viper::Ready_Perception(CGameObject* pOwner, const AIPERCEPTION_DATA& Desc)
{
    m_pPerception = CPerception::Create(Desc,ENUM_CLASS(TEAM::DRAGONIAN));
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

HRESULT CAI_Controller_Viper::Ready_BlackBoard(CGameObject* pOwner)
{
    m_pBB = CBlackBoard::Create();
    if (nullptr == m_pBB)
        return E_FAIL;

    return S_OK;
}

HRESULT CAI_Controller_Viper::Ready_BehaviorTree(CGameObject* pOwner)
{
    if (nullptr == m_pBB)
        return E_FAIL;

    if (nullptr == m_pBT)
        return E_FAIL;

    m_pBT->Set_BlackBoard(m_pBB);
    CViper* pViper = static_cast<CViper*>(pOwner);
    m_pBT->Set_Name(pViper->Get_Name());

    return S_OK;
}

CONDITION CAI_Controller_Viper::GetCallbackCondition(CGameObject* pOwner, const string& name)
{
    CViper* pViper = static_cast<CViper*>(pOwner);
    if (nullptr == pViper)
        return nullptr;

#pragma region DAMAGED

    if ("Dead" == name)
    {
        return [pViper](CBlackBoard* BB)->_bool
            {

                BB->Set_Value<_bool>(pViper->Get_Name(), "DamageInterrupt", false);


                if (pViper->Get_CurrentHP() <= 0.f)
                {
                   /* static_cast<CUI_Inven*>(CClientInstance::GetInstance()->Get_RootUI(TEXT("Inven")))->Add_Item(1001);
                    static_cast<CUI_Inven*>(CClientInstance::GetInstance()->Get_RootUI(TEXT("Inven")))->Add_Item(1002);
                    static_cast<CUI_Inven*>(CClientInstance::GetInstance()->Get_RootUI(TEXT("Inven")))->Add_Item(1003);
                    static_cast<CAmount*>(CClientInstance::GetInstance()->Get_RootUI(TEXT("Amount")))->Add_Value(CAmount::AMOUNT_TYPE::GOLD, 1000);*/

                    return true;

                }
                else
                    return false;
            };
    }

    else if ("Groggy" == name)
    {
    	return [pViper](CBlackBoard* BB)->_bool
    		{
    			HITREACTION eHitRection = static_cast<HITREACTION>(BB->Get_Value<_uint>(pViper->Get_Name(), "DamageType"));
    			_float fCurrentStamina = pViper->Get_CurrentStamina();
    		
    			if (fCurrentStamina <= 0.1f)
    			{
    				eHitRection = HITREACTION::GROGGY;
    				return true;
    			}
    			else
    				return false;
    		};
    }


    else if ("Hit" == name)
    {
        return [pViper](CBlackBoard* BB) -> _bool
            {
                if (pViper->Get_CurrentHP() <= 0)
                    return false;

                HITREACTION eHitRection = static_cast<HITREACTION>(
                    BB->Get_Value<_uint>(pViper->Get_Name(), "DamageType"));

                if (!BB->Get_Value<_bool>(pViper->Get_Name(), "isHit") &&
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


#pragma region PHASE

    else if ("Phase1" == name)
    {
        return [pViper](CBlackBoard* BB) -> _bool
            {
                
                if(CViper::PHASE::PHASE1 == pViper->Get_Phase())
                    return true;

                return false;
            };
    }

#pragma endregion


#pragma region COMBAT


    else if ("P1_StingGrab" == name)
    {
        return [pViper, this](CBlackBoard* BB)->_bool
            {
                _float fHpRatio = pViper->Get_CurrentHP() / pViper->Get_MaxHP();
                if (fHpRatio >= 0.4f)
                    return false;

               
                _float fDist = BB->Get_Value<_float>(pViper->Get_Name(), "TargetDist");
                _float fJumpRange = BB->Get_Value<_float>(pViper->Get_Name(), "JumpAttackRange");

                _bool isAttackFinished = BB->Get_Value<_bool>(pViper->Get_Name(), "isP1_StingGrabFinished");
                if (fDist != 0 && fDist <= fJumpRange)
                {
                    return true;
                }
                else
                    return false;
            };
    }



    else if ("P1_5HitCombo" == name)
    {
        return [pViper, this](CBlackBoard* BB)->_bool
            {
                _float fHpRatio = pViper->Get_CurrentHP() / pViper->Get_MaxHP();
                if (fHpRatio >= 0.4f)
                    return false;
                // HP가 60퍼 밑으로 내려가면
                _float fDiffScale = (0.4f - fHpRatio);
                // 최소 확률
                _float fMinChance = 0.15f;
                _float fChance = fMinChance + fDiffScale * 0.4f; // 최소 확률 + 확률 보정치
                // fDiffScale 이 커질 수록 확률이 증가 하게된다.

                // 여기다가 확률 추가
                if (m_pGameInstance->Rand(0, 1) > fChance)
                    return false;

                _float fDist = BB->Get_Value<_float>(pViper->Get_Name(), "TargetDist");
                _float fAttackRange = BB->Get_Value<_float>(pViper->Get_Name(), "AttackRange");

                _bool isAttackFinished = BB->Get_Value<_bool>(pViper->Get_Name(), "isP1_5HitComboFinished");
                if (fDist != 0 && fDist <= fAttackRange)
                {
                    return true;
                }
                else
                    return false;
            };
    }



    else if ("P1_SideMove" == name)
    {
        return [pViper, this](CBlackBoard* BB)->_bool
            {
                _float fHpRatio = pViper->Get_CurrentHP() / pViper->Get_MaxHP();
                if (fHpRatio >= 0.6f)
                    return false;
                // HP가 60퍼 밑으로 내려가면
                _float fDiffScale = (0.6f - fHpRatio);
                // 최소 확률
                _float fMinChance = 0.15f;
                _float fChance = fMinChance + fDiffScale * 0.6f; // 최소 확률 + 확률 보정치
                // fDiffScale 이 커질 수록 확률이 증가 하게된다.

                // 여기다가 확률 추가
                if (m_pGameInstance->Rand(0, 1) > fChance)
                    return false;

                _float fDist = BB->Get_Value<_float>(pViper->Get_Name(), "TargetDist");
                //_float fDivourRange = BB->Get_Value<_float>(pViper->Get_Name(), "SideMoveRange");
                _float fAttackRange = BB->Get_Value<_float>(pViper->Get_Name(), "AttackRange");

                _bool isAttackFinished = BB->Get_Value<_bool>(pViper->Get_Name(), "isP1_SideMoveFinished");
                if (fDist != 0 && fDist <= fAttackRange + 10.f)
                {
                    return true;
                }
                else
                    return false;
            };
    }


    else if ("P1_Devour" == name)
    {
        return [pViper, this](CBlackBoard* BB)->_bool
            {
                _float fHpRatio = pViper->Get_CurrentHP() / pViper->Get_MaxHP();
                if (fHpRatio >= 0.6f)
                    return false;
                // HP가 60퍼 밑으로 내려가면
                _float fDiffScale = (0.6f - fHpRatio);
                // 최소 확률
                _float fMinChance = 0.15f;
                _float fChance = fMinChance + fDiffScale * 0.6f; // 최소 확률 + 확률 보정치
                // fDiffScale 이 커질 수록 확률이 증가 하게된다.

                // 여기다가 확률 추가
                if (m_pGameInstance->Rand(0, 1) > fChance)
                    return false;

                _float fDist = BB->Get_Value<_float>(pViper->Get_Name(), "TargetDist");
                _float fDivourRange = BB->Get_Value<_float>(pViper->Get_Name(), "DevourRange");

                _bool isAttackFinished = BB->Get_Value<_bool>(pViper->Get_Name(), "isP1_DevourFinished");
                if (fDist != 0 && fDist <= fDivourRange + 10.f)
                {
                    return true;
                }
                else
                    return false;
            };
    }



    else if ("P1_JumpSmash" == name)
    {
        return [pViper, this](CBlackBoard* BB)->_bool
            {
                _float fHpRatio = pViper->Get_CurrentHP() / pViper->Get_MaxHP();
                if (fHpRatio >= 0.6f)
                    return false;
                // HP가 60퍼 밑으로 내려가면
                _float fDiffScale = (0.6f - fHpRatio);
                // 최소 확률
                _float fMinChance = 0.15f;
                _float fChance = fMinChance + fDiffScale * 0.6f; // 최소 확률 + 확률 보정치
                // fDiffScale 이 커질 수록 확률이 증가 하게된다.

                // 여기다가 확률 추가
                if (m_pGameInstance->Rand(0, 1) > fChance)
                    return false;

                _float fDist = BB->Get_Value<_float>(pViper->Get_Name(), "TargetDist");
                _float fRunRange = BB->Get_Value<_float>(pViper->Get_Name(), "RunRange");

                _bool isAttackFinished = BB->Get_Value<_bool>(pViper->Get_Name(), "isP1_JumpSmashFinished");
                if (fDist != 0 && fDist <= fRunRange + 10.f)
                {
                    return true;
                }
                else
                    return false;
            };
    }




    else if ("P1_LockOn" == name)
    {
        return [pViper](CBlackBoard* BB)->_bool
            {

                _float fDist = BB->Get_Value<_float>(pViper->Get_Name(), "TargetDist");
                _float fAttackRange = BB->Get_Value<_float>(pViper->Get_Name(), "AttackRange");
                _float fRockOnRange = BB->Get_Value<_float>(pViper->Get_Name(), "LockOnRange");
                _float fChaseRange = BB->Get_Value<_float>(pViper->Get_Name(), "ChaseRange");

                if (fDist > fAttackRange + 0.5f && fDist <= fRockOnRange
                    && !BB->Get_Value<_bool>(pViper->Get_Name(), "isP1_LockOn_Finished"))
                {
                    return true;
                }
                else
                    return false;
            };
    }

    else if ("P1_TurnAttack" == name)
    {
        return [pViper, this](CBlackBoard* BB)->_bool
            {
                _float fHpRatio = pViper->Get_CurrentHP() / pViper->Get_MaxHP();
                if (fHpRatio >= 0.6f)
                    return false;
               
                _float fDist = BB->Get_Value<_float>(pViper->Get_Name(), "TargetDist");
                _float fAttackRange = BB->Get_Value<_float>(pViper->Get_Name(), "AttackRange");

                _bool isAttackFinished = BB->Get_Value<_bool>(pViper->Get_Name(), "isP1_TurnAttackFinished");
                if (fDist != 0 && fDist <= fAttackRange + 10.f)
                {
                    return true;
                }
                else
                    return false;
            };
    }

    else if ("P1_SlashBackJump" == name)
    {
        return [pViper,this](CBlackBoard* BB)->_bool
            {
                _float fHpRatio = pViper->Get_CurrentHP() / pViper->Get_MaxHP();
                if (fHpRatio >= 0.6f)
                    return false;
                // HP가 60퍼 밑으로 내려가면
                _float fDiffScale = (0.6f - fHpRatio);
                // 최소 확률
                _float fMinChance = 0.15f;
                _float fChance = fMinChance + fDiffScale * 0.6f; // 최소 확률 + 확률 보정치
                // fDiffScale 이 커질 수록 확률이 증가 하게된다.
                
                // 여기다가 확률 추가
                if (m_pGameInstance->Rand(0, 1) > fChance)
                    return false;

                _float fDist = BB->Get_Value<_float>(pViper->Get_Name(), "TargetDist");
                _float fAttackRange = BB->Get_Value<_float>(pViper->Get_Name(), "AttackRange");

                _bool isAttackFinished = BB->Get_Value<_bool>(pViper->Get_Name(), "isP1_SlashBackJumpFinished");
                if (fDist != 0 && fDist <= fAttackRange)
                {
                    return true;
                }
                else
                    return false;
            };
    }

    else if ("P1_Slow3Hit" == name)
    {
        return [pViper](CBlackBoard* BB)->_bool
            {
                 _float fHpRatio = pViper->Get_CurrentHP() / pViper->Get_MaxHP();
                if (fHpRatio >= 0.6f)
                    return false;


                _float fDist = BB->Get_Value<_float>(pViper->Get_Name(), "TargetDist");
                _float fAttackRange = BB->Get_Value<_float>(pViper->Get_Name(), "AttackRange");

                _bool isAttackFinished = BB->Get_Value<_bool>(pViper->Get_Name(), "isP1_Slow3HitFinished");
                if (fDist != 0 && fDist <= fAttackRange + 10.f)
                {
                    return true;
                }
                else
                    return false;
            };
    }

    else if ("P1_ThrowBlade" == name)
    {
        return [pViper](CBlackBoard* BB)->_bool
            {

                _float fDist = BB->Get_Value<_float>(pViper->Get_Name(), "TargetDist");
                _float fAttackRange = BB->Get_Value<_float>(pViper->Get_Name(), "AttackRange");
                _float fThrowRange = BB->Get_Value<_float>(pViper->Get_Name(), "ThorwRange");

                _bool isAttackFinished = BB->Get_Value<_bool>(pViper->Get_Name(), "isP1_ThrowBladeFinished");
                if (fDist <= fThrowRange  && fDist >= fAttackRange)
                {
                    BB->Set_Value<_bool>(pViper->Get_Name(), "AttackInterrupt", true);
                    return true;
                }
                else
                    return false;
            };
            }


    else if ("P1_StingSlash" == name)
    {
        return [pViper](CBlackBoard* BB)->_bool
            {

                _float fDist = BB->Get_Value<_float>(pViper->Get_Name(), "TargetDist");
                _float fAttackRange = BB->Get_Value<_float>(pViper->Get_Name(), "AttackRange");

                _bool isAttackFinished = BB->Get_Value<_bool>(pViper->Get_Name(), "isP1_StingSlashFinished");
                if (fDist != 0 && fDist <= fAttackRange)
                {
                    return true;
                }
                else
                    return false;
            };
    }

    else if ("P1_Slow2Hit" == name)
    {
        return [pViper](CBlackBoard* BB)->_bool
            {

                _float fDist = BB->Get_Value<_float>(pViper->Get_Name(), "TargetDist");
                _float fAttackRange = BB->Get_Value<_float>(pViper->Get_Name(), "AttackRange");

                _bool isAttackFinished = BB->Get_Value<_bool>(pViper->Get_Name(), "isP1_Slow2HitFinished");
                if (fDist != 0 && fDist <= fAttackRange + 10.f)
                {
                    return true;
                }
                else
                    return false;
            };
    }

    else if ("P1_Quick2Hit" == name)
    {
        return [pViper](CBlackBoard* BB)->_bool
            {

                _float fDist = BB->Get_Value<_float>(pViper->Get_Name(), "TargetDist");
                _float fAttackRanage = BB->Get_Value<_float>(pViper->Get_Name(), "AttackRange");


                _bool isAttackFinished = BB->Get_Value<_bool>(pViper->Get_Name(), "P1_Quick2HitFinished");
                if (fDist != 0 && fDist <= fAttackRanage + 5.f)
                {
                    return true;
                }
                else
                    return false;
            };
    }

#pragma endregion

#pragma region NONCOMBAT

    else if ("P1_Run" == name)
    {
        return [pViper](CBlackBoard* BB) ->_bool
            {
             
                _float fDist = BB->Get_Value<_float>(pViper->Get_Name(), "TargetDist");
                _float fRunRange = BB->Get_Value<_float>(pViper->Get_Name(), "RunRange");
                _float fAttackRange = BB->Get_Value<_float>(pViper->Get_Name(), "AttackRange");
                _uint iStepCnt = BB->Get_Value<_uint>(pViper->Get_Name(), "WalkStepCount");

                if ((fDist >= fAttackRange && fDist <= fRunRange) || iStepCnt >= 4)
                    return true;

                return false;
            };
    }

    else if ("P1_Walk" == name)
    {
        return [pViper](CBlackBoard* BB) ->_bool
            {
                _float fDist = BB->Get_Value<_float>(pViper->Get_Name(), "TargetDist");
                _float fChaseRange = BB->Get_Value<_float>(pViper->Get_Name(), "ChaseRange");

                if (fDist < fChaseRange)
                    return true;
                
                return false;
            };
    }

#pragma endregion


    return nullptr;
}

ACTION CAI_Controller_Viper::GetCallbackAction(CGameObject* pOwner, const string& name)
{
    CViper* pViper = static_cast<CViper*>(pOwner);
    if (nullptr == pViper)
        return nullptr;


#pragma region DAMAGED

    if ("Dead" == name)
    {

        return [pViper](CBlackBoard* BB)-> BTNODESTATE
            {
                if (BB->Get_Value<_bool>(pViper->Get_Name(), "isDeadFinished"))
                {
                    return BTNODESTATE::SUCCESS;
                }

                BB->Set_Value(pViper->Get_Name(), "isDead", true);

                pViper->Get_Controller()->Get_State_Machine()->
                    Change_State(ENUM_CLASS(VIPER_STATE_P1::DEAD), pViper);
                return BTNODESTATE::RUNNING;
            };
    }

    else if ("Groggy" == name)
    {
    	return [pViper](CBlackBoard* BB)-> BTNODESTATE
    		{

    			if (BB->Get_Value<_bool>(pViper->Get_Name(), "isGroggyFinished"))
    			{
    				return BTNODESTATE::SUCCESS;
    			}

    			BB->Set_Value(pViper->Get_Name(), "isGroggy", false);

    			pViper->Get_Controller()->Get_State_Machine()->
    				Change_State(ENUM_CLASS(VIPER_STATE_P1::GROGGY), pViper);
    			return BTNODESTATE::RUNNING;

    		};
    }


    else if ("Hit" == name)
    {
        return [pViper](CBlackBoard* BB) -> BTNODESTATE
            {
                
                if (true == BB->Get_Value<_bool>(pViper->Get_Name(), "isHitFinished"))
                {
                    BB->Set_Value(pViper->Get_Name(), "DamageInterrupt", false);
                    return BTNODESTATE::SUCCESS;
                }

                BB->Set_Value(pViper->Get_Name(), "isHit", true);
                BB->Set_Value(pViper->Get_Name(), "isHitFinished", false);

                pViper->Get_Controller()->Get_State_Machine()->Change_State(
                    ENUM_CLASS(VIPER_STATE_P1::HIT), pViper);

                return BTNODESTATE::RUNNING;
            };
    }

#pragma endregion

#pragma region COMBAT_ACTIONS


    else if ("P1_StingGrab" == name)
    {

        return [pViper](CBlackBoard* BB)-> BTNODESTATE
            {

                if (BB->Get_Value<_bool>(pViper->Get_Name(), "isGroggy"))
                    return BTNODESTATE::FAILURE;


                if (BB->Get_Value<_bool>(pViper->Get_Name(), "isP1_StingGrabFinished"))
                {
                    return BTNODESTATE::SUCCESS;
                }

                BB->Set_Value(pViper->Get_Name(), "isSuperArmor", true);

                pViper->Get_Controller()->Get_State_Machine()->
                    Change_State(ENUM_CLASS(VIPER_STATE_P1::STINGGRAB), pViper);

                return BTNODESTATE::RUNNING;
            };
    }


    else if ("P1_5HitCombo" == name)
    {

        return [pViper](CBlackBoard* BB)-> BTNODESTATE
            {

                if (BB->Get_Value<_bool>(pViper->Get_Name(), "isGroggy"))
                    return BTNODESTATE::FAILURE;

                if (BB->Get_Value<_bool>(pViper->Get_Name(), "isP1_5HitComboFinished"))
                {
                    return BTNODESTATE::SUCCESS;
                }

                BB->Set_Value(pViper->Get_Name(), "isSuperArmor", true);

                pViper->Get_Controller()->Get_State_Machine()->
                    Change_State(ENUM_CLASS(VIPER_STATE_P1::COMBO5HIT), pViper);

                return BTNODESTATE::RUNNING;
            };
    }


    else if ("P1_SideMove" == name)
    {

        return [pViper](CBlackBoard* BB)-> BTNODESTATE
            {
                
                if (BB->Get_Value<_bool>(pViper->Get_Name(), "isGroggy"))
                    return BTNODESTATE::FAILURE;

                if (BB->Get_Value<_bool>(pViper->Get_Name(), "isP1_SideMoveFinished"))
                {
                    return BTNODESTATE::SUCCESS;
                }

                BB->Set_Value(pViper->Get_Name(), "isSuperArmor", true);

                pViper->Get_Controller()->Get_State_Machine()->
                    Change_State(ENUM_CLASS(VIPER_STATE_P1::SIDEMOVE), pViper);

                return BTNODESTATE::RUNNING;
            };
    }

    else if ("P1_Devour" == name)
    {
        return [pViper](CBlackBoard* BB)-> BTNODESTATE
            {

                if (BB->Get_Value<_bool>(pViper->Get_Name(), "isGroggy"))
                    return BTNODESTATE::FAILURE;

                if (BB->Get_Value<_bool>(pViper->Get_Name(), "isP1_DevourFinished"))
                {
                    return BTNODESTATE::SUCCESS;
                }

                BB->Set_Value(pViper->Get_Name(), "isSuperArmor", true);

                pViper->Get_Controller()->Get_State_Machine()->
                    Change_State(ENUM_CLASS(VIPER_STATE_P1::DIVOUR), pViper);

                return BTNODESTATE::RUNNING;
            };
    }

    else if ("P1_JumpSmash" == name)
    {
        return [pViper](CBlackBoard* BB)-> BTNODESTATE
            {

                if (BB->Get_Value<_bool>(pViper->Get_Name(), "isGroggy"))
                    return BTNODESTATE::FAILURE;

                if (BB->Get_Value<_bool>(pViper->Get_Name(), "isP1_JumpSmashFinished"))
                {
                    return BTNODESTATE::SUCCESS;
                }

                BB->Set_Value(pViper->Get_Name(), "isSuperArmor", true);

                pViper->Get_Controller()->Get_State_Machine()->
                    Change_State(ENUM_CLASS(VIPER_STATE_P1::JUMPSMASH), pViper);

                return BTNODESTATE::RUNNING;
            };
    }

    else if ("P1_LockOn" == name)
    {

        return [pViper](CBlackBoard* BB) ->BTNODESTATE
            {

                if (BB->Get_Value<_bool>(pViper->Get_Name(), "isGroggy"))
                    return BTNODESTATE::FAILURE;

                if (BB->Get_Value<_bool>(pViper->Get_Name(), "isP1_LockOn_Finished"))
                {
                    return BTNODESTATE::SUCCESS;
                }

                pViper->Get_Controller()->Get_State_Machine()->Change_State(ENUM_CLASS(VIPER_STATE_P1::LOCKON),pViper);
                return BTNODESTATE::RUNNING;


            };

    }

    else if ("P1_TurnAttack" == name)
    {
        return [pViper](CBlackBoard* BB)-> BTNODESTATE
            {
                if (BB->Get_Value<_bool>(pViper->Get_Name(), "isGroggy"))
                    return BTNODESTATE::FAILURE;


                if (BB->Get_Value<_bool>(pViper->Get_Name(), "isP1_TurnAttackFinished"))
                {
                    return BTNODESTATE::SUCCESS;
                }

                BB->Set_Value(pViper->Get_Name(), "isSuperArmor", true);

                pViper->Get_Controller()->Get_State_Machine()->
                    Change_State(ENUM_CLASS(VIPER_STATE_P1::TURNATTACK), pViper);

                return BTNODESTATE::RUNNING;
            };
    }





    else if ("P1_SlashBackJump" == name)
    {
        return [pViper](CBlackBoard* BB)-> BTNODESTATE
            {

                if (BB->Get_Value<_bool>(pViper->Get_Name(), "isGroggy"))
                    return BTNODESTATE::FAILURE;

                if (BB->Get_Value<_bool>(pViper->Get_Name(), "isP1_SlashBackJumpFinished"))
                {
                    return BTNODESTATE::SUCCESS;
                }

                BB->Set_Value(pViper->Get_Name(), "isSuperArmor", true);

                pViper->Get_Controller()->Get_State_Machine()->
                    Change_State(ENUM_CLASS(VIPER_STATE_P1::SLASHBACKJUMP), pViper);

                return BTNODESTATE::RUNNING;
            };
    }




    else if ("P1_Slow3Hit" == name)
    {
        return [pViper](CBlackBoard* BB)-> BTNODESTATE
            {

                if (BB->Get_Value<_bool>(pViper->Get_Name(), "isGroggy"))
                    return BTNODESTATE::FAILURE;

                if (BB->Get_Value<_bool>(pViper->Get_Name(), "isP1_Slow3HitFinished"))
                {
                    return BTNODESTATE::SUCCESS;
                }

                BB->Set_Value(pViper->Get_Name(), "isSuperArmor", true);

                pViper->Get_Controller()->Get_State_Machine()->
                    Change_State(ENUM_CLASS(VIPER_STATE_P1::SLOW3HIT), pViper);

                return BTNODESTATE::RUNNING;
            };
    }



    else if ("P1_ThrowBlade" == name)
    {
        return [pViper](CBlackBoard* BB)-> BTNODESTATE
            {

                if (BB->Get_Value<_bool>(pViper->Get_Name(), "isGroggy"))
                    return BTNODESTATE::FAILURE;

                if (BB->Get_Value<_bool>(pViper->Get_Name(), "isP1_ThrowBladeFinished"))
                {
                    return BTNODESTATE::SUCCESS;
                }

                BB->Set_Value(pViper->Get_Name(), "isSuperArmor", true);

                pViper->Get_Controller()->Get_State_Machine()->
                    Change_State(ENUM_CLASS(VIPER_STATE_P1::THROWBLADE), pViper);

                return BTNODESTATE::RUNNING;
            };
            }




    else if ("P1_StingSlash" == name)
    {

        return [pViper](CBlackBoard* BB)-> BTNODESTATE
            {

                if (BB->Get_Value<_bool>(pViper->Get_Name(), "isGroggy"))
                    return BTNODESTATE::FAILURE;

                if (BB->Get_Value<_bool>(pViper->Get_Name(), "isP1_StingSlashFinished"))
                {
                    return BTNODESTATE::SUCCESS;
                }

                BB->Set_Value(pViper->Get_Name(), "isSuperArmor", true);

                pViper->Get_Controller()->Get_State_Machine()->
                    Change_State(ENUM_CLASS(VIPER_STATE_P1::STINGSLASHCOMBO), pViper);

                return BTNODESTATE::RUNNING;
            };
    }



    else if ("P1_Slow2Hit" == name)
    {
        return [pViper](CBlackBoard* BB)-> BTNODESTATE
            {
                if (BB->Get_Value<_bool>(pViper->Get_Name(), "isGroggy"))
                    return BTNODESTATE::FAILURE;


                if (BB->Get_Value<_bool>(pViper->Get_Name(), "isP1_Slow2HitFinished"))
                {
                    return BTNODESTATE::SUCCESS;
                }

                BB->Set_Value(pViper->Get_Name(), "isSuperArmor", true);

                pViper->Get_Controller()->Get_State_Machine()->
                    Change_State(ENUM_CLASS(VIPER_STATE_P1::SLOW2HIT), pViper);

                return BTNODESTATE::RUNNING;
            };
    }



    else if ("P1_Quick2Hit" == name)
    {
        return [pViper](CBlackBoard* BB)-> BTNODESTATE
            {

                if (BB->Get_Value<_bool>(pViper->Get_Name(), "isGroggy"))
                    return BTNODESTATE::FAILURE;

                if (BB->Get_Value<_bool>(pViper->Get_Name(), "isP1_Quick2HitFinished"))
                {
                    return BTNODESTATE::SUCCESS;
                }

                BB->Set_Value(pViper->Get_Name(), "isSuperArmor", true);

                pViper->Get_Controller()->Get_State_Machine()->
                    Change_State(ENUM_CLASS(VIPER_STATE_P1::QUICK2HIT), pViper);

                return BTNODESTATE::RUNNING;
            };
    }


#pragma endregion

#pragma region NONCOMBAT

    else if ("P1_Walk" == name)
    {
        return [pViper](CBlackBoard* BB) ->BTNODESTATE
            {
                _bool isGroggy = BB->Get_Value<_bool>(pViper->Get_Name(), "isGroggy");
                if (isGroggy) 
                    return BTNODESTATE::FAILURE;

                _float fDist = BB->Get_Value<_float>(pViper->Get_Name(), "TargetDist");
                _float fWalkRange = BB->Get_Value<_float>(pViper->Get_Name(), "WalkRange");
                _uint iStepCnt =  BB->Get_Value<_uint>(pViper->Get_Name(), "WalkStepCount");

             
                if (iStepCnt >= 4)
                {
                    pViper->Get_Controller()->Get_State_Machine()->Change_State(
                        ENUM_CLASS(VIPER_STATE_P1::RUN), pViper);
                    return BTNODESTATE::SUCCESS;
                }

                if (fDist <= fWalkRange)
                    return BTNODESTATE::SUCCESS;

                pViper->Get_Controller()->Get_State_Machine()->Change_State(
                    ENUM_CLASS(VIPER_STATE_P1::WALK), pViper);

                return BTNODESTATE::RUNNING;

            };
    }

    else if ("P1_Run" == name)
    {
        return [pViper](CBlackBoard* BB) ->BTNODESTATE
            {

                _bool isGroggy = BB->Get_Value<_bool>(pViper->Get_Name(), "isGroggy");
                if (isGroggy) 
                    return BTNODESTATE::FAILURE;


                _float fDist = BB->Get_Value<_float>(pViper->Get_Name(), "TargetDist");
                _float fAttackRange = BB->Get_Value<_float>(pViper->Get_Name(), "AttackRange");

                if (fDist < fAttackRange)
                    return BTNODESTATE::SUCCESS;
               
                pViper->Get_Controller()->Get_State_Machine()->Change_State(
                    ENUM_CLASS(VIPER_STATE_P1::RUN), pViper);

                BB->Set_Value<_uint>(pViper->Get_Name(), "WalkStepCount", 0);
                return BTNODESTATE::RUNNING;


            };
    }




    else if ("Idle" == name)
    {
        return [pViper](CBlackBoard* BB)->BTNODESTATE
            {

                if (BB->Get_Value<_bool>(pViper->Get_Name(), "isDead")) return BTNODESTATE::FAILURE;
                if (BB->Get_Value<_bool>(pViper->Get_Name(), "isDetected")) return BTNODESTATE::FAILURE;
                _bool isDamaged = BB->Get_Value<_bool>(pViper->Get_Name(), "DamageInterrupt");
                _float fDist = BB->Get_Value<_float>(pViper->Get_Name(), "TargetDist");
                _float fStopRange = BB->Get_Value<_float>(pViper->Get_Name(), "MoveStopRange");

                if (isDamaged)
                    return BTNODESTATE::FAILURE;

                if (fDist >= fStopRange)
                    return BTNODESTATE::FAILURE;

                pViper->Get_Controller()->Get_State_Machine()->Change_State(ENUM_CLASS(VIPER_STATE_P1::IDLE), pViper);
                return BTNODESTATE::RUNNING;
            };
    }



#pragma endregion


    return nullptr;
}

TERMINATE CAI_Controller_Viper::GetCallbackTeminate(CGameObject* pOwner, const string& name)
{
    CViper* pViper = static_cast<CViper*>(pOwner);
    if (nullptr == pViper)
        return nullptr;

#pragma region DAMAGED

    if ("Dead" == name)
    {
        return [pViper](CBlackBoard* BB, BTNODESTATE eState)
            {
                if (nullptr == BB)
                    return;

                if (eState == BTNODESTATE::SUCCESS || eState == BTNODESTATE::FAILURE)
                {
                    BB->Set_Value<_bool>(pViper->Get_Name(), "DamageInterrupt", false);
                    BB->Set_Value<_bool>(pViper->Get_Name(), "isDead", false);
                }
            };
    }

    else if ("Groggy" == name)
    {
        return [pViper](CBlackBoard* BB, BTNODESTATE eState)
            {
                if (nullptr == BB)
                    return;

                if (eState == BTNODESTATE::SUCCESS || eState == BTNODESTATE::FAILURE)
                {
                    BB->Set_Value<_bool>(pViper->Get_Name(), "isGroggy", false);
                    BB->Set_Value<_bool>(pViper->Get_Name(), "isGroggyFinished", false);
                    //BB->Set_Value<_uint>(pViper->Get_Name(), "DamageType", ENUM_CLASS(HITREACTION::NONE));
                }
            };
    }



    else if ("Hit" == name)
    {
        return [pViper](CBlackBoard* BB, BTNODESTATE eState)
            {
                if (nullptr == BB)
                    return;

                if (eState == BTNODESTATE::SUCCESS || eState == BTNODESTATE::FAILURE)
                {
                    BB->Set_Value<_bool>(pViper->Get_Name(), "isHit", false);
                    BB->Set_Value<_bool>(pViper->Get_Name(), "isHitFinished", false);
                    BB->Set_Value<_bool>(pViper->Get_Name(), "DamageInterrupt", false);
                    BB->Set_Value<_uint>(pViper->Get_Name(), "DamageType", ENUM_CLASS(HITREACTION::NONE));
                }
            };
    }

#pragma endregion

#pragma region COMBAT_TERMINATES
    
    else if ("P1_StingGrab" == name)
    {
        return [pViper](CBlackBoard* BB, BTNODESTATE eState)
            {
                if (nullptr == BB)
                    return;

                if (eState == BTNODESTATE::SUCCESS || eState == BTNODESTATE::FAILURE)
                {
                    BB->Set_Value<_bool>(pViper->Get_Name(), "isP1_StingGrabFinished", false);
                    BB->Set_Value(pViper->Get_Name(), "isSuperArmor", false);
                    BB->Set_Value<_bool>(pViper->Get_Name(), "isHit", false);
                    BB->Set_Value<_bool>(pViper->Get_Name(), "isHitFinished", false);
                    BB->Set_Value<_uint>(pViper->Get_Name(), "DamageType", ENUM_CLASS(HITREACTION::NONE));
                }
            };
    }


    else if ("P1_5HitCombo" == name)
    {
        return [pViper](CBlackBoard* BB, BTNODESTATE eState)
            {
                if (nullptr == BB)
                    return;

                if (eState == BTNODESTATE::SUCCESS || eState == BTNODESTATE::FAILURE)
                {
                    BB->Set_Value<_bool>(pViper->Get_Name(), "isP1_SideMoveFinished", false);
                    BB->Set_Value(pViper->Get_Name(), "isSuperArmor", false);
                    BB->Set_Value<_bool>(pViper->Get_Name(), "isHit", false);
                    BB->Set_Value<_bool>(pViper->Get_Name(), "isHitFinished", false);
                    BB->Set_Value<_uint>(pViper->Get_Name(), "DamageType", ENUM_CLASS(HITREACTION::NONE));
                }
            };
    }


    else if ("P1_SideMove" == name)
    {
        return [pViper](CBlackBoard* BB, BTNODESTATE eState)
            {
                if (nullptr == BB)
                    return;

                if (eState == BTNODESTATE::SUCCESS || eState == BTNODESTATE::FAILURE)
                {
                    BB->Set_Value<_bool>(pViper->Get_Name(), "isP1_SideMoveFinished", false);
                    BB->Set_Value(pViper->Get_Name(), "isSuperArmor", false);
                    BB->Set_Value<_bool>(pViper->Get_Name(), "isHit", false);
                    BB->Set_Value<_bool>(pViper->Get_Name(), "isHitFinished", false);
                    BB->Set_Value<_uint>(pViper->Get_Name(), "DamageType", ENUM_CLASS(HITREACTION::NONE));
                }
            };
    }


    else if ("P1_Devour" == name)
    {
        return [pViper](CBlackBoard* BB, BTNODESTATE eState)
            {
                if (nullptr == BB)
                    return;

                if (eState == BTNODESTATE::SUCCESS || eState == BTNODESTATE::FAILURE)
                {
                    BB->Set_Value<_bool>(pViper->Get_Name(), "isP1_DevourFinished", false);
                    BB->Set_Value(pViper->Get_Name(), "isSuperArmor", false);
                    BB->Set_Value<_bool>(pViper->Get_Name(), "isHit", false);
                    BB->Set_Value<_bool>(pViper->Get_Name(), "isHitFinished", false);
                    BB->Set_Value<_uint>(pViper->Get_Name(), "DamageType", ENUM_CLASS(HITREACTION::NONE));
                }
            };
    }


    else if ("P1_JumpSmash" == name)
    {
        return [pViper](CBlackBoard* BB, BTNODESTATE eState)
            {
                if (nullptr == BB)
                    return;

                if (eState == BTNODESTATE::SUCCESS || eState == BTNODESTATE::FAILURE)
                {
                    BB->Set_Value<_bool>(pViper->Get_Name(), "isP1_JumpSmashFinished", false);
                    BB->Set_Value(pViper->Get_Name(), "isSuperArmor", false);
                    BB->Set_Value<_bool>(pViper->Get_Name(), "isHit", false);
                    BB->Set_Value<_bool>(pViper->Get_Name(), "isHitFinished", false);
                    BB->Set_Value<_uint>(pViper->Get_Name(), "DamageType", ENUM_CLASS(HITREACTION::NONE));
                }
            };
    }



    else if ("P1_TurnAttack" == name)
    {
        return [pViper](CBlackBoard* BB, BTNODESTATE eState)
            {
                if (nullptr == BB)
                    return;

                if (eState == BTNODESTATE::SUCCESS || eState == BTNODESTATE::FAILURE)
                {
                    BB->Set_Value<_bool>(pViper->Get_Name(), "isP1_TurnAttackFinished", false);
                    BB->Set_Value(pViper->Get_Name(), "isSuperArmor", false);
                    BB->Set_Value<_bool>(pViper->Get_Name(), "isHit", false);
                    BB->Set_Value<_bool>(pViper->Get_Name(), "isHitFinished", false);
                    BB->Set_Value<_uint>(pViper->Get_Name(), "DamageType", ENUM_CLASS(HITREACTION::NONE));
                }
            };
    }

 
    else if ("P1_SlashBackJump" == name)
    {
        return [pViper](CBlackBoard* BB, BTNODESTATE eState)
            {
                if (nullptr == BB)
                    return;

                if (eState == BTNODESTATE::SUCCESS || eState == BTNODESTATE::FAILURE)
                {
                    BB->Set_Value<_bool>(pViper->Get_Name(), "isP1_SlashBackJumpFinished", false);
                    BB->Set_Value(pViper->Get_Name(), "isSuperArmor", false);
                    BB->Set_Value<_bool>(pViper->Get_Name(), "isHit", false);
                    BB->Set_Value<_bool>(pViper->Get_Name(), "isHitFinished", false);
                    BB->Set_Value<_uint>(pViper->Get_Name(), "DamageType", ENUM_CLASS(HITREACTION::NONE));
                }
            };
    }

    else if ("P1_Slow3Hit" == name)
    {
        return [pViper](CBlackBoard* BB, BTNODESTATE eState)
            {
                if (nullptr == BB)
                    return;

                if (eState == BTNODESTATE::SUCCESS || eState == BTNODESTATE::FAILURE)
                {
                    BB->Set_Value<_bool>(pViper->Get_Name(), "isP1_Slow3HitFinished", false);
                    BB->Set_Value(pViper->Get_Name(), "isSuperArmor", false);
                    BB->Set_Value<_bool>(pViper->Get_Name(), "isHit", false);
                    BB->Set_Value<_bool>(pViper->Get_Name(), "isHitFinished", false);
                    BB->Set_Value<_uint>(pViper->Get_Name(), "DamageType", ENUM_CLASS(HITREACTION::NONE));
                }
            };
    }


    else if ("P1_LockOn" == name)
    {
        return [pViper, this](CBlackBoard* BB, BTNODESTATE eState)
            {
                if (nullptr == BB) return;

                if (eState == BTNODESTATE::SUCCESS || eState == BTNODESTATE::FAILURE)
                {
                    BB->Set_Value<_bool>(pViper->Get_Name(), "isP1_LockOn_Finished", false);
                    BB->Set_Value(pViper->Get_Name(), "isSuperArmor", false);
                    BB->Set_Value<_bool>(pViper->Get_Name(), "isHit", false);
                    BB->Set_Value<_bool>(pViper->Get_Name(), "isHitFinished", false);
                    BB->Set_Value<_uint>(pViper->Get_Name(), "DamageType", ENUM_CLASS(HITREACTION::NONE));
                }
            };
    }


    else if ("P1_ThrowBlade" == name)
    {
        return [pViper](CBlackBoard* BB, BTNODESTATE eState)
            {
                if (nullptr == BB)
                    return;

                if (eState == BTNODESTATE::SUCCESS || eState == BTNODESTATE::FAILURE)
                {
                    BB->Set_Value<_bool>(pViper->Get_Name(), "isP1_ThrowBladeFinished", false);
                    BB->Set_Value<_bool>(pViper->Get_Name(), "AttackInterrupt", false);
                    BB->Set_Value(pViper->Get_Name(), "isSuperArmor", false);
                    BB->Set_Value<_bool>(pViper->Get_Name(), "isHit", false);
                    BB->Set_Value<_bool>(pViper->Get_Name(), "isHitFinished", false);
                    BB->Set_Value<_uint>(pViper->Get_Name(), "DamageType", ENUM_CLASS(HITREACTION::NONE));
                }
            };
            }

    else if ("P1_StingSlash" == name)
    {
        return [pViper](CBlackBoard* BB, BTNODESTATE eState)
            {
                if (nullptr == BB)
                    return;

                if (eState == BTNODESTATE::SUCCESS || eState == BTNODESTATE::FAILURE)
                {
                    BB->Set_Value<_bool>(pViper->Get_Name(), "isP1_StingSlashFinished", false);
                    BB->Set_Value(pViper->Get_Name(), "isSuperArmor", false);
                    BB->Set_Value<_bool>(pViper->Get_Name(), "isHit", false);
                    BB->Set_Value<_bool>(pViper->Get_Name(), "isHitFinished", false);
                    BB->Set_Value<_uint>(pViper->Get_Name(), "DamageType", ENUM_CLASS(HITREACTION::NONE));
                }
            };
    }



    else if ("P1_Slow2Hit" == name)
    {
        return [pViper](CBlackBoard* BB, BTNODESTATE eState)
            {
                if (nullptr == BB)
                    return;

                if (eState == BTNODESTATE::SUCCESS || eState == BTNODESTATE::FAILURE)
                {
                    BB->Set_Value<_bool>(pViper->Get_Name(), "isP1_Slow2HitFinished", false);
                    BB->Set_Value(pViper->Get_Name(), "isSuperArmor", false);
                    BB->Set_Value<_bool>(pViper->Get_Name(), "isHit", false);
                    BB->Set_Value<_bool>(pViper->Get_Name(), "isHitFinished", false);
                    BB->Set_Value<_uint>(pViper->Get_Name(), "DamageType", ENUM_CLASS(HITREACTION::NONE));
                }
            };
    }

    else if ("P1_Quick2Hit" == name)
    {
        return [pViper](CBlackBoard* BB, BTNODESTATE eState)
            {
                if (nullptr == BB)
                    return;

                if (eState == BTNODESTATE::SUCCESS || eState == BTNODESTATE::FAILURE)
                {
                    BB->Set_Value<_bool>(pViper->Get_Name(), "isP1_Quick2HitFinished", false);
                    BB->Set_Value(pViper->Get_Name(), "isSuperArmor", false);
                    BB->Set_Value<_bool>(pViper->Get_Name(), "isHit", false);
                    BB->Set_Value<_bool>(pViper->Get_Name(), "isHitFinished", false);
                    BB->Set_Value<_uint>(pViper->Get_Name(), "DamageType", ENUM_CLASS(HITREACTION::NONE));
                }
            };
    }


#pragma endregion



#pragma region NONCOMBAT

    else if ("P1_Walk" == name)
    {
        return [pViper](CBlackBoard* BB, BTNODESTATE eState)
            {
                if (nullptr == BB)
                    return;

                if (eState == BTNODESTATE::SUCCESS || eState == BTNODESTATE::FAILURE)
                {
                   
                }
            };
    }

#pragma endregion

    return nullptr;
}

INTERRUPTCONDITION CAI_Controller_Viper::GetCallbackInterruptCondition(CGameObject* pOwner, const string& name)
{
    CViper* pViper = static_cast<CViper*>(pOwner);
    if (nullptr == pViper)
        return nullptr;

    if ("Root_Interrupt" == name)
    {
        return [pViper](CBlackBoard* BB)
            {
                if (BB->Get_Value<_bool>(pViper->Get_Name(), "isDead"))
                    return true;

                if (BB->Get_Value<_bool>(pViper->Get_Name(), "isGroggy"))
                    return true;
                
                if (BB->Get_Value<_bool>(pViper->Get_Name(), "DamageInterrupt"))
                {
                    _bool isSuperArmor = BB->Get_Value<_bool>(pViper->Get_Name(), "isSuperArmor");
                    if (isSuperArmor)
                        return false;

                    return true;

                }
                return false;
            };
    }


    return nullptr;
}

PERCEPTIONCALLBACK CAI_Controller_Viper::GetCallBackPerception(CGameObject* pOwner, const string& name)
{
    if (name == "Target")
    {
        return [this](CGameObject* pTarget, const STIMULUS& Stim)
            {
                if (Stim.eType == SENSETYPE::SIGHT)
                {
                    if (Stim.bSensed)
                    {
                        m_pBB->Set_Value(m_strMonstertag, "Target", pTarget);
                        m_pBB->Set_Value(m_strMonstertag, "isDetected", true);
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
                        m_pBB->Set_Value<_uint>(m_strMonstertag, "DamageType", Stim.iDamageType);
                        m_pBB->Set_Value(m_strMonstertag, "DamageInterrupt", true);
                        m_pBB->Set_Value(m_strMonstertag, "DamageACC", m_pPerception->Get_DamageAcc());
                    }
                    else
                    {
                        m_pBB->Set_Value(m_strMonstertag, "DamageACC", m_pPerception->Get_DamageAcc());
                    }
                }
            };
    };

    return nullptr;
}

//SCORE CAI_Controller_Viper::GetCallbackScore(CGameObject* pOwner, const string& name)
//{
//    CViper* pViper = static_cast<CViper*>(pOwner);
//    if (nullptr == pViper)
//        return nullptr;
//
//    if ("P1_LockOn" == name)
//    {
//        return [pViper](CBlackBoard* BB)->_float
//            {
//               
//                _float fDist = BB->Get_Value<_float>(pViper->Get_Name(), "TargetDist");
//                _float fLockOnRange = BB->Get_Value<_float>(pViper->Get_Name(), "LockOnRange");
//                _float fChaseRange = BB->Get_Value<_float>(pViper->Get_Name(), "ChaseRange");
//                _float fCoolDownRemain = BB->Get_Value<_float>(pViper->Get_Name(), "P1_LockOn_CD_Remain");
//                _float fCoolDownMax = BB->Get_Value<_float>(pViper->Get_Name(), "P1_LockOn_CD_Max");
//                _uint iLastSkill = BB->Get_Value<_uint>(pViper->Get_Name(), "LastSkill");
//                _float fDot = BB->Get_Value<_float>(pViper->Get_Name(), "fDot");
//
//                if (fCoolDownRemain > 0.f)
//                    return 0.f;
//
//
//                _float fDistScore = UtilityScore::DistanceScore(fDist, fLockOnRange, fChaseRange);
//                _float fCoolDownScore = UtilityScore::Utility_Remap(fCoolDownRemain, 0.f, fCoolDownMax, true);
//                _float fAngleScore = UtilityScore::Utility_Remap(fDot, 0.f, 1.f);
//                _float fChainPenalty = (iLastSkill == ENUM_CLASS(VIPER_SKILL::LOCKON)) ? 0.5f : 1.f;
//                _float fWeight = 0.5f;
//
//                _float finalScore =  fAngleScore * fCoolDownScore * fWeight * fChainPenalty;
//                return UtilityScore::Utility_Clamp(finalScore);
//            };
//    }
//
//    else if ("P1_Slow2Hit" == name)
//    {
//        return [pViper](CBlackBoard* BB)->_float
//            {
//                _float fDist = BB->Get_Value<_float>(pViper->Get_Name(), "TargetDist");
//                _float fAttackRange = BB->Get_Value<_float>(pViper->Get_Name(), "AttackRange");
//                _float fChaseRange = BB->Get_Value<_float>(pViper->Get_Name(), "ChaseRange");
//                _float fCoolDownRemain = BB->Get_Value<_float>(pViper->Get_Name(), "Slow2Hit_CD_Remain");
//                _float fCoolDownMax = BB->Get_Value<_float>(pViper->Get_Name(), "Slow2Hit_CD_Max");
//                _uint iLastSkill = BB->Get_Value<_uint>(pViper->Get_Name(), "LastSkill");
//                _float fDistanceScore = UtilityScore::DistanceScore(fDist, 0.f, fAttackRange);
//                _float fCoolDownScore = UtilityScore::Utility_Remap(fCoolDownRemain, 0.f, fCoolDownMax, true);
//
//                const _float fWeight = 1.1f;
//                _float fChainPenalty = (iLastSkill == ENUM_CLASS(VIPER_SKILL::SLOW2HIT)) ? 0.5f : 1.f;
//
//                _float fFinalScore = fDistanceScore * fCoolDownScore * fWeight * fChainPenalty;
//
//                return UtilityScore::Utility_Clamp(fFinalScore);
//
//            };
//    }
//
//    else if ("P1_Quick2Hit" == name)
//    {
//        return [pViper](CBlackBoard* BB)->_float
//            {
//                _float fDist = BB->Get_Value<_float>(pViper->Get_Name(), "TargetDist");
//                _float fAttackRange = BB->Get_Value<_float>(pViper->Get_Name(), "AttackRange");
//                _float fChaseRange = BB->Get_Value<_float>(pViper->Get_Name(), "ChaseRange");
//                _float fCoolDownRemain = BB->Get_Value<_float>(pViper->Get_Name(), "Quick2Hit_CD_Remain");
//                _float fCoolDownMax = BB->Get_Value<_float>(pViper->Get_Name(), "Quick2Hit_CD_Max");
//                _uint iLastSkill = BB->Get_Value<_uint>(pViper->Get_Name(), "LastSkill");
//                // 嫄곕━ ?먯닔
//                _float fDistanceScore = UtilityScore::DistanceScore(fDist, 0.f, fAttackRange);
//                // 荑⑦????먯닔
//                _float fCoolDownScore = UtilityScore::Utility_Remap(fCoolDownRemain, 0.f, fCoolDownMax, true);
//
//                // 媛以묒튂
//                const _float fWeight = 1.3f; // 湲곕낯 怨듦꺽?쇰줈 媛以묒튂瑜?議곌툑 ?믨쾶 以??
//                // 諛섎났 ?ㅽ궗 ?대㈃ ?섎꼸??
//                _float fChainPenalty = (iLastSkill == ENUM_CLASS(VIPER_SKILL::QUICK2HIT)) ? 0.6f : 1.f;
//                // ?⑹궛
//                _float fFinalScore = fDistanceScore * fCoolDownScore * fWeight * fChainPenalty;
//                // ?뺢퇋??
//                return UtilityScore::Utility_Clamp(fFinalScore);
//                
//            };
//    }
//
//  
//    return nullptr;
//}
//
//
//HRESULT CAI_Controller_Viper::Ready_CoolDown()
//{
//    m_SkillCoolDowns["P1_LockOn"].Init(12.f);
//
//    m_SkillCoolDowns["P1_Quick2Hit"].Init(4.5f);
//    m_SkillCoolDowns["P1_StingSlash"].Init(3.f);
//    m_SkillCoolDowns["P1_Slow2Hit"].Init(5.5f);
//
//    m_SkillCoolDowns["P1_Slow3Hit"].Init(8.f);
//    m_SkillCoolDowns["P1_SlashBackJump"].Init(10.f);
//    
//    for (auto& pair : m_SkillCoolDowns)
//    {
//        const string& strSkill = pair.first;
//        SKILLCOOLDOWN& CoolDown = pair.second;
//
//        string key = strSkill + "_CD_Max";
//        m_pBB->Set_Value(m_strMonstertag, key, CoolDown.fMax);
//    }
//
//    return S_OK;
//}

CAI_Controller_Viper* CAI_Controller_Viper::Create(CCreature* pOwner)
{
    CAI_Controller_Viper* pInstance = new CAI_Controller_Viper();
    if (FAILED(pInstance->Initialize(pOwner)))
    {
        MSG_BOX(TEXT("Failed Create CAI_Controller_Viper"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CAI_Controller_Viper::Free()
{
    __super::Free();
}

