#include "Sequence_Viper_SecondPhase.h"
#include "GameInstance.h"
#include "ClientInstance.h"
#include "Transform.h"
#include "GameObject.h"
#include "Obelisk.h"

CSequence_Viper_SecondPhase::CSequence_Viper_SecondPhase()
    : m_pGameInstance{ CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CSequence_Viper_SecondPhase::Initialize(const SEQ_REQ_PLAY_DESC& tDesc)
{


    return S_OK;
}

void CSequence_Viper_SecondPhase::Update(_float fTimeDelta)
{
    m_fTime += fTimeDelta;
    

    if (m_fTime >= 0.7f && m_Obelisks.size() != 0)
    {
        //m_Obelisks.back()->Destory();
        //m_Obelisks.pop();

        //m_fTime = 0.f;
    }

    if (m_fTime >= 3.f)
        m_isEnd = true;
}

void CSequence_Viper_SecondPhase::Pause()
{

}

void CSequence_Viper_SecondPhase::Resume()
{

}

void CSequence_Viper_SecondPhase::StopImmediate()
{

}

void CSequence_Viper_SecondPhase::Jump(_float fTime)
{
    m_fTime = fTime;
}

_bool CSequence_Viper_SecondPhase::IsEnd() const
{
    return m_isEnd;
}

void CSequence_Viper_SecondPhase::Push_Obelisk(CObelisk* pObelisk)
{
    m_Obelisks.push(pObelisk);
}


CSequence_Viper_SecondPhase* CSequence_Viper_SecondPhase::Create()
{
    return new CSequence_Viper_SecondPhase();
}

void CSequence_Viper_SecondPhase::Free()
{
    __super::Free();

    Safe_Release(m_pGameInstance);

}
