#include "AS_Move_Gomdol.h"
#include "AI_Controller.h"
#include "Gomdol.h"
#include "GameInstance.h"
#include "BlackBoard.h"
#include "Body_Gomdol.h"

CAS_Move_Gomdol::CAS_Move_Gomdol()
{
}

void CAS_Move_Gomdol::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
	CGomdol* pGomdol = static_cast<CGomdol*>(pOwner);
	CTransform* pTransform = static_cast<CTransform*>(pOwner->Get_Component(TEXT("Com_Transform")));
	CModel* pModel = static_cast<CModel*>(pGomdol->Get_Body()->Get_Component(TEXT("Com_Model")));
    CBlackBoard* pBB = pGomdol->Get_Controller()->Get_BlackBoard();

	CMonster::MONSTER_INFO Info{};
    Info.iStateFlag = pBB->Get_Value<_uint>(pGomdol->Get_Name(), "iMovementFlag");
    m_iPrevMovementFlag = Info.iStateFlag;

    if (Info.iStateFlag == Info.WALK)
    {
       /* m_fSpeedPerSec = pTransform->Get_SpeedPerSec();
        pModel->Set_Animation(1);
        m_AnimationSpeed = 1.f;*/
        m_fSpeedPerSec = pBB->Get_Value<_float>(pGomdol->Get_Name(), "RunSpeed");
        pModel->Set_Animation(2);
        m_AnimationSpeed = 1.25f;


    }
    else if (Info.iStateFlag == Info.RUN)
    {
        m_fSpeedPerSec = pBB->Get_Value<_float>(pGomdol->Get_Name(), "RunSpeed");
        pModel->Set_Animation(2);
        m_AnimationSpeed = 1.25f;
    }



}

void CAS_Move_Gomdol::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    CGomdol* pGomdol = static_cast<CGomdol*>(pOwner);
    CBlackBoard* pBB = pGomdol->Get_Controller()->Get_BlackBoard();
    CTransform* pTransform = static_cast<CTransform*>(pOwner->Get_Component(TEXT("Com_Transform")));
    CModel* pModel = static_cast<CModel*>(pGomdol->Get_Body()->Get_Component(TEXT("Com_Model")));

    _float fAttackRange = pBB->Get_Value<_float>(pGomdol->Get_Name(), "AttackRange");
    
    pGomdol->Get_Controller()->
        AI_MoveTo(pOwner,
            pBB->Get_Value<CGameObject*>(pGomdol->Get_Name(), "Target"),
            fAttackRange - 0.5f,
            m_fSpeedPerSec,
            fTimeDelta);

    if (pModel->Play_Animation(fTimeDelta * m_AnimationSpeed)) {}


}

void CAS_Move_Gomdol::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{

}

void CAS_Move_Gomdol::Start_Decel()
{

}

CAS_Move_Gomdol* CAS_Move_Gomdol::Create()
{
	return new CAS_Move_Gomdol();
}

void CAS_Move_Gomdol::Free()
{
	__super::Free();
}
