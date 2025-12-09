#include "Sequence_Embars_Puzzle_First.h"
#include "GameInstance.h"
#include "ClientInstance.h"
#include "Transform.h"
#include "GameObject.h"
#include "SkipButton.h"
#include "UI_HUD.h"

CSequence_Embars_Puzzle_First::CSequence_Embars_Puzzle_First()
    : m_pGameInstance{ CGameInstance::GetInstance() }
    , m_pClientInstance{ CClientInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
    Safe_AddRef(m_pClientInstance);
}

HRESULT CSequence_Embars_Puzzle_First::Initialize(const SEQ_REQ_PLAY_DESC& tDesc)
{
    m_Id = tDesc.tId;
    m_fTime = tDesc.fStartTime;

    m_pClientInstance->Camera_Set_Animation_Json("../../Client/Bin/Data/Camera/Animation/VerticalGate1");
    m_pClientInstance->Camera_Set_Animation_Json("../../Client/Bin/Data/Camera/Animation/Turn_Elevator");
    static_cast<CUI_HUD*>(m_pClientInstance->Get_RootUI(TEXT("HUD")))->Switch_Panel(false);
    m_pClientInstance->Set_PlayerInput(false);
    return S_OK;
}

void CSequence_Embars_Puzzle_First::Update(_float fTimeDelta)
{
    m_fTime += fTimeDelta;

    Skip_KeyInput(fTimeDelta);

    if (!m_isSkip)
    {
        if (m_fTime >= 3.0f && !m_isElevatorFadeOut)
        {
            m_pClientInstance->Fade_Out();
            m_isElevatorFadeOut = true;
        }
        else if (m_fTime >= 4.f && !m_isElevatorCamera)
        {
            m_pClientInstance->Fade_In();
            m_pClientInstance->Camera_Set_Animation(TEXT("Turn_Elevator"));
            m_isElevatorCamera = true;
        }
        else if (m_fTime >= 9.f && !m_isVerticalGateFadeOut)
        {
            m_pClientInstance->Fade_Out();
            m_isVerticalGateFadeOut = true;
        }
        else if (m_fTime >= 10.f && !m_isVerticalGateCamera)
        {
            m_pClientInstance->Fade_In();
            m_pClientInstance->Camera_Set_Animation(TEXT("VerticalGate1"));
            m_isVerticalGateCamera = true;
        }
        else if (m_fTime >= 15.f && !m_isMovePlayerFadeOut)
        {
            m_pClientInstance->Fade_Out();
            m_isMovePlayerFadeOut = true;
        }
        else if (m_fTime > 16.f && !m_isMovePlayer)
        {
            m_pClientInstance->Camera_Set_FixEnd();
            m_isMovePlayer = true;
        }
        else if (m_fTime >= 17.f)
        {
            m_pClientInstance->Fade_In();            
            m_pClientInstance->Set_PlayerInput(true);
            static_cast<CUI_HUD*>(m_pClientInstance->Get_RootUI(TEXT("HUD")))->Switch_Panel(true);
            m_isEnd = true;
        }
    }
    else {
        m_fSkipTime += fTimeDelta;
        if (m_fSkipTime > 0.f && !m_isSkipFadeOut)
        {
            CSkipButton::SKIPBUTTON_DESC SkipBtnDesc{};
            SkipBtnDesc.isOpen = false;
            m_pClientInstance->UI_UpdateSwitch(TEXT("SkipButton"), &SkipBtnDesc);

            m_pClientInstance->Fade_Out();
            m_isSkipFadeOut = true;
        }

        if (m_fSkipTime > 1.f && !m_isSkipCameraSet)
        {
            m_pClientInstance->Camera_Force_AniEnd();
            m_pGameInstance->Emit_Event<EventElevatorSkip>(ENUM_CLASS(EVENT_TYPE::ELEVATOR_SKIP), { EventElevatorSkip::SkipEvent() });
            m_isSkipCameraSet = true;
        }

        if (m_fSkipTime > 2.f && !m_isSkipFadeIn)
        {
            m_pClientInstance->Fade_In();            
            m_isSkipFadeIn = true;
        }

        if (m_fSkipTime > 3.f && !m_isEnd)
        {
            m_pClientInstance->Set_PlayerInput(true);
            static_cast<CUI_HUD*>(m_pClientInstance->Get_RootUI(TEXT("HUD")))->Switch_Panel(true);
            m_isEnd = true;
        }
    }
}

void CSequence_Embars_Puzzle_First::Pause()
{

}

void CSequence_Embars_Puzzle_First::Resume()
{

}

void CSequence_Embars_Puzzle_First::StopImmediate()
{

}

void CSequence_Embars_Puzzle_First::Jump(_float fTime)
{
    m_fTime = fTime;
}

_bool CSequence_Embars_Puzzle_First::IsEnd() const
{
    return m_isEnd;
}

void CSequence_Embars_Puzzle_First::Skip_KeyInput(_float fTimeDelta)
{
    if (!m_isSkipKey)
    {
        if (m_pGameInstance->Key_Down(DIK_G))
        {
            CSkipButton::SKIPBUTTON_DESC SkipBtnDesc{};
            SkipBtnDesc.isOpen = true;
            SkipBtnDesc.Event = [this]() {
                Set_Skip(true);
                };

            CClientInstance::GetInstance()->UI_UpdateSwitch(TEXT("SkipButton"), &SkipBtnDesc);

            m_isSkipKey = true;
        }
    }


}

CSequence_Embars_Puzzle_First* CSequence_Embars_Puzzle_First::Create()
{
    return new CSequence_Embars_Puzzle_First();
}

void CSequence_Embars_Puzzle_First::Free()
{
    __super::Free();

    Safe_Release(m_pGameInstance);
    Safe_Release(m_pClientInstance);
}
