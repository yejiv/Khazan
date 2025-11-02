#include "AS_Rush_Yetuga.h"
#include "AI_Controller.h"
#include "Yetuga.h"
#include "GameInstance.h"
#include "BlackBoard.h"
#include "FSM_Yetuga.h"
#include "Body_Yetuga.h"

CAS_Rush_Yetuga::CAS_Rush_Yetuga()
{
}

void CAS_Rush_Yetuga::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    CYetuga* pYetuga = static_cast<CYetuga*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pYetuga->Get_Body()->Get_Component(TEXT("Com_Model")));

    pModel->Set_AnimationSet("Rush");

}

void CAS_Rush_Yetuga::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    CYetuga* pYetuga = static_cast<CYetuga*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pYetuga->Get_Body()->Get_Component(TEXT("Com_Model")));

    _bool isCrashed = m_pGameInstance->Get_BlackBoard()->Get_Value<_bool>(pYetuga->Get_Name(), "isCrahsedWall");

    if (isCrashed && !m_isCrashed)
    {
        pModel->Set_Animation(38);
        m_isCrashed = true;
    }
        
    if (pModel->Play_Animation(fTimeDelta))
    {
        m_pGameInstance->Get_BlackBoard()->Set_Value<_bool>(pYetuga->Get_Name(), "isRushFinished", true);
    }

}

void CAS_Rush_Yetuga::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
    CYetuga* pYetuga = static_cast<CYetuga*>(pOwner);
    m_pGameInstance->Get_BlackBoard()->Set_Value<_bool>(pYetuga->Get_Name(), "isCrahsedWall", false);
    m_isCrashed = false;
}

CAS_Rush_Yetuga* CAS_Rush_Yetuga::Create()
{
    return new CAS_Rush_Yetuga();
}

void CAS_Rush_Yetuga::Free()
{
    __super::Free();
}
