#include "AS_CutScene_Yetuga.h"
#include "Yetuga.h"
#include "AI_Controller.h"
#include "FSM_Yetuga.h"
#include "GameInstance.h"
#include "Body_Yetuga.h"

CAS_CutScene_Yetuga::CAS_CutScene_Yetuga()
{
}

void CAS_CutScene_Yetuga::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    CYetuga* pYetuga = static_cast<CYetuga*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pYetuga->Get_Body()->Get_Component(TEXT("Com_Model")));

    pModel->Set_AnimationSet("CutScene");

    CTransform* pTransform = static_cast<CTransform*>(pOwner->Get_Component(TEXT("Com_Transform")));
    pTransform->Set_State(STATE::POSITION, XMVectorSet(537.354f,18.684f, 221.961f, 1.f));

}

void CAS_CutScene_Yetuga::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    CYetuga* pYetuga = static_cast<CYetuga*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pYetuga->Get_Body()->Get_Component(TEXT("Com_Model")));

    //CTransform* pTransform = static_cast<CTransform*>(pOwner->Get_Component(TEXT("Com_Transform")));


    if (pModel->Play_Animation(fTimeDelta * 0.76))
    {
        pFSM->Change_State(ENUM_CLASS(YETUGA_STATE::IDLE), pOwner);
    }
    
}

void CAS_CutScene_Yetuga::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
    
}

CAS_CutScene_Yetuga* CAS_CutScene_Yetuga::Create()
{
    return new CAS_CutScene_Yetuga();
}

void CAS_CutScene_Yetuga::Free()
{
    __super::Free();
}
