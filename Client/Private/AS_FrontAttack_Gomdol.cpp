#include "AS_FrontAttack_Gomdol.h"
#include "AI_Controller.h"
#include "Gomdol.h"
#include "GameInstance.h"
#include "BlackBoard.h"
#include "Body_Gomdol.h"

CAS_FrontAttack_Gomdol::CAS_FrontAttack_Gomdol()
{
}

void CAS_FrontAttack_Gomdol::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    CGomdol* pYetuga = static_cast<CGomdol*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pYetuga->Get_Body()->Get_Component(TEXT("Com_Model")));

    _uint iRand = static_cast<_uint>(m_pGameInstance->Rand(0.f, 2.f));
    _uint iAnimID = 0;
    HITREACTION eHitreaction =
        static_cast<HITREACTION>(m_pGameInstance->Get_BlackBoard()->Get_Value<_uint>(pYetuga->Get_Name(), "DamageType"));

    if (HITREACTION::KNOCKBACK_STRONG == eHitreaction)
        iAnimID = 21;
    else
    {
        switch (iRand)
        {
        case 0:
            iAnimID = 12;
            break;
        case 1:
            iAnimID = 13;
            break;
        }
    }

    pModel->Set_Animation(iAnimID);
}

void CAS_FrontAttack_Gomdol::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    CGomdol* pYetuga = static_cast<CGomdol*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pYetuga->Get_Body()->Get_Component(TEXT("Com_Model")));

    if (pModel->Play_Animation(fTimeDelta))
    {
        m_pGameInstance->Get_BlackBoard()->Set_Value<_bool>(pYetuga->Get_Name(), "isFrontAttackFinished", true);
    }
}

void CAS_FrontAttack_Gomdol::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
}

CAS_FrontAttack_Gomdol* CAS_FrontAttack_Gomdol::Create()
{
    return new CAS_FrontAttack_Gomdol();
}

void CAS_FrontAttack_Gomdol::Free()
{
    __super::Free();
}
