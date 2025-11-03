#include "AS_Amageddon_Yetuga.h"
#include "AI_Controller.h"
#include "Yetuga.h"
#include "GameInstance.h"
#include "BlackBoard.h"
#include "FSM_Yetuga.h"
#include "Body_Yetuga.h"

CAS_Amageddon_Yetuga::CAS_Amageddon_Yetuga()
{

}

void CAS_Amageddon_Yetuga::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    CYetuga* pYetuga = static_cast<CYetuga*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pYetuga->Get_Body()->Get_Component(TEXT("Com_Model")));

    pModel->Set_Animation(56);
}

void CAS_Amageddon_Yetuga::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    CYetuga* pYetuga = static_cast<CYetuga*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pYetuga->Get_Body()->Get_Component(TEXT("Com_Model")));

    if (pModel->Play_Animation(fTimeDelta))
    {
        m_pGameInstance->Get_BlackBoard()->Set_Value<_bool>(pYetuga->Get_Name(), "isAmageddonFinished", true);
    }


}

void CAS_Amageddon_Yetuga::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{

}

CAS_Amageddon_Yetuga* CAS_Amageddon_Yetuga::Create()
{
    return new CAS_Amageddon_Yetuga();
}

void CAS_Amageddon_Yetuga::Free()
{
    __super::Free();
}
