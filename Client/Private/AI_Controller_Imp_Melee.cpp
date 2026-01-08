#include "AI_Controller_Imp_Melee.h"
#include "Imp_Melee.h"
#include "FSM_Imp_Melee.h"
#include "BehaviorTree.h"
#include "GameInstance.h"
#include "Perception.h"



CAI_Controller_Imp_Melee::CAI_Controller_Imp_Melee()
{
}

HRESULT CAI_Controller_Imp_Melee::Initialize(CCreature* pOwner)
{
    CImp_Melee* pImp = static_cast<CImp_Melee*>(pOwner);

    if (FAILED(__super::Initialize(pImp, pImp->Get_Name())))
        return E_FAIL;

    m_pFSM = CFSM_Imp_Melee::Create(pOwner);
    if (nullptr == m_pFSM)
        return E_FAIL;

    m_pBB->Set_Value(m_strMonstertag, "CurrentTime", 0.f);

    return S_OK;

}

void CAI_Controller_Imp_Melee::Update(CGameObject* pOwner, _float fTimeDelta)
{
    if (!m_isActive)
        return;

    m_pPerception->Update(pOwner, m_pBB, fTimeDelta);
    Update_Aggro(pOwner, fTimeDelta);

    _float fPervTime = m_pBB->Get_Value<_float>(m_strMonstertag, "CurrentTime");

    if (m_pBB->Get_Value<_bool>(m_strMonstertag, "HasAggro"))
        m_pBB->Set_Value<_float>(m_strMonstertag, "CurrentTime", fPervTime + fTimeDelta);
    else
        m_pBB->Set_Value(m_strMonstertag, "CurrentTime", 0.f);


    if (!m_pBB->Get_Value<_bool>(m_strMonstertag, "isDeadFinished"))
        m_pBT->Update();

    m_pFSM->Update(pOwner, fTimeDelta);


}

void CAI_Controller_Imp_Melee::Update_Aggro(CGameObject* pOwner, _float fTimeDelta)
{
    CGameObject* pTarget = m_pBB->Get_Value<CGameObject*>(m_strMonstertag, "Target");
    _bool isDetected = m_pBB->Get_Value<_bool>(m_strMonstertag, "isDetected");

    
    const _float fFrogetDelay = 10.f;
    if (isDetected)
    {
        m_fLostSightTime = 0.f;
        m_pBB->Set_Value<_bool>(m_strMonstertag, "HasAggro", true);
    }
    else
    {
        if (m_pBB->Get_Value<_bool>(m_strMonstertag, "HasAggro"))
        {
            m_fLostSightTime += fTimeDelta;
            if (m_fLostSightTime > fFrogetDelay)
            {
                m_pBB->Set_Value<_bool>(m_strMonstertag, "isDetected", false);
                m_pBB->Set_Value<_bool>(m_strMonstertag, "HasAggro", false);
            }
        }
    }
}

HRESULT CAI_Controller_Imp_Melee::Ready_Perception(CGameObject* pOwner, const AIPERCEPTION_DATA& Desc)
{
    m_pPerception = CPerception::Create(Desc, ENUM_CLASS(TEAM::GOBLIN));
    if (nullptr == m_pPerception)
        return E_FAIL;

    m_pPerception->Set_PerceptionCallBack([this, pOwner, Desc](CGameObject* pTarget, const STIMULUS& Stim)
        {
            for (_uint i = 0; i < Desc.CallbackTags.size(); i++)
            {
                string strCallbackTag = Desc.CallbackTags[i];
                auto cb = GetCallBackPerception(pOwner, strCallbackTag);
                if(nullptr != cb)
                    cb(pTarget, Stim);
            }
        });

    return S_OK;

}

HRESULT CAI_Controller_Imp_Melee::Ready_BlackBoard(CGameObject* pOwner)
{
    m_pBB = CBlackBoard::Create();
    if (nullptr == m_pBB)
        return E_FAIL;

    return S_OK;
}

HRESULT CAI_Controller_Imp_Melee::Ready_BehaviorTree()
{
    if (nullptr == m_pBB)
        return E_FAIL;


    if (nullptr == m_pBT)
        return E_FAIL;

    m_pBT->Set_BlackBoard(m_pBB);

 
    return S_OK;
}

CONDITION CAI_Controller_Imp_Melee::GetCallbackCondition(CGameObject* pOwner, const string& name)
{
    CImp_Melee* pImp = static_cast<CImp_Melee*>(pOwner);
    if (nullptr == pImp)
        return nullptr;

    if ("Dead" == name)
    {
        return [pImp](CBlackBoard* BB)->_bool
            {

                BB->Set_Value<_bool>(pImp->Get_Name(), "DamageInterrupt", false);

                if (pImp->Get_CurrentHP() <= 0.f)
                {
                    return true;
                }
                else
                    return false;
            };
    }



#pragma region HIT SEQUENCE


    if ("Hit" == name)
    {
        return [pImp](CBlackBoard* BB) -> _bool
            {

                if (!BB->Get_Value<_bool>(pImp->Get_Name(), "isHit"))
                {
                    // DamageType 체크
                    HITREACTION eHitRection = static_cast<HITREACTION>(
                        BB->Get_Value<_uint>(pImp->Get_Name(), "DamageType"));

                    if (eHitRection == HITREACTION::KNOCKBACK_WEAK ||
                        eHitRection == HITREACTION::KNOCKBACK_NORMAL ||
                        eHitRection == HITREACTION::KNOCKBACK_STRONG)
                    {
                        // 조건 통과 (부수효과 없음)
                        return true;
                    }
                    return false;
                }
                return false;
            };
    }


#pragma endregion
   


#pragma region AGGRO SEQUENCE
    
    else if ("Detected" == name)
    {
        return [pImp](CBlackBoard* BB) -> _bool
            {

                _bool HasAggro = BB->Get_Value<_bool>(pImp->Get_Name(), "HasAggro");
                if (HasAggro)
                    return true;
                else
                    return false;
            };
    }

#pragma endregion

  
#pragma region COMBAT SELECTOR


    else if ("ChainSmash" == name)
    {
        return [pImp](CBlackBoard* BB)->_bool
            {

                _bool isSleep = BB->Get_Value<_bool>(pImp->Get_Name(), "isSleep");

                //cout << "isSleep " << isSleep << endl;

                _float fDist = BB->Get_Value<_float>(pImp->Get_Name(), "TargetDist");
                _float fAttackRanage = BB->Get_Value<_float>(pImp->Get_Name(), "AttackRange");

                if (fDist <= fAttackRanage)
                {
                    return true;
                }
                else
                    return false;
            };
    }

    else if ("NonStopAttack" == name)
    {
        return [pImp](CBlackBoard* BB)->_bool
            {
                //cout << "NonStopAttack Condition" << endl;
                _float fDist = BB->Get_Value<_float>(pImp->Get_Name(), "TargetDist");
                _float fAttackRanage = BB->Get_Value<_float>(pImp->Get_Name(), "AttackRange");

                if (fDist <= fAttackRanage)
                {
                    return true;
                }
                else
                    return false;
            };
    }

    if ("Move" == name)
    {
        return [pImp](CBlackBoard* BB)
            {
                _float fDist = BB->Get_Value<_float>(pImp->Get_Name(), "TargetDist");
                _float fChaseRange = BB->Get_Value<_float>(pImp->Get_Name(), "ChaseRange");
                _float fStopRange = BB->Get_Value<_float>(pImp->Get_Name(), "MoveStopRange");

                if (!BB->Get_Value<_bool>(pImp->Get_Name(), "HasAggro"))
                    return false;

                if (fDist <= fStopRange)
                    return false;

                return fDist <= fChaseRange;
            };
    }




#pragma endregion


#pragma region NORMAL SELECTOR

     else if ("Sleep" == name)
     {
         return [pImp](CBlackBoard* BB)->_bool
             {
                 _bool HasAggro = BB->Get_Value<_bool>(pImp->Get_Name(), "HasAggro");
                 _bool isSleep = BB->Get_Value<_bool>(pImp->Get_Name(), "isSleep");

                 // 감지 되면 Sleep 진입 금지
                if(HasAggro)
                       return false;

                // Sleep 중이 아니면 진입
                 if(!isSleep)
                     return true;
                 
                 return false;
             };
     }

#pragma endregion

    return nullptr;
}

ACTION CAI_Controller_Imp_Melee::GetCallbackAction(CGameObject* pOwner, const string& name)
{
    CImp_Melee* pImp = static_cast<CImp_Melee*>(pOwner);
    if (nullptr == pImp)
        return nullptr;


    if ("Dead" == name)
    {

        return [pImp](CBlackBoard* BB)-> BTNODESTATE
            {
                if (BB->Get_Value<_bool>(pImp->Get_Name(), "isDeadFinished"))
                {

                    return BTNODESTATE::SUCCESS;
                }

                BB->Set_Value(pImp->Get_Name(), "isDead", true);

                pImp->Get_Controller()->Get_State_Machine()->
                    Change_State(ENUM_CLASS(IMPMELEE_STATE::DEAD), pImp);
                return BTNODESTATE::RUNNING;
            };
    }



#pragma region HIT SEQUENCE

    if ("Hit" == name)
    {
        return [pImp](CBlackBoard* BB)->BTNODESTATE
            {
                // 애니 종료 플래그가 true면 SUCCESS
                if (true == BB->Get_Value<_bool>(pImp->Get_Name(), "isHitFinished"))
                {
                    BB->Set_Value<_bool>(pImp->Get_Name(), "DamageInterrupt", false);
                    BB->Set_Value<_uint>(pImp->Get_Name(), "DamageType", ENUM_CLASS(HITREACTION::NONE));
                    return BTNODESTATE::SUCCESS;
                }
   
                BB->Set_Value(pImp->Get_Name(), "isHit", true);
                BB->Set_Value(pImp->Get_Name(), "isHitFinished", false);

                pImp->Get_Controller()->Get_State_Machine()->Change_State(
                    ENUM_CLASS(IMPMELEE_STATE::HIT), pImp);

                return BTNODESTATE::RUNNING;

            };
    }


#pragma endregion

#pragma region COMBAT SELECTOR

    else if ("ChainSmash" == name)
    {
        return [pImp](CBlackBoard* BB)-> BTNODESTATE
            {

                _bool isDamaged = BB->Get_Value<_bool>(pImp->Get_Name(), "DamageInterrupt");
                if (isDamaged)
                    return BTNODESTATE::SUCCESS;

                if (BB->Get_Value<_bool>(pImp->Get_Name(), "isChainSmashFinished"))
                    return BTNODESTATE::SUCCESS;

                pImp->Get_Controller()->Get_State_Machine()->
                    Change_State(ENUM_CLASS(IMPMELEE_STATE::HIT3), pImp);

                BB->Set_Value(pImp->Get_Name(), "isChainSmash", true);

                return BTNODESTATE::RUNNING;

            };
    }


    else if ("NonStopAttack" == name)
    {
        return [pImp](CBlackBoard* BB)-> BTNODESTATE
            {
                _bool isDamaged = BB->Get_Value<_bool>(pImp->Get_Name(), "DamageInterrupt");
                if (isDamaged)
                    return BTNODESTATE::SUCCESS;

                if (BB->Get_Value<_bool>(pImp->Get_Name(), "isNonStopAttackFinished"))
                {
                    return BTNODESTATE::SUCCESS;
                }

                BB->Set_Value(pImp->Get_Name(), "isNonStopAttack", true);

                pImp->Get_Controller()->Get_State_Machine()->
                    Change_State(ENUM_CLASS(IMPMELEE_STATE::HIT2), pImp);
                return BTNODESTATE::RUNNING;

            };
    }

    if ("Move" == name)
    {
        return [pImp](CBlackBoard* BB)->BTNODESTATE
            {

                _float fDist = BB->Get_Value<_float>(pImp->Get_Name(), "TargetDist");
                _float fStopRange = BB->Get_Value<_float>(pImp->Get_Name(), "MoveStopRange");
                _float fChaseRange = BB->Get_Value<_float>(pImp->Get_Name(), "ChaseRange");
                _bool isDamaged = BB->Get_Value<_bool>(pImp->Get_Name(), "DamageInterrupt");


                if (isDamaged)
                    return BTNODESTATE::SUCCESS;

                if (fDist <= fStopRange)
                    return BTNODESTATE::SUCCESS;

                if (!BB->Get_Value<_bool>(pImp->Get_Name(), "HasAggro"))
                    return BTNODESTATE::FAILURE;


                if (fDist > fChaseRange)
                    return BTNODESTATE::FAILURE;


                pImp->Get_Controller()->Get_State_Machine()->Change_State(ENUM_CLASS(IMPMELEE_STATE::MOVE), pImp);


                return BTNODESTATE::RUNNING;
            };
    }


#pragma endregion


#pragma region NON ATTACK SELECTOR


    else if ("Sleep" == name)
    {
          return [pImp](CBlackBoard* BB)->BTNODESTATE
              {
                 
                  if (BB->Get_Value<_bool>(pImp->Get_Name(), "isSleepFinished"))
                  {
                      return BTNODESTATE::SUCCESS;
                  }

                  BB->Set_Value<_bool>(pImp->Get_Name(), "isSleep", true);

                  pImp->Get_Controller()->Get_State_Machine()->Change_State(ENUM_CLASS(IMPMELEE_STATE::SLEEP), pImp);
                  return BTNODESTATE::RUNNING;
              };
    }

    else if ("Idle" == name)
    {
        return [pImp](CBlackBoard* BB)->BTNODESTATE
            {
                _bool HasAggro = BB->Get_Value<_bool>(pImp->Get_Name(), "HasAggro");
                if (HasAggro)
                    return BTNODESTATE::FAILURE;

                _bool isDamaged = BB->Get_Value<_bool>(pImp->Get_Name(), "DamageInterrupt");
                if (isDamaged)
                    return BTNODESTATE::FAILURE;

                _float fDist = BB->Get_Value<_float>(pImp->Get_Name(), "TargetDist");
                _float fStopRange = BB->Get_Value<_float>(pImp->Get_Name(), "MoveStopRange");
                _float fAttackRange = BB->Get_Value<_float>(pImp->Get_Name(), "AttackRange");


                if (fDist <= fAttackRange)
                    return BTNODESTATE::FAILURE;


                if (fDist <= fStopRange)
                    return BTNODESTATE::FAILURE;

              
                pImp->Get_Controller()->Get_State_Machine()->Change_State(ENUM_CLASS(IMPMELEE_STATE::IDLE), pImp);

                return BTNODESTATE::RUNNING;
            };
    }


#pragma endregion

    return nullptr;
}

TERMINATE CAI_Controller_Imp_Melee::GetCallbackTeminate(CGameObject* pOwner, const string& name)
{
    CImp_Melee* pImp = static_cast<CImp_Melee*>(pOwner);
    if (nullptr == pImp)
        return nullptr;



    if ("Dead" == name)
    {
        return [pImp](CBlackBoard* BB, BTNODESTATE eState)
            {
                if (nullptr == BB)
                    return;

                if (eState == BTNODESTATE::SUCCESS || eState == BTNODESTATE::FAILURE)
                {
                    BB->Set_Value<_bool>(pImp->Get_Name(), "isDead", false);
                    BB->Set_Value<_bool>(pImp->Get_Name(), "DamageInterrupt", false);
                }
            };
    }



#pragma region HIT SEQUENCE

    if ("Hit" == name)
    {
        return [pImp](CBlackBoard* BB, BTNODESTATE eState)
            {
                if (nullptr == BB)
                    return;

                if (eState == BTNODESTATE::SUCCESS || eState == BTNODESTATE::FAILURE)
                {
                    BB->Set_Value<_bool>(pImp->Get_Name(), "isHit", false);
                    BB->Set_Value<_bool>(pImp->Get_Name(), "isHitFinished", false);
                    BB->Set_Value<_bool>(pImp->Get_Name(), "DamageInterrupt", false);

                }
            };
    }

#pragma endregion


#pragma region ATTACK SELECTOR

    else if ("ChainSmash" == name)
    {
        return [pImp](CBlackBoard* BB, BTNODESTATE eState)
            {
                if (nullptr == BB)
                    return;

                if (eState == BTNODESTATE::SUCCESS || eState == BTNODESTATE::FAILURE)
                {
                    BB->Set_Value<_bool>(pImp->Get_Name(), "isChainSmash", false);
                    BB->Set_Value<_bool>(pImp->Get_Name(), "isChainSmashFinished", false);

                }
            };
    }


    else if ("NonStopAttack" == name)
    {
        return [pImp](CBlackBoard* BB, BTNODESTATE eState)
            {
                if (nullptr == BB)
                    return;

                if (eState == BTNODESTATE::SUCCESS || eState == BTNODESTATE::FAILURE)
                {
                    BB->Set_Value<_bool>(pImp->Get_Name(), "isNonStopAttack", false);
                    BB->Set_Value<_bool>(pImp->Get_Name(), "isNonStopAttackFinished", false);

                }
            };
    }


    else if ("Move" == name)
    {
        return [pImp](CBlackBoard* BB, BTNODESTATE eState)
            {
                if (nullptr == BB)
                    return;

                if (eState == BTNODESTATE::SUCCESS || eState == BTNODESTATE::FAILURE)
                {
                    BB->Set_Value<_uint>(pImp->Get_Name(), "isMovementFlag", 0);
                }
            };
    }

#pragma endregion


#pragma region NON ATTACK SELECTOR


    if ("Sleep" == name)
    {
        return [pImp](CBlackBoard* BB, BTNODESTATE eState)
            {
                if (!BB) return;

                if (eState == BTNODESTATE::SUCCESS || eState == BTNODESTATE::FAILURE)
                {
                   /* BB->Set_Value<_bool>(pImp->Get_Name(), "isSleepFinished", false);
                    BB->Set_Value<_bool>(pImp->Get_Name(), "isSleep", false);*/
                }
            };
    }


#pragma endregion


    return nullptr;


}

INTERRUPTCONDITION CAI_Controller_Imp_Melee::GetCallbackInterruptCondition(CGameObject* pOwner, const string& name)
{
    CImp_Melee* pImp = static_cast<CImp_Melee*>(pOwner);
    if (nullptr == pImp)
        return nullptr;

    if ("Root_Interrupt" == name)
    {
        return [pImp](CBlackBoard* BB)
            {
                if (BB->Get_Value<_bool>(pImp->Get_Name(), "isDead"))
                    return true;
                if (BB->Get_Value<_bool>(pImp->Get_Name(), "DamageInterrupt"))
                    return true;

                return false;
            };
    }
    return nullptr;
}

PERCEPTIONCALLBACK CAI_Controller_Imp_Melee::GetCallBackPerception(CGameObject* pOwner, const string& name)
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
                        m_pPerception->Set_Fov();
                        
                    }
                    else
                    {
                        m_pPerception->Reset_Fov();
                    }
                }
            };
    }

    else if (name == "DamageInterrupt")
    {
        return [this](CGameObject* pTarget, const STIMULUS& Stim)
            {
                if (Stim.eType == SENSETYPE::DAMAGE)
                {
                    if (Stim.bSensed)
                    {
                        m_pBB->Set_Value<_uint>(m_strMonstertag, "DamageType", Stim.iDamageType);
                        m_pBB->Set_Value(m_strMonstertag, "DamageInterrupt", true);
                        m_pBB->Set_Value(m_strMonstertag, "isDetected", true);
                    }
                }
            };
    }

    return nullptr;

}

CAI_Controller_Imp_Melee* CAI_Controller_Imp_Melee::Create(CCreature* pOwner)
{
    CAI_Controller_Imp_Melee* pInstance = new CAI_Controller_Imp_Melee();
    if (FAILED(pInstance->Initialize(pOwner)))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Failed Create : CAI_Controller_Imp_Melee"));
    }

    return pInstance;
}

void CAI_Controller_Imp_Melee::Free()
{
    __super::Free();
}
