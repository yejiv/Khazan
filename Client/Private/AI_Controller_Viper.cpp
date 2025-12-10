#include "AI_Controller_Viper.h"
#include "Viper.h"
#include "Perception.h"
#include "BlackBoard.h"
#include "BehaviorTree.h"
#include "FSM_Viper.h"
#include "UtilityScore.h"
#include "GameInstance.h"
#include "Sequence_Viper_SecondPhase.h"
#include "ClientInstance.h"
#include "BossHp.h"
#include "UI_HUD.h"
#include "SkySphere.h"
#include "CloudSphere.h"

CAI_Controller_Viper::CAI_Controller_Viper()
{
}


HRESULT CAI_Controller_Viper::Initialize(CCreature* pOwner)
{
    CViper* pViper = static_cast<CViper*>(pOwner);

    if (FAILED(__super::Initialize(pViper, pViper->Get_Name())))
        return E_FAIL;

    m_pFSM = CFSM_Viper::Create(pOwner);
    if (nullptr == m_pFSM)
        return E_FAIL;


    m_fAnimSpeed = 1.f;

    return S_OK;
}

void CAI_Controller_Viper::Update(CGameObject* pOwner, _float fTimeDelta)
{
    
    if (m_pGameInstance->Key_Pressing(DIK_RCONTROL, fTimeDelta))
    {
        // 컷신 스테이트 변경
        if (m_pGameInstance->Key_Down(DIK_O))
        {
            CViper* pViper = static_cast<CViper*>(pOwner);
            
            m_pFSM->Change_State(ENUM_CLASS(VIPER_STATE_P1::CUTSCENE_START), pViper);

        }

        if (m_pGameInstance->Key_Down(DIK_P))
        {
            CViper* pViper = static_cast<CViper*>(pOwner);
            m_pFSM->Change_State(ENUM_CLASS(VIPER_STATE_P1::CUTSCENE_PHASE2), pViper);

        }
    }

    if (m_pGameInstance->Key_Down(DIK_Y))
    {

        CViper* pViper = static_cast<CViper*>(pOwner);
        CGameObject* pTarget = m_pBB->Get_Value<CGameObject*>(m_strMonstertag, "Target");
        //pViper->Take_Damage(10.f,HITREACTION::KNOCKBACK_WEAK,pTarget);
        //pViper->Consume_Stamina(10.f);
        //m_pFSM->Change_State(ENUM_CLASS(VIPER_STATE_P1::CUTSCENE_START), pViper);
                
        // 버서커 시작 셰이더 세팅, 카메라 쉐이킹
        CClientInstance::GetInstance()->ActiveCamera_Shaking(2.f, 1.f);
        Set_ViperBerserkerStart_ShaderSettings();

        m_pBB->Set_Value<_bool>(m_strMonstertag, "is_Berserker", true);

    }

    if (m_pGameInstance->Key_Pressing(DIK_RCONTROL, fTimeDelta))
    {

        if (m_pGameInstance->Key_Down(DIK_B))
        {
            //m_pBB->Set_Value<_uint>(m_strMonstertag,"DebugIndex",0);
            CViper* pViper = static_cast<CViper*>(pOwner);
            //pViper->Set_PhaseWeapon_Phase2();
            pViper->Set_Weapon_Phase1();
            m_pFSM->Change_State(ENUM_CLASS(VIPER_STATE_P1::QUICK2HIT), pViper);
        }

        if (m_pGameInstance->Key_Down(DIK_N))
        {
            //m_pBB->Set_Value<_uint>(m_strMonstertag, "DebugIndex", 1);
            CViper* pViper = static_cast<CViper*>(pOwner);
            //pViper->Set_PhaseWeapon_Phase2();
            pViper->Set_Weapon_Phase1();
            m_pFSM->Change_State(ENUM_CLASS(VIPER_STATE_P1::JUMPSMASH), pViper);
        }

        if (m_pGameInstance->Key_Down(DIK_M))
        {
            CViper* pViper = static_cast<CViper*>(pOwner);

            //pViper->Set_PhaseWeapon_Phase2();
            pViper->Set_Weapon_Phase1();
            m_pFSM->Change_State(ENUM_CLASS(VIPER_STATE_P1::COMBO5HIT), pViper);
        }

    }


    if (m_pGameInstance->Key_Down(DIK_J))
    {
        m_isActiveController = true;
        CViper* pViper = static_cast<CViper*>(pOwner);
        m_pFSM->Change_State(ENUM_CLASS(VIPER_STATE_P1::IDLE), pOwner);
    }



    if (m_pBB->Get_Value<_bool>(m_strMonstertag, "is_Berserker"))
    {
        m_fAnimSpeed = 1.2f;
        m_fCurrentTime += fTimeDelta;
        if (m_fCurrentTime >= 30.f)
        {
            m_pBB->Set_Value<_bool>(m_strMonstertag, "is_P2Loar", true);
            m_pBB->Set_Value<_bool>(m_strMonstertag, "is_Berserker", false);
            m_fAnimSpeed = 1.f;
            m_fCurrentTime = 0.f;
        }

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

          //if (!m_pBB->Get_Value<_bool>(m_strMonstertag, "isDeadFinished"))
          //    m_pBT->Update();
    }

    m_pFSM->Update(pOwner, fTimeDelta * m_fAnimSpeed);

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
                    if(pViper->Get_Phase() == CViper::PHASE::PHASE1)
                    BB->Set_Value<_bool>(pViper->Get_Name(), "Phase1Finished", true);
                    return true;
                }
                else
                    return false;
            };
    }

    if ("P2_CutScene" == name)
    {
        return [pViper](CBlackBoard* BB)->_bool
            {
                if (BB->Get_Value<_bool>(pViper->Get_Name(),"Phase1Finished"))
                {
                    pViper->Get_Transform()->Rotation(0, XMConvertToRadians(180.f), 0.f);
                    BB->Set_Value<_bool>(pViper->Get_Name(), "Phase1Finished", false);
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

    else if ("Phase2" == name)
    {
        return [pViper](CBlackBoard* BB) -> _bool
            {

                if (CViper::PHASE::PHASE2 == pViper->Get_Phase())
                    return true;

                return false;
            };
    }

    else if ("P2_Roar" == name)
    {
        return [pViper](CBlackBoard* BB)->_bool
            {
                _bool isRoar = BB->Get_Value<_bool>(pViper->Get_Name(), "is_P2Loar");
                if (isRoar)
                    return true;
                return false;
            };
    }


#pragma endregion


#pragma region LOCKON

    else if ("P2_LockOn" == name)
    {
        return [pViper, this](CBlackBoard* BB)->_bool
            {
                _float fDist = BB->Get_Value<_float>(pViper->Get_Name(), "TargetDist");
                _float fLockOnRange = BB->Get_Value<_float>(pViper->Get_Name(), "LockOnRange");
                _float fAttackRange = BB->Get_Value<_float>(pViper->Get_Name(), "AttackRange");

                _bool isBerserker = BB->Get_Value<_bool>(pViper->Get_Name(), "is_Berserker");

                if (isBerserker)
                    return false;
                
                // 락온 거리 조건
                if (fDist > fAttackRange + 20.f && fDist <= fLockOnRange)
                    return true;

                return false;
            };
    }
#pragma endregion

#pragma region DIR

    else if ("P2_Front" == name)
    {
        return [pViper, this](CBlackBoard* BB)->_bool
            {

                _bool isBerserker = BB->Get_Value<_bool>(pViper->Get_Name(), "is_Berserker");
                if (isBerserker) return false;

                _uint iDir = BB->Get_Value<_uint>(pViper->Get_Name(), "LockDir");
                if (iDir == ENUM_CLASS(DIRECTION::F))
                    return true;

                return false;
            };
    }

    else if ("P2_Left" == name)
    {
        return [pViper, this](CBlackBoard* BB)->_bool
            {

                _bool isBerserker = BB->Get_Value<_bool>(pViper->Get_Name(), "is_Berserker");
                if (isBerserker) return false;

                _uint iDir = BB->Get_Value<_uint>(pViper->Get_Name(), "LockDir");
                if (iDir == ENUM_CLASS(DIRECTION::L))
                    return true;

                return false;
            };
    }

    else if ("P2_Right" == name)
    {
        return [pViper, this](CBlackBoard* BB)->_bool
            {

                _bool isBerserker = BB->Get_Value<_bool>(pViper->Get_Name(), "is_Berserker");
                if (isBerserker) return false;

                _uint iDir = BB->Get_Value<_uint>(pViper->Get_Name(), "LockDir");
                if (iDir == ENUM_CLASS(DIRECTION::R))
                    return true;

                return false;
            };
    }

    else if ("P2_Back" == name)
    {
        return [pViper, this](CBlackBoard* BB)->_bool
            {
                _bool isBerserker = BB->Get_Value<_bool>(pViper->Get_Name(), "is_Berserker");
                if (isBerserker) return false;

                _uint iDir = BB->Get_Value<_uint>(pViper->Get_Name(), "LockDir");
                if (iDir == ENUM_CLASS(DIRECTION::B))
                    return true;

                return false;
            };
    }

#pragma endregion



#pragma region COMBAT_PHASE2


    else if ("P2_BerserkerMode" == name)
    {
        return [pViper, this](CBlackBoard* BB)->_bool
            {

                _bool isBerserker = BB->Get_Value<_bool>(pViper->Get_Name(), "is_Berserker");

                if (!isBerserker)
                    return false;

                return true;
            };

    }

    else if ("P2_SwingRound" == name)
    {
        return [pViper, this](CBlackBoard* BB)->_bool
            {

                _float fHpRatio = pViper->Get_CurrentHP() / pViper->Get_MaxHP();
                if (fHpRatio >= 0.4f)
                    return false;

                _float fDist = BB->Get_Value<_float>(pViper->Get_Name(), "TargetDist");
                _float fChaseRnage = BB->Get_Value<_float>(pViper->Get_Name(), "ChaseRange");

                if (fDist != 0 && fDist <= fChaseRnage)
                {
                    BB->Set_Value(pViper->Get_Name(), "AttackInterrupt", true);
                    return true;
                }
                else
                    return false;
            };
    }

    else if ("P2_BerserkerJump" == name)
    {
        return [pViper, this](CBlackBoard* BB)->_bool
            {

                _bool isBerserker = BB->Get_Value<_bool>(pViper->Get_Name(), "is_Berserker");

                if (!isBerserker)
                    return false;

                _uint iBersekerIndex = BB->Get_Value<_uint>(pViper->Get_Name(), "BerserkerIndex");

                _float fDist = BB->Get_Value<_float>(pViper->Get_Name(), "TargetDist");
                _float fJumpRnage = BB->Get_Value<_float>(pViper->Get_Name(), "JumpAttackRange");
                _float fAttackRange = BB->Get_Value<_float>(pViper->Get_Name(), "AttackRange");

                if (iBersekerIndex == 0 || iBersekerIndex == 2)
                {
                    if (fDist > fAttackRange + 10.f && fDist <= fJumpRnage)
                    {
                        _float fChance = m_pGameInstance->Rand(0.f, 1.f);

                        if (fChance >= 0)
                            BB->Set_Value<_uint>(pViper->Get_Name(), "BerserkerJumpCount", 1);
                        /* else
                             BB->Set_Value<_uint>(pViper->Get_Name(), "BerserkerJumpCount", 0);*/
                        return true;
                    }
                    return false;
                }
                else
                    return false;
        };
    }

    else if ("P2_BersekerDashUpper" == name)
    {
        return [pViper, this](CBlackBoard* BB)->_bool
            {

                _float fDist = BB->Get_Value<_float>(pViper->Get_Name(), "TargetDist");
                _float fDashAttackRange = BB->Get_Value<_float>(pViper->Get_Name(), "DashAttackRange");
                _float fAttackRange = BB->Get_Value<_float>(pViper->Get_Name(), "AttackRange");

                _bool isBerserker = BB->Get_Value<_bool>(pViper->Get_Name(), "is_Berserker");

                if (isBerserker)
                {
                    _uint iBersekerIndex = BB->Get_Value<_uint>(pViper->Get_Name(), "BerserkerIndex");

                    if (iBersekerIndex == 1)
                    {
                        if (fDist != 0 && fDist <= 70.f)
                        {
                            return true;
                        }
                        else
                            return false;
                    }
                }
            };
    }


 
    else if ("P2_DashDrift" == name)
    {
        return [pViper, this](CBlackBoard* BB)->_bool
            {

                _float fHpRatio = pViper->Get_CurrentHP() / pViper->Get_MaxHP();
                if (fHpRatio >= 0.6f)
                    return false;

                _float fDist = BB->Get_Value<_float>(pViper->Get_Name(), "TargetDist");
                _float fJumpAttackRange = BB->Get_Value<_float>(pViper->Get_Name(), "JumpAttackRange");
                _float fAttackRange = BB->Get_Value<_float>(pViper->Get_Name(), "AttackRange");

                if (fDist > fAttackRange + 10.f && fDist <= fJumpAttackRange)
                {
                    DIRECTION_INFO Info = {};
                    Info.iDirFlag = BB->Get_Value<_uint>(pViper->Get_Name(), "TargetDirection");

                    if (Info.Check_Flag(DIRECTION_INFO::DIR::F))
                    {
                        BB->Set_Value(pViper->Get_Name(), "AttackInterrupt", true);
                        return true;
                    }

                    return false;
                }
                else
                    return false;
            };
    }

    else if ("P2_ThrowRock" == name)
    {
        return [pViper, this](CBlackBoard* BB)->_bool
            {

                _float fDist = BB->Get_Value<_float>(pViper->Get_Name(), "TargetDist");
                _float fAttackRange = BB->Get_Value<_float>(pViper->Get_Name(), "AttackRange");
                _float fLockOnRange = BB->Get_Value<_float>(pViper->Get_Name(), "LockOnRange");
                _bool isAttackFinished = BB->Get_Value<_bool>(pViper->Get_Name(), "is_P2_ThrowRockFinished");

                if (fDist >= fAttackRange && fDist <= fLockOnRange)
                {
                    BB->Set_Value(pViper->Get_Name(), "AttackInterrupt", true);
                    return true;
                }
                else
                    return false;
            };
    }

    else if ("P2_SwingCombo" == name)
    {
        return [pViper, this](CBlackBoard* BB)->_bool
            {

                _float fHpRatio = pViper->Get_CurrentHP() / pViper->Get_MaxHP();
                if (fHpRatio >= 0.7f)
                    return false;

                _float fDist = BB->Get_Value<_float>(pViper->Get_Name(), "TargetDist");
                _float fAttackRange = BB->Get_Value<_float>(pViper->Get_Name(), "AttackRange");

                if (fDist != 0 && fDist <= fAttackRange)
                {
                    DIRECTION_INFO Info = {};
                    Info.iDirFlag = BB->Get_Value<_uint>(pViper->Get_Name(), "TargetDirection");

                    if (Info.Check_Flag(DIRECTION_INFO::DIR::F))
                        return true;

                    return false;
                }
                else
                    return false;
            };
    }

    else if ("P2_JumpAttack" == name)
    {
        return [pViper, this](CBlackBoard* BB)->_bool
            {

                _float fHpRatio = pViper->Get_CurrentHP() / pViper->Get_MaxHP();
                if (fHpRatio >= 0.7f)
                    return false;


                _float fDist = BB->Get_Value<_float>(pViper->Get_Name(), "TargetDist");
                _float fRunRange = BB->Get_Value<_float>(pViper->Get_Name(), "RunRange");
                _float fChaseRange = BB->Get_Value<_float>(pViper->Get_Name(), "ChaseRange");
                
                _bool isBerserker = BB->Get_Value<_bool>(pViper->Get_Name(), "is_Berserker");

                if (isBerserker)
                {
                    _uint iBersekerIndex = BB->Get_Value<_uint>(pViper->Get_Name(), "BerserkerIndex");

                    if (iBersekerIndex == 3)
                    {
                        BB->Set_Value(pViper->Get_Name(), "AttackInterrupt", true);
                        return true;
                    }
                    return false;
                }

                else
                {
                    if (fDist > fRunRange && fDist <= fChaseRange)
                    {
                        BB->Set_Value(pViper->Get_Name(), "AttackInterrupt", true);
                        return true;
                    }
                    else
                        return false;
                }
               
        };
    }

    else if ("P2_SideMove" == name)
    {
        return [pViper, this](CBlackBoard* BB)->_bool
            {
                _float fDist = BB->Get_Value<_float>(pViper->Get_Name(), "TargetDist");
                _float fDashAttackRange = BB->Get_Value<_float>(pViper->Get_Name(), "DashAttackRange");
                _bool isAttackFinished = BB->Get_Value<_bool>(pViper->Get_Name(), "is_P2_SideMoveFinished");

                if (fDist != 0 && fDist <= fDashAttackRange)
                {
                    return true;
                }
                else
                    return false;
            };
    }

    else if ("P2_BackJump" == name)
    {
        return [pViper, this](CBlackBoard* BB)->_bool
            {
                _float fDist = BB->Get_Value<_float>(pViper->Get_Name(), "TargetDist");
                _float fAttackRange = BB->Get_Value<_float>(pViper->Get_Name(), "AttackRange");
                _bool isAttackFinished = BB->Get_Value<_bool>(pViper->Get_Name(), "is_P2_BackJumpFinished");

                if (fDist != 0 && fDist <= fAttackRange)
                {
                    return true;
                }
                else
                    return false;
            };
    }

    else if ("P2_DashUpperStr" == name)
    {
        return [pViper, this](CBlackBoard* BB)->_bool
            {

                _float fHpRatio = pViper->Get_CurrentHP() / pViper->Get_MaxHP();
                if (fHpRatio >= 0.7f)
                    return false;

                _float fDist = BB->Get_Value<_float>(pViper->Get_Name(), "TargetDist");
                _float fDashAttackRange = BB->Get_Value<_float>(pViper->Get_Name(), "DashAttackRange");
                _float fAttackRange = BB->Get_Value<_float>(pViper->Get_Name(), "AttackRange");
                _bool isAttackFinished = BB->Get_Value<_bool>(pViper->Get_Name(), "is_P2_DashUpperStrFinished");

                if (fDist > fAttackRange && fDist <= 70.f)
                {
                    return true;
                }
                else
                    return false;
            };
    }

    else if ("P2_DashUpper" == name)
    {
        return [pViper, this](CBlackBoard* BB)->_bool
            {

                _float fDist = BB->Get_Value<_float>(pViper->Get_Name(), "TargetDist");
                _float fDashAttackRange = BB->Get_Value<_float>(pViper->Get_Name(), "DashAttackRange");
                _float fAttackRange = BB->Get_Value<_float>(pViper->Get_Name(), "AttackRange");


                if (fDist != 0 && fDist <= 70.f)
                {
                    return true;
                }
                else
                    return false;
                
        };
    }

    else if ("P2_FakeRunAttack" == name)
    {
        return [pViper, this](CBlackBoard* BB)->_bool
            {

                _float fHpRatio = pViper->Get_CurrentHP() / pViper->Get_MaxHP();
                if (fHpRatio >= 0.6f)
                    return false;

                _float fDist = BB->Get_Value<_float>(pViper->Get_Name(), "TargetDist");
                _float fAttackRange = BB->Get_Value<_float>(pViper->Get_Name(), "RunRange");


                if (fDist != 0 && fDist <= fAttackRange)
                {
                    return true;
                }
                else
                    return false;
            };
    }

    else if ("P2_HandSwing3Hit" == name)
    {
        return [pViper, this](CBlackBoard* BB)->_bool
            {

                _float fDist = BB->Get_Value<_float>(pViper->Get_Name(), "TargetDist");
                _float fAttackRange = BB->Get_Value<_float>(pViper->Get_Name(), "AttackRange");

                if (fDist != 0 && fDist <= fAttackRange)
                {
                    return true;
                }
                else
                    return false;
            };
    }

    else if ("P2_SlashStomp" == name)
    {
        return [pViper, this](CBlackBoard* BB)->_bool
            {
                _float fDist = BB->Get_Value<_float>(pViper->Get_Name(), "TargetDist");
                _float fAttackRange = BB->Get_Value<_float>(pViper->Get_Name(), "AttackRange");

                _bool isAttackFinished = BB->Get_Value<_bool>(pViper->Get_Name(), "is_P2_SlashStompFinished");

                if (fDist != 0 && fDist <= fAttackRange)
                {
                    return true;
                }
                else
                    return false;
            };
    }

    else if ("P2_SlashDouble" == name)
    {
        return [pViper, this](CBlackBoard* BB)->_bool
            {
                _float fDist = BB->Get_Value<_float>(pViper->Get_Name(), "TargetDist");
                _float fAttackRange = BB->Get_Value<_float>(pViper->Get_Name(), "AttackRange");

                _bool isAttackFinished = BB->Get_Value<_bool>(pViper->Get_Name(), "is_P2_SlashDoubleFinished");

                if (fDist != 0 && fDist <= fAttackRange)
                {
                    return true;
                }
                else
                    return false;
            };
    }

    else if ("P2_HandUpper" == name)
    {
        return [pViper, this](CBlackBoard* BB)->_bool
            {
                _float fDist = BB->Get_Value<_float>(pViper->Get_Name(), "TargetDist");
                _float fAttackRange = BB->Get_Value<_float>(pViper->Get_Name(), "AttackRange");

                _bool isAttackFinished = BB->Get_Value<_bool>(pViper->Get_Name(), "is_P2_HandUpperFinished");

                if (fDist != 0 && fDist <= fAttackRange)
                {
                    return true;
                }
                else
                    return false;
            };
    }

    else if ("P2_HandSwing2Hit" == name)
    {
        return [pViper, this](CBlackBoard* BB)->_bool
            {
                _float fDist = BB->Get_Value<_float>(pViper->Get_Name(), "TargetDist");
                _float fAttackRange = BB->Get_Value<_float>(pViper->Get_Name(), "AttackRange");

                _bool isAttackFinished = BB->Get_Value<_bool>(pViper->Get_Name(), "is_P2_HandSwing2HitFinished");

                if (fDist != 0 && fDist <= fAttackRange)
                {
                    return true;
                }
                else
                    return false;
            };
    }

    else if ("P2_HandStompStr" == name)
    {
        return [pViper, this](CBlackBoard* BB)->_bool
            {

                _float fHpRatio = pViper->Get_CurrentHP() / pViper->Get_MaxHP();
                if (fHpRatio >= 0.7f)
                    return false;


                _float fDist = BB->Get_Value<_float>(pViper->Get_Name(), "TargetDist");
                _float fAttackRange = BB->Get_Value<_float>(pViper->Get_Name(), "AttackRange");

                _bool isAttackFinished = BB->Get_Value<_bool>(pViper->Get_Name(), "is_P2_HandStompStrFinished");

                if (fDist != 0 && fDist <= fAttackRange)
                {
                    return true;
                }
                else
                    return false;
            };
    }


    else if ("P2_HandStomp" == name)
    {
        return [pViper, this](CBlackBoard* BB)->_bool
            {
               
                _float fDist = BB->Get_Value<_float>(pViper->Get_Name(), "TargetDist");
                _float fAttackRange = BB->Get_Value<_float>(pViper->Get_Name(), "AttackRange");

                _bool isAttackFinished = BB->Get_Value<_bool>(pViper->Get_Name(), "is_P2_HandStompFinished");

                if (fDist != 0 && fDist <= fAttackRange)
                {
                    return true;
                }
                else
                    return false;
            };
    }


#pragma endregion

#pragma region NONCOMBAT_PAHSE2

    else if ("P2_Run" == name)
    {
        return [pViper](CBlackBoard* BB) ->_bool
            {

                _float fDist = BB->Get_Value<_float>(pViper->Get_Name(), "TargetDist");
                _float fChaseRange = BB->Get_Value<_float>(pViper->Get_Name(), "ChaseRange");
                _float fAttackRange = BB->Get_Value<_float>(pViper->Get_Name(), "AttackRange");
                if ((fDist >= fAttackRange && fDist <= fChaseRange))
                    return true;

                return false;
            };
    }
#pragma endregion


#pragma region COMBAT_PHASE1


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

#pragma region NONCOMBAT_PHASE1

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
                if (pViper->Get_Phase() == CViper::PHASE::PHASE1)
                    return BTNODESTATE::SUCCESS;

                pViper->Get_Controller()->Get_State_Machine()->
                    Change_State(ENUM_CLASS(VIPER_STATE_P1::P2_DEAD), pViper);
                return BTNODESTATE::RUNNING;
            };
    }

    else if ("P2_CutScene" == name)
    {
        return [pViper,this](CBlackBoard* BB)-> BTNODESTATE
            {
              
                if (m_is2PhaseCutSceneFinished)
                {
                    pViper->Set_HP(100.f,100.f);
                    pViper->Set_Stamina(1200.f, 1200.f);

                        static_cast<CUI_HUD*>(CClientInstance::GetInstance()->Get_RootUI(TEXT("HUD")))->Switch_Panel(true);

                        pViper->Set_HPUI(false);

                    return BTNODESTATE::SUCCESS;
                }


                CSequence_Viper_SecondPhase* pSeq = dynamic_cast<CSequence_Viper_SecondPhase*>(CClientInstance::GetInstance()->Find_Sequence(TEXT("Viper_SecondPhase")));
                SEQ_REQ_PLAY_DESC tPlayDesc{};
                tPlayDesc.tId.iSeq = 110010;
                tPlayDesc.pAsset = L"Viper_CutScene";
                tPlayDesc.fStartTime = 0.f;
                CClientInstance::GetInstance()->Remove_Sequence(TEXT("Viper_SecondPhase"));
                m_pGameInstance->SEQ_AdoptAndPlay(pSeq, tPlayDesc, true);

               
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
                    BB->Set_Value<_uint>(pViper->Get_Name(), "DamageType", ENUM_CLASS(HITREACTION::NONE));

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


#pragma region LOCKON

    else if ("P2_LockOn" == name)
    {
        return [pViper](CBlackBoard* BB)->BTNODESTATE
            {
                if (BB->Get_Value<_bool>(pViper->Get_Name(), "isGroggy"))
                    return BTNODESTATE::FAILURE;

                if (BB->Get_Value<_bool>(pViper->Get_Name(), "isP2_LockOn_Finished"))
                {
                    return BTNODESTATE::SUCCESS;
                }

                BB->Set_Value<_bool>(pViper->Get_Name(), "isP2LockOn",true);


                pViper->Get_Controller()->Get_State_Machine()->Change_State(ENUM_CLASS(VIPER_STATE_P1::P2_LOCKON), pViper);

                return BTNODESTATE::RUNNING;
            };
    }
#pragma endregion
    


#pragma region COMBAT_PHASE2

    else if ("P2_Roar" == name)
    {
        return [pViper](CBlackBoard* BB)->BTNODESTATE
            {
                if (BB->Get_Value<_bool>(pViper->Get_Name(), "is_P2_RoarFinished"))
                {
                    // 로어끝
                    return BTNODESTATE::SUCCESS;
                }

                BB->Set_Value(pViper->Get_Name(), "isSuperArmor", true);

                pViper->Get_Controller()->Get_State_Machine()->
                    Change_State(ENUM_CLASS(VIPER_STATE_P1::P2_ROAR), pViper);

                return BTNODESTATE::RUNNING;


            };
    }
   

    else if ("P2_BerserkerJump" == name)
    {

        return [pViper](CBlackBoard* BB)-> BTNODESTATE
            {

                if (BB->Get_Value<_bool>(pViper->Get_Name(), "isGroggy"))
                    return BTNODESTATE::FAILURE;


                if (BB->Get_Value<_bool>(pViper->Get_Name(), "is_P2_BerserkerJumpFinished"))
                {
                    if (BB->Get_Value<_bool>(pViper->Get_Name(), "is_Berserker"))
                    {
                        _uint iBersekerIndex = BB->Get_Value<_uint>(pViper->Get_Name(), "BerserkerIndex");
                        BB->Set_Value<_uint>(pViper->Get_Name(), "BerserkerIndex", iBersekerIndex + 1);
                    }
                    return BTNODESTATE::SUCCESS;
                }

                BB->Set_Value(pViper->Get_Name(), "isSuperArmor", true);

                pViper->Get_Controller()->Get_State_Machine()->
                    Change_State(ENUM_CLASS(VIPER_STATE_P1::P2_BERSERKERJUMP), pViper);

                return BTNODESTATE::RUNNING;
            };
            }




    else if ("P2_SwingRound" == name)
    {

        return [pViper](CBlackBoard* BB)-> BTNODESTATE
            {

                if (BB->Get_Value<_bool>(pViper->Get_Name(), "isGroggy"))
                    return BTNODESTATE::FAILURE;


                if (BB->Get_Value<_bool>(pViper->Get_Name(), "is_P2_SwingRoundFinished"))
                {
                    return BTNODESTATE::SUCCESS;
                }

                BB->Set_Value(pViper->Get_Name(), "isSuperArmor", true);

                pViper->Get_Controller()->Get_State_Machine()->
                    Change_State(ENUM_CLASS(VIPER_STATE_P1::P2_SWINGROUND), pViper);

                return BTNODESTATE::RUNNING;
            };
    }


    else if ("P2_DashDrift" == name)
    {

        return [pViper](CBlackBoard* BB)-> BTNODESTATE
            {

                if (BB->Get_Value<_bool>(pViper->Get_Name(), "isGroggy"))
                    return BTNODESTATE::FAILURE;


                if (BB->Get_Value<_bool>(pViper->Get_Name(), "is_P2_DashDriftFinished"))
                {
                    BB->Set_Value<_bool>(pViper->Get_Name(), "isAttackInterrupt", false);
                    return BTNODESTATE::SUCCESS;
                }

                BB->Set_Value(pViper->Get_Name(), "isSuperArmor", true);

                pViper->Get_Controller()->Get_State_Machine()->
                    Change_State(ENUM_CLASS(VIPER_STATE_P1::P2_DASHDRIFT), pViper);

                return BTNODESTATE::RUNNING;
            };
    }

    else if ("P2_ThrowRock" == name)
    {

        return [pViper](CBlackBoard* BB)-> BTNODESTATE
            {

                if (BB->Get_Value<_bool>(pViper->Get_Name(), "isGroggy"))
                    return BTNODESTATE::FAILURE;


                if (BB->Get_Value<_bool>(pViper->Get_Name(), "is_P2_ThrowRockFinished"))
                {
                    BB->Set_Value<_bool>(pViper->Get_Name(), "isAttackInterrupt", false);
                    return BTNODESTATE::SUCCESS;
                }

                BB->Set_Value(pViper->Get_Name(), "isSuperArmor", true);
                BB->Set_Value<_bool>(pViper->Get_Name(), "isAttackInterrupt", true);

                pViper->Get_Controller()->Get_State_Machine()->
                    Change_State(ENUM_CLASS(VIPER_STATE_P1::P2_THROWROCK), pViper);

                return BTNODESTATE::RUNNING;
            };
    }




    else if ("P2_SwingCombo" == name)
    {

        return [pViper](CBlackBoard* BB)-> BTNODESTATE
            {

                if (BB->Get_Value<_bool>(pViper->Get_Name(), "isGroggy"))
                    return BTNODESTATE::FAILURE;


                if (BB->Get_Value<_bool>(pViper->Get_Name(), "is_P2_SwingComboFinished"))
                {
                    return BTNODESTATE::SUCCESS;
                }

                BB->Set_Value(pViper->Get_Name(), "isSuperArmor", true);

                pViper->Get_Controller()->Get_State_Machine()->
                    Change_State(ENUM_CLASS(VIPER_STATE_P1::P2_SWINGCOMBO), pViper);

                return BTNODESTATE::RUNNING;
            };
    }


    else if ("P2_JumpAttack" == name)
    {

        return [pViper](CBlackBoard* BB)-> BTNODESTATE
            {

                if (BB->Get_Value<_bool>(pViper->Get_Name(), "isGroggy"))
                    return BTNODESTATE::FAILURE;


                if (BB->Get_Value<_bool>(pViper->Get_Name(), "is_P2_JumpAttackFinished"))
                {
                    if (BB->Get_Value<_bool>(pViper->Get_Name(), "is_Berserker"))
                    {
                        _uint iBersekerIndex = BB->Get_Value<_uint>(pViper->Get_Name(), "BerserkerIndex");
                        BB->Set_Value<_uint>(pViper->Get_Name(), "BerserkerIndex", iBersekerIndex + 1);
                    }
                    BB->Set_Value<_bool>(pViper->Get_Name(), "isAttackInterrupt", false);
                    return BTNODESTATE::SUCCESS;
                }

                BB->Set_Value(pViper->Get_Name(), "isSuperArmor", true);
                BB->Set_Value<_bool>(pViper->Get_Name(), "isAttackInterrupt", true);

                pViper->Get_Controller()->Get_State_Machine()->
                    Change_State(ENUM_CLASS(VIPER_STATE_P1::P2_JUMPATTACK), pViper);

                return BTNODESTATE::RUNNING;
            };
    }


    else if ("P2_SideMove" == name)
    {

        return [pViper](CBlackBoard* BB)-> BTNODESTATE
            {

                if (BB->Get_Value<_bool>(pViper->Get_Name(), "isGroggy"))
                    return BTNODESTATE::FAILURE;


                if (BB->Get_Value<_bool>(pViper->Get_Name(), "is_P2_SideMoveFinished"))
                {
                    // AttackInterrupt
                    return BTNODESTATE::SUCCESS;
                }

                BB->Set_Value(pViper->Get_Name(), "isSuperArmor", true);

                pViper->Get_Controller()->Get_State_Machine()->
                    Change_State(ENUM_CLASS(VIPER_STATE_P1::P2_SIDEMOVE), pViper);

                return BTNODESTATE::RUNNING;
            };
    }



    else if ("P2_BackJump" == name)
    {

        return [pViper](CBlackBoard* BB)-> BTNODESTATE
            {

                if (BB->Get_Value<_bool>(pViper->Get_Name(), "isGroggy"))
                    return BTNODESTATE::FAILURE;


                if (BB->Get_Value<_bool>(pViper->Get_Name(), "is_P2_BackJumpFinished"))
                {
                    // AttackInterrupt
                    return BTNODESTATE::SUCCESS;
                }

                BB->Set_Value(pViper->Get_Name(), "isSuperArmor", true);

                pViper->Get_Controller()->Get_State_Machine()->
                    Change_State(ENUM_CLASS(VIPER_STATE_P1::P2_BACKJUMP), pViper);

                return BTNODESTATE::RUNNING;
            };
    }


    else if ("P2_DashUpperStr" == name)
    {

        return [pViper](CBlackBoard* BB)-> BTNODESTATE
            {

                if (BB->Get_Value<_bool>(pViper->Get_Name(), "isGroggy"))
                    return BTNODESTATE::FAILURE;


                if (BB->Get_Value<_bool>(pViper->Get_Name(), "is_P2_DashUpperStrFinished"))
                {
                    // AttackInterrupt
                    return BTNODESTATE::SUCCESS;
                }

                BB->Set_Value(pViper->Get_Name(), "isSuperArmor", true);

                pViper->Get_Controller()->Get_State_Machine()->
                    Change_State(ENUM_CLASS(VIPER_STATE_P1::P2_DASTUPPERSTR), pViper);

                return BTNODESTATE::RUNNING;
            };
    }


    else if ("P2_DashUpper" == name)
    {

        return [pViper](CBlackBoard* BB)-> BTNODESTATE
            {

                if (BB->Get_Value<_bool>(pViper->Get_Name(), "isGroggy"))
                    return BTNODESTATE::FAILURE;


                if (BB->Get_Value<_bool>(pViper->Get_Name(), "is_P2_DashUpperFinished"))
                {
                    if (BB->Get_Value<_bool>(pViper->Get_Name(), "is_Berserker"))
                    {
                        _uint iBersekerIndex = BB->Get_Value<_uint>(pViper->Get_Name(), "BerserkerIndex");
                        BB->Set_Value<_uint>(pViper->Get_Name(), "BerserkerIndex", iBersekerIndex + 1);
                    }
                    return BTNODESTATE::SUCCESS;
                }

                BB->Set_Value(pViper->Get_Name(), "isSuperArmor", true);

                pViper->Get_Controller()->Get_State_Machine()->
                    Change_State(ENUM_CLASS(VIPER_STATE_P1::P2_DASHUPPER), pViper);

                return BTNODESTATE::RUNNING;
            };
    }


    else if ("P2_FakeRunAttack" == name)
    {

        return [pViper](CBlackBoard* BB)-> BTNODESTATE
            {

                if (BB->Get_Value<_bool>(pViper->Get_Name(), "isGroggy"))
                    return BTNODESTATE::FAILURE;


                if (BB->Get_Value<_bool>(pViper->Get_Name(), "is_P2_FakeRunAttackFinished"))
                {
                    return BTNODESTATE::SUCCESS;
                }

                BB->Set_Value(pViper->Get_Name(), "isSuperArmor", true);

                pViper->Get_Controller()->Get_State_Machine()->
                    Change_State(ENUM_CLASS(VIPER_STATE_P1::P2_FAKERUNATTACK), pViper);

                return BTNODESTATE::RUNNING;
            };
    }





    else if ("P2_HandSwing3Hit" == name)
    {

        return [pViper](CBlackBoard* BB)-> BTNODESTATE
            {

                if (BB->Get_Value<_bool>(pViper->Get_Name(), "isGroggy"))
                    return BTNODESTATE::FAILURE;


                if (BB->Get_Value<_bool>(pViper->Get_Name(), "is_P2_HandSwing3HitFinished"))
                {
                    return BTNODESTATE::SUCCESS;
                }

                BB->Set_Value(pViper->Get_Name(), "isSuperArmor", true);

                pViper->Get_Controller()->Get_State_Machine()->
                    Change_State(ENUM_CLASS(VIPER_STATE_P1::P2_HANDSWING3HIT), pViper);

                return BTNODESTATE::RUNNING;
            };
    }



   
    else if ("P2_SlashStomp" == name)
    {

        return [pViper](CBlackBoard* BB)-> BTNODESTATE
            {

                if (BB->Get_Value<_bool>(pViper->Get_Name(), "isGroggy"))
                    return BTNODESTATE::FAILURE;


                if (BB->Get_Value<_bool>(pViper->Get_Name(), "is_P2_SlashStompFinished"))
                {
                    return BTNODESTATE::SUCCESS;
                }

                BB->Set_Value(pViper->Get_Name(), "isSuperArmor", true);

                pViper->Get_Controller()->Get_State_Machine()->
                    Change_State(ENUM_CLASS(VIPER_STATE_P1::P2_SLASHSTOMP), pViper);

                return BTNODESTATE::RUNNING;
            };
    }


    else if ("P2_SlashDouble" == name)
    {

        return [pViper](CBlackBoard* BB)-> BTNODESTATE
            {

                if (BB->Get_Value<_bool>(pViper->Get_Name(), "isGroggy"))
                    return BTNODESTATE::FAILURE;


                if (BB->Get_Value<_bool>(pViper->Get_Name(), "is_P2_SlashDoubleFinished"))
                {
                    return BTNODESTATE::SUCCESS;
                }

                BB->Set_Value(pViper->Get_Name(), "isSuperArmor", true);

                pViper->Get_Controller()->Get_State_Machine()->
                    Change_State(ENUM_CLASS(VIPER_STATE_P1::P2_SLASHDOUBLE), pViper);

                return BTNODESTATE::RUNNING;
            };
    }

    else if ("P2_HandUpper" == name)
    {

        return [pViper](CBlackBoard* BB)-> BTNODESTATE
            {

                if (BB->Get_Value<_bool>(pViper->Get_Name(), "isGroggy"))
                    return BTNODESTATE::FAILURE;


                if (BB->Get_Value<_bool>(pViper->Get_Name(), "is_P2_HandUpperFinished"))
                {
                    return BTNODESTATE::SUCCESS;
                }

                BB->Set_Value(pViper->Get_Name(), "isSuperArmor", true);

                pViper->Get_Controller()->Get_State_Machine()->
                    Change_State(ENUM_CLASS(VIPER_STATE_P1::P2_HANDUPPER), pViper);

                return BTNODESTATE::RUNNING;
            };
    }


    else if ("P2_HandSwing2Hit" == name)
    {

        return [pViper](CBlackBoard* BB)-> BTNODESTATE
            {

                if (BB->Get_Value<_bool>(pViper->Get_Name(), "isGroggy"))
                    return BTNODESTATE::FAILURE;


                if (BB->Get_Value<_bool>(pViper->Get_Name(), "is_P2_HandSwing2HitFinished"))
                {
                    return BTNODESTATE::SUCCESS;
                }

                BB->Set_Value(pViper->Get_Name(), "isSuperArmor", true);

                pViper->Get_Controller()->Get_State_Machine()->
                    Change_State(ENUM_CLASS(VIPER_STATE_P1::P2_HANDSWING2HIT), pViper);

                return BTNODESTATE::RUNNING;
            };
    }


    else if ("P2_HandStompStr" == name)
    {

        return [pViper](CBlackBoard* BB)-> BTNODESTATE
            {

                if (BB->Get_Value<_bool>(pViper->Get_Name(), "isGroggy"))
                    return BTNODESTATE::FAILURE;


                if (BB->Get_Value<_bool>(pViper->Get_Name(), "is_P2_HandStompStrFinished"))
                {
                    return BTNODESTATE::SUCCESS;
                }

                BB->Set_Value(pViper->Get_Name(), "isSuperArmor", true);

                pViper->Get_Controller()->Get_State_Machine()->
                    Change_State(ENUM_CLASS(VIPER_STATE_P1::P2_HANDSTOMPSTR), pViper);

                return BTNODESTATE::RUNNING;
            };
    }


    else if ("P2_HandStomp" == name)
    {

        return [pViper](CBlackBoard* BB)-> BTNODESTATE
            {

                if (BB->Get_Value<_bool>(pViper->Get_Name(), "isGroggy"))
                    return BTNODESTATE::FAILURE;


                if (BB->Get_Value<_bool>(pViper->Get_Name(), "is_P2_HandStompFinished"))
                {
                    return BTNODESTATE::SUCCESS;
                }

                BB->Set_Value(pViper->Get_Name(), "isSuperArmor", true);

                pViper->Get_Controller()->Get_State_Machine()->
                    Change_State(ENUM_CLASS(VIPER_STATE_P1::P2_HANDSTOMP), pViper);

                return BTNODESTATE::RUNNING;
            };
    }


#pragma endregion

#pragma region NONCOMBAT_PAHSE2

    else if ("P2_Run" == name)
    {
        return [pViper](CBlackBoard* BB) ->BTNODESTATE
            {

                _bool isGroggy = BB->Get_Value<_bool>(pViper->Get_Name(), "isGroggy");
                if (isGroggy)
                    return BTNODESTATE::FAILURE;

                _float fDist = BB->Get_Value<_float>(pViper->Get_Name(), "TargetDist");
                _float fAttackRange = BB->Get_Value<_float>(pViper->Get_Name(), "AttackRange");
                _float fJumpAttackRange = BB->Get_Value<_float>(pViper->Get_Name(), "JumpAttackRange");


                if (fDist < fJumpAttackRange)
                    return BTNODESTATE::SUCCESS;

                if (fDist < fAttackRange)
                    return BTNODESTATE::SUCCESS;

                pViper->Get_Controller()->Get_State_Machine()->Change_State(
                    ENUM_CLASS(VIPER_STATE_P1::P2_RUN), pViper);

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
                    //BB->Set_Value<_uint>(pViper->Get_Name(), "DamageType", ENUM_CLASS(HITREACTION::NONE));
                }
            };
    }

#pragma endregion


#pragma region LOCKON

    else if ("P2_LockOn" == name)
    {
        return [pViper](CBlackBoard* BB, BTNODESTATE eState)
            {
                if (nullptr == BB)
                    return;

                if (eState == BTNODESTATE::SUCCESS || eState == BTNODESTATE::FAILURE)
                {
                    BB->Set_Value<_bool>(pViper->Get_Name(), "isP2_LockOn_Finished", false);
                    BB->Set_Value<_bool>(pViper->Get_Name(), "isP2LockOn", false);
                }
            };
    }
#pragma endregion




#pragma region COMBAT_PHASE2

    else if ("P2_BerserkerJump" == name)
    {
        return [pViper](CBlackBoard* BB, BTNODESTATE eState)
            {
                if (nullptr == BB)
                    return;

                if (eState == BTNODESTATE::SUCCESS || eState == BTNODESTATE::FAILURE)
                {
                    BB->Set_Value<_bool>(pViper->Get_Name(), "is_P2_BerserkerJumpFinished", false);
                    BB->Set_Value(pViper->Get_Name(), "isSuperArmor", false);
                    BB->Set_Value<_bool>(pViper->Get_Name(), "isHit", false);
                    BB->Set_Value<_bool>(pViper->Get_Name(), "isHitFinished", false);
                    //BB->Set_Value<_uint>(pViper->Get_Name(), "DamageType", ENUM_CLASS(HITREACTION::NONE));
                }
            };
    }

    else if ("P2_Roar" == name)
    {
        return [pViper](CBlackBoard* BB, BTNODESTATE eState)
            {
                if (nullptr == BB)
                    return;

                if (eState == BTNODESTATE::SUCCESS || eState == BTNODESTATE::FAILURE)
                {
                    BB->Set_Value<_bool>(pViper->Get_Name(), "is_P2_RoarFinished", false);
                    BB->Set_Value<_bool>(pViper->Get_Name(), "isP2_Roar", false);
                    BB->Set_Value(pViper->Get_Name(), "isSuperArmor", false);
                    BB->Set_Value<_bool>(pViper->Get_Name(), "isHit", false);
                    BB->Set_Value<_bool>(pViper->Get_Name(), "isHitFinished", false);
                    //BB->Set_Value<_uint>(pViper->Get_Name(), "DamageType", ENUM_CLASS(HITREACTION::NONE));
                }
            };
    }



    else if ("P2_SwingRound" == name)
    {
        return [pViper](CBlackBoard* BB, BTNODESTATE eState)
            {
                if (nullptr == BB)
                    return;

                if (eState == BTNODESTATE::SUCCESS || eState == BTNODESTATE::FAILURE)
                {
                    BB->Set_Value<_bool>(pViper->Get_Name(), "is_P2_SwingRoundFinished", false);
                    BB->Set_Value(pViper->Get_Name(), "isSuperArmor", false);
                    BB->Set_Value<_bool>(pViper->Get_Name(), "isHit", false);
                    BB->Set_Value<_bool>(pViper->Get_Name(), "isHitFinished", false);
                    //BB->Set_Value<_uint>(pViper->Get_Name(), "DamageType", ENUM_CLASS(HITREACTION::NONE));
                }
            };
    }


    else if ("P2_DashDrift" == name)
    {
        return [pViper](CBlackBoard* BB, BTNODESTATE eState)
            {
                if (nullptr == BB)
                    return;

                if (eState == BTNODESTATE::SUCCESS || eState == BTNODESTATE::FAILURE)
                {
                    BB->Set_Value<_bool>(pViper->Get_Name(), "is_P2_DashDriftFinished", false);
                    BB->Set_Value(pViper->Get_Name(), "isSuperArmor", false);
                    BB->Set_Value<_bool>(pViper->Get_Name(), "isHit", false);
                    BB->Set_Value<_bool>(pViper->Get_Name(), "isHitFinished", false);
                    //BB->Set_Value<_uint>(pViper->Get_Name(), "DamageType", ENUM_CLASS(HITREACTION::NONE));
                }
            };
    }

    else if ("P2_ThrowRock" == name)
    {
        return [pViper](CBlackBoard* BB, BTNODESTATE eState)
            {
                if (nullptr == BB)
                    return;

                if (eState == BTNODESTATE::SUCCESS || eState == BTNODESTATE::FAILURE)
                {
                    BB->Set_Value<_bool>(pViper->Get_Name(), "is_P2_ThrowRockFinished", false);
                    BB->Set_Value(pViper->Get_Name(), "isSuperArmor", false);
                    BB->Set_Value<_bool>(pViper->Get_Name(), "isHit", false);
                    BB->Set_Value<_bool>(pViper->Get_Name(), "isHitFinished", false);
                    //BB->Set_Value<_uint>(pViper->Get_Name(), "DamageType", ENUM_CLASS(HITREACTION::NONE));
                }
            };
    }



    else if ("P2_SwingCombo" == name)
    {
        return [pViper](CBlackBoard* BB, BTNODESTATE eState)
            {
                if (nullptr == BB)
                    return;

                if (eState == BTNODESTATE::SUCCESS || eState == BTNODESTATE::FAILURE)
                {
                    BB->Set_Value<_bool>(pViper->Get_Name(), "is_P2_SwingComboFinished", false);
                    BB->Set_Value(pViper->Get_Name(), "isSuperArmor", false);
                    BB->Set_Value<_bool>(pViper->Get_Name(), "isHit", false);
                    BB->Set_Value<_bool>(pViper->Get_Name(), "isHitFinished", false);
                    //BB->Set_Value<_uint>(pViper->Get_Name(), "DamageType", ENUM_CLASS(HITREACTION::NONE));
                }
            };
    }





    else if ("P2_JumpAttack" == name)
    {
        return [pViper](CBlackBoard* BB, BTNODESTATE eState)
            {
                if (nullptr == BB)
                    return;

                if (eState == BTNODESTATE::SUCCESS || eState == BTNODESTATE::FAILURE)
                {
                    BB->Set_Value<_bool>(pViper->Get_Name(), "is_P2_JumpAttackFinished", false);
                    BB->Set_Value(pViper->Get_Name(), "isSuperArmor", false);
                    BB->Set_Value<_bool>(pViper->Get_Name(), "isHit", false);
                    BB->Set_Value<_bool>(pViper->Get_Name(), "isHitFinished", false);
                    //BB->Set_Value<_uint>(pViper->Get_Name(), "DamageType", ENUM_CLASS(HITREACTION::NONE));
                }
            };
    }


    else if ("P2_SideMove" == name)
    {
        return [pViper](CBlackBoard* BB, BTNODESTATE eState)
            {
                if (nullptr == BB)
                    return;

                if (eState == BTNODESTATE::SUCCESS || eState == BTNODESTATE::FAILURE)
                {
                    BB->Set_Value<_bool>(pViper->Get_Name(), "is_P2_SideMoveFinished", false);
                    BB->Set_Value(pViper->Get_Name(), "isSuperArmor", false);
                    BB->Set_Value<_bool>(pViper->Get_Name(), "isHit", false);
                    BB->Set_Value<_bool>(pViper->Get_Name(), "isHitFinished", false);
                    //BB->Set_Value<_uint>(pViper->Get_Name(), "DamageType", ENUM_CLASS(HITREACTION::NONE));
                }
            };
    }



    else if ("P2_BackJump" == name)
    {
        return [pViper](CBlackBoard* BB, BTNODESTATE eState)
            {
                if (nullptr == BB)
                    return;

                if (eState == BTNODESTATE::SUCCESS || eState == BTNODESTATE::FAILURE)
                {
                    BB->Set_Value<_bool>(pViper->Get_Name(), "is_P2_BackJumpFinished", false);
                    BB->Set_Value(pViper->Get_Name(), "isSuperArmor", false);
                    BB->Set_Value<_bool>(pViper->Get_Name(), "isHit", false);
                    BB->Set_Value<_bool>(pViper->Get_Name(), "isHitFinished", false);
                    //BB->Set_Value<_uint>(pViper->Get_Name(), "DamageType", ENUM_CLASS(HITREACTION::NONE));
                }
            };
    }



    else if ("P2_DashUpperStr" == name)
    {
        return [pViper](CBlackBoard* BB, BTNODESTATE eState)
            {
                if (nullptr == BB)
                    return;

                if (eState == BTNODESTATE::SUCCESS || eState == BTNODESTATE::FAILURE)
                {
                    BB->Set_Value<_bool>(pViper->Get_Name(), "is_P2_DashUpperStrFinished", false);
                    BB->Set_Value(pViper->Get_Name(), "isSuperArmor", false);
                    BB->Set_Value<_bool>(pViper->Get_Name(), "isHit", false);
                    BB->Set_Value<_bool>(pViper->Get_Name(), "isHitFinished", false);
                    //BB->Set_Value<_uint>(pViper->Get_Name(), "DamageType", ENUM_CLASS(HITREACTION::NONE));
                }
            };
    }

    else if ("P2_DashUpper" == name)
    {
        return [pViper](CBlackBoard* BB, BTNODESTATE eState)
            {
                if (nullptr == BB)
                    return;

                if (eState == BTNODESTATE::SUCCESS || eState == BTNODESTATE::FAILURE)
                {
                    BB->Set_Value<_bool>(pViper->Get_Name(), "is_P2_DashUpperFinished", false);
                    BB->Set_Value(pViper->Get_Name(), "isSuperArmor", false);
                    BB->Set_Value<_bool>(pViper->Get_Name(), "isHit", false);
                    BB->Set_Value<_bool>(pViper->Get_Name(), "isHitFinished", false);
                    //BB->Set_Value<_uint>(pViper->Get_Name(), "DamageType", ENUM_CLASS(HITREACTION::NONE));
                }
            };
    }


    else if ("P2_FakeRunAttack" == name)
    {
        return [pViper](CBlackBoard* BB, BTNODESTATE eState)
            {
                if (nullptr == BB)
                    return;

                if (eState == BTNODESTATE::SUCCESS || eState == BTNODESTATE::FAILURE)
                {
                    BB->Set_Value<_bool>(pViper->Get_Name(), "is_P2_FakeRunAttackFinished", false);
                    BB->Set_Value(pViper->Get_Name(), "isSuperArmor", false);
                    BB->Set_Value<_bool>(pViper->Get_Name(), "isHit", false);
                    BB->Set_Value<_bool>(pViper->Get_Name(), "isHitFinished", false);
                    //BB->Set_Value<_uint>(pViper->Get_Name(), "DamageType", ENUM_CLASS(HITREACTION::NONE));
                }
            };
    }

    else if ("P2_HandSwing3Hit" == name)
    {
        return [pViper](CBlackBoard* BB, BTNODESTATE eState)
            {
                if (nullptr == BB)
                    return;

                if (eState == BTNODESTATE::SUCCESS || eState == BTNODESTATE::FAILURE)
                {
                    BB->Set_Value<_bool>(pViper->Get_Name(), "is_P2_HandSwing3HitFinished", false);
                    BB->Set_Value(pViper->Get_Name(), "isSuperArmor", false);
                    BB->Set_Value<_bool>(pViper->Get_Name(), "isHit", false);
                    BB->Set_Value<_bool>(pViper->Get_Name(), "isHitFinished", false);
                    //BB->Set_Value<_uint>(pViper->Get_Name(), "DamageType", ENUM_CLASS(HITREACTION::NONE));
                }
            };
    }


    else if ("P2_SlashStomp" == name)
    {
        return [pViper](CBlackBoard* BB, BTNODESTATE eState)
            {
                if (nullptr == BB)
                    return;

                if (eState == BTNODESTATE::SUCCESS || eState == BTNODESTATE::FAILURE)
                {
                    BB->Set_Value<_bool>(pViper->Get_Name(), "is_P2_SlashStompFinished", false);
                    BB->Set_Value(pViper->Get_Name(), "isSuperArmor", false);
                    BB->Set_Value<_bool>(pViper->Get_Name(), "isHit", false);
                    BB->Set_Value<_bool>(pViper->Get_Name(), "isHitFinished", false);
                    //BB->Set_Value<_uint>(pViper->Get_Name(), "DamageType", ENUM_CLASS(HITREACTION::NONE));
                }
            };
    }



    else if ("P2_SlashDouble" == name)
    {
        return [pViper](CBlackBoard* BB, BTNODESTATE eState)
            {
                if (nullptr == BB)
                    return;

                if (eState == BTNODESTATE::SUCCESS || eState == BTNODESTATE::FAILURE)
                {
                    BB->Set_Value<_bool>(pViper->Get_Name(), "is_P2_SlashDoubleFinished", false);
                    BB->Set_Value(pViper->Get_Name(), "isSuperArmor", false);
                    BB->Set_Value<_bool>(pViper->Get_Name(), "isHit", false);
                    BB->Set_Value<_bool>(pViper->Get_Name(), "isHitFinished", false);
                    //BB->Set_Value<_uint>(pViper->Get_Name(), "DamageType", ENUM_CLASS(HITREACTION::NONE));
                }
            };
    }


    else if ("P2_HandUpper" == name)
    {
        return [pViper](CBlackBoard* BB, BTNODESTATE eState)
            {
                if (nullptr == BB)
                    return;

                if (eState == BTNODESTATE::SUCCESS || eState == BTNODESTATE::FAILURE)
                {
                    BB->Set_Value<_bool>(pViper->Get_Name(), "is_P2_HandUpperFinished", false);
                    BB->Set_Value(pViper->Get_Name(), "isSuperArmor", false);
                    BB->Set_Value<_bool>(pViper->Get_Name(), "isHit", false);
                    BB->Set_Value<_bool>(pViper->Get_Name(), "isHitFinished", false);
                    //BB->Set_Value<_uint>(pViper->Get_Name(), "DamageType", ENUM_CLASS(HITREACTION::NONE));
                }
            };
    }


    else if ("P2_HandSwing2Hit" == name)
    {
        return [pViper](CBlackBoard* BB, BTNODESTATE eState)
            {
                if (nullptr == BB)
                    return;

                if (eState == BTNODESTATE::SUCCESS || eState == BTNODESTATE::FAILURE)
                {
                    BB->Set_Value<_bool>(pViper->Get_Name(), "is_P2_HandSwing2HitFinished", false);
                    BB->Set_Value(pViper->Get_Name(), "isSuperArmor", false);
                    BB->Set_Value<_bool>(pViper->Get_Name(), "isHit", false);
                    BB->Set_Value<_bool>(pViper->Get_Name(), "isHitFinished", false);
                    //BB->Set_Value<_uint>(pViper->Get_Name(), "DamageType", ENUM_CLASS(HITREACTION::NONE));
                }
            };
    }


    else if ("P2_HandStompStr" == name)
    {
        return [pViper](CBlackBoard* BB, BTNODESTATE eState)
            {
                if (nullptr == BB)
                    return;

                if (eState == BTNODESTATE::SUCCESS || eState == BTNODESTATE::FAILURE)
                {
                    BB->Set_Value<_bool>(pViper->Get_Name(), "is_P2_HandStompStrFinished", false);
                    BB->Set_Value(pViper->Get_Name(), "isSuperArmor", false);
                    BB->Set_Value<_bool>(pViper->Get_Name(), "isHit", false);
                    BB->Set_Value<_bool>(pViper->Get_Name(), "isHitFinished", false);
                    //BB->Set_Value<_uint>(pViper->Get_Name(), "DamageType", ENUM_CLASS(HITREACTION::NONE));
                }
            };
    }


    else if ("P2_HandStomp" == name)
    {
        return [pViper](CBlackBoard* BB, BTNODESTATE eState)
            {
                if (nullptr == BB)
                    return;

                if (eState == BTNODESTATE::SUCCESS || eState == BTNODESTATE::FAILURE)
                {
                    BB->Set_Value<_bool>(pViper->Get_Name(), "is_P2_HandStompFinished", false);
                    BB->Set_Value(pViper->Get_Name(), "isSuperArmor", false);
                    BB->Set_Value<_bool>(pViper->Get_Name(), "isHit", false);
                    BB->Set_Value<_bool>(pViper->Get_Name(), "isHitFinished", false);
                    //BB->Set_Value<_uint>(pViper->Get_Name(), "DamageType", ENUM_CLASS(HITREACTION::NONE));
                }
            };
    }


#pragma endregion

#pragma region NONCOMBAT_PAHSE2

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
                    //BB->Set_Value<_uint>(pViper->Get_Name(), "DamageType", ENUM_CLASS(HITREACTION::NONE));
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
                    //BB->Set_Value<_uint>(pViper->Get_Name(), "DamageType", ENUM_CLASS(HITREACTION::NONE));
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
                    //BB->Set_Value<_uint>(pViper->Get_Name(), "DamageType", ENUM_CLASS(HITREACTION::NONE));
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
                    //BB->Set_Value<_uint>(pViper->Get_Name(), "DamageType", ENUM_CLASS(HITREACTION::NONE));
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
                    //BB->Set_Value<_uint>(pViper->Get_Name(), "DamageType", ENUM_CLASS(HITREACTION::NONE));
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
                    //BB->Set_Value<_uint>(pViper->Get_Name(), "DamageType", ENUM_CLASS(HITREACTION::NONE));
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
                    //BB->Set_Value<_uint>(pViper->Get_Name(), "DamageType", ENUM_CLASS(HITREACTION::NONE));
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
                    //BB->Set_Value<_uint>(pViper->Get_Name(), "DamageType", ENUM_CLASS(HITREACTION::NONE));
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
                    //BB->Set_Value<_uint>(pViper->Get_Name(), "DamageType", ENUM_CLASS(HITREACTION::NONE));
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
                    //BB->Set_Value<_uint>(pViper->Get_Name(), "DamageType", ENUM_CLASS(HITREACTION::NONE));
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
                    //BB->Set_Value<_uint>(pViper->Get_Name(), "DamageType", ENUM_CLASS(HITREACTION::NONE));
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
                    //BB->Set_Value<_uint>(pViper->Get_Name(), "DamageType", ENUM_CLASS(HITREACTION::NONE));
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
                    //BB->Set_Value<_uint>(pViper->Get_Name(), "DamageType", ENUM_CLASS(HITREACTION::NONE));
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
                if (BB->Get_Value<_bool>(pViper->Get_Name(),"isDead"))
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

void CAI_Controller_Viper::Set_ViperBerserkerStart_ShaderSettings()
{
    // 림라이트 끄기
    m_pGameInstance->Set_EnableRimLight(false);

    // 광전사 모드 셰이더 세팅
    _float fDuration = 3.f;

    // 메인 조명 끄기
    LIGHT_TRANSITION_DESC LightDesc{};
    LightDesc.fDuration = fDuration;
    LightDesc.vFadeTime = _float2(fDuration, 0.f);
    LightDesc.vDiffuse = _float4(0.f, 0.f, 0.f, 0.f);
    LightDesc.vAmbient = _float4(0.f, 0.f, 0.f, 0.f);
    LightDesc.vSpecular = LightDesc.vDiffuse;
    LightDesc.isReturnToStart = false;
    m_pGameInstance->Start_LightTransition(TEXT("MainLight"), ENUM_CLASS(CClientInstance::GetInstance()->Get_CurrLevel()), LightDesc);

    // 점 조명 : 그레이 조명 켜지기, 오렌지, 화이트, 무기 조명은 꺼지기
    LightDesc.fDuration = fDuration;
    LightDesc.vFadeTime = _float2(fDuration, 0.f);
    LightDesc.vDiffuse = _float4(0.409f, 0.381f, 0.295f, 1.f);
    LightDesc.vAmbient = _float4(0.7f, 0.7f, 0.7f, 0.7f);
    LightDesc.vSpecular = LightDesc.vDiffuse;
    LightDesc.isReturnToStart = false;
    m_pGameInstance->Start_LightTransition(TEXT("Player_PointLight_Gray"), ENUM_CLASS(CClientInstance::GetInstance()->Get_CurrLevel()), LightDesc);

    LightDesc.fDuration = fDuration;
    LightDesc.vFadeTime = _float2(fDuration, 0.f);
    LightDesc.vDiffuse = _float4(0.f, 0.f, 0.f, 0.f);
    LightDesc.vAmbient = _float4(0.f, 0.f, 0.f, 0.f);
    LightDesc.vSpecular = LightDesc.vDiffuse;
    LightDesc.isReturnToStart = false;
    m_pGameInstance->Start_LightTransition(TEXT("Player_PointLight_White"), ENUM_CLASS(CClientInstance::GetInstance()->Get_CurrLevel()), LightDesc);

    LightDesc.fDuration = fDuration;
    LightDesc.vFadeTime = _float2(fDuration, 0.f);
    LightDesc.vDiffuse = _float4(0.f, 0.f, 0.f, 0.f);
    LightDesc.vAmbient = _float4(0.f, 0.f, 0.f, 0.f);
    LightDesc.vSpecular = LightDesc.vDiffuse;
    LightDesc.isReturnToStart = false;
    m_pGameInstance->Start_LightTransition(TEXT("Player_PointLight_Orange"), ENUM_CLASS(CClientInstance::GetInstance()->Get_CurrLevel()), LightDesc);

    LightDesc.fDuration = fDuration;
    LightDesc.vFadeTime = _float2(fDuration, 0.f);
    LightDesc.vDiffuse = _float4(0.f, 0.f, 0.f, 0.f);
    LightDesc.vAmbient = _float4(0.f, 0.f, 0.f, 0.f);
    LightDesc.vSpecular = LightDesc.vDiffuse;
    LightDesc.isReturnToStart = false;
    m_pGameInstance->Start_LightTransition(TEXT("Viper_TwinBlade_R"), ENUM_CLASS(CClientInstance::GetInstance()->Get_CurrLevel()), LightDesc);

    // 포그 검정
    FOG_TRANSITION_DESC FogDesc{};
    FogDesc.fDensity = 0.05f;
    FogDesc.fBias = 0.95f;
    FogDesc.vColor = _float4(0.f, 0.f, 0.f, 0.f);
    FogDesc.isUseHeight = false;
    FogDesc.isUseNoise = false;
    FogDesc.Callback = [&]() { m_pGameInstance->Set_EnableFog(false); };
    m_pGameInstance->Start_FogTransition(fDuration, FogDesc);

    // 스카이 검정
    SKY_DESC SkyDesc{};
    SkyDesc.vNebulaColorR = _float3(0.f, 0.f, 0.f);
    SkyDesc.vNebulaColorG = _float3(0.f, 0.f, 0.f);
    SkyDesc.vNebulaColorB = _float3(0.f, 0.f, 0.f);
    SkyDesc.fStarStrength = 0.f;
    SkyDesc.fMoonSize = 0.8f;
    SkyDesc.vMoonDirection = _float3(-0.21f, 0.19f, 1.f);
    SkyDesc.vMoonColor = _float3(0.f, 0.f, 0.f);
    SkyDesc.fMoonIntensity = 0.f;
    static_cast<CSkySphere*>(m_pGameInstance->Find_GameObject(ENUM_CLASS(LEVEL::VIPER), TEXT("Layer_Sky"), 0))->Start_LerpSky(SkyDesc, fDuration);

    // 클라우드 세팅
    CLOUD_DESC CloudDesc{};
    CloudDesc.vCloudColor = _float3(0.f, 0.f, 0.f);
    CloudDesc.fCloudSpeed = 0.f;
    CloudDesc.fCloudScale = 0.f;
    CloudDesc.fCloudDensity = 0.f;
    CloudDesc.fCloudLightIntensity = 0.f;
    CloudDesc.vLightDir = _float3(0.f, 0.f, 0.f);
    CloudDesc.fDynamic = 0.f;
    static_cast<CCloudSphere*>(m_pGameInstance->Find_GameObject(ENUM_CLASS(LEVEL::VIPER), TEXT("Layer_Sky"), 1))->Start_LerpCloud(CloudDesc, fDuration);
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

