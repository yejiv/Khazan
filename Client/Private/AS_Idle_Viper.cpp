#include "AS_Idle_Viper.h"
#include "GameInstance.h"
#include "BlackBoard.h"
#include "Viper.h"
#include "Body_Viper.h"


CAS_Idle_Viper::CAS_Idle_Viper()
{

}

void CAS_Idle_Viper::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    CViper* pViper = static_cast<CViper*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pViper->Get_Body()->Get_Component(TEXT("Com_Model")));
    
    pModel->Set_Animation(112);

}

void CAS_Idle_Viper::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    CViper* pViper = static_cast<CViper*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pViper->Get_Body()->Get_Component(TEXT("Com_Model")));

    pModel->Play_Animation(fTimeDelta);
}

void CAS_Idle_Viper::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{

}

CAS_Idle_Viper* CAS_Idle_Viper::Create()
{
    return new CAS_Idle_Viper();
}

void CAS_Idle_Viper::Free()
{
    __super::Free();
}
