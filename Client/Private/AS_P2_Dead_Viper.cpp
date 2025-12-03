#include "AS_P2_Dead_Viper.h"
#include "Viper.h"
#include "Body_Phase2_Viper.h"
#include "BlackBoard.h"
#include "AI_Controller.h"
#include "GameInstance.h"
#include "FSM_Viper.h"
#include "ClientInstance.h"
#include "BossHp.h"
#include "UI_Inven.h"
#include "Amount.h"

CAS_P2_Dead_Viper::CAS_P2_Dead_Viper()
{

}

void CAS_P2_Dead_Viper::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{

    CViper* pViper = static_cast<CViper*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pViper->Get_P2Body()->Get_Component(TEXT("Com_Model")));
    CBlackBoard* pBB = pViper->Get_Controller()->Get_BlackBoard();
    m_pGameInstance->Emit_Event< EVENT_ANNOUNCE_RESULT>(ENUM_CLASS(EVENT_TYPE::ANNOUNCE_RESULT), {});
    pModel->Set_Animation(25);
    static_cast<CUI_Inven*>(CClientInstance::GetInstance()->Get_RootUI(TEXT("Inven")))->Add_Item(1001);
    static_cast<CUI_Inven*>(CClientInstance::GetInstance()->Get_RootUI(TEXT("Inven")))->Add_Item(1002);
    static_cast<CUI_Inven*>(CClientInstance::GetInstance()->Get_RootUI(TEXT("Inven")))->Add_Item(1003);
    static_cast<CAmount*>(CClientInstance::GetInstance()->Get_RootUI(TEXT("Amount")))->Add_Value(CAmount::AMOUNT_TYPE::GOLD, 5000);


}

void CAS_P2_Dead_Viper::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    CViper* pViper = static_cast<CViper*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pViper->Get_P2Body()->Get_Component(TEXT("Com_Model")));


    if (pModel->Play_Animation(fTimeDelta))
    {
        CBlackBoard* pBB = pViper->Get_Controller()->Get_BlackBoard();
        pBB->Set_Value<_bool>(pViper->Get_Name(), "isDeadFinished",true);
        CBossHp::BOSSMON_UPDATE_DESC Desc;
       
        Desc.isOpen = false;
        CClientInstance::GetInstance()->UI_UpdateSwitch(TEXT("BossHp"), &Desc);
        pViper->Creature_Release();
    }
}

void CAS_P2_Dead_Viper::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{

}

CAS_P2_Dead_Viper* CAS_P2_Dead_Viper::Create()
{
    return new CAS_P2_Dead_Viper();
}

void CAS_P2_Dead_Viper::Free()
{
    __super::Free();
}
