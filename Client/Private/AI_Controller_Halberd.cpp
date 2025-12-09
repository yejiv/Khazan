#include "AI_Controller_Halberd.h"
#include "GameInstance.h"

#include "FSM_Default.h"
#include "BehaviorTree.h"
#include "Perception.h"

#include "AS_Halberd_Dead.h"
#include "AS_Halberd_Groggy.h"
#include "AS_Halberd_Brutal.h"
#include "AS_Halberd_Attack_Long.h"
#include "AS_Halberd_Attack_Default.h"
#include "AS_Halberd_Damage.h"
#include "AS_Halberd_LockOn.h"
#include "AS_Halberd_Sleep.h"
#include "AS_Halberd_Turn.h"

CAI_Controller_Halberd::CAI_Controller_Halberd()
{
}

HRESULT CAI_Controller_Halberd::Initialize(CCreature* pOwner)
{
    CHalberd* pCast_Owner = static_cast<CHalberd*>(pOwner);
    m_pMonData = &pCast_Owner->Get_Data();

    CHECK_FAILED(__super::Initialize(pCast_Owner, pCast_Owner->Get_Name()), E_FAIL);
    CHECK_FAILED(Ready_FSM(pCast_Owner), E_FAIL);

    m_pBB->Set_Value(m_strMonstertag, "CurrentTime", 0.f);

    return S_OK;

}

void CAI_Controller_Halberd::Update(CGameObject* pOwner, _float fTimeDelta)
{
    if (!m_isActive)
        return;

    if (*m_pMonData->pCulHp <= 0.f)
    {
        m_pFSM->Change_State(ENUM_CLASS(CHalberd::MONSTATE::DEAD), pOwner);
    }
    else if (m_pMonData->eHitType == HITREACTION::BRUTAL_ATTACK)
    {
        m_pFSM->Change_State(ENUM_CLASS(CHalberd::MONSTATE::BRUTAL), pOwner);
    }
    else if (*m_pMonData->pCulStamina <= 0.f)

    {
        m_pFSM->Change_State(ENUM_CLASS(CHalberd::MONSTATE::GRORRY), pOwner);
    }
    else
    {
        m_pPerception->Update(pOwner, m_pBB, fTimeDelta);
        m_pBT->Update();
    }

    m_pFSM->Update(pOwner, fTimeDelta * m_pMonData->fDeltaSpeed);
}

HRESULT CAI_Controller_Halberd::Ready_Perception(CGameObject* pOwner, const AIPERCEPTION_DATA& Desc)
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

HRESULT CAI_Controller_Halberd::Ready_BlackBoard(CGameObject* pOwner)
{
    m_pBB = CBlackBoard::Create();
    if (nullptr == m_pBB)
        return E_FAIL;

    return S_OK;
}

HRESULT CAI_Controller_Halberd::Ready_BehaviorTree()
{
    if (nullptr == m_pBB)
        return E_FAIL;

    m_pBT->Set_BlackBoard(m_pBB);

    if (nullptr == m_pBT)
        return E_FAIL;

    return S_OK;
}

CONDITION CAI_Controller_Halberd::GetCallbackCondition(CGameObject* pOwner, const string& name)
{
    return nullptr;
}

ACTION CAI_Controller_Halberd::GetCallbackAction(CGameObject* pOwner, const string& name)
{
    if ("Damage_Check" == name)             return [this, pOwner](CBlackBoard* BB)-> BTNODESTATE {return this->Damage_Check(pOwner); };
    else if ("Damage" == name)              return [this, pOwner](CBlackBoard* BB)-> BTNODESTATE {return this->Damage(pOwner); };
    else if ("Turn_Check" == name)          return [this, pOwner](CBlackBoard* BB)-> BTNODESTATE {return this->Turn_Check(pOwner); };
    else if ("Turn" == name)                return [this, pOwner](CBlackBoard* BB)-> BTNODESTATE {return this->Turn(pOwner); };
    else if ("Attack_Long_Check" == name)   return [this, pOwner](CBlackBoard* BB)-> BTNODESTATE {return this->Attack_Long_Check(pOwner); };
    else if ("Attack_Long" == name)         return [this, pOwner](CBlackBoard* BB)-> BTNODESTATE {return this->Attack_Long(pOwner); };
    else if ("Attack_Check" == name)        return [this, pOwner](CBlackBoard* BB)-> BTNODESTATE {return this->Attack_Check(pOwner); };
    else if ("Attack_Default" == name)      return [this, pOwner](CBlackBoard* BB)-> BTNODESTATE {return this->Attack_Default(pOwner); };
    else if ("LockOn" == name)              return [this, pOwner](CBlackBoard* BB)-> BTNODESTATE {return this->LockOn(pOwner); };
    else if ("Sleep" == name)               return [this, pOwner](CBlackBoard* BB)-> BTNODESTATE {return this->Sleep(pOwner); };

    return nullptr;
}

TERMINATE CAI_Controller_Halberd::GetCallbackTeminate(CGameObject* pOwner, const string& name)
{
    return nullptr;
}

INTERRUPTCONDITION CAI_Controller_Halberd::GetCallbackInterruptCondition(CGameObject* pOwner, const string& name)
{
    return nullptr;
}

PERCEPTIONCALLBACK CAI_Controller_Halberd::GetCallBackPerception(CGameObject* pOwner, const string& name)
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
                        m_pBB->Set_Value(m_strMonstertag, "DamageInterrupt", false);
                        m_pBB->Set_Value(m_strMonstertag, "isDetected", true);

                        if (m_pMonData->eHitType != HITREACTION::BRUTAL_ATTACK)
                            m_pMonData->eHitType = static_cast<HITREACTION>(m_pBB->Get_Value<_uint>(m_strMonstertag, "DamageType"));

                    }
                }
            };
    }

    return nullptr;
}

HRESULT CAI_Controller_Halberd::Ready_FSM(class CCreature* pOwner)
{
    CFSM_Default* pFsm = CFSM_Default::Create(pOwner);

    CHECK_NULLPTR(pFsm, E_FAIL);

    CHECK_FAILED(pFsm->Add_State(ENUM_CLASS(CHalberd::MONSTATE::DEAD), CAS_Halberd_Dead::Create()), E_FAIL);
    CHECK_FAILED(pFsm->Add_State(ENUM_CLASS(CHalberd::MONSTATE::GRORRY), CAS_Halberd_Groggy::Create()), E_FAIL);
    CHECK_FAILED(pFsm->Add_State(ENUM_CLASS(CHalberd::MONSTATE::ATTACK_LONG), CAS_Halberd_Attack_Long::Create()), E_FAIL);
    
    CHECK_FAILED(pFsm->Add_State(ENUM_CLASS(CHalberd::MONSTATE::ATTACK_DEFAULT), CAS_Halberd_Attack_default::Create()), E_FAIL);
    CHECK_FAILED(pFsm->Add_State(ENUM_CLASS(CHalberd::MONSTATE::BRUTAL), CAS_Halberd_Brutal::Create()), E_FAIL);
    
    CHECK_FAILED(pFsm->Add_State(ENUM_CLASS(CHalberd::MONSTATE::LOCKON), CAS_Halberd_LockOn::Create()), E_FAIL);
    CHECK_FAILED(pFsm->Add_State(ENUM_CLASS(CHalberd::MONSTATE::DAMAGE), CAS_Halberd_Damage::Create()), E_FAIL);
    
    CHECK_FAILED(pFsm->Add_State(ENUM_CLASS(CHalberd::MONSTATE::TURN), CAS_Halberd_Turn::Create()), E_FAIL);
    CHECK_FAILED(pFsm->Add_State(ENUM_CLASS(CHalberd::MONSTATE::SLEEP), CAS_Halberd_Sleep::Create()), E_FAIL);
    
    m_pFSM = pFsm;

    m_pMonData->isSleep = true;

    pFsm->Set_CulState(ENUM_CLASS(CHalberd::MONSTATE::SLEEP), pOwner);
    return S_OK;

}

BTNODESTATE CAI_Controller_Halberd::Damage_Check(CGameObject* pOwner)
{
    if (!m_pMonData->isSleep && m_pMonData->eHitType != HITREACTION::END)
    {
        m_pMonData->isDamage = true;
        return BTNODESTATE::SUCCESS;
    }

    return BTNODESTATE::FAILURE;
}

BTNODESTATE CAI_Controller_Halberd::Damage(CGameObject* pOwner)
{
    if (m_pMonData->isDamage)
    {
        if (!m_pFSM->Check_Flag(ENUM_CLASS(CHalberd::MONSTATE::DAMAGE)))
            m_pFSM->Change_State(ENUM_CLASS(CHalberd::MONSTATE::DAMAGE), pOwner);

        return BTNODESTATE::RUNNING;
    }
    return BTNODESTATE::SUCCESS;
}

BTNODESTATE CAI_Controller_Halberd::Turn_Check(CGameObject* pOwner)
{
    if (m_pMonData->isSleep)
        return BTNODESTATE::FAILURE;

    TARGET_DIR eDir = m_pMonData->pOwner->Get_DIR();
    if (eDir != TARGET_DIR::F && eDir != TARGET_DIR::FR && eDir != TARGET_DIR::FL)
    {
        m_pMonData->isTurn = true;
        return BTNODESTATE::SUCCESS;
    }

    return BTNODESTATE::FAILURE;
}

BTNODESTATE CAI_Controller_Halberd::Turn(CGameObject* pOwner)
{
    if (m_pMonData->isTurn)
    {
        if (!m_pFSM->Check_Flag(ENUM_CLASS(CHalberd::MONSTATE::TURN)))
            m_pFSM->Change_State(ENUM_CLASS(CHalberd::MONSTATE::TURN), pOwner);

        return BTNODESTATE::RUNNING;
    }

    return BTNODESTATE::FAILURE;
}

BTNODESTATE CAI_Controller_Halberd::Attack_Long_Check(CGameObject* pOwner)
{
    if (!m_pMonData->isSleep && !m_pMonData->pOwner->Check_Ranage("AttackRange_Rush") && m_pMonData->fLong_AttackCool <= 0.f)
    {
        m_pMonData->eAttackState = CHalberd::ATTACKSTATE::LONG;
        return BTNODESTATE::SUCCESS;
    }
    else
        return BTNODESTATE::FAILURE;
 
}

BTNODESTATE CAI_Controller_Halberd::Attack_Long(CGameObject* pOwner)
{
    if (m_pMonData->eAttackState == CHalberd::ATTACKSTATE::LONG)
    {
        if (!m_pFSM->Check_Flag(ENUM_CLASS(CHalberd::MONSTATE::ATTACK_LONG)))
            m_pFSM->Change_State(ENUM_CLASS(CHalberd::MONSTATE::ATTACK_LONG), pOwner);

        return BTNODESTATE::RUNNING;
    }

    return BTNODESTATE::SUCCESS;
}

BTNODESTATE CAI_Controller_Halberd::Attack_Check(CGameObject* pOwner)
{
    if (!m_pMonData->isSleep && m_pMonData->pOwner->Check_Ranage("AttackRange_Middle") && m_pMonData->fAttackCool <= 0.f)
    {
         m_pMonData->eAttackState = CHalberd::ATTACKSTATE::DEFAULT;
         return BTNODESTATE::SUCCESS;
    }
    else
        return BTNODESTATE::FAILURE;
}

BTNODESTATE CAI_Controller_Halberd::Attack_Default(CGameObject* pOwner)
{
    if (m_pMonData->eAttackState == CHalberd::ATTACKSTATE::DEFAULT)
    {
        if (!m_pFSM->Check_Flag(ENUM_CLASS(CHalberd::MONSTATE::ATTACK_DEFAULT)))
            m_pFSM->Change_State(ENUM_CLASS(CHalberd::MONSTATE::ATTACK_DEFAULT), pOwner);

        return BTNODESTATE::RUNNING;
    }
    return BTNODESTATE::SUCCESS;
}

BTNODESTATE CAI_Controller_Halberd::LockOn(CGameObject* pOwner)
{
    if (!m_pMonData->isSleep)
    {
        if (!m_pFSM->Check_Flag(ENUM_CLASS(CHalberd::MONSTATE::LOCKON)))
            m_pFSM->Change_State(ENUM_CLASS(CHalberd::MONSTATE::LOCKON), pOwner);

        return BTNODESTATE::SUCCESS;
    }

    return BTNODESTATE::FAILURE;
}

BTNODESTATE CAI_Controller_Halberd::Sleep(CGameObject* pOwner)
{
    if (!m_pFSM->Check_Flag(ENUM_CLASS(CHalberd::MONSTATE::SLEEP)))
        m_pFSM->Change_State(ENUM_CLASS(CHalberd::MONSTATE::SLEEP), pOwner);

    if (m_pMonData->isSleep && m_pBB->Get_Value<_bool>(m_strMonstertag, "isDetected"))
        m_pMonData->isSleep = false;
    else if (m_pMonData->isStateFiash)
        return BTNODESTATE::SUCCESS;

    return BTNODESTATE::RUNNING;
}

CAI_Controller_Halberd* CAI_Controller_Halberd::Create(CCreature* pOwner)
{
    CAI_Controller_Halberd* pInstance = new CAI_Controller_Halberd();
    if (FAILED(pInstance->Initialize(pOwner)))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Failed Create : CAI_Controller_Halberd"));
    }

    return pInstance;
}

void CAI_Controller_Halberd::Free()
{
    __super::Free();
    m_pMonData = nullptr;
}
