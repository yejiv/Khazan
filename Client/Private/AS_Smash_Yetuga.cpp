#include "AS_Smash_Yetuga.h"
#include "AI_Controller.h"
#include "Yetuga.h"
#include "GameInstance.h"
#include "BlackBoard.h"
#include "FSM_Yetuga.h"
#include "Body_Yetuga.h"

CAS_Smash_Yetuga::CAS_Smash_Yetuga()
{
}

void CAS_Smash_Yetuga::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    CYetuga* pYetuga = static_cast<CYetuga*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pYetuga->Get_Body()->Get_Component(TEXT("Com_Model")));

    _uint iRand = static_cast<_uint>(m_pGameInstance->Rand(0.f, 4.f));

    _uint animID = 0;

    DIRECTION_INFO Info{};
    Info.iDirFlag = m_pGameInstance->Get_BlackBoard()->Get_Value<_uint>(pYetuga->Get_Name(), "TargetDirection");

    if (Info.Check_Flag(Info.F))
    {
        switch (iRand)
        {
        case 0: 
            animID = 33; 
            break;
        case 1: 
            animID = 32; 
            break;
        case 2:
            animID = 16;
            break;
        case 3:
            animID = 18;
            break;
        }
    }


    pModel->Set_Animation(animID);
}

void CAS_Smash_Yetuga::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    CYetuga* pYetuga = static_cast<CYetuga*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pYetuga->Get_Body()->Get_Component(TEXT("Com_Model")));

    CTransform* pTransform = static_cast<CTransform*>(pOwner->Get_Component(TEXT("Com_Transform")));
    CTransform* pTargetTransform = static_cast<CTransform*>(m_pGameInstance->Get_BlackBoard()->
        Get_Value<CGameObject*>(pYetuga->Get_Name(), "Target")->Get_Component(TEXT("Com_Transform")));
    _vector vTargetLoc = pTargetTransform->Get_State(STATE::POSITION);

    pTransform->LookAt(vTargetLoc);

    if (pModel->Play_Animation(fTimeDelta))
    {
        m_pGameInstance->Get_BlackBoard()->Set_Value<_bool>(pYetuga->Get_Name(),"isNormalSmashFinished",true);
    }


}

void CAS_Smash_Yetuga::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
}

CAS_Smash_Yetuga* CAS_Smash_Yetuga::Create()
{
    return new CAS_Smash_Yetuga();
}

void CAS_Smash_Yetuga::Free()
{
    __super::Free();
}
