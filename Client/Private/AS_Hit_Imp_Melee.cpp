#include "AS_Hit_Imp_Melee.h"
#include "Imp_Melee.h"
#include "GameInstance.h"
#include "Body_Imp_Melee.h"
#include "BlackBoard.h"
#include "AI_Controller.h"
CAS_Hit_Imp_Melee::CAS_Hit_Imp_Melee()
{

}

void CAS_Hit_Imp_Melee::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    CImp_Melee* pImp = static_cast<CImp_Melee*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pImp->Get_Body()->Get_Component(TEXT("Com_Model")));
    CBlackBoard* pBB = pImp->Get_Controller()->Get_BlackBoard();
    HITREACTION eHitreaction = static_cast<HITREACTION>(pBB->Get_Value<_uint>(pImp->Get_Name(), "DamageType"));

    HIT_DIR eHitDir = Convert_HitFlag(pImp->Get_HitDirInfo());
    _uint iAnimIndex = Make_AnimIndex(eHitreaction, eHitDir);

    pModel->Set_Animation(iAnimIndex);
    pModel->Set_AnimationLoop(false);


}

void CAS_Hit_Imp_Melee::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    CImp_Melee* pImp = static_cast<CImp_Melee*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pImp->Get_Body()->Get_Component(TEXT("Com_Model")));

    if (pModel->Play_Animation(fTimeDelta))
    {
        CBlackBoard* pBB = pImp->Get_Controller()->Get_BlackBoard();
        pBB->Set_Value<_bool>(pImp->Get_Name(), "isHitFinished", true);
    }

}

void CAS_Hit_Imp_Melee::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{

}


HIT_DIR CAS_Hit_Imp_Melee::Convert_HitFlag(DIRECTION_INFO Info)
{
    if (Info.Check_Flag(DIRECTION_INFO::F)) return HIT_DIR::FRONT;
    if (Info.Check_Flag(DIRECTION_INFO::B)) return HIT_DIR::BACK;
    if (Info.Check_Flag(DIRECTION_INFO::L)) return HIT_DIR::LEFT;
    if (Info.Check_Flag(DIRECTION_INFO::R)) return HIT_DIR::RIGTH;

    return HIT_DIR::FRONT;
}

_uint CAS_Hit_Imp_Melee::Make_AnimIndex(HITREACTION eHitreaction, HIT_DIR eHitDir)
{
    // static 으로 두는 이유는  매번 Enter로 들어오면 변수를 만들어야해서 딱 처음 1번만 초기화 하도록하기 위해서
    static _uint table[ENUM_CLASS(HITREACTION::END)][ENUM_CLASS(HIT_DIR::END)] =
    {
        {0,0,0,0,},
        {0,0,0,0,},
        // WEAK
        {5,4,2,3},
        // NORMAL
        {5,4,2,3},
        // STRONG
        {5,4,2,3},
        // PARRY
        {6,6,6,6},
        {0,0,0,0,},
        {0,0,0,0,},
        {5,4,2,3},

    };

    return table[ENUM_CLASS(eHitreaction)][ENUM_CLASS(eHitDir)];
}



CAS_Hit_Imp_Melee* CAS_Hit_Imp_Melee::Create()
{
    return new CAS_Hit_Imp_Melee();
}

void CAS_Hit_Imp_Melee::Free()
{
    __super::Free();
}
