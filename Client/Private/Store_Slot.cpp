#include "Store_Slot.h"
#include "GameInstance.h"
#include "ClientInstance.h"

#include "UI_Atlas_Icon.h"

CStore_Slot::CStore_Slot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Panel{ pDevice, pContext }
{
}

CStore_Slot::CStore_Slot(const CStore_Slot& Prototype)
    : CUI_Panel(Prototype)
{
}

void CStore_Slot::Setting_Slot(const ITEM_DATA* pData)
{
    m_pAtlasIcon->Set_Texture(CClientInstance::GetInstance()->Get_AtlasUV(WStringToAnsi(pData->strIconName), 2), 2);
    switch (pData->iGrade)
    {
    case 0: m_vUV[m_iState] = CClientInstance::GetInstance()->Get_AtlasUV("T_Slot_Inven_02_Common.png", m_iTexPass);
        return;
    case 1: m_vUV[m_iState] = CClientInstance::GetInstance()->Get_AtlasUV("T_Slot_Inven_03_UnCommon.png", m_iTexPass);
        return;
    case 2: m_vUV[m_iState] = CClientInstance::GetInstance()->Get_AtlasUV("T_Slot_Inven_04_Rare.png", m_iTexPass);
        return;
    case 3: m_vUV[m_iState] = CClientInstance::GetInstance()->Get_AtlasUV("T_Slot_Inven_05_Unique.png", m_iTexPass);
        return;
    case 4: m_vUV[m_iState] = CClientInstance::GetInstance()->Get_AtlasUV("T_Slot_Inven_06_Legendary.png", m_iTexPass);
        return;
    case 5: m_vUV[m_iState] = CClientInstance::GetInstance()->Get_AtlasUV("T_Slot_Inven_07_Epic.png", m_iTexPass);
        return;
    }


}

HRESULT CStore_Slot::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CStore_Slot::Initialize_Clone(void* pArg)
{
    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    return S_OK;
}
void CStore_Slot::Priority_Update(_float fTimeDelta)
{
    if (!m_isVisible)
        return;

    __super::Priority_Update(fTimeDelta);
}

void CStore_Slot::Update(_float fTimeDelta)
{
    if (!m_isVisible)
        return;

    __super::Update(fTimeDelta);
}

void CStore_Slot::Late_Update(_float fTimeDelta)
{
    if (!m_isVisible)
        return;

    CClientInstance::GetInstance()->Add_UIRender(UI_RENDER_TYPE::ATLAS, this);
    __super::Late_Update(fTimeDelta);
}

HRESULT CStore_Slot::Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg)
{
    CHECK_FAILED(__super::Load_UI(pInData, iPrototypeLevelID, pArg),E_FAIL);

    m_pAtlasIcon = static_cast<CUI_Atlas_Icon*>(m_Children[0]);
    Safe_AddRef(m_pAtlasIcon);
    return S_OK;
}

CStore_Slot* CStore_Slot::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CStore_Slot* pInstance = new CStore_Slot(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed Created : CStore_Slot"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CStore_Slot::Clone(void* pArg)
{
    CStore_Slot* pInstance = new CStore_Slot(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed Cloned : CStore_Slot"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CStore_Slot::Free()
{
    __super::Free();
    Safe_Release(m_pAtlasIcon);
}
