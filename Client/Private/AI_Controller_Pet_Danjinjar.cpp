#include "AI_Controller_Pet_Danjinjar.h"
#include "GameInstance.h"

#include "FSM_Default.h"
#include "BehaviorTree.h"
#include "Perception.h"

#include "AS_Pet_Danjinjar_Idle.h"
#include "AS_Pet_Danjinjar_Move.h"
#include "AS_Pet_Danjinjar_Damage.h"
#include "AS_Pet_Danjinjar_TP.h"

CAI_Controller_Pet_Danjinjar::CAI_Controller_Pet_Danjinjar()
{
}

HRESULT CAI_Controller_Pet_Danjinjar::Initialize(CCreature* pOwner)
{
    CPet_Danjinjar* pCast_Owner = static_cast<CPet_Danjinjar*>(pOwner);
    m_pMonData = &pCast_Owner->Get_Data();

    CHECK_FAILED(__super::Initialize(pCast_Owner, pCast_Owner->Get_Name()), E_FAIL);
    CHECK_FAILED(Ready_FSM(pCast_Owner), E_FAIL);

    m_pBB->Set_Value(m_strMonstertag, "CurrentTime", 0.f);


    return S_OK;

}

void CAI_Controller_Pet_Danjinjar::Update(CGameObject* pOwner, _float fTimeDelta)
{
    if (!m_isActive)
        return;

    m_pBT->Update();
    m_pFSM->Update(pOwner, fTimeDelta);

}

HRESULT CAI_Controller_Pet_Danjinjar::Ready_Perception(CGameObject* pOwner, const AIPERCEPTION_DATA& Desc)
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

HRESULT CAI_Controller_Pet_Danjinjar::Ready_BlackBoard(CGameObject* pOwner)
{
    m_pBB = CBlackBoard::Create();
    if (nullptr == m_pBB)
        return E_FAIL;

    return S_OK;
}

HRESULT CAI_Controller_Pet_Danjinjar::Ready_BehaviorTree()
{
    if (nullptr == m_pBB)
        return E_FAIL;

    m_pBT->Set_BlackBoard(m_pBB);

    if (nullptr == m_pBT)
        return E_FAIL;

    return S_OK;
}

CONDITION CAI_Controller_Pet_Danjinjar::GetCallbackCondition(CGameObject* pOwner, const string& name)
{
    return nullptr;
}

ACTION CAI_Controller_Pet_Danjinjar::GetCallbackAction(CGameObject* pOwner, const string& name)
{
    if ("Move" == name)  return [this, pOwner](CBlackBoard* BB)-> BTNODESTATE {return this->Move(pOwner); };
    else if ("Idle" == name)  return [this, pOwner](CBlackBoard* BB)-> BTNODESTATE {return this->Idle(pOwner); };
    else if ("TP" == name)  return [this, pOwner](CBlackBoard* BB)-> BTNODESTATE {return this->TP(pOwner); };
    else if ("Damage" == name)  return [this, pOwner](CBlackBoard* BB)-> BTNODESTATE {return this->Damage(pOwner); };

    return nullptr;
}

TERMINATE CAI_Controller_Pet_Danjinjar::GetCallbackTeminate(CGameObject* pOwner, const string& name)
{
    return nullptr;
}

INTERRUPTCONDITION CAI_Controller_Pet_Danjinjar::GetCallbackInterruptCondition(CGameObject* pOwner, const string& name)
{
    return nullptr;
}

PERCEPTIONCALLBACK CAI_Controller_Pet_Danjinjar::GetCallBackPerception(CGameObject* pOwner, const string& name)
{
    return nullptr;
}

HRESULT CAI_Controller_Pet_Danjinjar::Ready_FSM(class CCreature* pOwner)
{
    CFSM_Default* pFsm = CFSM_Default::Create(pOwner);

    CHECK_NULLPTR(pFsm, E_FAIL);

    CHECK_FAILED(pFsm->Add_State(ENUM_CLASS(CPet_Danjinjar::MONSTATE::IDLE), CAS_Pet_Danjinjar_Idle::Create()), E_FAIL);
    CHECK_FAILED(pFsm->Add_State(ENUM_CLASS(CPet_Danjinjar::MONSTATE::MOVE), CAS_Pet_Danjinjar_Move::Create()), E_FAIL);
    CHECK_FAILED(pFsm->Add_State(ENUM_CLASS(CPet_Danjinjar::MONSTATE::DAMAGE), CAS_Pet_Danjinjar_Damage::Create()), E_FAIL);
    CHECK_FAILED(pFsm->Add_State(ENUM_CLASS(CPet_Danjinjar::MONSTATE::TP), CAS_Pet_Danjinjar_TP::Create()), E_FAIL);
    m_pFSM = pFsm;

    pFsm->Set_CulState(ENUM_CLASS(CPet_Danjinjar::MONSTATE::IDLE), pOwner);

    return S_OK;
}

BTNODESTATE CAI_Controller_Pet_Danjinjar::TP(CGameObject* pOwner)
{
    if (!m_pMonData->pOwner->Check_Ranage("TPRanage") || m_pMonData->isTP)
    {   
        if (!m_pFSM->Check_Flag(ENUM_CLASS(CPet_Danjinjar::MONSTATE::TP)))
            m_pFSM->Change_State(ENUM_CLASS(CPet_Danjinjar::MONSTATE::TP), pOwner);

        return BTNODESTATE::SUCCESS;
    }
    return BTNODESTATE::FAILURE;
}

BTNODESTATE CAI_Controller_Pet_Danjinjar::Damage(CGameObject* pOwner)
{
    if (m_pMonData->isDamage && !m_pMonData->isEnd)
    {
        if (!m_pFSM->Check_Flag(ENUM_CLASS(CPet_Danjinjar::MONSTATE::DAMAGE)))
            m_pFSM->Change_State(ENUM_CLASS(CPet_Danjinjar::MONSTATE::DAMAGE), pOwner);

        return BTNODESTATE::RUNNING;
    }
    return BTNODESTATE::FAILURE;
}

BTNODESTATE CAI_Controller_Pet_Danjinjar::Move(CGameObject* pOwner)
{
    if (m_pMonData->isIdle)
        return BTNODESTATE::FAILURE;

    if (!m_pMonData->pOwner->Check_Ranage("MoveRange"))
    {
        if (!m_pFSM->Check_Flag(ENUM_CLASS(CPet_Danjinjar::MONSTATE::MOVE)))
            m_pFSM->Change_State(ENUM_CLASS(CPet_Danjinjar::MONSTATE::MOVE), pOwner);

        return BTNODESTATE::SUCCESS;
    }
    return BTNODESTATE::FAILURE;
}

BTNODESTATE CAI_Controller_Pet_Danjinjar::Idle(CGameObject* pOwner)
{
    if (!m_pFSM->Check_Flag(ENUM_CLASS(CPet_Danjinjar::MONSTATE::IDLE)))
        m_pFSM->Change_State(ENUM_CLASS(CPet_Danjinjar::MONSTATE::IDLE), pOwner);

    if(m_pMonData->isEnd)
        return BTNODESTATE::RUNNING;

    return BTNODESTATE::SUCCESS;
}

CAI_Controller_Pet_Danjinjar* CAI_Controller_Pet_Danjinjar::Create(CCreature* pOwner)
{
    CAI_Controller_Pet_Danjinjar* pInstance = new CAI_Controller_Pet_Danjinjar();
    if (FAILED(pInstance->Initialize(pOwner)))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Failed Create : CAI_Controller_Pet_Danjinjar"));
    }

    return pInstance;
}

void CAI_Controller_Pet_Danjinjar::Free()
{
    __super::Free();
}
