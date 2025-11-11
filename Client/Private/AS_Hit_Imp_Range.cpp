#include "AS_Hit_Imp_Range.h"
#include "Imp_Range.h"
#include "GameInstance.h"
#include "Body_Imp_Range.h"
#include "BlackBoard.h"

CAS_Hit_Imp_Range::CAS_Hit_Imp_Range()
{

}

void CAS_Hit_Imp_Range::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    CImp_Range* pImp = static_cast<CImp_Range*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pImp->Get_Body()->Get_Component(TEXT("Com_Model")));
    
    HITREACTION eHitreaction = static_cast<HITREACTION>(m_pGameInstance->Get_BlackBoard()->Get_Value<_uint>(pImp->Get_Name(), "DamageType"));
    DIRECTION_INFO Info{};
    Info.iDirFlag = m_pGameInstance->Get_BlackBoard()->Get_Value<_uint>(pImp->Get_Name(), "HitDirection");

    HIT_DIR eHitDir = Convert_HitFlag(Info);
    _uint iAnimIndex = Make_AnimIndex(eHitreaction, eHitDir);

    pModel->Set_Animation(iAnimIndex);
    pModel->Set_AnimationLoop(false);


}

void CAS_Hit_Imp_Range::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    CImp_Range* pImp = static_cast<CImp_Range*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pImp->Get_Body()->Get_Component(TEXT("Com_Model")));

    if (pModel->Play_Animation(fTimeDelta))
    {
        m_pGameInstance->Get_BlackBoard()->Set_Value<_bool>(pImp->Get_Name(), "isHitFinished", true);
    }

}

void CAS_Hit_Imp_Range::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{

}


HIT_DIR CAS_Hit_Imp_Range::Convert_HitFlag(DIRECTION_INFO Info)
{
    if (Info.Check_Flag(DIRECTION_INFO::F)) return HIT_DIR::FRONT;
    if (Info.Check_Flag(DIRECTION_INFO::B)) return HIT_DIR::BACK;
    if (Info.Check_Flag(DIRECTION_INFO::L)) return HIT_DIR::LEFT;
    if (Info.Check_Flag(DIRECTION_INFO::R)) return HIT_DIR::RIGTH;

    return HIT_DIR::FRONT;
}

_uint CAS_Hit_Imp_Range::Make_AnimIndex(HITREACTION eHitreaction, HIT_DIR eHitDir)
{
    // static 으로 두는 이유는  매번 Enter로 들어오면 변수를 만들어야해서 딱 처음 1번만 초기화 하도록하기 위해서
    static _uint table[ENUM_CLASS(HITREACTION::END)][ENUM_CLASS(HIT_DIR::END)] =
    {
        // WEAK
        {16,17,18,19},
        // NORMAL
        {16,17,18,19},
        // STRONG
        {21,22,23,24},
        // PARRY
        {21,21,21,21},
    };

    return table[ENUM_CLASS(eHitreaction)][ENUM_CLASS(eHitDir)];
}



CAS_Hit_Imp_Range* CAS_Hit_Imp_Range::Create()
{
    return new CAS_Hit_Imp_Range();
}

void CAS_Hit_Imp_Range::Free()
{
    __super::Free();
}
