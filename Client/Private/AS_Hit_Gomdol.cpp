#include "AS_Hit_Gomdol.h"
#include "Gomdol.h"
#include "GameInstance.h"
#include "Body_Gomdol.h"
#include "BlackBoard.h"
#include "AI_Controller.h"
CAS_Hit_Gomdol::CAS_Hit_Gomdol()
{

}

void CAS_Hit_Gomdol::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    CGomdol* pGomdol = static_cast<CGomdol*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pGomdol->Get_Body()->Get_Component(TEXT("Com_Model")));
    CBlackBoard* pBB = pGomdol->Get_Controller()->Get_BlackBoard();
    HITREACTION eHitreaction = static_cast<HITREACTION>(pBB->Get_Value<_uint>(pGomdol->Get_Name(), "DamageType"));

    HIT_DIR eHitDir = Convert_HitFlag(pGomdol->Get_HitDirInfo());
    _uint iAnimIndex = Make_AnimIndex(eHitreaction, eHitDir);

    pModel->Set_Animation(iAnimIndex);
    pModel->Set_AnimationLoop(false);

    if (iAnimIndex >= 9 && iAnimIndex <= 12)
        pGomdol->SFX_HIT(0);
    else if (iAnimIndex >= 1 && iAnimIndex <= 4)
        pGomdol->SFX_HIT(1);
    else if (iAnimIndex >= 5 && iAnimIndex <= 8)
        pGomdol->SFX_HIT(2);



}

void CAS_Hit_Gomdol::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    CGomdol* pGomdol = static_cast<CGomdol*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pGomdol->Get_Body()->Get_Component(TEXT("Com_Model")));

    if (pModel->Play_Animation(fTimeDelta))
    {
        CBlackBoard* pBB = pGomdol->Get_Controller()->Get_BlackBoard();
        pBB->Set_Value<_bool>(pGomdol->Get_Name(), "isHitFinished", true);
    }

}

void CAS_Hit_Gomdol::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{

}


HIT_DIR CAS_Hit_Gomdol::Convert_HitFlag(DIRECTION_INFO Info)
{
    if (Info.Check_Flag(DIRECTION_INFO::F)) return HIT_DIR::FRONT;
    if (Info.Check_Flag(DIRECTION_INFO::B)) return HIT_DIR::BACK;
    if (Info.Check_Flag(DIRECTION_INFO::L)) return HIT_DIR::LEFT;
    if (Info.Check_Flag(DIRECTION_INFO::R)) return HIT_DIR::RIGTH;

    return HIT_DIR::FRONT;
}

_uint CAS_Hit_Gomdol::Make_AnimIndex(HITREACTION eHitreaction, HIT_DIR eHitDir)
{
    // static 으로 두는 이유는  매번 Enter로 들어오면 변수를 만들어야해서 딱 처음 1번만 초기화 하도록하기 위해서
    static _uint table[ENUM_CLASS(HITREACTION::END)][ENUM_CLASS(HIT_DIR::END)] =
    {
        {0,0,0,0,},
        {0,0,0,0,},
        // WEAK
        {9,10,11,12},
        // NORMAL
        {1,4,2,3},
        // STRONG
        {5,6,7,8},
        // PARRY
        {6,6,6,6},
        {0,0,0,0,},
        {0,0,0,0,},
        {1,4,2,3},

    };

    return table[ENUM_CLASS(eHitreaction)][ENUM_CLASS(eHitDir)];
}



CAS_Hit_Gomdol* CAS_Hit_Gomdol::Create()
{
    return new CAS_Hit_Gomdol();
}

void CAS_Hit_Gomdol::Free()
{
    __super::Free();
}
