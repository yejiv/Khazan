#include "AS_Slow2Hit_VIper.h"
#include "Viper.h"
#include "Body_Viper.h"
#include "BlackBoard.h"
#include "AI_Controller.h"


CAS_Slow2Hit_VIper::CAS_Slow2Hit_VIper()
{

}

void CAS_Slow2Hit_VIper::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    CViper* pViper = static_cast<CViper*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pViper->Get_Body()->Get_Component(TEXT("Com_Model")));

    pModel->Set_Animation(112);
}

void CAS_Slow2Hit_VIper::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    CViper* pViper = static_cast<CViper*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pViper->Get_Body()->Get_Component(TEXT("Com_Model")));

    if (pModel->Play_Animation(fTimeDelta))
    {
        pViper->Get_Controller()->Get_BlackBoard()->Set_Value<_bool>(pViper->Get_Name(), "isP1_Slow2HitFinished", true);
    }

}

void CAS_Slow2Hit_VIper::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
}

CAS_Slow2Hit_VIper* CAS_Slow2Hit_VIper::Create()
{
    return new CAS_Slow2Hit_VIper();
}

void CAS_Slow2Hit_VIper::Free()
{
    __super::Free();
}
