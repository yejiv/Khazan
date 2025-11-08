#include "AS_Idle_Gomdol.h"
#include "AI_Controller.h"
#include "Gomdol.h"
#include "GameInstance.h"
#include "BlackBoard.h"
#include "Body_Gomdol.h"

CAS_Idle_Gomdol::CAS_Idle_Gomdol()
{
}

void CAS_Idle_Gomdol::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    CGomdol* pGomdol = static_cast<CGomdol*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pGomdol->Get_Body()->Get_Component(TEXT("Com_Model")));

    pModel->Set_Animation(0);

}

void CAS_Idle_Gomdol::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    CGomdol* pGomdol = static_cast<CGomdol*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pGomdol->Get_Body()->Get_Component(TEXT("Com_Model")));


    pModel->Play_Animation(fTimeDelta);
}

void CAS_Idle_Gomdol::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
}

CAS_Idle_Gomdol* CAS_Idle_Gomdol::Create()
{
	return new CAS_Idle_Gomdol();
}

void CAS_Idle_Gomdol::Free()
{
    __super::Free();
}
