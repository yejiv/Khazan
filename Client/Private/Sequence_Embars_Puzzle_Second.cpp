#include "Sequence_Embars_Puzzle_Second.h"
#include "GameInstance.h"
#include "ClientInstance.h"
#include "Transform.h"
#include "GameObject.h"

CSequence_Embars_Puzzle_Second::CSequence_Embars_Puzzle_Second()
    : m_pGameInstance{ CGameInstance::GetInstance() }
    , m_pClientInstance{ CClientInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
    Safe_AddRef(m_pClientInstance);
}

HRESULT CSequence_Embars_Puzzle_Second::Initialize(const SEQ_REQ_PLAY_DESC& tDesc)
{
    m_Id = tDesc.tId;
    m_fTime = tDesc.fStartTime;

    m_pClientInstance->Camera_Set_Animation_Json("../../Client/Bin/Data/Camera/Animation/VerticalGate2");
    m_pClientInstance->Camera_Set_Animation_Json("../../Client/Bin/Data/Camera/Animation/Turn_Elevator");

    return S_OK;
}

void CSequence_Embars_Puzzle_Second::Update(_float fTimeDelta)
{
    m_fTime += fTimeDelta;

    if (m_fTime >= 3.0f && !m_isElevatorFadeOut)
    {
        CClientInstance::GetInstance()->Fade_Out();
        m_isElevatorFadeOut = true;
    }
    else if (m_fTime >= 4.0f && !m_isElevatorCamera)
    {
        CClientInstance::GetInstance()->Fade_In();
        m_pClientInstance->Camera_Set_Animation(TEXT("Turn_Elevator"));
        m_isElevatorCamera = true;
    }
    else if (m_fTime >= 12.0f && !m_isVerticalGateFadeOut)
    {
        CClientInstance::GetInstance()->Fade_Out();
        m_isVerticalGateFadeOut = true;
    }
    else if (m_fTime >= 13.0f && !m_isVerticalGateCamera)
    {
        CClientInstance::GetInstance()->Fade_In();
        m_pClientInstance->Camera_Set_Animation(TEXT("VerticalGate2"));
        m_isVerticalGateCamera = true;
    }
    else if (m_fTime >= 19.0f && !m_isMovePlayerFadeOut)
    {
        CClientInstance::GetInstance()->Fade_Out();
        m_isMovePlayerFadeOut = true;
    }
    else if (m_fTime > 20.0f && !m_isMovePlayer)
    {
        m_pClientInstance->Camera_Set_FixEnd();
        m_isMovePlayer = true;
    }
    else if (m_fTime >= 21.f)
    {
        CClientInstance::GetInstance()->Fade_In();
        m_isEnd = true;
    }

}

void CSequence_Embars_Puzzle_Second::Pause()
{

}

void CSequence_Embars_Puzzle_Second::Resume()
{

}

void CSequence_Embars_Puzzle_Second::StopImmediate()
{

}

void CSequence_Embars_Puzzle_Second::Jump(_float fTime)
{
    m_fTime = fTime;
}

_bool CSequence_Embars_Puzzle_Second::IsEnd() const
{
    return m_isEnd;
}

CSequence_Embars_Puzzle_Second* CSequence_Embars_Puzzle_Second::Create()
{
    return new CSequence_Embars_Puzzle_Second();
}

void CSequence_Embars_Puzzle_Second::Free()
{
    __super::Free();

    Safe_Release(m_pGameInstance);
    Safe_Release(m_pClientInstance);
}
