#include "AI_Controller_Gomdol.h"
#include "Gomdol.h"
#include "FSM_Gomdol.h"
#include "BlackBoard.h"
#include "BehaviorTree.h"
#include "GameInstance.h"
#include "Perception.h"


CAI_Controller_Gomdol::CAI_Controller_Gomdol()
{
}

HRESULT CAI_Controller_Gomdol::Initialize(CCreature* pOwner)
{
    CGomdol* pGomdol = static_cast<CGomdol*>(pOwner);
    if (FAILED(__super::Initialize(pGomdol, pGomdol->Get_Name())))
        return E_FAIL;
    
    m_pFSM = CFSM_Gomdol::Create(pOwner);
    if (nullptr == m_pFSM)
        return E_FAIL;

    m_pBB->Set_Value(m_strMonstertag, "CurrentTime", 0.f);
    m_isActiveController = true;

    return S_OK;
}

void CAI_Controller_Gomdol::Update(CGameObject* pOwner, _float fTimeDelta)
{

    if (m_pGameInstance->Key_Pressing(DIK_RCONTROL, fTimeDelta))
   {

       if (m_pGameInstance->Key_Down(DIK_B))
       {
           CGomdol* pGomdol = static_cast<CGomdol*>(pOwner);
           m_isActiveController = false;
           pGomdol->Get_Transform()->Set_State(STATE::POSITION, XMVectorSet(-4.f, 0.f, 17.78f, 1.f));
           pGomdol->Set_InitPosition();
           m_pBB->Set_Value<_bool>(m_strMonstertag, "isDetected",false);
           m_pFSM->Change_State(ENUM_CLASS(GOMDOL_STATE::SLEEP), pGomdol);

       }
       if (m_pGameInstance->Key_Down(DIK_N))
       {
           CGomdol* pGomdol = static_cast<CGomdol*>(pOwner);
           m_isActiveController = true;
           m_pBB->Set_Value<_bool>(pGomdol->Get_Name(), "isSleep", false);
           m_pBB->Set_Value<_bool>(m_strMonstertag, "isDetected", false);
           m_pFSM->Change_State(ENUM_CLASS(GOMDOL_STATE::SLEEP), pGomdol);
           
       }
   }

    if (m_isActiveController)
    {
        m_pPerception->Update(pOwner, m_pBB, fTimeDelta);
        Update_Aggro(pOwner, fTimeDelta);

        _float fPervTime = m_pBB->Get_Value<_float>(m_strMonstertag, "CurrentTime");

        if (m_pBB->Get_Value<_bool>(m_strMonstertag, "HasAggro"))
            m_pBB->Set_Value<_float>(m_strMonstertag, "CurrentTime", fPervTime + fTimeDelta);
        else
            m_pBB->Set_Value(m_strMonstertag, "CurrentTime", 0.f);


        if (!m_pBB->Get_Value<_bool>(m_strMonstertag, "isDeadFinished"))
            m_pBT->Update();
      
    }
    m_pFSM->Update(pOwner, fTimeDelta);
}


void CAI_Controller_Gomdol::Update_Aggro(CGameObject* pOwner, _float fTimeDelta)
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



HRESULT CAI_Controller_Gomdol::Ready_Perception(CGameObject* pOwner, const AIPERCEPTION_DATA& Desc)
{
    m_pPerception = CPerception::Create(Desc, ENUM_CLASS(TEAM::BEAST));
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

HRESULT CAI_Controller_Gomdol::Ready_BlackBoard(CGameObject* pOwner)
{
    m_pBB = CBlackBoard::Create();
    if (nullptr == m_pBB)
        return E_FAIL;

    return S_OK;
}

HRESULT CAI_Controller_Gomdol::Ready_BehaviorTree()
{
    if (nullptr == m_pBB)
        return E_FAIL;

    m_pBT->Set_BlackBoard(m_pBB);

    if (nullptr == m_pBT)
        return E_FAIL;

    return S_OK;
}

CONDITION CAI_Controller_Gomdol::GetCallbackCondition(CGameObject* pOwner, const string& name)
{
    CGomdol* pGomdol = static_cast<CGomdol*>(pOwner);
    if (nullptr == pGomdol)
        return nullptr;


    if ("Dead" == name)
    {
        return [pGomdol](CBlackBoard* BB)->_bool
            {

                BB->Set_Value<_bool>(pGomdol->Get_Name(), "DamageInterrupt", false);

                if (pGomdol->Get_CurrentHP() <= 0.f)
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
        return [pGomdol](CBlackBoard* BB) -> _bool
            {

                if (!BB->Get_Value<_bool>(pGomdol->Get_Name(), "isHit"))
                {
                    // DamageType 체크
                    HITREACTION eHitRection = static_cast<HITREACTION>(
                        BB->Get_Value<_uint>(pGomdol->Get_Name(), "DamageType"));

                    if (eHitRection == HITREACTION::KNOCKBACK_WEAK ||
                        eHitRection == HITREACTION::KNOCKBACK_NORMAL ||
                        eHitRection == HITREACTION::KNOCKBACK_STRONG ||
                        eHitRection == HITREACTION::BRUTAL_ATTACK)
                    {
                        return true;
                    }
                    return false;
                }
                return false;
            };
    }


#pragma endregion



#pragma region SLEEP SEQUENCE

    if ("Sleep" == name)
    {
        return [pGomdol](CBlackBoard* BB)->_bool
            {

                if (!BB->Get_Value<_bool>(pGomdol->Get_Name(), "isSleep"))
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

    else if ("FrontAttack" == name)
    {
        return [pGomdol](CBlackBoard* BB)->_bool
            {
                _float fDist = BB->Get_Value<_float>(pGomdol->Get_Name(), "TargetDist");
                _float fAttackRanage = BB->Get_Value<_float>(pGomdol->Get_Name(), "AttackRange");

                if (fDist <= fAttackRanage)
                {

                    _bool isDamaged = BB->Get_Value<_bool>(pGomdol->Get_Name(), "DamageInterrupt");
                    if (isDamaged)
                        return false;

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
        return [pGomdol](CBlackBoard* BB)->_bool
            {

                if (BB->Get_Value<_bool>(pGomdol->Get_Name(), "isDead")) return false;

                _float fDist = BB->Get_Value<_float>(pGomdol->Get_Name(), "TargetDist");
                _float fChaseRange = BB->Get_Value<_float>(pGomdol->Get_Name(), "ChaseRange");
                _float fRunRange = BB->Get_Value<_float>(pGomdol->Get_Name(), "RunRange");

                CGomdol::MONSTER_INFO Info{};
                Info.Clear_State();

            
                if (fDist <= fChaseRange)
                {

                    return true;
                }
                else
                    return false;
            };
    }
#pragma endregion

    return nullptr;
}

ACTION CAI_Controller_Gomdol::GetCallbackAction(CGameObject* pOwner, const string& name)
{
    CGomdol* pGomdol = static_cast<CGomdol*>(pOwner);
    if (nullptr == pGomdol)
        return nullptr;



#pragma region HIT SEQUENCE

    if ("Hit" == name)
    {
        return [pGomdol](CBlackBoard* BB)->BTNODESTATE
            {
                // 애니 종료 플래그가 true면 SUCCESS
                if (true == BB->Get_Value<_bool>(pGomdol->Get_Name(), "isHitFinished"))
                {
                    BB->Set_Value<_bool>(pGomdol->Get_Name(), "DamageInterrupt", false);
                    BB->Set_Value<_uint>(pGomdol->Get_Name(), "DamageType", ENUM_CLASS(HITREACTION::NONE));
                    return BTNODESTATE::SUCCESS;
                }

                BB->Set_Value(pGomdol->Get_Name(), "isHit", true);
                BB->Set_Value(pGomdol->Get_Name(), "isHitFinished", false);

                pGomdol->Get_Controller()->Get_State_Machine()->Change_State(
                    ENUM_CLASS(GOMDOL_STATE::HIT), pGomdol);

                return BTNODESTATE::RUNNING;

            };
    }


#pragma endregion


#pragma region SLEEP SEQUENCE

    if ("Sleep" == name)
    {
        return [pGomdol](CBlackBoard* BB)->BTNODESTATE
            {
                if (BB->Get_Value<_bool>(pGomdol->Get_Name(), "isSleepFinished"))
                {
       
                    return BTNODESTATE::SUCCESS; 
                }
                BB->Set_Value<_bool>(pGomdol->Get_Name(), "isSleep", true);
                pGomdol->Get_Controller()->Get_State_Machine()->Change_State(ENUM_CLASS(GOMDOL_STATE::SLEEP), pGomdol);
                return BTNODESTATE::RUNNING;
            };
    }

#pragma endregion


#pragma region ATTACK SELECTOR

    else if ("FrontAttack" == name)
    {
        return [pGomdol](CBlackBoard* BB)-> BTNODESTATE
            {
             
                _bool isDamaged = BB->Get_Value<_bool>(pGomdol->Get_Name(), "DamageInterrupt");
                if (isDamaged)
                    return BTNODESTATE::FAILURE;

                if (BB->Get_Value<_bool>(pGomdol->Get_Name(), "isFrontAttackFinished"))
                {
                    return BTNODESTATE::SUCCESS;
                }

                BB->Set_Value(pGomdol->Get_Name(), "isFrontAttack", true);

                pGomdol->Get_Controller()->Get_State_Machine()->
                    Change_State(ENUM_CLASS(GOMDOL_STATE::ATTACK), pGomdol);
                return BTNODESTATE::RUNNING;


            };
    }
   
#pragma endregion


#pragma region NON ATTACK SELECTOR
    if ("Move" == name)
    {
        return [pGomdol](CBlackBoard* BB)->BTNODESTATE
            {
                _bool isDamaged = BB->Get_Value<_bool>(pGomdol->Get_Name(), "DamageInterrupt");
                if (isDamaged)
                    return BTNODESTATE::FAILURE;

                if (BB->Get_Value<_float>(pGomdol->Get_Name(), "TargetDist") <=
                    BB->Get_Value<_float>(pGomdol->Get_Name(), "AttackRange"))
                    return BTNODESTATE::SUCCESS;

                _float fDist = BB->Get_Value<_float>(pGomdol->Get_Name(), "TargetDist");
                _float fChaseRange = BB->Get_Value<_float>(pGomdol->Get_Name(), "ChaseRange");
                _float fRunRange = BB->Get_Value<_float>(pGomdol->Get_Name(), "RunRange");

                CGomdol::MONSTER_INFO Info{};
                Info.Clear_State();

                if (fDist <= fRunRange)
                    Info.Add_State(Info.WALK);
                else
                    Info.Add_State(Info.RUN);
                

                BB->Set_Value<_uint>(pGomdol->Get_Name(), "iMovementFlag", Info.iStateFlag);

                pGomdol->Get_Controller()->Get_State_Machine()->Change_State(ENUM_CLASS(GOMDOL_STATE::MOVE), pGomdol);

                return BTNODESTATE::RUNNING;
            };
    }

    if ("Idle" == name)
    {
        return [pGomdol](CBlackBoard* BB)->BTNODESTATE
            {
                if (BB->Get_Value<_bool>(pGomdol->Get_Name(), "isDead")) return BTNODESTATE::FAILURE;
                _bool isDamaged = BB->Get_Value<_bool>(pGomdol->Get_Name(), "DamageInterrupt");
                if (isDamaged)
                    return BTNODESTATE::FAILURE;

                pGomdol->Get_Controller()->Get_State_Machine()->Change_State(ENUM_CLASS(GOMDOL_STATE::IDLE), pGomdol);
                return BTNODESTATE::RUNNING;
            };
    }


#pragma endregion


    return nullptr;
}

TERMINATE CAI_Controller_Gomdol::GetCallbackTeminate(CGameObject* pOwner, const string& name)
{
    CGomdol* pGomdol = static_cast<CGomdol*>(pOwner);
    if (nullptr == pGomdol)
        return nullptr;


#pragma region HIT SEQUENCE

    if ("Hit" == name)
    {
        return [pGomdol](CBlackBoard* BB, BTNODESTATE eState)
            {
                if (nullptr == BB)
                    return;

                if (eState == BTNODESTATE::SUCCESS || eState == BTNODESTATE::FAILURE)
                {
                    BB->Set_Value<_bool>(pGomdol->Get_Name(), "isHit", false);
                    BB->Set_Value<_bool>(pGomdol->Get_Name(), "isHitFinished", false);
                    BB->Set_Value<_bool>(pGomdol->Get_Name(), "DamageInterrupt", false);

                }
            };
    }

#pragma endregion



#pragma region SLEEP SEQUENCE

    else if ("Sleep" == name)
   {
       return [pGomdol](CBlackBoard* BB, BTNODESTATE eState)
           {
               if (nullptr == BB)
                   return;

               if (eState == BTNODESTATE::SUCCESS || eState == BTNODESTATE::FAILURE)
               {
                   BB->Set_Value<_bool>(pGomdol->Get_Name(), "isSleepFinished", false);

               }
           };
   }

#pragma endregion


#pragma region ATTACK SELECTOR

    else if ("FrontAttack" == name)
    {
        return [pGomdol](CBlackBoard* BB, BTNODESTATE eState)
            {
                if (nullptr == BB)
                    return;

                if (eState == BTNODESTATE::SUCCESS || eState == BTNODESTATE::FAILURE)
                {
                    BB->Set_Value<_bool>(pGomdol->Get_Name(), "isFrontAttack", false);
                    BB->Set_Value<_bool>(pGomdol->Get_Name(), "isFrontAttackFinished", false);
                }
            };
    }

#pragma endregion


#pragma region NON ATTACK SELECTOR


    else if ("Move" == name)
    {
        return [pGomdol](CBlackBoard* BB, BTNODESTATE eState)
            {
                if (nullptr == BB)
                    return;

                if (eState == BTNODESTATE::SUCCESS || eState == BTNODESTATE::FAILURE)
                {
                }
            };
    }


    return nullptr;

#pragma endregion
}

INTERRUPTCONDITION CAI_Controller_Gomdol::GetCallbackInterruptCondition(CGameObject* pOwner, const string& name)
{
    CGomdol* pGomdol = static_cast<CGomdol*>(pOwner);
    if (nullptr == pGomdol)
        return nullptr;

    if("Root_Interrupt" == name)
    {
        return [pGomdol](CBlackBoard* BB)
            {
                if (BB->Get_Value<_bool>(pGomdol->Get_Name(), "isDead"))
                    return true;
                if (BB->Get_Value<_bool>(pGomdol->Get_Name(), "DamageInterrupt"))
                    return true;

                return false;
            };
    }

    return nullptr;
}

PERCEPTIONCALLBACK CAI_Controller_Gomdol::GetCallBackPerception(CGameObject* pOwner, const string& name)
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
                    else
                    {
                        m_pPerception->Reset_Fov();
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
                        m_pBB->Set_Value(m_strMonstertag, "isDetected", true);
                    }
                }
            };
    };

    return nullptr;
}

CAI_Controller_Gomdol* CAI_Controller_Gomdol::Create(CCreature* pOwner)
{
    CAI_Controller_Gomdol* pInstance = new CAI_Controller_Gomdol();
    if (FAILED(pInstance->Initialize(pOwner)))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Failed Create : CAI_Controller_Gomdol"));
    }

    return pInstance;
}

void CAI_Controller_Gomdol::Free()
{
    __super::Free();
}
