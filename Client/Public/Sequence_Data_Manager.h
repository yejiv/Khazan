#pragma once
#include "Client_Defines.h"
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Client)

class CSequence_Data_Manager final : public CBase
{
private:
    CSequence_Data_Manager();
    virtual ~CSequence_Data_Manager() = default;

public:
    HRESULT Initialize();

public:
    HRESULT Push_Sequence(_wstring strName, class ISeqInstance* pSequence);
    class ISeqInstance* Find_Sequence(_wstring strName);
    HRESULT Remove_Sequence(_wstring strName);

public:
    void Clear();

private:
    class CGameInstance* m_pGameInstance = { nullptr };
    class CClientInstance* m_pClientInstance = { nullptr };

    map<_wstring, class ISeqInstance*> m_Sequences;

public:
    static CSequence_Data_Manager* Create();
    virtual void Free() override;
};

NS_END