#include "AS_Dead_Yetuga.h"
#include "AI_Controller.h"
#include "Yetuga.h"
#include "GameInstance.h"
#include "BlackBoard.h"
#include "FSM_Yetuga.h"
#include "Body_Yetuga.h"
#include "ClientInstance.h"
#include "BossHp.h"
#include "UI_Inven.h"
#include "Amount.h"
CAS_Dead_Yetuga::CAS_Dead_Yetuga()
{
}

void CAS_Dead_Yetuga::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    CClientInstance::GetInstance()->Add_SkillExp(200.f);

    CYetuga* pYetuga = static_cast<CYetuga*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pYetuga->Get_Body()->Get_Component(TEXT("Com_Model")));
    m_pGameInstance->Emit_Event< EVENT_ANNOUNCE_RESULT>(ENUM_CLASS(EVENT_TYPE::ANNOUNCE_RESULT), {});
    pModel->Set_Animation(94);
    static_cast<CUI_Inven*>(CClientInstance::GetInstance()->Get_RootUI(TEXT("Inven")))->Add_Item(1001);
    static_cast<CUI_Inven*>(CClientInstance::GetInstance()->Get_RootUI(TEXT("Inven")))->Add_Item(1002);
    static_cast<CUI_Inven*>(CClientInstance::GetInstance()->Get_RootUI(TEXT("Inven")))->Add_Item(1003);
    static_cast<CAmount*>(CClientInstance::GetInstance()->Get_RootUI(TEXT("Amount")))->Add_Value(CAmount::AMOUNT_TYPE::GOLD, 3000);
    m_pGameInstance->Emit_Event<EventPopBN>(ENUM_CLASS(EVENT_TYPE::BLADENEXUS_POP), { true });
    m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_yetuga_roar_die_01 (SFX).wav"), pYetuga->Get_Position(), pYetuga->Get_SoundChannel(ENUM_CLASS(MONSFX::ATVO)), 50.f);

    CClientInstance::GetInstance()->BGM_HeinMach_Day(4.f);
}

void CAS_Dead_Yetuga::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    CYetuga* pYetuga = static_cast<CYetuga*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pYetuga->Get_Body()->Get_Component(TEXT("Com_Model")));

    if (pModel->Play_Animation(fTimeDelta))
    {
        CBlackBoard* pBB = pYetuga->Get_Controller()->Get_BlackBoard();
        pBB->Set_Value<_bool>(pYetuga->Get_Name(), "isDeadFinished", true);
        CBossHp::BOSSMON_UPDATE_DESC Desc;
        Desc.isOpen = false;
        CClientInstance::GetInstance()->UI_UpdateSwitch(TEXT("BossHp"), &Desc);
        pYetuga->Creature_Release();
    }

}

void CAS_Dead_Yetuga::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{

}

CAS_Dead_Yetuga* CAS_Dead_Yetuga::Create()
{
    return new CAS_Dead_Yetuga();
}

void CAS_Dead_Yetuga::Free()
{
    __super::Free();
}
