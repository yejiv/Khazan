#include "Sequence_Dummy.h"

CSequence_Dummy::CSequence_Dummy()
{
}

HRESULT CSequence_Dummy::Initialize(const SEQ_REQ_PLAY_DESC& tDesc)
{
    m_Id = tDesc.tId; 
    m_Time = tDesc.fStartTime;
    m_State = STATE::Playing; 
    
    return S_OK;
}

void CSequence_Dummy::Update(_float fTimeDelta)
{
    if (m_State != STATE::Playing) return;
    m_Time += fTimeDelta;


    if (m_Time >= 5.f) 
        m_State = STATE::End;
}

void CSequence_Dummy::Pause()
{
    if (m_State == STATE::Playing) 
        m_State = STATE::Paused;
}

void CSequence_Dummy::Resume()
{
    if (m_State == STATE::Paused) 
        m_State = STATE::Playing;
}

void CSequence_Dummy::StopImmediate()
{
    m_State = STATE::End;
}

void CSequence_Dummy::Jump(_float fTime)
{
    m_Time = fTime;
}

_bool CSequence_Dummy::IsEnd() const
{
    return m_State == STATE::End;
}

CSequence_Dummy* CSequence_Dummy::Create()
{
    return new CSequence_Dummy();
}

void CSequence_Dummy::Free()
{
    __super::Free();
}
