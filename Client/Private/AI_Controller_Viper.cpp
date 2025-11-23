#include "AI_Controller_Viper.h"
#include "Viper.h"
#include "Perception.h"
#include "BlackBoard.h"
#include "BehaviorTree.h"
#include "FSM_Viper.h"
#include "UtilityScore.h"


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

    if (FAILED(Ready_CoolDown()))
        return E_FAIL;

    return S_OK;
}

void CAI_Controller_Viper::Update(CGameObject* pOwner, _float fTimeDelta)
{
    m_pPerception->Update(pOwner, m_pBB, fTimeDelta);
    _float fPrevTime = m_pBB->Get_Value<_float>(m_strMonstertag, "CurrentTime");


  
    if (m_pBB->Get_Value<_bool>(m_strMonstertag, "isDetected"))
    {

        for (auto& pair : m_SkillCoolDowns)
        {
            const string& strSkill = pair.first;
            SKILLCOOLDOWN& CoolDown = pair.second;

            CoolDown.Update(fTimeDelta);
            string strRemainKey = strSkill + "_CD_Remain";
            m_pBB->Set_Value(m_strMonstertag, strRemainKey, CoolDown.fRemain);
        }
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


#pragma region NONCOMBAT

    else if ("P1_Run" == name)
    {
        return [pViper](CBlackBoard* BB) ->_bool
            {
              // 뛰는 거리, 되면 무조건 뛰어온다.
              // Walk Animation에서 발자국수가 4번 이상이 되면 들어오도록 해야함
              // 여기서 체크하면 Walk는 셀렉터에 의해서 안들어가는 형태 
                _float fDist = BB->Get_Value<_float>(pViper->Get_Name(), "TargetDist");
                _float fRunRange = BB->Get_Value<_float>(pViper->Get_Name(), "RunRange");
                _uint iStepCnt = BB->Get_Value<_uint>(pViper->Get_Name(), "WalkStepCount");

                if (fDist <= fRunRange || iStepCnt >= 4)
                    return true;

                return false;
            };
    }

    else if ("P1_Walk" == name)
    {
        return [pViper](CBlackBoard* BB) ->_bool
            {
                // 뛰는거리에서 걷는 거리 사이에 있으면 Walk 대신 발자국수가 4번 이상 올라가면 Run으로 State가 바껴야함
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

    else if ("Hit" == name)
    {
        return [pViper](CBlackBoard* BB) -> BTNODESTATE
            {

                // 애니 종료 플래그가 true면 SUCCESS
                if (true == BB->Get_Value<_bool>(pViper->Get_Name(), "isHitFinished"))
                {
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

#pragma region COMBAT


    else if ("P1_LockOn" == name)
    {
        return [pViper, this](CBlackBoard* BB)->BTNODESTATE
            {
                if (BB->Get_Value<_bool>(pViper->Get_Name(), "isP1_LockOnFinished"))
                {
                    return BTNODESTATE::SUCCESS;
                }

                SKILLCOOLDOWN& cd = this->m_SkillCoolDowns["P1_LockOn"];
                cd.Consume();

                pViper->Get_Controller()->Get_State_Machine()->Change_State(
                    ENUM_CLASS(VIPER_STATE_P1::LOCKON), pViper);

                return BTNODESTATE::RUNNING;
            };
    }


    else if ("P1_Slow2Hit" == name)
    {
        return [pViper, this](CBlackBoard* BB)->BTNODESTATE
            {
                if (BB->Get_Value<_bool>(pViper->Get_Name(), "isSlow2HitFinished"))
                {
                    return BTNODESTATE::SUCCESS;
                }

                SKILLCOOLDOWN& CoolDown = this->m_SkillCoolDowns["Slow2Hit"];
                CoolDown.Consume();

                BB->Set_Value(pViper->Get_Name(), "Slow2Hit_CD_Remain", CoolDown.fRemain);

                pViper->Get_Controller()->Get_State_Machine()->Change_State(ENUM_CLASS(VIPER_STATE_P1::SLOW2HIT), pViper);

                return BTNODESTATE::RUNNING;

            };
    }


    else if ("P1_Quick2Hit" == name)
    {
        return [pViper,this](CBlackBoard* BB)->BTNODESTATE
            {
                if (BB->Get_Value<_bool>(pViper->Get_Name(), "isQuick2HitFinished"))
                {
                    return BTNODESTATE::SUCCESS;
                }

                SKILLCOOLDOWN& CoolDown = this->m_SkillCoolDowns["Quick2Hit"];
                CoolDown.Consume();

                BB->Set_Value(pViper->Get_Name(), "Quick2Hit_CD_Remain", CoolDown.fRemain);

                pViper->Get_Controller()->Get_State_Machine()->Change_State(ENUM_CLASS(VIPER_STATE_P1::QUICK2HIT),pViper);

                return BTNODESTATE::RUNNING;

            };
    }
#pragma endregion



#pragma region NONCOMBAT

    else if ("P1_Walk" == name)
    {
        return [pViper](CBlackBoard* BB) ->BTNODESTATE
            {
                _float fDist = BB->Get_Value<_float>(pViper->Get_Name(), "TargetDist");
                _float fWalkRange = BB->Get_Value<_float>(pViper->Get_Name(), "WalkRange");
                _uint iStepCnt =  BB->Get_Value<_uint>(pViper->Get_Name(), "WalkStepCount");

                if (iStepCnt >= 4)
                    return BTNODESTATE::SUCCESS;

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

#pragma region COMBAT
   

    else if ("P1_LockOn" == name)
    {
        return [pViper](CBlackBoard* BB, BTNODESTATE eState)
            {
                if (nullptr == BB)
                    return;

                if (eState == BTNODESTATE::SUCCESS || eState == BTNODESTATE::FAILURE)
                {
                    BB->Set_Value<_uint>(pViper->Get_Name(), "LastSkill", ENUM_CLASS(VIPER_SKILL::LOCKON));
                    BB->Set_Value<_bool>(pViper->Get_Name(), "isP1_LockOnFinished", false);
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
                    BB->Set_Value<_uint>(pViper->Get_Name(), "LastSkill", ENUM_CLASS(VIPER_SKILL::SLOW2HIT));
                    BB->Set_Value<_bool>(pViper->Get_Name(), "isSlow2HitFinished", false);
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
                    BB->Set_Value<_uint>(pViper->Get_Name(), "LastSkill", ENUM_CLASS(VIPER_SKILL::QUICK2HIT));
                    BB->Set_Value<_bool>(pViper->Get_Name(), "isQuick2HitFinished", false);
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
                    BB->Set_Value<_bool>(pViper->Get_Name(), "isWalk", false);
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

SCORE CAI_Controller_Viper::GetCallbackScore(CGameObject* pOwner, const string& name)
{
    CViper* pViper = static_cast<CViper*>(pOwner);
    if (nullptr == pViper)
        return nullptr;

    if ("P1_LockOn" == name)
    {
        return [pViper](CBlackBoard* BB)->_float
            {
               
                _float fDist = BB->Get_Value<_float>(pViper->Get_Name(), "TargetDist");
                _float fLockOnRange = BB->Get_Value<_float>(pViper->Get_Name(), "LockOnRange");
                _float fChaseRange = BB->Get_Value<_float>(pViper->Get_Name(), "ChaseRange");
                _float fCoolDownRemain = BB->Get_Value<_float>(pViper->Get_Name(), "P1_LockOn_CD_Remain");
                _float fCoolDownMax = BB->Get_Value<_float>(pViper->Get_Name(), "P1_LockOn_CD_Max");
                _uint iLastSkill = BB->Get_Value<_uint>(pViper->Get_Name(), "LastSkill");
                _float fDot = BB->Get_Value<_float>(pViper->Get_Name(), "fDot");

                if (fCoolDownRemain > 0.f)
                    return 0.f;


                _float fDistScore = UtilityScore::DistanceScore(fDist, fLockOnRange, fChaseRange);
                _float fCoolDownScore = UtilityScore::Utility_Remap(fCoolDownRemain, 0.f, fCoolDownMax, true);
                _float fAngleScore = UtilityScore::Utility_Remap(fDot, 0.f, 1.f);
                _float fChainPenalty = (iLastSkill == ENUM_CLASS(VIPER_SKILL::LOCKON)) ? 0.5f : 1.f;
                _float fWeight = 0.5f;

                _float finalScore =  fAngleScore * fCoolDownScore * fWeight * fChainPenalty;
                return UtilityScore::Utility_Clamp(finalScore);
            };
    }

    else if ("P1_Slow2Hit" == name)
    {
        return [pViper](CBlackBoard* BB)->_float
            {
                _float fDist = BB->Get_Value<_float>(pViper->Get_Name(), "TargetDist");
                _float fAttackRange = BB->Get_Value<_float>(pViper->Get_Name(), "AttackRange");
                _float fChaseRange = BB->Get_Value<_float>(pViper->Get_Name(), "ChaseRange");
                _float fCoolDownRemain = BB->Get_Value<_float>(pViper->Get_Name(), "Slow2Hit_CD_Remain");
                _float fCoolDownMax = BB->Get_Value<_float>(pViper->Get_Name(), "Slow2Hit_CD_Max");
                _uint iLastSkill = BB->Get_Value<_uint>(pViper->Get_Name(), "LastSkill");
                _float fDistanceScore = UtilityScore::DistanceScore(fDist, 0.f, fAttackRange);
                _float fCoolDownScore = UtilityScore::Utility_Remap(fCoolDownRemain, 0.f, fCoolDownMax, true);

                const _float fWeight = 1.1f;
                _float fChainPenalty = (iLastSkill == ENUM_CLASS(VIPER_SKILL::SLOW2HIT)) ? 0.5f : 1.f;

                _float fFinalScore = fDistanceScore * fCoolDownScore * fWeight * fChainPenalty;

                return UtilityScore::Utility_Clamp(fFinalScore);

            };
    }

    else if ("P1_Quick2Hit" == name)
    {
        return [pViper](CBlackBoard* BB)->_float
            {
                _float fDist = BB->Get_Value<_float>(pViper->Get_Name(), "TargetDist");
                _float fAttackRange = BB->Get_Value<_float>(pViper->Get_Name(), "AttackRange");
                _float fChaseRange = BB->Get_Value<_float>(pViper->Get_Name(), "ChaseRange");
                _float fCoolDownRemain = BB->Get_Value<_float>(pViper->Get_Name(), "Quick2Hit_CD_Remain");
                _float fCoolDownMax = BB->Get_Value<_float>(pViper->Get_Name(), "Quick2Hit_CD_Max");
                _uint iLastSkill = BB->Get_Value<_uint>(pViper->Get_Name(), "LastSkill");
                // 거리 점수
                _float fDistanceScore = UtilityScore::DistanceScore(fDist, 0.f, fAttackRange);
                // 쿨타임 점수
                _float fCoolDownScore = UtilityScore::Utility_Remap(fCoolDownRemain, 0.f, fCoolDownMax, true);

                // 가중치
                const _float fWeight = 1.3f; // 기본 공격으로 가중치를 조금 높게 준다.
                // 반복 스킬 이면 페널티
                _float fChainPenalty = (iLastSkill == ENUM_CLASS(VIPER_SKILL::QUICK2HIT)) ? 0.6f : 1.f;
                // 합산
                _float fFinalScore = fDistanceScore * fCoolDownScore * fWeight * fChainPenalty;
                // 정규화
                return UtilityScore::Utility_Clamp(fFinalScore);
                
            };
    }

  
    return nullptr;
}

HRESULT CAI_Controller_Viper::Ready_CoolDown()
{
    m_SkillCoolDowns["Quick2Hit"].Init(2.5f);
    m_SkillCoolDowns["Slow2Hit"].Init(3.5f);
    m_SkillCoolDowns["P1_LockOn"].Init(12.f);
    
    for (auto& pair : m_SkillCoolDowns)
    {
        const string& strSkill = pair.first;
        SKILLCOOLDOWN& CoolDown = pair.second;

        string key = strSkill + "_CD_Max";
        m_pBB->Set_Value(m_strMonstertag, key, CoolDown.fMax);
    }

    return S_OK;
}

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

