#include "AS_Turn_Yetuga.h"
#include "AI_Controller.h"
#include "Yetuga.h"
#include "GameInstance.h"
#include "BlackBoard.h"
#include "FSM_Yetuga.h"
#include "Body_Yetuga.h"

CAS_Turn_Yetuga::CAS_Turn_Yetuga()
{
  
}

void CAS_Turn_Yetuga::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    CYetuga* pYetuga = static_cast<CYetuga*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pYetuga->Get_Body()->Get_Component(TEXT("Com_Model")));
    CBlackBoard* pBB = pYetuga->Get_Controller()->Get_BlackBoard();

    DIRECTION_INFO Info{};
    Info.iDirFlag = pBB->Get_Value<_uint>("Yetuga", "TargetDirection");

    if (Info.Check_Flag(DIRECTION_INFO::DIR::L))
    {
        if (!Info.Check_Flag(DIRECTION_INFO::DIR::B))
        {
            // 왼쪽 90도 회전
            pModel->Set_Animation(8);
        }
            
        else
        {
            // 왼쪽 뒤 (180도 회전)
            pModel->Set_Animation(10);
        }
           
    }
    else if (Info.Check_Flag(DIRECTION_INFO::DIR::R))
    {
        if (!Info.Check_Flag(DIRECTION_INFO::DIR::B))
        {
            // 오른쪽 90도 회전
            pModel->Set_Animation(9);
        }
            
        else
        {
            // 오른쪽 뒤 (180도 회전)
            pModel->Set_Animation(11);
        }
    }
}

void CAS_Turn_Yetuga::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    CYetuga* pYetuga = static_cast<CYetuga*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pYetuga->Get_Body()->Get_Component(TEXT("Com_Model")));
    CBlackBoard* pBB = pYetuga->Get_Controller()->Get_BlackBoard();

    CTransform* pOwnerTransform = static_cast<CTransform*>(pOwner->Get_Component(TEXT("Com_Transform")));
    _vector vLook = XMVector3Normalize(pOwnerTransform->Get_State(STATE::LOOK));
    _float3 vTempDir = pBB->Get_Value<_float3>("Yetuga", "TargetDir");
    _vector vTargetDir = XMVector3Normalize(XMLoadFloat3(&vTempDir));
    
    vLook = XMVectorSetY(vLook, 0.f);
    vTargetDir = XMVectorSetY(vTargetDir, 0.f);
    vLook = XMVector3Normalize(vLook);
    vTargetDir = XMVector3Normalize(vTargetDir);

    _float fDot = XMVectorGetX(XMVector3Dot(vLook, vTargetDir));
    _float fAngle = acosf(clamp(fDot, -1.f, 1.f));
    
    _float fAnimRatio = pModel->MakeRatio();
    
    _float fTurnSpeed = 0.8f * (fAngle / XM_PI);
    _vector vLerpDir = XMVector3Normalize(XMVectorLerp(vLook, vTargetDir, fAnimRatio * fTurnSpeed));

    _vector vPosition = pOwnerTransform->Get_State(STATE::POSITION);
    _vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
    _vector vRight = XMVector3Normalize(XMVector3Cross(vUp, vLerpDir));
    vUp = XMVector3Normalize(XMVector3Cross(vLerpDir, vRight));

    pOwnerTransform->Set_State(STATE::LOOK, vLerpDir);
    pOwnerTransform->Set_State(STATE::RIGHT, vRight);
    pOwnerTransform->Set_State(STATE::UP, vUp);

    if (fAngle < XMConvertToRadians(5.f) || pModel->Play_Animation(fTimeDelta))
    {
        // 블랙보드 갱신
        pBB->Set_Value<_bool>("Yetuga", "isTurnFinished", true);
        pFSM->Change_State(ENUM_CLASS(YETUGA_STATE::IDLE), pOwner);
    }

}

void CAS_Turn_Yetuga::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
}

CAS_Turn_Yetuga* CAS_Turn_Yetuga::Create()
{
	return new CAS_Turn_Yetuga();
}

void CAS_Turn_Yetuga::Free()
{
    __super::Free();
}
