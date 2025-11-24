#include "AS_ThrowBlade_Viper.h"
#include "Viper.h"
#include "Body_Viper.h"
#include "BlackBoard.h"
#include "AI_Controller.h"
#include "GameInstance.h"


CAS_ThrowBlade_Viper::CAS_ThrowBlade_Viper()
{

}

void CAS_ThrowBlade_Viper::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    CViper* pViper = static_cast<CViper*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pViper->Get_Body()->Get_Component(TEXT("Com_Model")));

    pModel->Set_Animation(23);
}

void CAS_ThrowBlade_Viper::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    CViper* pViper = static_cast<CViper*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pViper->Get_Body()->Get_Component(TEXT("Com_Model")));

    if (pModel->Play_Animation(fTimeDelta))
    {
        pViper->Get_Controller()->Get_BlackBoard()->Set_Value<_bool>(pViper->Get_Name(), "isP1_ThrowBladeFinished", true);
    }

}

void CAS_ThrowBlade_Viper::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
}

CAS_ThrowBlade_Viper* CAS_ThrowBlade_Viper::Create()
{
    return new CAS_ThrowBlade_Viper();
}

void CAS_ThrowBlade_Viper::Free()
{
    __super::Free();
}
