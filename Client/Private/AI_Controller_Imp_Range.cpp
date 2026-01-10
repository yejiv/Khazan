#include "AI_Controller_Imp_Range.h"
#include "Imp_Range.h"
#include "FSM_Imp_Range.h"
#include "BehaviorTree.h"
#include "GameInstance.h"
#include "Perception.h"


CAI_Controller_Imp_Range::CAI_Controller_Imp_Range()
{
}

HRESULT CAI_Controller_Imp_Range::Initialize(CCreature* pOwner)
{
    CImp_Range* pImp = static_cast<CImp_Range*>(pOwner);
    if (FAILED(__super::Initialize(pImp, pImp->Get_Name())))
        return E_FAIL;

    m_pFSM = CFSM_Imp_Range::Create(pOwner);
    if (nullptr == m_pFSM)
        return E_FAIL;

    m_pBB->Set_Value(m_strMonstertag, "CurrentTime", 0.f);

    return S_OK;

}

void CAI_Controller_Imp_Range::Update(CGameObject* pOwner, _float fTimeDelta)
{
    if (!m_isActive)
        return;
    

    m_pPerception->Update(pOwner,m_pBB,fTimeDelta);

    Update_Aggro(pOwner,fTimeDelta);

    _float fPervTime = m_pBB->Get_Value<_float>(m_strMonstertag, "CurrentTime");

    if (m_pBB->Get_Value<_bool>(m_strMonstertag, "HasAggro"))
    {
        m_pBB->Set_Value<_float>(m_strMonstertag, "CurrentTime", fPervTime + fTimeDelta);
        m_pBT->Update();
    }
    else
        m_pBB->Set_Value(m_strMonstertag, "CurrentTime", 0.f);

    m_pFSM->Update(pOwner, fTimeDelta);

}

void CAI_Controller_Imp_Range::Update_Aggro(CGameObject* pOwner, _float fTimeDelta)
{
    
    CGameObject* pTarget = m_pBB->Get_Value<CGameObject*>(m_strMonstertag, "Target");
    _bool isDetected = m_pBB->Get_Value<_bool>(m_strMonstertag, "isDetected");

    static _float fLostSightTime = 0.f;
    static const _float fFrogetDelay = 10.f;
    if (isDetected)
    {
        fLostSightTime = 0.f;
        m_pBB->Set_Value<_bool>(m_strMonstertag, "HasAggro", true);
    }
    else
    {
        if (m_pBB->Get_Value<_bool>(m_strMonstertag, "HasAggro"))
        {
            fLostSightTime += fTimeDelta;
            if (fLostSightTime > fFrogetDelay)
            {
                m_pBB->Set_Value<_bool>(m_strMonstertag, "isDetected", false);
                m_pBB->Set_Value<_bool>(m_strMonstertag, "HasAggro", false);
            }
        }
    }
}

HRESULT CAI_Controller_Imp_Range::Ready_Perception(CGameObject* pOwner, const AIPERCEPTION_DATA& Desc)
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
                cb(pTarget, Stim);
            }
        });

    return S_OK;

}

HRESULT CAI_Controller_Imp_Range::Ready_BlackBoard(CGameObject* pOwner)
{
    m_pBB = CBlackBoard::Create();
    if (nullptr == m_pBB)
        return E_FAIL;

    return S_OK;
}

HRESULT CAI_Controller_Imp_Range::Ready_BehaviorTree()
{
    if (nullptr == m_pBB)
        return E_FAIL;

    m_pBT->Set_BlackBoard(m_pBB);

    if (nullptr == m_pBT)
        return E_FAIL;

    return S_OK;
}

CONDITION CAI_Controller_Imp_Range::GetCallbackCondition(CGameObject* pOwner, const string& name)
{
    CImp_Range* pImp = static_cast<CImp_Range*>(pOwner);
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
                {
                    return false;
                }
            };
    }



#pragma region HIT SEQUENCE

  
    if ("Hit" == name)
    {
        return [pImp](CBlackBoard* BB) -> _bool
            {

                BB->Set_Value<_bool>(pImp->Get_Name(), "DamageInterrupt", false);
                
                if (!BB->Get_Value<_bool>(pImp->Get_Name(), "isHit"))
                {
                    // DamageType 체크
                    HITREACTION eHitRection = static_cast<HITREACTION>(
                        BB->Get_Value<_uint>(pImp->Get_Name(), "DamageType"));

                    if (eHitRection == HITREACTION::KNOCKBACK_WEAK ||
                        eHitRection == HITREACTION::KNOCKBACK_NORMAL ||
                        eHitRection == HITREACTION::KNOCKBACK_STRONG ||
                        eHitRection == HITREACTION::BRUTAL_ATTACK)
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



#pragma region SLEEP SEQUENCE

    else if ("Sleep" == name)
    {
        return [pImp](CBlackBoard* BB)->_bool
            {
                // if (!BB->Get_Value<_bool>(pImp->Get_Name(), "isSleep"))
                if (!BB->Get_Value<_bool>(pImp->Get_Name(), "isWakeUp"))
                {

                    return true;
                }
                else
                {
                    return false;
                }
            };
    }

#pragma endregion

#pragma region ATTACK SELECTOR


    //else if ("LockOn" == name)
    //{
    //    return [pImp](CBlackBoard* BB)->_bool
    //        {
    //            _float fDist = BB->Get_Value<_float>(pImp->Get_Name(), "TargetDist");
    //            _float fAttackRanage = BB->Get_Value<_float>(pImp->Get_Name(), "BoomarangRange");

    //            if (fDist <= fAttackRanage && !BB->Get_Value<_bool>(pImp->Get_Name(), "isLockOn"))
    //            {
    //                BB->Set_Value<_bool>(pImp->Get_Name(), "isRetreat",fDist < fAttackRanage + 100.f);
    //                BB->Set_Value<_bool>(pImp->Get_Name(), "isCircleRight",rand()%2 == 0);
    //                return true;
    //            }
    //            else
    //                return false;
    //        };
    //}



 
    else if ("Boomarang" == name)
    {
        return [pImp](CBlackBoard* BB)->_bool
            {
       

                _float fDist = BB->Get_Value<_float>(pImp->Get_Name(), "TargetDist");
                _float fAttackRanage = BB->Get_Value<_float>(pImp->Get_Name(), "BoomarangRange");

                if (fDist <= fAttackRanage)
                {
                    return true;
                }
                else
                    return false;
            };
    }

    else if ("Magic" == name)
    {
        return [pImp](CBlackBoard* BB)->_bool
            {

                

                _float fDist = BB->Get_Value<_float>(pImp->Get_Name(), "TargetDist");
                _float fAttackRanage = BB->Get_Value<_float>(pImp->Get_Name(), "MagicRange");

                if (fDist <= fAttackRanage)
                {
                   
                    return true;
                }
                else
                    return false;
            };
    }


 
#pragma endregion


#pragma region NON ATTACK SELECTOR

    if ("Move" == name)
    {
        return [pImp](CBlackBoard* BB)
            {
                _float fDist = BB->Get_Value<_float>(pImp->Get_Name(), "TargetDist");
                _float fChaseRange = BB->Get_Value<_float>(pImp->Get_Name(), "ChaseRange");
                _float fStopRange = BB->Get_Value<_float>(pImp->Get_Name(), "MoveStopRange");


                //cout << "MoveCondition" << endl;

                if (!BB->Get_Value<_bool>(pImp->Get_Name(), "isDetected"))
                    return false;

                if (fDist <= fStopRange)
                    return false;

                return fDist <= fChaseRange;
            };
    }
#pragma endregion

    return nullptr;
}

ACTION CAI_Controller_Imp_Range::GetCallbackAction(CGameObject* pOwner, const string& name)
{
    CImp_Range* pImp = static_cast<CImp_Range*>(pOwner);
    if (nullptr == pImp)
        return nullptr;

    if ("Dead" == name)
    {
        return [pImp](CBlackBoard* BB)-> BTNODESTATE
            {
                if (BB->Get_Value<_bool>(pImp->Get_Name(), "isDeadFinished"))
                    return BTNODESTATE::SUCCESS;
                
                pImp->Get_Controller()->Get_State_Machine()->
                    Change_State(ENUM_CLASS(IMPRANGE_STATE::DEAD), pImp);
                return BTNODESTATE::RUNNING;
            };
    }


#pragma region HIT SEQUENCE

    if ("Hit" == name)
    {
        return [pImp](CBlackBoard* BB)->BTNODESTATE
            {
                if (true == BB->Get_Value<_bool>(pImp->Get_Name(), "isHitFinished"))
                {
                    BB->Set_Value<_bool>(pImp->Get_Name(), "DamageInterrupt", false);
                    BB->Set_Value<_uint>(pImp->Get_Name(), "DamageType", ENUM_CLASS(HITREACTION::NONE));
                    return BTNODESTATE::SUCCESS;
                }
              
                BB->Set_Value(pImp->Get_Name(), "isHit", true);
                BB->Set_Value(pImp->Get_Name(), "isHitFinished", false);

                pImp->Get_Controller()->Get_State_Machine()->Change_State(
                    ENUM_CLASS(IMPRANGE_STATE::HIT), pImp);

                return BTNODESTATE::RUNNING;

            };
    }

#pragma endregion

#pragma region SLEEP SEQUENCE

    else if ("Sleep" == name)
    {
        return [pImp](CBlackBoard* BB)->BTNODESTATE
            {

                _bool isDamaged = BB->Get_Value<_bool>(pImp->Get_Name(), "DamageInterrupt");
                if (isDamaged)
                    return BTNODESTATE::SUCCESS;


                if (BB->Get_Value<_bool>(pImp->Get_Name(), "isSleepFinished"))
                {

                    return BTNODESTATE::SUCCESS;
                }
                BB->Set_Value<_bool>(pImp->Get_Name(), "isWakeUp", true);
                pImp->Get_Controller()->Get_State_Machine()->Change_State(ENUM_CLASS(IMPRANGE_STATE::SLEEP), pImp);
                return BTNODESTATE::RUNNING;
            };
    }

#pragma endregion

#pragma region ATTACK SELECTOR


    //else if ("LockOn" == name)
    //{
    //    return [pImp](CBlackBoard* BB)-> BTNODESTATE
    //        {
    //            _bool isDamaged = BB->Get_Value<_bool>(pImp->Get_Name(), "DamageInterrupt");
    //            if (isDamaged)
    //                return BTNODESTATE::SUCCESS;

    //            if (BB->Get_Value<_bool>(pImp->Get_Name(), "isLockOnFinished"))
    //            {
    //                return BTNODESTATE::SUCCESS;
    //            }

    //            BB->Set_Value(pImp->Get_Name(), "isLockOnAttack", true);

    //            pImp->Get_Controller()->Get_State_Machine()->
    //                Change_State(ENUM_CLASS(IMPRANGE_STATE::LOCKON), pImp);
    //            return BTNODESTATE::RUNNING;

    //        };
    //}




    else if ("Boomarang" == name)
    {
        return [pImp](CBlackBoard* BB)-> BTNODESTATE
            {
                _bool isDamaged = BB->Get_Value<_bool>(pImp->Get_Name(), "DamageInterrupt");
                if (isDamaged)
                    return BTNODESTATE::SUCCESS;

                if (BB->Get_Value<_bool>(pImp->Get_Name(), "isBoomarangFinished"))
                {
                    return BTNODESTATE::SUCCESS;
                }

                BB->Set_Value(pImp->Get_Name(), "isBoomarangAttack", true);

                pImp->Get_Controller()->Get_State_Machine()->
                    Change_State(ENUM_CLASS(IMPRANGE_STATE::BOOMARANG), pImp);
                return BTNODESTATE::RUNNING;

            };
    }

    else if ("Magic" == name)
    {
        return [pImp](CBlackBoard* BB)-> BTNODESTATE
            {
            
                _bool isDamaged = BB->Get_Value<_bool>(pImp->Get_Name(), "DamageInterrupt");
                if (isDamaged)
                    return BTNODESTATE::SUCCESS;

                if (BB->Get_Value<_bool>(pImp->Get_Name(), "isMagicFinished"))
                    return BTNODESTATE::SUCCESS;

                pImp->Get_Controller()->Get_State_Machine()->
                    Change_State(ENUM_CLASS(IMPRANGE_STATE::MAGIC), pImp);

                BB->Set_Value(pImp->Get_Name(), "isMagic", true);

                return BTNODESTATE::RUNNING;

            };
    }


#pragma endregion


#pragma region NON ATTACK SELECTOR

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

                if (!BB->Get_Value<_bool>(pImp->Get_Name(), "isDetected"))
                    return BTNODESTATE::FAILURE;


                if (fDist > fChaseRange)
                    return BTNODESTATE::FAILURE;


                pImp->Get_Controller()->Get_State_Machine()->Change_State(ENUM_CLASS(IMPRANGE_STATE::MOVE), pImp);


                return BTNODESTATE::RUNNING;
            };
    }

    if ("Idle" == name)
    {
        return [pImp](CBlackBoard* BB)->BTNODESTATE
            {

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


                pImp->Get_Controller()->Get_State_Machine()->Change_State(ENUM_CLASS(IMPRANGE_STATE::IDLE), pImp);

                return BTNODESTATE::RUNNING;
            };
    }


#pragma endregion

    return nullptr;
}

TERMINATE CAI_Controller_Imp_Range::GetCallbackTeminate(CGameObject* pOwner, const string& name)
{
    CImp_Range* pImp = static_cast<CImp_Range*>(pOwner);
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


#pragma region SLEEP SEQUENCE

    else if ("Sleep" == name)
    {
        return [pImp](CBlackBoard* BB, BTNODESTATE eState)
            {
                if (nullptr == BB)
                    return;

                if (eState == BTNODESTATE::SUCCESS || eState == BTNODESTATE::FAILURE)
                    BB->Set_Value<_bool>(pImp->Get_Name(), "isSleepFinished", false);
            };
    }

#pragma endregion


    else if ("Boomarang" == name)
    {
        return [pImp](CBlackBoard* BB, BTNODESTATE eState)
            {
                if (nullptr == BB)
                    return;

                if (eState == BTNODESTATE::SUCCESS || eState == BTNODESTATE::FAILURE)
                {
                    BB->Set_Value<_bool>(pImp->Get_Name(), "isBoomarang", false);
                    BB->Set_Value<_bool>(pImp->Get_Name(), "isBoomarangFinished", false);
                }
            };
    }




#pragma region ATTACK SELECTOR

    else if ("Boomarang" == name)
    {
        return [pImp](CBlackBoard* BB, BTNODESTATE eState)
        {
            if (nullptr == BB)
                return;

            if (eState == BTNODESTATE::SUCCESS || eState == BTNODESTATE::FAILURE)
            {
                BB->Set_Value<_bool>(pImp->Get_Name(), "isBoomarang", false);
                BB->Set_Value<_bool>(pImp->Get_Name(), "isBoomarangFinished", false);
            }
        };
    }


    else if ("Magic" == name)
    {
        return [pImp](CBlackBoard* BB, BTNODESTATE eState)
        {
            if (nullptr == BB)
                return;

            if (eState == BTNODESTATE::SUCCESS || eState == BTNODESTATE::FAILURE)
            {
                BB->Set_Value<_bool>(pImp->Get_Name(), "isMagic", false);
                BB->Set_Value<_bool>(pImp->Get_Name(), "isMagicFinished", false);
            }
        };
    }

#pragma endregion


#pragma region NON ATTACK SELECTOR


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
        return nullptr;

#pragma endregion
}

INTERRUPTCONDITION CAI_Controller_Imp_Range::GetCallbackInterruptCondition(CGameObject* pOwner, const string& name)
{
    CImp_Range* pImp = static_cast<CImp_Range*>(pOwner);
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

PERCEPTIONCALLBACK CAI_Controller_Imp_Range::GetCallBackPerception(CGameObject* pOwner, const string& name)
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
                        m_pBB->Set_Value(m_strMonstertag, "isDetected", false);
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

CAI_Controller_Imp_Range* CAI_Controller_Imp_Range::Create(CCreature* pOwner)
{
    CAI_Controller_Imp_Range* pInstance = new CAI_Controller_Imp_Range();
    if (FAILED(pInstance->Initialize(pOwner)))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Failed Create : CAI_Controller_Imp_Range"));
    }

    return pInstance;
}

void CAI_Controller_Imp_Range::Free()
{
    __super::Free();
}
