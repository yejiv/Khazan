#include "Sequence_Data_Manager.h"
#include "GameInstance.h"
#include "ClientInstance.h"

CSequence_Data_Manager::CSequence_Data_Manager()
    : m_pGameInstance{ CGameInstance::GetInstance() }
    , m_pClientInstance{ CClientInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
    Safe_AddRef(m_pClientInstance);
}

HRESULT CSequence_Data_Manager::Initialize()
{

    return S_OK;
}

HRESULT CSequence_Data_Manager::Push_Sequence(_wstring strName, ISeqInstance* pSequence)
{
    if (Find_Sequence(strName) != nullptr)
        return E_FAIL;

    m_Sequences.emplace(strName, pSequence);
}

class ISeqInstance* CSequence_Data_Manager::Find_Sequence(_wstring strName)
{
    auto iter = m_Sequences.find(strName);

    if (iter == m_Sequences.end())
        return nullptr;

    if (iter->second == nullptr)
    {
        m_Sequences.erase(strName);
        return nullptr;
    }
        

    return iter->second;
}

void CSequence_Data_Manager::Clear()
{
    m_Sequences.clear();
}

HRESULT CSequence_Data_Manager::Remove_Sequence(_wstring strName)
{
    if (Find_Sequence(strName) == nullptr)
        return E_FAIL;

    m_Sequences.erase(strName);
}

CSequence_Data_Manager* CSequence_Data_Manager::Create()
{
    CSequence_Data_Manager* pInstance = new CSequence_Data_Manager();
    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX(TEXT("Failed Created : CSequence_Data_Manager"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CSequence_Data_Manager::Free()
{
    __super::Free();
    Safe_Release(m_pGameInstance);
    Safe_Release(m_pClientInstance);

    m_Sequences.clear();
}


