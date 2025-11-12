#include "AS_LockOn_Yetuga.h"
#include "AI_Controller.h"
#include "Yetuga.h"
#include "GameInstance.h"
#include "BlackBoard.h"
#include "FSM_Yetuga.h"
#include "Body_Yetuga.h"
#include "ClientInstance.h"


CAS_LockOn_Yetuga::CAS_LockOn_Yetuga()
{

}

void CAS_LockOn_Yetuga::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{

    m_fDotThreshold = cosf(XMConvertToRadians(10.f));
    m_fMoveSpeed = 0.5f;

    CYetuga* pYetuga = static_cast<CYetuga*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pYetuga->Get_Body()->Get_Component(TEXT("Com_Model")));
    CBlackBoard* pBB = pYetuga->Get_Controller()->Get_BlackBoard();
    m_eDirInfo.iDirFlag = pBB->Get_Value<_uint>(pYetuga->Get_Name(), "TargetDirection");

    if (m_eDirInfo.Check_Flag(m_eDirInfo.L))
        pModel->Set_Animation(4);

    else if (m_eDirInfo.Check_Flag(m_eDirInfo.R))
        pModel->Set_Animation(5);


}

void CAS_LockOn_Yetuga::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    CYetuga* pYetuga = static_cast<CYetuga*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pYetuga->Get_Body()->Get_Component(TEXT("Com_Model")));
    CTransform* pOwnerTransform = static_cast<CTransform*>(pOwner->Get_Component(TEXT("Com_Transform")));
    CBlackBoard* pBB = pYetuga->Get_Controller()->Get_BlackBoard();
    _float fDot = pBB->Get_Value<_float>(pYetuga->Get_Name(), "fDot");

    // 여기서 탈출조건
    if (fDot > m_fDotThreshold)
    {
        pBB->Set_Value<_bool>("Yetuga", "isLockOnFinished", true);
    }

    pYetuga->Get_Controller()->AI_MoveTo(pYetuga,
        pBB->Get_Value<CGameObject*>(pYetuga->Get_Name(),"Target"),
            m_fMoveSpeed,1.f,fTimeDelta);


    pModel->Play_Animation(fTimeDelta);
}

void CAS_LockOn_Yetuga::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{

}

CAS_LockOn_Yetuga* CAS_LockOn_Yetuga::Create()
{
    return new CAS_LockOn_Yetuga();
}

void CAS_LockOn_Yetuga::Free()
{

    __super::Free();
}
