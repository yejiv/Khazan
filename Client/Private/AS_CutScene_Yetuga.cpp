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

    CTransform* pTransform = static_cast<CTransform*>(pOwner->Get_Component(TEXT("Com_Transform")));
    pTransform->Set_State(STATE::POSITION, XMVectorSet(537.354f,18.684f, 221.961f, 1.f));

}

void CAS_CutScene_Yetuga::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    CYetuga* pYetuga = static_cast<CYetuga*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pYetuga->Get_Body()->Get_Component(TEXT("Com_Model")));
    CBlackBoard* pBB = pYetuga->Get_Controller()->Get_BlackBoard();
    CGameObject* pTarget = pBB->Get_Value<CGameObject*>(pYetuga->Get_Name(), "Target");
    CTransform* pOwnerTransform = static_cast<CTransform*>(pYetuga->Get_Component(TEXT("Com_Transform")));
    CTransform* pTargetTransform = static_cast<CTransform*>(pTarget->Get_Component(TEXT("Com_Transform")));
    _vector vTargetPos = pTargetTransform->Get_State(STATE::POSITION);
    //pOwnerTransform->LookAt(vTargetPos);

    if (m_pGameInstance->Key_Down(DIK_0))
        Change_CutSceneState(CUTSCENE_STATE::JUMP, pModel, pYetuga);
    else if (m_pGameInstance->Key_Down(DIK_1))
        Change_CutSceneState(CUTSCENE_STATE::LAND, pModel, pYetuga);
    else if (m_pGameInstance->Key_Down(DIK_2))
        Change_CutSceneState(CUTSCENE_STATE::ROAR1, pModel, pYetuga);
    else if (m_pGameInstance->Key_Down(DIK_3))
        Change_CutSceneState(CUTSCENE_STATE::ROAR2, pModel, pYetuga);
    
    
    if (m_eState == CUTSCENE_STATE::RUN)
        pOwnerTransform->Go_Straight(fTimeDelta);

    if (pModel->Play_Animation(fTimeDelta))
    {
        if (CUTSCENE_STATE::ROAR2 == m_eState)
        {
            pModel->Set_Animation(6); // 뛰기
            m_eState = CUTSCENE_STATE::RUN;
        }


        /* 스프린트  
         pModel->Set_Animation(7);*/
        
    }

}

void CAS_CutScene_Yetuga::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{

}

void CAS_CutScene_Yetuga::YetugaScene_Jump(CYetuga* pYetuga)
{
    CModel* pModel = static_cast<CModel*>(pYetuga->Get_Body()->Get_Component(TEXT("Com_Model")));
    Change_CutSceneState(CUTSCENE_STATE::JUMP, pModel, pYetuga);

}

void CAS_CutScene_Yetuga::YetugaScene_Land(CYetuga* pYetuga)
{
    CModel* pModel = static_cast<CModel*>(pYetuga->Get_Body()->Get_Component(TEXT("Com_Model")));
    Change_CutSceneState(CUTSCENE_STATE::LAND, pModel, pYetuga);
}

void CAS_CutScene_Yetuga::YetugaScene_Roar1(CYetuga* pYetuga)
{
    CModel* pModel = static_cast<CModel*>(pYetuga->Get_Body()->Get_Component(TEXT("Com_Model")));
    Change_CutSceneState(CUTSCENE_STATE::ROAR1, pModel, pYetuga);
}

void CAS_CutScene_Yetuga::YetugaScene_Roar2(CYetuga* pYetuga)
{
    CModel* pModel = static_cast<CModel*>(pYetuga->Get_Body()->Get_Component(TEXT("Com_Model")));
    Change_CutSceneState(CUTSCENE_STATE::ROAR2, pModel, pYetuga);
}


void CAS_CutScene_Yetuga::Change_CutSceneState(CUTSCENE_STATE eNextState, CModel* pModel, CYetuga* pYetuga)
{
    if (m_eState == eNextState)
        return;

    m_eState = eNextState;

    switch (m_eState)
    {
    case Client::CUTSCENE_STATE::JUMP:
    {
        pModel->Set_Animation(ENUM_CLASS(CUTSCENE_STATE::JUMP));
        _vector vGoalPos = XMVectorSet(-0.98f, -0.08f, -0.17f, 1.f);
        pYetuga->Yetuga_Jump(vGoalPos, 30.f, 30.f);
    }
        break;
    case Client::CUTSCENE_STATE::LAND:
    {
        pModel->Set_Animation(ENUM_CLASS(CUTSCENE_STATE::LAND));
        _vector vGoalPos = XMVectorSet(520.f, -11.952f, 228.48f, 1.f);
        pYetuga->Yetuga_Land(vGoalPos, 20.f);
    }
        break;
    case Client::CUTSCENE_STATE::ROAR1:
        m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_yetuga_roar_f2_foley_01 (SFX).wav"), pYetuga->Get_Position(), pYetuga->Get_SoundChannel(ENUM_CLASS(MONSFX::SWISH)), 30.f);
        pModel->Set_Animation(ENUM_CLASS(CUTSCENE_STATE::ROAR1));
        break;
    case Client::CUTSCENE_STATE::ROAR2:
        m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_yetuga_roar_f_foley_01 (SFX).wav"), pYetuga->Get_Position(), pYetuga->Get_SoundChannel(ENUM_CLASS(MONSFX::SWISH)), 30.f);
        pModel->Set_Animation(ENUM_CLASS(CUTSCENE_STATE::ROAR2));
        break;

    }


}

CAS_CutScene_Yetuga* CAS_CutScene_Yetuga::Create()
{
    return new CAS_CutScene_Yetuga();
}

void CAS_CutScene_Yetuga::Free()
{
    __super::Free();
}
