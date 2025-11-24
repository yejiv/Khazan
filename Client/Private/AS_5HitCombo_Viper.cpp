#include "AS_5HitCombo_Viper.h"
#include "Viper.h"
#include "Body_Viper.h"
#include "BlackBoard.h"
#include "AI_Controller.h"
#include "GameInstance.h"


CAS_5HitCombo_Viper::CAS_5HitCombo_Viper()
{

}

void CAS_5HitCombo_Viper::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    CViper* pViper = static_cast<CViper*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pViper->Get_Body()->Get_Component(TEXT("Com_Model")));

    pModel->Set_Animation(109);
}

void CAS_5HitCombo_Viper::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    CViper* pViper = static_cast<CViper*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pViper->Get_Body()->Get_Component(TEXT("Com_Model")));

    if (pModel->Play_Animation(fTimeDelta))
    {
        pViper->Get_Controller()->Get_BlackBoard()->Set_Value<_bool>(pViper->Get_Name(), "isP1_5HitComboFinished", true);
    }

}

void CAS_5HitCombo_Viper::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
}

CAS_5HitCombo_Viper* CAS_5HitCombo_Viper::Create()
{
    return new CAS_5HitCombo_Viper();
}

void CAS_5HitCombo_Viper::Free()
{
    __super::Free();
}
