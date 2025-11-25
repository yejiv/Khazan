#include "AS_Hit_Viper.h"
#include "AI_Controller.h"
#include "Viper.h"
#include "GameInstance.h"
#include "BlackBoard.h"
#include "FSM_Viper.h"
#include "Body_Viper.h"


CAS_Hit_Viper::CAS_Hit_Viper()
{

}

void CAS_Hit_Viper::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    CViper* pViper = static_cast<CViper*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pViper->Get_Body()->Get_Component(TEXT("Com_Model")));
    CBlackBoard* pBB = pViper->Get_Controller()->Get_BlackBoard();

    HITREACTION eHitreaction = static_cast<HITREACTION>(pBB->Get_Value<_uint>(pViper->Get_Name(), "DamageType"));
    DIRECTION_INFO Info{};
    Info.iDirFlag = pBB->Get_Value<_uint>(pViper->Get_Name(), "HitDirection");

    HIT_DIR eHitDir = Convert_HitFlag(Info);
    _uint iAnimIndex = Make_AnimIndex(eHitreaction, eHitDir);

    pModel->Set_Animation(iAnimIndex);
    pModel->Set_AnimationLoop(false);

}

void CAS_Hit_Viper::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    CViper* pViper = static_cast<CViper*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pViper->Get_Body()->Get_Component(TEXT("Com_Model")));

    if (pModel->Play_Animation(fTimeDelta))
    {
        CBlackBoard* pBB = pViper->Get_Controller()->Get_BlackBoard();
        pBB->Set_Value<_bool>(pViper->Get_Name(), "isHitFinished", true);
        pBB->Set_Value<_bool>(pViper->Get_Name(), "IsHitEvent", false);
        pFSM->Change_State(ENUM_CLASS(VIPER_STATE_P1::IDLE), pOwner);
    }

}

void CAS_Hit_Viper::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{

}

void CAS_Hit_Viper::OnCollision(COLLISION_DESC* pDesc, _uint iCollisionLayer, CGameObject* pOwner)
{


}

HIT_DIR CAS_Hit_Viper::Convert_HitFlag(DIRECTION_INFO Info)
{
    if (Info.Check_Flag(DIRECTION_INFO::F)) return HIT_DIR::FRONT;
    if (Info.Check_Flag(DIRECTION_INFO::B)) return HIT_DIR::BACK;
    if (Info.Check_Flag(DIRECTION_INFO::L)) return HIT_DIR::LEFT;
    if (Info.Check_Flag(DIRECTION_INFO::R)) return HIT_DIR::RIGTH;

    return HIT_DIR::FRONT;
}

_uint CAS_Hit_Viper::Make_AnimIndex(HITREACTION eHitreaction, HIT_DIR eHitDir)
{
    // static 으로 두는 이유는  매번 Enter로 들어오면 변수를 만들어야해서 딱 처음 1번만 초기화 하도록하기 위해서
    static _uint table[ENUM_CLASS(HITREACTION::END)][ENUM_CLASS(HIT_DIR::END)] =
    {
        // WEAK
        {81,84,82,83},
        // NORMAL
        {40,37,38,39},
        // STRONG
        {76,43,44,45},
        // PARRY
        {54,54,54,54},
    };

    return table[ENUM_CLASS(eHitreaction)][ENUM_CLASS(eHitDir)];
}

CAS_Hit_Viper* CAS_Hit_Viper::Create()
{
    return new CAS_Hit_Viper();
}

void CAS_Hit_Viper::Free()
{
    __super::Free();
}
