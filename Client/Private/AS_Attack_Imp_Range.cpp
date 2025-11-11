#include "AS_Attack_Imp_Range.h"
#include "Imp_Range.h"
#include "GameInstance.h"
#include "Body_Imp_Range.h"
#include "BlackBoard.h"

CAS_Attack_Imp_Range::CAS_Attack_Imp_Range()
{

}

void CAS_Attack_Imp_Range::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    CImp_Range* pImp = static_cast<CImp_Range*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pImp->Get_Body()->Get_Component(TEXT("Com_Model")));
    pModel->Set_Animation(14);
}

void CAS_Attack_Imp_Range::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    CImp_Range* pImp = static_cast<CImp_Range*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pImp->Get_Body()->Get_Component(TEXT("Com_Model")));

    if (pModel->Play_Animation(fTimeDelta))
    {
        m_pGameInstance->Get_BlackBoard()->Set_Value<_bool>(pImp->Get_Name(), "isMagicFinished", true);
    }
}

void CAS_Attack_Imp_Range::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{

}

CAS_Attack_Imp_Range* CAS_Attack_Imp_Range::Create()
{
    return new CAS_Attack_Imp_Range();
}

void CAS_Attack_Imp_Range::Free()
{
    __super::Free();
}
