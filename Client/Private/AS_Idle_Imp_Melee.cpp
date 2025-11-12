#include "AS_Idle_Imp_Melee.h"
#include "Imp_Melee.h"
#include "GameInstance.h"
#include "Body_Imp_Melee.h"

AS_Idle_Imp_Melee::AS_Idle_Imp_Melee()
{
}

void AS_Idle_Imp_Melee::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
	CImp_Melee* pImp = static_cast<CImp_Melee*>( pOwner );
	CModel* pModel = static_cast< CModel* >( pImp->Get_Body()->Get_Component(TEXT("Com_Model")) );
	pModel->Set_Animation(31);
}

void AS_Idle_Imp_Melee::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
	CImp_Melee* pImp = static_cast<CImp_Melee*>( pOwner );
	CModel* pModel = static_cast< CModel* >( pImp->Get_Body()->Get_Component(TEXT("Com_Model")));

	pModel->Play_Animation(fTimeDelta);

}

void AS_Idle_Imp_Melee::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{

}

AS_Idle_Imp_Melee* AS_Idle_Imp_Melee::Create()
{
    return new AS_Idle_Imp_Melee();
}

void AS_Idle_Imp_Melee::Free()
{
	__super::Free();
}
