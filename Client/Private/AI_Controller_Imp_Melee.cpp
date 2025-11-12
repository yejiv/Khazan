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
    m_pPerception->Update(pOwner, fTimeDelta);

    _float fPervTime = m_pBB->Get_Value<_float>(m_strMonstertag, "CurrentTime");
    if (m_pBB->Get_Value<_bool>(m_strMonstertag, "isDetected"))
        m_pBB->Set_Value<_float>(m_strMonstertag, "CurrentTime", fPervTime + fTimeDelta);
    else
        m_pBB->Set_Value(m_strMonstertag, "CurrentTime", 0.f);

    m_pBT->Update();

    m_pFSM->Update(pOwner, fTimeDelta);
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
                cb(pTarget, Stim);
            }
        });

    return S_OK;

}

HRESULT CAI_Controller_Imp_Melee::Ready_BlackBoard(CGameObject* pOwner)
{
    m_pBB = m_pGameInstance->Get_BlackBoard();
    if (nullptr == m_pBB)
        return E_FAIL;

    return S_OK;
}

HRESULT CAI_Controller_Imp_Melee::Ready_BehaviorTree()
{
    if (nullptr == m_pBB)
        return E_FAIL;

    m_pBT->Set_BlackBoard(m_pBB);

    if (nullptr == m_pBT)
        return E_FAIL;

    return S_OK;
}

CONDITION CAI_Controller_Imp_Melee::GetCallbackCondition(CGameObject* pOwner, const string& name)
{
    CImp_Melee* pImp = static_cast<CImp_Melee*>(pOwner);
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






#pragma endregion


#pragma region NON ATTACK SELECTOR

    if ("Move" == name)
    {
        return [pImp](CBlackBoard* BB)->_bool
            {
                cout << "MoveCondition" << endl;

                _float fDist = BB->Get_Value<_float>(pImp->Get_Name(), "TargetDist");
                _float fChaseRange = BB->Get_Value<_float>(pImp->Get_Name(), "ChaseRange");

                if (fDist <= fChaseRange)
                    return true;
                else
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
                pImp->Get_Controller()->Get_State_Machine()->Change_State(ENUM_CLASS(IMPMELEE_STATE::SLEEP), pImp);
                return BTNODESTATE::RUNNING;
            };
    }

#pragma endregion

#pragma region ATTACK SELECTOR




#pragma endregion


#pragma region NON ATTACK SELECTOR

    if ("Move" == name)
    {
        return [pImp](CBlackBoard* BB)->BTNODESTATE
            {
                pImp->Get_Controller()->Get_State_Machine()->Change_State(ENUM_CLASS(IMPMELEE_STATE::MOVE), pImp);

                _float fDist = BB->Get_Value<_float>(pImp->Get_Name(), "TargetDist");
                _float fAttackRange = BB->Get_Value<_float>(pImp->Get_Name(), "AttackRange");

                if (fDist <= fAttackRange - 0.5f)
                    return BTNODESTATE::SUCCESS;

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

                pImp->Get_Controller()->Get_State_Machine()->Change_State(ENUM_CLASS(IMPMELEE_STATE::IDLE), pImp);
                cout << "IDLERunning" << endl;

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

#pragma region SLEEP SEQUENCE

    if ("Sleep" == name)
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

#pragma region ATTACK SELECTOR



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
                    pImp->Get_Controller()->Get_State_Machine()->Change_State(ENUM_CLASS(IMPMELEE_STATE::IDLE), pImp);
                }
            };
    }
    return nullptr;

#pragma endregion
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
                _bool isDead = BB->Get_Value<_bool>(pImp->Get_Name(), "isDead");
                _bool isDamaged = BB->Get_Value<_bool>(pImp->Get_Name(), "DamageInterrupt");
                _bool isGroggy = BB->Get_Value<_bool>(pImp->Get_Name(), "isGroogy");

                if (isDead) return true;

                if (isGroggy) return true;

                if (isDamaged) return true;

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
