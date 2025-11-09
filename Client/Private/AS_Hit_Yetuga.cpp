#include "AS_Hit_Yetuga.h"
#include "AI_Controller.h"
#include "Yetuga.h"
#include "GameInstance.h"
#include "BlackBoard.h"
#include "FSM_Yetuga.h"
#include "Body_Yetuga.h"

CAS_Hit_Yetuga::CAS_Hit_Yetuga()
{

}

void CAS_Hit_Yetuga::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    CYetuga* pYetuga = static_cast<CYetuga*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pYetuga->Get_Body()->Get_Component(TEXT("Com_Model")));

    HITREACTION eHitreaction = static_cast<HITREACTION>(m_pGameInstance->Get_BlackBoard()->Get_Value<_uint>(pYetuga->Get_Name(), "DamageType"));
    DIRECTION_INFO Info{};
    Info.iDirFlag = m_pGameInstance->Get_BlackBoard()->Get_Value<_uint>("Yetuga", "TargetDirection");

    HIT_DIR eHitDir = Convert_HitFlag(Info);
    _uint iAnimIndex = Make_AnimIndex(eHitreaction,eHitDir);

    pModel->Set_Animation(iAnimIndex);
    pModel->Set_AnimationLoop(false);

}

void CAS_Hit_Yetuga::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    CYetuga* pYetuga = static_cast<CYetuga*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pYetuga->Get_Body()->Get_Component(TEXT("Com_Model")));

    if (pModel->Play_Animation(fTimeDelta))
    {
        m_pGameInstance->Get_BlackBoard()->Set_Value<_bool>(pYetuga->Get_Name(), "isHitFinished", true);
    }

}

void CAS_Hit_Yetuga::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{

}

HIT_DIR CAS_Hit_Yetuga::Convert_HitFlag(DIRECTION_INFO Info)
{
    if (Info.Check_Flag(DIRECTION_INFO::F)) return HIT_DIR::FRONT;
    if (Info.Check_Flag(DIRECTION_INFO::B)) return HIT_DIR::BACK;
    if (Info.Check_Flag(DIRECTION_INFO::L)) return HIT_DIR::LEFT;
    if (Info.Check_Flag(DIRECTION_INFO::R)) return HIT_DIR::RIGTH;

    return HIT_DIR::FRONT;
}

_uint CAS_Hit_Yetuga::Make_AnimIndex(HITREACTION eHitreaction, HIT_DIR eHitDir)
{
    // static 으로 두는 이유는  매번 Enter로 들어오면 변수를 만들어야해서 딱 처음 1번만 초기화 하도록하기 위해서
    static _uint table[ENUM_CLASS(HITREACTION::END)][ENUM_CLASS(HIT_DIR::END)] =
    {
        // WEAK
        {74,75,76,77},
        // NORMAL
        {63,62,66,67},
        // STRONG
        {79,80,81,82},
        // PARRY
        {73,73,73,73},
    };

    return table[ENUM_CLASS(eHitreaction)][ENUM_CLASS(eHitDir)];
}

CAS_Hit_Yetuga* CAS_Hit_Yetuga::Create()
{
    return new CAS_Hit_Yetuga();
}

void CAS_Hit_Yetuga::Free()
{
	__super::Free();
}
