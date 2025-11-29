#include "AI_Controller_Elamain.h"
#include "GameInstance.h"

#include "FSM_Default.h"
#include "BehaviorTree.h"
#include "Perception.h"

#include "AS_Dr_Rampage_Attack_Back.h"
#include "AS_Dr_Rampage_Attack_Rush.h"
#include "AS_Dr_Rampage_Attack_Default.h"
#include "AS_Dr_Rampage_Damage.h"
#include "AS_Dr_Rampage_LockOn.h"
#include "AS_Dr_Rampage_Brutal.h"
#include "AS_Dr_Rampage_Dead.h"
#include "AS_Dr_Rampage_Groggy.h"
#include "AS_Dr_Rampage_Sleep.h"
#include "AS_Dr_Rampage_Walk.h"
#include "AS_Dr_Rampage_Turn.h"
#include "AS_Dr_Rampage_Page2.h"

CAI_Controller_Elamain::CAI_Controller_Elamain()
{
}

HRESULT CAI_Controller_Elamain::Initialize(CCreature* pOwner)
{
    CDragonian_Rampage* pCast_Owner = static_cast<CDragonian_Rampage*>(pOwner);
    m_pMonData = &pCast_Owner->Get_Data();

    CHECK_FAILED(__super::Initialize(pCast_Owner, pCast_Owner->Get_Name()), E_FAIL);
    CHECK_FAILED(Ready_FSM(pCast_Owner), E_FAIL);

    m_pBB->Set_Value(m_strMonstertag, "CurrentTime", 0.f);

    return S_OK;

}

void CAI_Controller_Elamain::Update(CGameObject* pOwner, _float fTimeDelta)
{
    if (!m_isActive)
        return;

    //if (*m_pMonData->pCulHp <= 0.f)
    //{
    //    m_pFSM->Change_State(ENUM_CLASS(CDragonian_Rampage::MONSTATE::DEAD), pOwner);
    //}
    //else if (!m_pMonData->is2Page && m_pMonData->isPageChange)
    //{
    //    m_pFSM->Change_State(ENUM_CLASS(CDragonian_Rampage::MONSTATE::PAGE2), pOwner);
    //}
    //else if (m_pMonData->eHitType == HITREACTION::BRUTAL_ATTACK)
    //{
    //    m_pFSM->Change_State(ENUM_CLASS(CDragonian_Rampage::MONSTATE::BRUTAL), pOwner);
    //}
    //else if (*m_pMonData->pCulStamina <= 0.f)
    //{
    //    m_pFSM->Change_State(ENUM_CLASS(CDragonian_Rampage::MONSTATE::GRORRY), pOwner);
    //}
    //else
    //{
    //    m_pPerception->Update(pOwner, m_pBB, fTimeDelta);
    //    m_pBT->Update();
    //}

    //m_pFSM->Update(pOwner, fTimeDelta * m_pMonData->fDeltaSpeed);
}

HRESULT CAI_Controller_Elamain::Ready_Perception(CGameObject* pOwner, const AIPERCEPTION_DATA& Desc)
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

HRESULT CAI_Controller_Elamain::Ready_BlackBoard(CGameObject* pOwner)
{
    m_pBB = CBlackBoard::Create();
    if (nullptr == m_pBB)
        return E_FAIL;

    return S_OK;
}

HRESULT CAI_Controller_Elamain::Ready_BehaviorTree()
{
    if (nullptr == m_pBB)
        return E_FAIL;

    m_pBT->Set_BlackBoard(m_pBB);

    if (nullptr == m_pBT)
        return E_FAIL;

    return S_OK;
}

CONDITION CAI_Controller_Elamain::GetCallbackCondition(CGameObject* pOwner, const string& name)
{
    return nullptr;
}

ACTION CAI_Controller_Elamain::GetCallbackAction(CGameObject* pOwner, const string& name)
{
    if ("Attack_Check" == name)  return [this, pOwner](CBlackBoard* BB)-> BTNODESTATE {return this->Attack_Check(pOwner); };
    else if ("Attack_Rush" == name)  return [this, pOwner](CBlackBoard* BB)-> BTNODESTATE {return this->Attack_Rush(pOwner); };
    else if ("Attack_Back" == name)  return [this, pOwner](CBlackBoard* BB)-> BTNODESTATE {return this->Attack_Back(pOwner); };
    else if ("Attack_Default" == name)  return [this, pOwner](CBlackBoard* BB)-> BTNODESTATE {return this->Attack_Default(pOwner); };
    else if ("Damage_Check" == name)  return [this, pOwner](CBlackBoard* BB)-> BTNODESTATE {return this->Damage_Check(pOwner); };
    else if ("Damage" == name)  return [this, pOwner](CBlackBoard* BB)-> BTNODESTATE {return this->Damage(pOwner); };
    else if ("Turn_Check" == name)  return [this, pOwner](CBlackBoard* BB)-> BTNODESTATE {return this->Turn_Check(pOwner); };
    else if ("Turn" == name)  return [this, pOwner](CBlackBoard* BB)-> BTNODESTATE {return this->Turn(pOwner); };
    else if ("LockOn" == name)  return [this, pOwner](CBlackBoard* BB)-> BTNODESTATE {return this->LockOn(pOwner); };
    else if ("Walk" == name)  return [this, pOwner](CBlackBoard* BB)-> BTNODESTATE {return this->Walk(pOwner); };
    else if ("Sleep" == name) return [this, pOwner](CBlackBoard* BB)-> BTNODESTATE {return this->Sleep(pOwner); };

    return nullptr;
}

TERMINATE CAI_Controller_Elamain::GetCallbackTeminate(CGameObject* pOwner, const string& name)
{
    return nullptr;
}

INTERRUPTCONDITION CAI_Controller_Elamain::GetCallbackInterruptCondition(CGameObject* pOwner, const string& name)
{
    return nullptr;
}

PERCEPTIONCALLBACK CAI_Controller_Elamain::GetCallBackPerception(CGameObject* pOwner, const string& name)
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

HRESULT CAI_Controller_Elamain::Ready_FSM(class CCreature* pOwner)
{
    CFSM_Default* pFsm = CFSM_Default::Create(pOwner);

    CHECK_NULLPTR(pFsm, E_FAIL);

    CHECK_FAILED(pFsm->Add_State(ENUM_CLASS(CDragonian_Rampage::MONSTATE::PAGE2), CAS_Dr_Rampage_Page2::Create()), E_FAIL);
    CHECK_FAILED(pFsm->Add_State(ENUM_CLASS(CDragonian_Rampage::MONSTATE::ATTACK_BACK), CAS_Dr_Rampage_Attack_Back::Create()), E_FAIL);
    CHECK_FAILED(pFsm->Add_State(ENUM_CLASS(CDragonian_Rampage::MONSTATE::ATTACK_RUSH), CAS_Dr_Rampage_Attack_Rush::Create()), E_FAIL);
    CHECK_FAILED(pFsm->Add_State(ENUM_CLASS(CDragonian_Rampage::MONSTATE::ATTACK_DEFAULT), CAS_Dr_Rampage_Attack_Default::Create()), E_FAIL);
    CHECK_FAILED(pFsm->Add_State(ENUM_CLASS(CDragonian_Rampage::MONSTATE::DAMAGE), CAS_Dr_Rampage_Damage::Create()), E_FAIL);
    CHECK_FAILED(pFsm->Add_State(ENUM_CLASS(CDragonian_Rampage::MONSTATE::LOCKON), CAS_Dr_Rampage_LockOn::Create()), E_FAIL);
    CHECK_FAILED(pFsm->Add_State(ENUM_CLASS(CDragonian_Rampage::MONSTATE::BRUTAL), CAS_Dr_Rampage_Brutal::Create()), E_FAIL);
    CHECK_FAILED(pFsm->Add_State(ENUM_CLASS(CDragonian_Rampage::MONSTATE::DEAD), CAS_Dr_Rampage_Dead::Create()), E_FAIL);
    CHECK_FAILED(pFsm->Add_State(ENUM_CLASS(CDragonian_Rampage::MONSTATE::GRORRY), CAS_Dr_Rampage_Groggy::Create()), E_FAIL);
    CHECK_FAILED(pFsm->Add_State(ENUM_CLASS(CDragonian_Rampage::MONSTATE::TURN), CAS_Dr_Rampage_Turn::Create()), E_FAIL);
    CHECK_FAILED(pFsm->Add_State(ENUM_CLASS(CDragonian_Rampage::MONSTATE::SLEEP), CAS_Dr_Rampage_Sleep::Create()), E_FAIL);
    CHECK_FAILED(pFsm->Add_State(ENUM_CLASS(CDragonian_Rampage::MONSTATE::WALK), CAS_Dr_Rampage_Walk::Create()), E_FAIL);

    m_pFSM = pFsm;

    m_pMonData->isSleep = true;
    pFsm->Set_CulState(ENUM_CLASS(CDragonian_Rampage::MONSTATE::SLEEP), pOwner);

    return S_OK;
}

BTNODESTATE CAI_Controller_Elamain::Attack_Check(CGameObject* pOwner)
{
    if (m_pMonData->isSleep || m_pMonData->fAttackCool > 0.f)
        return BTNODESTATE::FAILURE;

    TARGET_DIR eDir = m_pMonData->pOwner->Get_DIR();
    if (eDir == TARGET_DIR::B || eDir == TARGET_DIR::BL || eDir == TARGET_DIR::BR)
        m_pMonData->eAttack_State = CDragonian_Rampage::ATTACKSTATE::BACK;
    else if (m_pMonData->pOwner->Check_Ranage("AttackRange"))
    {
        if (eDir == TARGET_DIR::FL || eDir == TARGET_DIR::FR || eDir == TARGET_DIR::F)
            m_pMonData->eAttack_State = CDragonian_Rampage::ATTACKSTATE::DEFAULT;
        else
            return BTNODESTATE::FAILURE;
    }
    else if (!m_pMonData->pOwner->Check_Ranage("AttackRange_Rush"))
    {
        m_pMonData->eAttack_State = CDragonian_Rampage::ATTACKSTATE::RUSH;
        return BTNODESTATE::SUCCESS;
    }
    else
        return BTNODESTATE::FAILURE;
}

BTNODESTATE CAI_Controller_Elamain::Attack_Rush(CGameObject* pOwner)
{
    if (m_pMonData->eAttack_State == CDragonian_Rampage::ATTACKSTATE::RUSH)
    {
        if (!m_pFSM->Check_Flag(ENUM_CLASS(CDragonian_Rampage::MONSTATE::ATTACK_RUSH)))
            m_pFSM->Change_State(ENUM_CLASS(CDragonian_Rampage::MONSTATE::ATTACK_RUSH), pOwner);

        return BTNODESTATE::RUNNING;
    }

    return BTNODESTATE::FAILURE;
}

BTNODESTATE CAI_Controller_Elamain::Attack_Back(CGameObject* pOwner)
{
    if (m_pMonData->eAttack_State == CDragonian_Rampage::ATTACKSTATE::BACK)
    {
        if (!m_pFSM->Check_Flag(ENUM_CLASS(CDragonian_Rampage::MONSTATE::ATTACK_BACK)))
            m_pFSM->Change_State(ENUM_CLASS(CDragonian_Rampage::MONSTATE::ATTACK_BACK), pOwner);

        return BTNODESTATE::RUNNING;
    }

    return BTNODESTATE::FAILURE;
}

BTNODESTATE CAI_Controller_Elamain::Attack_Default(CGameObject* pOwner)
{
    if (m_pMonData->eAttack_State == CDragonian_Rampage::ATTACKSTATE::DEFAULT)
    {
        if (!m_pFSM->Check_Flag(ENUM_CLASS(CDragonian_Rampage::MONSTATE::ATTACK_DEFAULT)))
            m_pFSM->Change_State(ENUM_CLASS(CDragonian_Rampage::MONSTATE::ATTACK_DEFAULT), pOwner);

        return BTNODESTATE::RUNNING;
    }

    return BTNODESTATE::FAILURE;
}

BTNODESTATE CAI_Controller_Elamain::Damage_Check(CGameObject* pOwner)
{
    if (!m_pMonData->isSleep && m_pMonData->eHitType != HITREACTION::END)
    {
        m_pMonData->isDamage = true;
        return BTNODESTATE::SUCCESS;
    }

    return BTNODESTATE::FAILURE;
}

BTNODESTATE CAI_Controller_Elamain::Damage(CGameObject* pOwner)
{
    if (m_pMonData->isDamage)
    {
        if (!m_pFSM->Check_Flag(ENUM_CLASS(CDragonian_Rampage::MONSTATE::DAMAGE)))
            m_pFSM->Change_State(ENUM_CLASS(CDragonian_Rampage::MONSTATE::DAMAGE), pOwner);

        return BTNODESTATE::RUNNING;
    }
    return BTNODESTATE::FAILURE;
}

BTNODESTATE CAI_Controller_Elamain::Turn_Check(CGameObject* pOwner)
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

BTNODESTATE CAI_Controller_Elamain::Turn(CGameObject* pOwner)
{
    if (m_pMonData->isTurn)
    {
        if (!m_pFSM->Check_Flag(ENUM_CLASS(CDragonian_Rampage::MONSTATE::DAMAGE)))
            m_pFSM->Change_State(ENUM_CLASS(CDragonian_Rampage::MONSTATE::TURN), pOwner);

        return BTNODESTATE::RUNNING;
    }
    return BTNODESTATE::FAILURE;
}

BTNODESTATE CAI_Controller_Elamain::LockOn(CGameObject* pOwner)
{
    if (!m_pMonData->isSleep && m_pMonData->isLockOn)
    {
        if (!m_pFSM->Check_Flag(ENUM_CLASS(CDragonian_Rampage::MONSTATE::LOCKON)))
            m_pFSM->Change_State(ENUM_CLASS(CDragonian_Rampage::MONSTATE::LOCKON), pOwner);

        return BTNODESTATE::SUCCESS;
    }

    return BTNODESTATE::FAILURE;
}

BTNODESTATE CAI_Controller_Elamain::Walk(CGameObject* pOwner)
{
    if (m_pMonData->isSleep)
        return BTNODESTATE::FAILURE;

    if (!m_pFSM->Check_Flag(ENUM_CLASS(CDragonian_Rampage::MONSTATE::WALK)))
        m_pFSM->Change_State(ENUM_CLASS(CDragonian_Rampage::MONSTATE::WALK), pOwner);

    return BTNODESTATE::SUCCESS;
}

BTNODESTATE CAI_Controller_Elamain::Sleep(CGameObject* pOwner)
{
    if (!m_pFSM->Check_Flag(ENUM_CLASS(CDragonian_Rampage::MONSTATE::SLEEP)))
        m_pFSM->Change_State(ENUM_CLASS(CDragonian_Rampage::MONSTATE::SLEEP), pOwner);

    _bool pisis = m_pBB->Get_Value<_bool>(m_strMonstertag, "isDetected");
    if (m_pMonData->isSleep && m_pBB->Get_Value<_bool>(m_strMonstertag, "isDetected"))
        m_pMonData->isSleep = false;
    else if (m_pMonData->isStateFiash)
        return BTNODESTATE::SUCCESS;

    return BTNODESTATE::RUNNING;
}

CAI_Controller_Elamain* CAI_Controller_Elamain::Create(CCreature* pOwner)
{
    CAI_Controller_Elamain* pInstance = new CAI_Controller_Elamain();
    if (FAILED(pInstance->Initialize(pOwner)))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Failed Create : CAI_Controller_Elamain"));
    }

    return pInstance;
}

void CAI_Controller_Elamain::Free()
{
    __super::Free();
}
