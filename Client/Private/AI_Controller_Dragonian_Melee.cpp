#include "AI_Controller_Dragonian_Melee.h"
#include "GameInstance.h"

#include "FSM_Default.h"
#include "BehaviorTree.h"
#include "Perception.h"

#include "Dragonian_Melee.h"

#include "AS_Dr_Melee_Attack.h"
#include "AS_Dr_Melee_Brutal.h"
#include "AS_Dr_Melee_Damage.h"
#include "AS_Dr_Melee_Dead.h"
#include "AS_Dr_Melee_Groggy.h"
#include "AS_Dr_Melee_LockOn.h"
#include "AS_Dr_Melee_Sleep.h"
#include "AS_Dr_Melee_Walk.h"

CAI_Controller_Dragonian_Melee::CAI_Controller_Dragonian_Melee()
{
}

HRESULT CAI_Controller_Dragonian_Melee::Initialize(CCreature* pOwner)
{
    CDragonian_Melee* pCast_Owner = static_cast<CDragonian_Melee*>(pOwner);
    m_pMonData = &pCast_Owner->Get_Data();

    CHECK_FAILED(__super::Initialize(pCast_Owner, pCast_Owner->Get_Name()), E_FAIL);
    CHECK_FAILED(Ready_FSM(pCast_Owner), E_FAIL);

    m_pBB->Set_Value(m_strMonstertag, "CurrentTime", 0.f);


    return S_OK;

}

void CAI_Controller_Dragonian_Melee::Update(CGameObject* pOwner, _float fTimeDelta)
{
    if (!m_isActive)
        return;

    if (*m_pMonData->pCulHp <= 0.f)
    {
        m_pFSM->Change_State(ENUM_CLASS(CDragonian_Melee::MONSTATE::DEAD), pOwner);
    }
    else if (m_pMonData->isBrutal)
    {
        m_pFSM->Change_State(ENUM_CLASS(CDragonian_Melee::MONSTATE::BRUTAL), pOwner);
    }
    else if (*m_pMonData->pCulStamina <= 0.f)
    {
        m_pFSM->Change_State(ENUM_CLASS(CDragonian_Melee::MONSTATE::GRORRY), pOwner);
    }
    else
    {
        m_pPerception->Update(pOwner, m_pBB, fTimeDelta);

        //Update_Aggro(pOwner, fTimeDelta);

        //_float fPervTime = m_pBB->Get_Value<_float>(m_strMonstertag, "CurrentTime");

        //if (m_pBB->Get_Value<_bool>(m_strMonstertag, "HasAggro"))
        //{
        //    m_pBB->Set_Value<_float>(m_strMonstertag, "CurrentTime", fPervTime + fTimeDelta);
        //    m_pBT->Update();
        //}
        //else
        //    m_pBB->Set_Value(m_strMonstertag, "CurrentTime", 0.f);

        m_pBT->Update();
    }

    m_pFSM->Update(pOwner, fTimeDelta);

}

void CAI_Controller_Dragonian_Melee::Update_Aggro(CGameObject* pOwner, _float fTimeDelta)
{
    CGameObject* pTarget = m_pBB->Get_Value<CGameObject*>(m_strMonstertag, "Target");
    _bool isDetected = m_pBB->Get_Value<_bool>(m_strMonstertag, "isDetected");

    static _float fLostSightTime = 0.f;
    static const _float fFrogetDelay = 10.f;
    if (isDetected)
    {
    }
    else
    {

    }
}

HRESULT CAI_Controller_Dragonian_Melee::Ready_Perception(CGameObject* pOwner, const AIPERCEPTION_DATA& Desc)
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

HRESULT CAI_Controller_Dragonian_Melee::Ready_BlackBoard(CGameObject* pOwner)
{
    m_pBB = CBlackBoard::Create();
    if (nullptr == m_pBB)
        return E_FAIL;

    return S_OK;
}

HRESULT CAI_Controller_Dragonian_Melee::Ready_BehaviorTree()
{
    if (nullptr == m_pBB)
        return E_FAIL;

    m_pBT->Set_BlackBoard(m_pBB);

    if (nullptr == m_pBT)
        return E_FAIL;

    return S_OK;
}

CONDITION CAI_Controller_Dragonian_Melee::GetCallbackCondition(CGameObject* pOwner, const string& name)
{
    return nullptr;
}

ACTION CAI_Controller_Dragonian_Melee::GetCallbackAction(CGameObject* pOwner, const string& name)
{
    if ("Attack_Check" == name)  return [this, pOwner](CBlackBoard* BB)-> BTNODESTATE {return this->Attack_Check(pOwner); };
    else if ("Attack_Change" == name)  return [this, pOwner](CBlackBoard* BB)-> BTNODESTATE {return this->Attack(pOwner); };
    else if ("Damage_Check" == name)  return [this, pOwner](CBlackBoard* BB)-> BTNODESTATE {return this->Damage_Check(pOwner); };
    else if ("Damage_Change" == name)  return [this, pOwner](CBlackBoard* BB)-> BTNODESTATE {return this->Damage(pOwner); };
    else if ("LockOn_Change" == name)  return [this, pOwner](CBlackBoard* BB)-> BTNODESTATE {return this->LockOn(pOwner); };
    else if ("Chase_Change" == name)  return [this, pOwner](CBlackBoard* BB)-> BTNODESTATE {return this->Chase(pOwner); };
    else if ("Sleep_Change" == name) return [this, pOwner](CBlackBoard* BB)-> BTNODESTATE {return this->Sleep(pOwner); };

    return nullptr;
}

TERMINATE CAI_Controller_Dragonian_Melee::GetCallbackTeminate(CGameObject* pOwner, const string& name)
{
    return nullptr;

}

INTERRUPTCONDITION CAI_Controller_Dragonian_Melee::GetCallbackInterruptCondition(CGameObject* pOwner, const string& name)
{
    return nullptr;
}

PERCEPTIONCALLBACK CAI_Controller_Dragonian_Melee::GetCallBackPerception(CGameObject* pOwner, const string& name)
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

HRESULT CAI_Controller_Dragonian_Melee::Ready_FSM(class CCreature* pOwner)
{
    CFSM_Default* pFsm = CFSM_Default::Create(pOwner);
    
    CHECK_NULLPTR(pFsm, E_FAIL);

    CHECK_FAILED(pFsm->Add_State(ENUM_CLASS(CDragonian_Melee::MONSTATE::ATTACK), CAS_Dr_Melee_Attack::Create()), E_FAIL);
    CHECK_FAILED(pFsm->Add_State(ENUM_CLASS(CDragonian_Melee::MONSTATE::BRUTAL), CAS_Dr_Melee_Brutal::Create()), E_FAIL);
    CHECK_FAILED(pFsm->Add_State(ENUM_CLASS(CDragonian_Melee::MONSTATE::DAMAGE), CAS_Dr_Melee_Damage::Create()), E_FAIL);
    CHECK_FAILED(pFsm->Add_State(ENUM_CLASS(CDragonian_Melee::MONSTATE::DEAD), CAS_Dr_Melee_Dead::Create()), E_FAIL);
    CHECK_FAILED(pFsm->Add_State(ENUM_CLASS(CDragonian_Melee::MONSTATE::GRORRY), CAS_Dr_Melee_Groggy::Create()), E_FAIL);
    CHECK_FAILED(pFsm->Add_State(ENUM_CLASS(CDragonian_Melee::MONSTATE::LOCKON), CAS_Dr_Melee_LockOn::Create()), E_FAIL);
    CHECK_FAILED(pFsm->Add_State(ENUM_CLASS(CDragonian_Melee::MONSTATE::SLEEP), CAS_Dr_Melee_Sleep::Create()), E_FAIL);
    CHECK_FAILED(pFsm->Add_State(ENUM_CLASS(CDragonian_Melee::MONSTATE::WALK), CAS_Dr_Melee_Walk::Create()), E_FAIL);
    m_pFSM = pFsm;

    m_pMonData->isSleep = true;
    pFsm->Set_CulState(ENUM_CLASS(CDragonian_Melee::MONSTATE::SLEEP), pOwner);

    return S_OK;
}

BTNODESTATE CAI_Controller_Dragonian_Melee::Attack_Check(CGameObject* pOwner)
{
    if (!m_pMonData->isSleep && m_pMonData->pOwner->Check_AttackRanage("AttackRange"))
    {
        m_pMonData->isAttack = true;
        return BTNODESTATE::SUCCESS;
    }

    return BTNODESTATE::FAILURE;
}

BTNODESTATE CAI_Controller_Dragonian_Melee::Attack(CGameObject* pOwner)
{
    if (m_pMonData->isAttack)
    {
        if (!m_pFSM->Check_Flag(ENUM_CLASS(CDragonian_Melee::MONSTATE::ATTACK)))
            m_pFSM->Change_State(ENUM_CLASS(CDragonian_Melee::MONSTATE::ATTACK), pOwner);

        return BTNODESTATE::RUNNING;
    }

    return BTNODESTATE::FAILURE;
}

BTNODESTATE CAI_Controller_Dragonian_Melee::Damage_Check(CGameObject* pOwner)
{
    if (m_pMonData->isSleep)
        return BTNODESTATE::FAILURE;

    return BTNODESTATE::FAILURE;
}

BTNODESTATE CAI_Controller_Dragonian_Melee::Damage(CGameObject* pOwner)
{
    if (m_pMonData->isSleep)
        return BTNODESTATE::FAILURE;

    return BTNODESTATE::FAILURE;
}

BTNODESTATE CAI_Controller_Dragonian_Melee::LockOn(CGameObject* pOwner)
{
    if (!m_pMonData->isSleep && m_pMonData->pOwner->Check_AttackRanage("WalkRange"))
    {
        if (!m_pFSM->Check_Flag(ENUM_CLASS(CDragonian_Melee::MONSTATE::LOCKON)))
            m_pFSM->Change_State(ENUM_CLASS(CDragonian_Melee::MONSTATE::LOCKON), pOwner);

        return BTNODESTATE::SUCCESS;
    }

    return BTNODESTATE::FAILURE;
}

BTNODESTATE CAI_Controller_Dragonian_Melee::Chase(CGameObject* pOwner)
{
    if (m_pMonData->isSleep)
        return BTNODESTATE::FAILURE;

    if (!m_pFSM->Check_Flag(ENUM_CLASS(CDragonian_Melee::MONSTATE::WALK)))
        m_pFSM->Change_State(ENUM_CLASS(CDragonian_Melee::MONSTATE::WALK), pOwner);

    return BTNODESTATE::SUCCESS;
}

BTNODESTATE CAI_Controller_Dragonian_Melee::Sleep(CGameObject* pOwner)
{
    if (!m_pFSM->Check_Flag(ENUM_CLASS(CDragonian_Melee::MONSTATE::SLEEP)))
        m_pFSM->Change_State(ENUM_CLASS(CDragonian_Melee::MONSTATE::SLEEP), pOwner);

    _bool pisis = m_pBB->Get_Value<_bool>(m_strMonstertag, "isDetected");
    if (m_pMonData->isSleep && m_pBB->Get_Value<_bool>(m_strMonstertag, "isDetected"))
        m_pMonData->isSleep = false;
    else if (m_pMonData->isStateFiash)
        return BTNODESTATE::SUCCESS;

    return BTNODESTATE::RUNNING;
}

CAI_Controller_Dragonian_Melee* CAI_Controller_Dragonian_Melee::Create(CCreature* pOwner)
{
    CAI_Controller_Dragonian_Melee* pInstance = new CAI_Controller_Dragonian_Melee();
    if (FAILED(pInstance->Initialize(pOwner)))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Failed Create : CAI_Controller_Dragonian_Melee"));
    }

    return pInstance;
}

void CAI_Controller_Dragonian_Melee::Free()
{
    __super::Free();
}
