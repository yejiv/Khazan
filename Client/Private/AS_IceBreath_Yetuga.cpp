#include "AS_IceBreath_Yetuga.h"
#include "AI_Controller.h"
#include "Yetuga.h"
#include "GameInstance.h"
#include "BlackBoard.h"
#include "FSM_Yetuga.h"
#include "Body_Yetuga.h"

CAS_IceBreath_Yetuga::CAS_IceBreath_Yetuga()
{

}

void CAS_IceBreath_Yetuga::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    CYetuga* pYetuga = static_cast<CYetuga*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pYetuga->Get_Body()->Get_Component(TEXT("Com_Model")));

    pModel->Set_Animation(49);
}

void CAS_IceBreath_Yetuga::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    CYetuga* pYetuga = static_cast<CYetuga*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pYetuga->Get_Body()->Get_Component(TEXT("Com_Model")));

    if (pModel->Play_Animation(fTimeDelta))
    {
        m_pGameInstance->Get_BlackBoard()->Set_Value<_bool>(pYetuga->Get_Name(), "isIceBreathFinished", true);
    }


}

void CAS_IceBreath_Yetuga::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{

}

CAS_IceBreath_Yetuga* CAS_IceBreath_Yetuga::Create()
{
    return new CAS_IceBreath_Yetuga();
}

void CAS_IceBreath_Yetuga::Free()
{
    __super::Free();
}
