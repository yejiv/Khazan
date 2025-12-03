#include "AS_Idle_Viper.h"
#include "GameInstance.h"
#include "BlackBoard.h"
#include "Viper.h"
#include "Body_Viper.h"
#include "Body_Phase2_Viper.h"


CAS_Idle_Viper::CAS_Idle_Viper()
{

}

void CAS_Idle_Viper::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    CViper* pViper = static_cast<CViper*>(pOwner);
    if (CViper::PHASE::PHASE1 == pViper->Get_Phase())
    {
        CModel* pModel = static_cast<CModel*>(pViper->Get_Body()->Get_Component(TEXT("Com_Model")));
        pModel->Set_Animation(54);
    }
   
    else if (CViper::PHASE::PHASE2 == pViper->Get_Phase())
    {
        CModel* pModel = static_cast<CModel*>(pViper->Get_P2Body()->Get_Component(TEXT("Com_Model")));
        pModel->Set_Animation(47);
    }

}

void CAS_Idle_Viper::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    CViper* pViper = static_cast<CViper*>(pOwner);
    if (CViper::PHASE::PHASE1 == pViper->Get_Phase())
    {
        CModel* pModel = static_cast<CModel*>(pViper->Get_Body()->Get_Component(TEXT("Com_Model")));
        pModel->Play_Animation(fTimeDelta);
        
    }
    else if (CViper::PHASE::PHASE2 == pViper->Get_Phase())
    {
        CModel* pModel = static_cast<CModel*>(pViper->Get_P2Body()->Get_Component(TEXT("Com_Model")));
        pModel->Play_Animation(fTimeDelta);
    }
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
