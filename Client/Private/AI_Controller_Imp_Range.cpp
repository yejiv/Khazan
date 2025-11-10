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
    m_pPerception->Update(pOwner, fTimeDelta);

    _float fPervTime = m_pBB->Get_Value<_float>(pOwner->Get_Name(), "CurrentTime");
    if (m_pBB->Get_Value<_bool>(pOwner->Get_Name(), "isDetected"))
        m_pBB->Set_Value<_float>(pOwner->Get_Name(), "CurrentTime", fPervTime + fTimeDelta);
    else
        m_pBB->Set_Value(m_strMonstertag, "CurrentTime", 0.f);

    m_pBT->Update();

    m_pFSM->Update(pOwner, fTimeDelta);
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
    m_pBB = m_pGameInstance->Get_BlackBoard();
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

#pragma region SLEEP SEQUENCE

    if ("Sleep" == name)
    {
        return [pImp](CBlackBoard* BB)->_bool
            {

                if (!BB->Get_Value<_bool>(pImp->Get_Name(), "isSleep"))
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

  


    else if ("Boomarang" == name)
    {
        return [pImp](CBlackBoard* BB)->_bool
            {
                cout << "FrontAttack Condition" << endl;

                _float fDist = BB->Get_Value<_float>(pImp->Get_Name(), "TargetDist");
                _float fAttackRanage = BB->Get_Value<_float>(pImp->Get_Name(), "BoomarangRange");

                if (fDist <= fAttackRanage && !BB->Get_Value<_bool>(pImp->Get_Name(), "isBoomarang"))
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

                if (fDist <= fAttackRanage && !BB->Get_Value<_bool>(pImp->Get_Name(), "isMagic"))
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
        return [pImp](CBlackBoard* BB)->_bool
            {
                _float fDist = BB->Get_Value<_float>(pImp->Get_Name(), "TargetDist");
                _float fChaseRange = BB->Get_Value<_float>(pImp->Get_Name(), "ChaseRange");
                _float fRunRange = BB->Get_Value<_float>(pImp->Get_Name(), "RunRange");

                CImp_Range::MONSTER_INFO Info{};
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

ACTION CAI_Controller_Imp_Range::GetCallbackAction(CGameObject* pOwner, const string& name)
{
    CImp_Range* pImp = static_cast<CImp_Range*>(pOwner);
    if (nullptr == pImp)
        return nullptr;


#pragma region SLEEP SEQUENCE

    if ("Sleep" == name)
    {
        return [pImp](CBlackBoard* BB)->BTNODESTATE
            {
                if (BB->Get_Value<_bool>(pImp->Get_Name(), "isSleepFinished"))
                {

                    return BTNODESTATE::SUCCESS;
                }
                BB->Set_Value<_bool>(pImp->Get_Name(), "isSleep", true);
                pImp->Get_Controller()->Get_State_Machine()->Change_State(ENUM_CLASS(IMPRANGE_STATE::SLEEP), pImp);
                return BTNODESTATE::RUNNING;
            };
    }

#pragma endregion


#pragma region ATTACK SELECTOR

    else if ("Boomarang" == name)
    {
        return [pImp](CBlackBoard* BB)-> BTNODESTATE
            {

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
             
                if (BB->Get_Value<_bool>(pImp->Get_Name(), "isMagicFinished"))
                {
                    return BTNODESTATE::SUCCESS;
                }

                BB->Set_Value(pImp->Get_Name(), "isMagicAttack", true);

                pImp->Get_Controller()->Get_State_Machine()->
                    Change_State(ENUM_CLASS(IMPRANGE_STATE::MAGIC), pImp);
                return BTNODESTATE::RUNNING;

            };
    }

#pragma endregion


#pragma region NON ATTACK SELECTOR

    if ("Move" == name)
    {
        return [pImp](CBlackBoard* BB)->BTNODESTATE
            {
                _bool isDamaged = BB->Get_Value<_bool>(pImp->Get_Name(), "DamageInterrupt");
                if (isDamaged)
                    return BTNODESTATE::SUCCESS;

                if (BB->Get_Value<_float>(pImp->Get_Name(), "TargetDist") <=
                    BB->Get_Value<_float>(pImp->Get_Name(), "AttackRange"))
                    return BTNODESTATE::SUCCESS;

                _float fDist = BB->Get_Value<_float>(pImp->Get_Name(), "TargetDist");
                _float fChaseRange = BB->Get_Value<_float>(pImp->Get_Name(), "ChaseRange");
                _float fRunRange = BB->Get_Value<_float>(pImp->Get_Name(), "RunRange");

                /*  cout << "DIST: " << fDist << endl;
              cout << "ChaseRange: " << fChaseRange << endl;
              cout << "AttackRange: " << fRunRange << endl;*/

                CImp_Range::MONSTER_INFO Info{};
                Info.Clear_State();

                if (fDist <= fRunRange)
                    Info.Add_State(Info.WALK);
                else
                    Info.Add_State(Info.RUN);


                BB->Set_Value<_uint>(pImp->Get_Name(), "iMovementFlag", Info.iStateFlag);

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

#pragma region SLEEP SEQUENCE

    if ("Sleep" == name)
    {
        return [pImp](CBlackBoard* BB, BTNODESTATE eState)
            {
                if (nullptr == BB)
                    return;

                if (eState == BTNODESTATE::SUCCESS || eState == BTNODESTATE::FAILURE)
                {
                    BB->Set_Value<_bool>(pImp->Get_Name(), "isSleepFinished", false);

                }
            };
            }

#pragma endregion

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

    if ("DamagedInterrupt" == name)
    {
        return [pImp](CBlackBoard* BB)
            {
                _bool isDamaged = BB->Get_Value<_bool>(pImp->Get_Name(), "DamageInterrupt");
                if (isDamaged)
                {
                    return true;
                }
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
                    }
                    else
                    {
                        m_pBB->Set_Value(m_strMonstertag, "isDetected", false);
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
