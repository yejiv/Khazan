#include "AS_RightHand_5Hit_Yetuga.h"
#include "AI_Controller.h"
#include "Yetuga.h"
#include "GameInstance.h"
#include "BlackBoard.h"
#include "FSM_Yetuga.h"
#include "Body_Yetuga.h"

CAS_RightHand_5Hit_Yetuga::CAS_RightHand_5Hit_Yetuga()
{
}

void CAS_RightHand_5Hit_Yetuga::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    CYetuga* pYetuga = static_cast<CYetuga*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pYetuga->Get_Body()->Get_Component(TEXT("Com_Model")));

    pModel->Set_Animation(61);
    pModel->Set_AnimationLoop(false);
}

void CAS_RightHand_5Hit_Yetuga::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    CYetuga* pYetuga = static_cast<CYetuga*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pYetuga->Get_Body()->Get_Component(TEXT("Com_Model")));

    if (pModel->Play_Animation(fTimeDelta))
    {
        m_pGameInstance->Get_BlackBoard()->Set_Value<_bool>(pYetuga->Get_Name(), "isAttack2Finished", true);
    }
}

void CAS_RightHand_5Hit_Yetuga::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
    m_iComboCount = 0;

}

void CAS_RightHand_5Hit_Yetuga::OnCollision(COLLISION_DESC* pDesc, _uint iCollisionLayer, CGameObject* pOwner)
{
    //if (m_iComboCount < 5)
    //    m_iComboCount++;

    //if (4 == m_iComboCount)
    //{
    //    // if 막타를 패리하면 스트롱 대미지 피격 인터룹트 호출 하는식으로

    //    CYetuga* pYetuga = static_cast<CYetuga*>(pOwner);
    //    pYetuga->Take_Damage(15.f,HITREACTION::KNOCKBACK_STRONG,1.f);
    //}
}


CAS_RightHand_5Hit_Yetuga* CAS_RightHand_5Hit_Yetuga::Create()
{
    return new CAS_RightHand_5Hit_Yetuga();
}

void CAS_RightHand_5Hit_Yetuga::Free()
{
    __super::Free();
}
