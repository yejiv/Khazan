#include "AS_StingSlashCombo_Viper.h"
#include "Viper.h"
#include "Body_Viper.h"
#include "BlackBoard.h"
#include "AI_Controller.h"


CAS_StingSlashCombo_Viper::CAS_StingSlashCombo_Viper()
{

}

void CAS_StingSlashCombo_Viper::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    CViper* pViper = static_cast<CViper*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pViper->Get_Body()->Get_Component(TEXT("Com_Model")));

    pModel->Set_AnimationSet("StingAttackCombo");
}

void CAS_StingSlashCombo_Viper::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    CViper* pViper = static_cast<CViper*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pViper->Get_Body()->Get_Component(TEXT("Com_Model")));

    if (pModel->Play_Animation(fTimeDelta))
    {
        pViper->Get_Controller()->Get_BlackBoard()->Set_Value<_bool>(pViper->Get_Name(), "isP1_StingSlashFinished", true);
    }

}

void CAS_StingSlashCombo_Viper::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
}

CAS_StingSlashCombo_Viper* CAS_StingSlashCombo_Viper::Create()
{
    return new CAS_StingSlashCombo_Viper();
}

void CAS_StingSlashCombo_Viper::Free()
{
    __super::Free();
}
