#include "AI_Controller_Elamein.h"
#include "GameInstance.h"

#include "FSM_Default.h"
#include "BehaviorTree.h"
#include "Perception.h"

#include "AS_Elamein_Dead.h"
#include "AS_Elamein_Groggy.h"
#include "AS_Elamein_Brutal.h"
#include "AS_Elamein_Attack_Long.h"
#include "AS_Elamein_Dodge.h"
#include "AS_Elamein_Attack_Enchant.h"
#include "AS_Elamein_Attack_Default.h"
#include "AS_Elamein_Damage.h"
#include "AS_Elamein_LockOn.h"
#include "AS_Elamein_Guard.h"
#include "AS_Elamein_Attack_Middle.h"
#include "AS_Elamein_Sleep.h"
#include "AS_Elamein_Turn.h"

CAI_Controller_Elamein::CAI_Controller_Elamein()
{
}

HRESULT CAI_Controller_Elamein::Initialize(CCreature* pOwner)
{
    CElamein* pCast_Owner = static_cast<CElamein*>(pOwner);
    m_pMonData = &pCast_Owner->Get_Data();

    CHECK_FAILED(__super::Initialize(pCast_Owner, pCast_Owner->Get_Name()), E_FAIL);
    CHECK_FAILED(Ready_FSM(pCast_Owner), E_FAIL);

    m_pBB->Set_Value(m_strMonstertag, "CurrentTime", 0.f);

    return S_OK;

}

void CAI_Controller_Elamein::Update(CGameObject* pOwner, _float fTimeDelta)
{
    if (!m_isActive)
        return;

    if (*m_pMonData->pCulHp <= 0.f)
    {
        m_pFSM->Change_State(ENUM_CLASS(CElamein::MONSTATE::DEAD), pOwner);
    }
    else if (m_pMonData->eHitType == HITREACTION::BRUTAL_ATTACK)
    {
        m_pFSM->Change_State(ENUM_CLASS(CElamein::MONSTATE::BRUTAL), pOwner);
    }
    else if (*m_pMonData->pCulStamina <= 0.f)

    {
        m_pFSM->Change_State(ENUM_CLASS(CElamein::MONSTATE::GRORRY), pOwner);
    }
    else
    {
        m_pPerception->Update(pOwner, m_pBB, fTimeDelta);
        m_pBT->Update();
    }

    m_pFSM->Update(pOwner, fTimeDelta * m_pMonData->fDeltaSpeed);
}

HRESULT CAI_Controller_Elamein::Ready_Perception(CGameObject* pOwner, const AIPERCEPTION_DATA& Desc)
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

HRESULT CAI_Controller_Elamein::Ready_BlackBoard(CGameObject* pOwner)
{
    m_pBB = CBlackBoard::Create();
    if (nullptr == m_pBB)
        return E_FAIL;

    return S_OK;
}

HRESULT CAI_Controller_Elamein::Ready_BehaviorTree()
{
    if (nullptr == m_pBB)
        return E_FAIL;

    m_pBT->Set_BlackBoard(m_pBB);

    if (nullptr == m_pBT)
        return E_FAIL;

    return S_OK;
}

CONDITION CAI_Controller_Elamein::GetCallbackCondition(CGameObject* pOwner, const string& name)
{
    return nullptr;
}

ACTION CAI_Controller_Elamein::GetCallbackAction(CGameObject* pOwner, const string& name)
{
    if ("Damage_Check" == name)             return [this, pOwner](CBlackBoard* BB)-> BTNODESTATE {return this->Damage_Check(pOwner); };
    else if ("Dodge" == name)               return [this, pOwner](CBlackBoard* BB)-> BTNODESTATE {return this->Dodge(pOwner); };
    else if ("Damage" == name)              return [this, pOwner](CBlackBoard* BB)-> BTNODESTATE {return this->Damage(pOwner); };
    else if ("Turn_Check" == name)          return [this, pOwner](CBlackBoard* BB)-> BTNODESTATE {return this->Turn_Check(pOwner); };
    else if ("Turn" == name)                return [this, pOwner](CBlackBoard* BB)-> BTNODESTATE {return this->Turn(pOwner); };
    else if ("Attack_Long_Check" == name)   return [this, pOwner](CBlackBoard* BB)-> BTNODESTATE {return this->Attack_Long_Check(pOwner); };
    else if ("Attack_Long" == name)         return [this, pOwner](CBlackBoard* BB)-> BTNODESTATE {return this->Attack_Long(pOwner); };
    else if ("Guard_Check" == name)         return [this, pOwner](CBlackBoard* BB)-> BTNODESTATE {return this->Guard_Check(pOwner); };
    else if ("Guard" == name)               return [this, pOwner](CBlackBoard* BB)-> BTNODESTATE {return this->Guard(pOwner); };
    else if ("Attack_Check" == name)        return [this, pOwner](CBlackBoard* BB)-> BTNODESTATE {return this->Attack_Check(pOwner); };
    else if ("Attack_Enchant" == name)      return [this, pOwner](CBlackBoard* BB)-> BTNODESTATE {return this->Attack_Enchant(pOwner); };
    else if ("Attack_Middle" == name)       return [this, pOwner](CBlackBoard* BB)-> BTNODESTATE {return this->Attack_Middle(pOwner); };
    else if ("Attack_Default" == name)      return [this, pOwner](CBlackBoard* BB)-> BTNODESTATE {return this->Attack_Default(pOwner); };
    else if ("LockOn" == name)              return [this, pOwner](CBlackBoard* BB)-> BTNODESTATE {return this->LockOn(pOwner); };
    else if ("Sleep" == name)               return [this, pOwner](CBlackBoard* BB)-> BTNODESTATE {return this->Sleep(pOwner); };

    return nullptr;
}

TERMINATE CAI_Controller_Elamein::GetCallbackTeminate(CGameObject* pOwner, const string& name)
{
    return nullptr;
}

INTERRUPTCONDITION CAI_Controller_Elamein::GetCallbackInterruptCondition(CGameObject* pOwner, const string& name)
{
    return nullptr;
}

PERCEPTIONCALLBACK CAI_Controller_Elamein::GetCallBackPerception(CGameObject* pOwner, const string& name)
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

HRESULT CAI_Controller_Elamein::Ready_FSM(class CCreature* pOwner)
{
    CFSM_Default* pFsm = CFSM_Default::Create(pOwner);

    CHECK_NULLPTR(pFsm, E_FAIL);

    CHECK_FAILED(pFsm->Add_State(ENUM_CLASS(CElamein::MONSTATE::ATTACK_LONG), CAS_Elamein_Attack_Long::Create()), E_FAIL);
    CHECK_FAILED(pFsm->Add_State(ENUM_CLASS(CElamein::MONSTATE::ATTACK_ENCHANT), CAS_Elamein_Attack_Enchant::Create()), E_FAIL);
    CHECK_FAILED(pFsm->Add_State(ENUM_CLASS(CElamein::MONSTATE::ATTACK_MIDDLE), CAS_Elamein_Attack_Middle::Create()), E_FAIL);
    CHECK_FAILED(pFsm->Add_State(ENUM_CLASS(CElamein::MONSTATE::ATTACK_DEFAULT), CAS_Elamein_Attack_default::Create()), E_FAIL);
    CHECK_FAILED(pFsm->Add_State(ENUM_CLASS(CElamein::MONSTATE::DEAD), CAS_Elamein_Dead::Create()), E_FAIL);
    CHECK_FAILED(pFsm->Add_State(ENUM_CLASS(CElamein::MONSTATE::GRORRY), CAS_Elamein_Groggy::Create()), E_FAIL);
    CHECK_FAILED(pFsm->Add_State(ENUM_CLASS(CElamein::MONSTATE::BRUTAL), CAS_Elamein_Brutal::Create()), E_FAIL);
    CHECK_FAILED(pFsm->Add_State(ENUM_CLASS(CElamein::MONSTATE::DODGE), CAS_Elamein_Dodge::Create()), E_FAIL);
    CHECK_FAILED(pFsm->Add_State(ENUM_CLASS(CElamein::MONSTATE::LOCKON), CAS_Elamein_LockOn::Create()), E_FAIL);
    CHECK_FAILED(pFsm->Add_State(ENUM_CLASS(CElamein::MONSTATE::DAMAGE), CAS_Elamein_Damage::Create()), E_FAIL);
    CHECK_FAILED(pFsm->Add_State(ENUM_CLASS(CElamein::MONSTATE::GUARD), CAS_Elamein_Guard::Create()), E_FAIL);
    CHECK_FAILED(pFsm->Add_State(ENUM_CLASS(CElamein::MONSTATE::TURN), CAS_Elamein_Turn::Create()), E_FAIL);
    CHECK_FAILED(pFsm->Add_State(ENUM_CLASS(CElamein::MONSTATE::SLEEP), CAS_Elamein_Sleep::Create()), E_FAIL);
    
    m_pFSM = pFsm;

    m_pMonData->isSleep = true;
    pFsm->Set_CulState(ENUM_CLASS(CElamein::MONSTATE::SLEEP), pOwner);

    return S_OK;
}

BTNODESTATE CAI_Controller_Elamein::Damage_Check(CGameObject* pOwner)
{
    if (!m_pMonData->isSleep && m_pMonData->eHitType != HITREACTION::END)
    {
        TARGET_DIR eDir = m_pMonData->pOwner->Get_DIR();
        if (m_pMonData->fDodgeCool > 0.f && (eDir == TARGET_DIR::F || eDir == TARGET_DIR::FR || eDir == TARGET_DIR::FL))
            m_pMonData->isDamage = true;
        else
            m_pMonData->isDodge = true;

        return BTNODESTATE::SUCCESS;
    }

    return BTNODESTATE::FAILURE;
}

BTNODESTATE CAI_Controller_Elamein::Dodge(CGameObject* pOwner)
{
    if (m_pMonData->isDodge)
    {
        if (!m_pFSM->Check_Flag(ENUM_CLASS(CElamein::MONSTATE::DODGE)))
            m_pFSM->Change_State(ENUM_CLASS(CElamein::MONSTATE::DODGE), pOwner);

        return BTNODESTATE::RUNNING;
    }

    return BTNODESTATE::FAILURE;
}

BTNODESTATE CAI_Controller_Elamein::Damage(CGameObject* pOwner)
{
    if (m_pMonData->isDamage)
    {
        if (!m_pFSM->Check_Flag(ENUM_CLASS(CElamein::MONSTATE::DAMAGE)))
            m_pFSM->Change_State(ENUM_CLASS(CElamein::MONSTATE::DAMAGE), pOwner);

        return BTNODESTATE::RUNNING;
    }
    return BTNODESTATE::FAILURE;
}

BTNODESTATE CAI_Controller_Elamein::Turn_Check(CGameObject* pOwner)
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

BTNODESTATE CAI_Controller_Elamein::Turn(CGameObject* pOwner)
{
    if (m_pMonData->isTurn)
    {
        if (!m_pFSM->Check_Flag(ENUM_CLASS(CElamein::MONSTATE::TURN)))
            m_pFSM->Change_State(ENUM_CLASS(CElamein::MONSTATE::TURN), pOwner);

        return BTNODESTATE::RUNNING;
    }

    return BTNODESTATE::FAILURE;
}

BTNODESTATE CAI_Controller_Elamein::Attack_Long_Check(CGameObject* pOwner)
{
    if (!m_pMonData->isSleep && !m_pMonData->pOwner->Check_Ranage("AttackRange_Rush") && m_pMonData->fLong_AttackCool <= 0.f)
    {
        m_pMonData->eAttackState = CElamein::ATTACKSTATE::LONG;
        return BTNODESTATE::SUCCESS;
    }
    else
        return BTNODESTATE::FAILURE;
 
}

BTNODESTATE CAI_Controller_Elamein::Attack_Long(CGameObject* pOwner)
{
    if (m_pMonData->eAttackState == CElamein::ATTACKSTATE::LONG)
    {
        if (!m_pFSM->Check_Flag(ENUM_CLASS(CElamein::MONSTATE::ATTACK_LONG)))
            m_pFSM->Change_State(ENUM_CLASS(CElamein::MONSTATE::ATTACK_LONG), pOwner);

        return BTNODESTATE::RUNNING;
    }

    return BTNODESTATE::SUCCESS;
}

BTNODESTATE CAI_Controller_Elamein::Guard_Check(CGameObject* pOwner)
{
    if (!m_pMonData->isSleep && m_pMonData->fGuardCool <= 0.f)
    {
        m_pMonData->isGuard = true;

        return BTNODESTATE::SUCCESS;
    }

    return BTNODESTATE::FAILURE;
}

BTNODESTATE CAI_Controller_Elamein::Guard(CGameObject* pOwner)
{
    if (m_pMonData->isGuard)
    { 
        if (!m_pFSM->Check_Flag(ENUM_CLASS(CElamein::MONSTATE::GUARD)))
            m_pFSM->Change_State(ENUM_CLASS(CElamein::MONSTATE::GUARD), pOwner);

        return BTNODESTATE::RUNNING;
    }

    return BTNODESTATE::FAILURE;
}

BTNODESTATE CAI_Controller_Elamein::Attack_Check(CGameObject* pOwner)
{
    if (!m_pMonData->isSleep)
    {
        _bool isMaxRange = m_pMonData->pOwner->Check_Ranage("AttackRange_Rush");
        _bool isMinRange = m_pMonData->pOwner->Check_Ranage("AttackRange");
        _bool isMiddleRange = m_pMonData->pOwner->Check_Ranage("AttackRange_Middle");
        if(isMaxRange && !isMinRange && m_pMonData->fSpecial_AttackCool <= 0.f)
        {
            m_pMonData->eAttackState = CElamein::ATTACKSTATE::ENCHANT;
            return BTNODESTATE::SUCCESS;
        }
        else if (isMiddleRange && m_pMonData->fAttackCool <= 0.f)
        {
            if (!isMinRange)
                 m_pMonData->eAttackState = CElamein::ATTACKSTATE::MIDDLE;
            else
                 m_pMonData->eAttackState = CElamein::ATTACKSTATE::DEFAULT;

            return BTNODESTATE::SUCCESS;
        }
        return BTNODESTATE::FAILURE;
    }
    else
        return BTNODESTATE::FAILURE;
}

BTNODESTATE CAI_Controller_Elamein::Attack_Enchant(CGameObject* pOwner)
{
    if (m_pMonData->eAttackState == CElamein::ATTACKSTATE::ENCHANT)
    {
        if (!m_pFSM->Check_Flag(ENUM_CLASS(CElamein::MONSTATE::ATTACK_ENCHANT)))
            m_pFSM->Change_State(ENUM_CLASS(CElamein::MONSTATE::ATTACK_ENCHANT), pOwner);

        return BTNODESTATE::RUNNING;
    }
    else if(m_pMonData->eAttackState == CElamein::ATTACKSTATE::END)
    {
        return BTNODESTATE::SUCCESS;
    }
    return BTNODESTATE::FAILURE;
}

BTNODESTATE CAI_Controller_Elamein::Attack_Middle(CGameObject* pOwner)
{
    if (m_pMonData->eAttackState == CElamein::ATTACKSTATE::MIDDLE)
    {
        if (!m_pFSM->Check_Flag(ENUM_CLASS(CElamein::MONSTATE::ATTACK_MIDDLE)))
            m_pFSM->Change_State(ENUM_CLASS(CElamein::MONSTATE::ATTACK_MIDDLE), pOwner);

        return BTNODESTATE::RUNNING;
    }
    else if (m_pMonData->eAttackState == CElamein::ATTACKSTATE::END)
    {
        return BTNODESTATE::SUCCESS;
    }
    return BTNODESTATE::FAILURE;
}

BTNODESTATE CAI_Controller_Elamein::Attack_Default(CGameObject* pOwner)
{
    if (m_pMonData->eAttackState == CElamein::ATTACKSTATE::DEFAULT)
    {
        if (!m_pFSM->Check_Flag(ENUM_CLASS(CElamein::MONSTATE::ATTACK_DEFAULT)))
            m_pFSM->Change_State(ENUM_CLASS(CElamein::MONSTATE::ATTACK_DEFAULT), pOwner);

        return BTNODESTATE::RUNNING;
    }
    return BTNODESTATE::SUCCESS;
}

BTNODESTATE CAI_Controller_Elamein::LockOn(CGameObject* pOwner)
{
    if (!m_pMonData->isSleep)
    {
        if (!m_pFSM->Check_Flag(ENUM_CLASS(CElamein::MONSTATE::LOCKON)))
            m_pFSM->Change_State(ENUM_CLASS(CElamein::MONSTATE::LOCKON), pOwner);

        return BTNODESTATE::SUCCESS;
    }

    return BTNODESTATE::FAILURE;
}

BTNODESTATE CAI_Controller_Elamein::Sleep(CGameObject* pOwner)
{
    if (!m_pFSM->Check_Flag(ENUM_CLASS(CElamein::MONSTATE::SLEEP)))
        m_pFSM->Change_State(ENUM_CLASS(CElamein::MONSTATE::SLEEP), pOwner);

    if (m_pMonData->isSleep && m_pBB->Get_Value<_bool>(m_strMonstertag, "isDetected"))
        m_pMonData->isSleep = false;
    else if (m_pMonData->isStateFiash)
        return BTNODESTATE::SUCCESS;

    return BTNODESTATE::RUNNING;
}

CAI_Controller_Elamein* CAI_Controller_Elamein::Create(CCreature* pOwner)
{
    CAI_Controller_Elamein* pInstance = new CAI_Controller_Elamein();
    if (FAILED(pInstance->Initialize(pOwner)))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Failed Create : CAI_Controller_Elamein"));
    }

    return pInstance;
}

void CAI_Controller_Elamein::Free()
{
    __super::Free();
}
