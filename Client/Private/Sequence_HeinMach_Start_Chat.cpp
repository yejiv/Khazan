#include "Sequence_HeinMach_Start_Chat.h"
#include "GameInstance.h"

CSequence_HeinMach_Start_Chat::CSequence_HeinMach_Start_Chat()
    : m_pGameInstance{ CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CSequence_HeinMach_Start_Chat::Initialize(const SEQ_REQ_PLAY_DESC& tDesc)
{
    m_Id = tDesc.tId;
    m_fTime = tDesc.fStartTime;

    return S_OK;
}

void CSequence_HeinMach_Start_Chat::Update(_float fTimeDelta)
{
    m_fTime += fTimeDelta;


    if (!m_isTalk3)
    {
        Event_Announce_Talk(3);
        m_isTalk3 = true;

        m_fTime = 0.f;
    }
   /* else if (m_isTalk3 && !m_isTalk4 && m_fTime >= 3.f)
    {
        Event_Announce_Talk(4);
        m_isTalk4 = true;

        m_fTime = 0.f;
    }
    else if (m_isTalk4 && !m_isTalk5 && m_fTime >= 3.f)
    {
        Event_Announce_Talk(5);
        m_isTalk5 = true;

        m_fTime = 0.f;
    }
    else if (m_isTalk5 && !m_isTalk6 && m_fTime >= 3.f)
    {
        Event_Announce_Talk(6);
        m_isTalk6 = true;

        m_fTime = 0.f;
    }
    else if (m_isTalk6 && !m_isTalk7 && m_fTime >= 3.f)
    {
        Event_Announce_Talk(7);
        m_isTalk7 = true;

        m_fTime = 0.f;
    }
    else if (m_isTalk7 && !m_isTalk8 && m_fTime >= 3.f)
    {
        Event_Announce_Talk(8);
        m_isTalk8 = true;

        m_fTime = 0.f;
    }
    else if (m_isTalk8 && !m_isTalk9 && m_fTime >= 3.f)
    {
        Event_Announce_Talk(9);
        m_isTalk9 = true;

        m_fTime = 0.f;
    }
    else if (m_isTalk9 && m_fTime >= 3.f)
    {
        m_isEnd = true;
    }*/

}

void CSequence_HeinMach_Start_Chat::Pause()
{
}

void CSequence_HeinMach_Start_Chat::Resume()
{
}

void CSequence_HeinMach_Start_Chat::StopImmediate()
{
}

void CSequence_HeinMach_Start_Chat::Jump(_float fTime)
{
}

_bool CSequence_HeinMach_Start_Chat::IsEnd() const
{
    return m_isEnd;
}

void CSequence_HeinMach_Start_Chat::Event_Announce_Talk(_int iIndex)
{
    m_pGameInstance->Emit_Event<EVENT_ANNOUNCE_TALK>(ENUM_CLASS(EVENT_TYPE::ANNOUNCE_TALK), EVENT_ANNOUNCE_TALK{ iIndex });
}

CSequence_HeinMach_Start_Chat* CSequence_HeinMach_Start_Chat::Create()
{
    return new CSequence_HeinMach_Start_Chat();
}

void CSequence_HeinMach_Start_Chat::Free()
{
    __super::Free();
    Safe_Release(m_pGameInstance);
}
