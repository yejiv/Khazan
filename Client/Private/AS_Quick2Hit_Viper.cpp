#include "AS_Quick2Hit_Viper.h"
#include "Viper.h"
#include "Body_Viper.h"
#include "AI_Controller.h"
#include "BlackBoard.h"
#include "FSM_Viper.h"

CAS_Quick2Hit_Viper::CAS_Quick2Hit_Viper()
{

}

void CAS_Quick2Hit_Viper::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    CViper* pViper = static_cast<CViper*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pViper->Get_Body()->Get_Component(TEXT("Com_Model")));

    pModel->Set_Animation(111);
     
}

void CAS_Quick2Hit_Viper::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    CViper* pViper = static_cast<CViper*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pViper->Get_Body()->Get_Component(TEXT("Com_Model")));


    if (pModel->Play_Animation(fTimeDelta))
    {
        pViper->Get_Controller()->Get_BlackBoard()->Set_Value<_bool>(pViper->Get_Name(), "isP1_Quick2HitFinished", true);
    }

}

void CAS_Quick2Hit_Viper::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{

}

CAS_Quick2Hit_Viper* CAS_Quick2Hit_Viper::Create()
{
    return new CAS_Quick2Hit_Viper();
}

void CAS_Quick2Hit_Viper::Free()
{
    __super::Free();
}
