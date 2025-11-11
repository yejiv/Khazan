#pragma once
#include "Client_Defines.h"
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Client)

class CInteract_Manager final : public CBase
{
private:
    CInteract_Manager();
    virtual ~CInteract_Manager() = default;

public:
    HRESULT Initialize();

public:
    void Add_BladeNexus(KHAZAN_MAP eMapName, INTER_BLADENEXUS_DESC* pDesc);
    INTER_BLADENEXUS_DESC* Find_BladeNexus(KHAZAN_MAP eMapName, _uint iID);
    INTER_BLADENEXUS_DESC* Find_BladeNexus(_uint iID);
    vector<INTER_BLADENEXUS_DESC*>* Find_MapBladeNexus(KHAZAN_MAP eMapName);
    void Unlock_BladeNexus(_uint iID);

private:
    class CGameInstance* m_pGameInstance = { nullptr };

public:
    map<KHAZAN_MAP, vector<INTER_BLADENEXUS_DESC*>> m_BladeNexus;

public:
    static CInteract_Manager* Create();
    virtual void Free() override;
};

NS_END