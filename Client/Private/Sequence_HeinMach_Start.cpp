#include "Sequence_HeinMach_Start.h"
#include "GameInstance.h"
#include "ClientInstance.h"

CSequence_HeinMach_Start::CSequence_HeinMach_Start()
    : m_pGameInstance{ CGameInstance::GetInstance() }
    , m_pClientInstance { CClientInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
    Safe_AddRef(m_pClientInstance);
}

HRESULT CSequence_HeinMach_Start::Initialize(const SEQ_REQ_PLAY_DESC& tDesc)
{
    m_Id = tDesc.tId;
    m_fTime = tDesc.fStartTime;

    return S_OK;
}

void CSequence_HeinMach_Start::Update(_float fTimeDelta)
{
    m_fTime += fTimeDelta;

    if (m_fTime > 1.f && !m_isCamera)
    {
        m_pClientInstance->Start_ForceOrbit(CAMERA_FORCE_DIR::FRONT);
        m_isCamera = true;
    }

    if (m_fTime > 3.f && !m_isFadeIn)
    {
        m_pClientInstance->Start_ForceOrbit(CAMERA_FORCE_DIR::FRONT);
        m_pClientInstance->Fade_In();
        m_isFadeIn = true;
    }

    if (m_fTime > 4.f)
    {
        m_isEnd = true;
    }
}

void CSequence_HeinMach_Start::Pause()
{
}

void CSequence_HeinMach_Start::Resume()
{
}

void CSequence_HeinMach_Start::StopImmediate()
{
}

void CSequence_HeinMach_Start::Jump(_float fTime)
{
}

_bool CSequence_HeinMach_Start::IsEnd() const
{
    return m_isEnd;
}

CSequence_HeinMach_Start* CSequence_HeinMach_Start::Create()
{
    return new CSequence_HeinMach_Start();
}

void CSequence_HeinMach_Start::Free()
{
    __super::Free();
    Safe_Release(m_pGameInstance);
    Safe_Release(m_pClientInstance);
}
