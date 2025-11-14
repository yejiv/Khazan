#include "Interact_Manager.h"
#include "GameInstance.h"
#include "Camera.h"


CInteract_Manager::CInteract_Manager()
    : m_pGameInstance{ CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}


HRESULT CInteract_Manager::Initialize()
{
    // 하인마흐
    INTER_BLADENEXUS_DESC* HEINFirstDesc = new INTER_BLADENEXUS_DESC();
    HEINFirstDesc->iID = 0;
    HEINFirstDesc->iLevel = ENUM_CLASS(LEVEL::HEINMACH);
    HEINFirstDesc->isUnLock = false;
    HEINFirstDesc->strName = TEXT("눈보라 협곡");
    HEINFirstDesc->vPos = _float4(-0.733f, 0.2f, 33.390f, 1.f);

    Add_BladeNexus(KHAZAN_MAP::HEINMACH, HEINFirstDesc);

    INTER_BLADENEXUS_DESC* HEINSecondDesc = new INTER_BLADENEXUS_DESC();
    HEINSecondDesc->iID = 1;
    HEINSecondDesc->iLevel = ENUM_CLASS(LEVEL::HEINMACH);
    HEINSecondDesc->isUnLock = false;
    HEINSecondDesc->strName = TEXT("냉기 서린 동굴");
    HEINSecondDesc->vPos = _float4(346.049f, -0.992f, 137.849f, 1.f);

    Add_BladeNexus(KHAZAN_MAP::HEINMACH, HEINSecondDesc);

    INTER_BLADENEXUS_DESC* HEINThirdDesc = new INTER_BLADENEXUS_DESC();
    HEINThirdDesc->iID = 2;
    HEINThirdDesc->iLevel = ENUM_CLASS(LEVEL::HEINMACH);
    HEINThirdDesc->isUnLock = false;
    HEINThirdDesc->strName = TEXT("설인의 대지");
    HEINThirdDesc->vPos = _float4(376.634f, 1.002f, 361.793f, 1.f);

    Add_BladeNexus(KHAZAN_MAP::HEINMACH, HEINThirdDesc);

    INTER_BLADENEXUS_DESC* HEINFourthDesc = new INTER_BLADENEXUS_DESC();
    HEINFourthDesc->iID = 3;
    HEINFourthDesc->iLevel = ENUM_CLASS(LEVEL::HEINMACH);
    HEINFourthDesc->isUnLock = false;
    HEINFourthDesc->strName = TEXT("예투가의 레어");
    HEINFourthDesc->vPos = _float4(0.f, 0.f, 0.f, 1.f);

    Add_BladeNexus(KHAZAN_MAP::HEINMACH, HEINFourthDesc);

    // 엠바스 유적지
    INTER_BLADENEXUS_DESC* EMBARSFirstDesc = new INTER_BLADENEXUS_DESC();
    EMBARSFirstDesc->iID = 4;
    EMBARSFirstDesc->iLevel = ENUM_CLASS(LEVEL::EMBARS);
    EMBARSFirstDesc->isUnLock = false;
    EMBARSFirstDesc->strName = TEXT("잊혀진 사원의 지하");
    EMBARSFirstDesc->vPos = _float4(0.f, 0.f, 0.f, 1.f);

    Add_BladeNexus(KHAZAN_MAP::EMBARS, EMBARSFirstDesc);

    INTER_BLADENEXUS_DESC* EMBARSSecondDesc = new INTER_BLADENEXUS_DESC();
    EMBARSSecondDesc->iID = 5;
    EMBARSSecondDesc->iLevel = ENUM_CLASS(LEVEL::EMBARS);
    EMBARSSecondDesc->isUnLock = false;
    EMBARSSecondDesc->strName = TEXT("잊혀진 사원의 깊은 곳");
    EMBARSSecondDesc->vPos = _float4(0.f, 0.f, 0.f, 1.f);

    Add_BladeNexus(KHAZAN_MAP::EMBARS, EMBARSSecondDesc);

    INTER_BLADENEXUS_DESC* EMBARSThirdDesc = new INTER_BLADENEXUS_DESC();
    EMBARSThirdDesc->iID = 6;
    EMBARSThirdDesc->iLevel = ENUM_CLASS(LEVEL::EMBARS);
    EMBARSThirdDesc->isUnLock = false;
    EMBARSThirdDesc->strName = TEXT("잊혀진 사원의 심장");
    EMBARSThirdDesc->vPos = _float4(0.f, 0.f, 0.f, 1.f);

    Add_BladeNexus(KHAZAN_MAP::EMBARS, EMBARSThirdDesc);

    return S_OK;
}

void CInteract_Manager::Add_BladeNexus(KHAZAN_MAP eMapName, INTER_BLADENEXUS_DESC* pDesc)
{
    auto iter = m_BladeNexus.find(eMapName);

    if (iter == m_BladeNexus.end())
    {
        vector<INTER_BLADENEXUS_DESC*> vDesc;
        vDesc.push_back(pDesc);
        m_BladeNexus.emplace(eMapName, vDesc);
    }
    else
    {
        iter->second.push_back(pDesc);
    }
}

INTER_BLADENEXUS_DESC* CInteract_Manager::Find_BladeNexus(KHAZAN_MAP eMapName, _uint iID)
{
    auto iter = m_BladeNexus.find(eMapName);

    if (iter == m_BladeNexus.end())
        return nullptr;

    for (auto Desc : iter->second)
    {
        if (Desc->iID == iID)
        {
            return Desc;
        }
    }

    return nullptr;
}

INTER_BLADENEXUS_DESC* CInteract_Manager::Find_BladeNexus(_uint iID)
{
    for (auto BladeNexus : m_BladeNexus)
    {
        for (auto BladeNexus : BladeNexus.second)
        {
            if (BladeNexus->iID == iID)
                return BladeNexus;
        }
    }
    return nullptr;
}

vector<INTER_BLADENEXUS_DESC*>* CInteract_Manager::Find_MapBladeNexus(KHAZAN_MAP eMapName)
{
    auto iter = m_BladeNexus.find(eMapName);

    if (iter == m_BladeNexus.end())
        return nullptr;

    return &(iter->second);
}

void CInteract_Manager::Unlock_BladeNexus(_uint iID)
{
    INTER_BLADENEXUS_DESC* pDesc = Find_BladeNexus(iID);

    if (pDesc == nullptr)
        return;

    pDesc->isUnLock = true;
}


CInteract_Manager* CInteract_Manager::Create()
{
    CInteract_Manager* pInstance = new CInteract_Manager();

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX(TEXT("Failed to Created : CInteract_Manager"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CInteract_Manager::Free()
{
    __super::Free();

    for (auto& Map : m_BladeNexus)
    {
        for (auto& Nexus : Map.second)
        {
            Safe_Delete(Nexus);
        }
        Map.second.clear();
    }
    m_BladeNexus.clear();

    Safe_Release(m_pGameInstance);

}
