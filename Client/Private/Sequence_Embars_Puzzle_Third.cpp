#include "Sequence_Embars_Puzzle_Third.h"
#include "GameInstance.h"
#include "ClientInstance.h"
#include "Transform.h"
#include "GameObject.h"

CSequence_Embars_Puzzle_Third::CSequence_Embars_Puzzle_Third()
    : m_pGameInstance{ CGameInstance::GetInstance() }
    , m_pClientInstance{ CClientInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
    Safe_AddRef(m_pClientInstance);
}

HRESULT CSequence_Embars_Puzzle_Third::Initialize(const SEQ_REQ_PLAY_DESC& tDesc)
{
    m_Id = tDesc.tId;
    m_fTime = tDesc.fStartTime;

    m_pClientInstance->Camera_Set_Animation_Json("../../Client/Bin/Data/Camera/Animation/Turn_Elevator");

    return S_OK;
}

void CSequence_Embars_Puzzle_Third::Update(_float fTimeDelta)
{
    m_fTime += fTimeDelta;

    if (m_fTime >= 0.f && !m_isElevatorFadeOut)
    {
        CClientInstance::GetInstance()->Fade_Out();
        m_isElevatorFadeOut = true;
    }
    else if (m_fTime >= 1.f && !m_isElevatorCamera)
    {
        CClientInstance::GetInstance()->Fade_In();
        m_pClientInstance->Camera_Set_Animation(TEXT("Turn_Elevator"));
        m_isElevatorCamera = true;
    }
    else if (m_fTime >= 4.f && !m_isMovePlayerFadeOut)
    {
        CClientInstance::GetInstance()->Fade_Out();
        m_isMovePlayerFadeOut = true;
    }
    else if (m_fTime > 5.f && !m_isMovePlayer)
    {
        m_pClientInstance->Camera_Set_FixEnd();
        m_isMovePlayer = true;
    }
    else if (m_fTime >= 7.f)
    {
        CClientInstance::GetInstance()->Fade_In();
        m_isEnd = true;
    }

}

void CSequence_Embars_Puzzle_Third::Pause()
{

}

void CSequence_Embars_Puzzle_Third::Resume()
{

}

void CSequence_Embars_Puzzle_Third::StopImmediate()
{

}

void CSequence_Embars_Puzzle_Third::Jump(_float fTime)
{
    m_fTime = fTime;
}

_bool CSequence_Embars_Puzzle_Third::IsEnd() const
{
    return m_isEnd;
}

CSequence_Embars_Puzzle_Third* CSequence_Embars_Puzzle_Third::Create()
{
    return new CSequence_Embars_Puzzle_Third();
}

void CSequence_Embars_Puzzle_Third::Free()
{
    __super::Free();

    Safe_Release(m_pGameInstance);
    Safe_Release(m_pClientInstance);
}
