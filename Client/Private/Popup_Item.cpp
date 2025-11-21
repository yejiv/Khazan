#include "Popup_Item.h"
#include "ClientInstance.h"
#include "GameInstance.h"

#include "UI_TextBox.h"
#include "UI_Default_Tex.h"
#include "UI_BackGround.h"
#include "Store_Slot.h"

CPopup_Item::CPopup_Item(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Panel{ pDevice, pContext }
{
}

CPopup_Item::CPopup_Item(const CPopup_Item& Prototype)
    : CUI_Panel(Prototype)
{
}

HRESULT CPopup_Item::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CPopup_Item::Initialize_Clone(void* pArg)
{
    CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);
    CHECK_FAILED(Ready_Componet(), E_FAIL);
    CHECK_FAILED(Ready_Object(), E_FAIL);

    m_fValue = 0.82f;
    return S_OK;
}

void CPopup_Item::Priority_Update(_float fTimeDelta)
{
    if (m_isChangeInputType)
    {
        m_pGameInstance->Change_InputType(INPUT_TYPE::UI);
        m_isChangeInputType = false;
    }
    if (!m_IsUpdate)
        return;

}

void CPopup_Item::Update(_float fTimeDelta)
{
    if (!m_IsUpdate)
        return;

    if (m_isOnStart)
    {
        m_isOnStart = false;
        return;
    }
    if (m_pGameInstance->Key_Down(DIK_F, INPUT_TYPE::POPUP))
    {
        m_Event();
        m_IsUpdate = false;
        m_isChangeInputType = true;
    }
    else if (m_pGameInstance->Key_Down(DIK_ESCAPE, INPUT_TYPE::POPUP))
    {
        m_IsUpdate = false;
        m_pGameInstance->Change_InputType(INPUT_TYPE::UI);
        m_isChangeInputType = true;
    }
}

void CPopup_Item::Late_Update(_float fTimeDelta)
{
    if (!m_IsUpdate)
        return;

    CClientInstance::GetInstance()->Add_UIRender(UI_RENDER_TYPE::DEFAULT, this);
    __super::Late_Update(fTimeDelta);
}

HRESULT CPopup_Item::Render()
{
    CHECK_FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix), E_FAIL);
    CHECK_FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix), E_FAIL);
    CHECK_FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);
    CHECK_FAILED(m_pTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_Texture", m_iTexPass), E_FAIL);

    CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_fValue", &m_fValue, sizeof(_float)), E_FAIL);
    CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float)), E_FAIL);
    CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_float4)), E_FAIL);

    m_pShaderCom->Begin(15);
    m_pVIBufferCom->Bind_Resources();
    m_pVIBufferCom->Render();

    return S_OK;
}

HRESULT CPopup_Item::Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg)
{
    __super::Load_UI(pInData, iPrototypeLevelID, pArg);

    for (auto pChild : m_Children)
    {
        string strName = pChild->Get_Name();
        if (strName == "Name")
        {
            m_pPanelName = static_cast<CUI_TextBox*>(pChild);
            Safe_AddRef(m_pPanelName);
        }
        else if (strName == "Info_1")
        {
            m_pPanelInfo = static_cast<CUI_TextBox*>(pChild);
            Safe_AddRef(m_pPanelInfo);
        }
        else if (strName == "Item_Slot")
        {
            m_pSlot = static_cast<CStore_Slot*>(pChild);
            Safe_AddRef(m_pSlot);
        }
        else if (strName == "Item_Name")
        {
            m_pItemName = static_cast<CUI_TextBox*>(pChild);
            Safe_AddRef(m_pItemName);
        }
        else if (strName == "Gold_Text")
        {
            m_pGold = static_cast<CUI_TextBox*>(pChild);
            Safe_AddRef(m_pGold);
        }
        else if (strName == "Line_0" || strName == "Line_1" || strName == "Line_2" || strName == "Line_3" || strName == "Line_4")
        {
            pChild->Set_Color({ 1.f, 1.f, 1.f, 0.7f });
        }
    }

    return S_OK;
}

HRESULT CPopup_Item::Update_Switch(void* pArg)
{
    POPUP_ITEM_DESC* pDesc = static_cast<POPUP_ITEM_DESC*>(pArg);

    if (pDesc->isSale)
    {
        m_pPanelName->Set_Text(TEXT("아이템 판매"));
        m_pPanelInfo->Set_Text(TEXT("아이템을 판매 하시겠습니까?"));
    }
    else
    {
        m_pPanelName->Set_Text(TEXT("아이템 구매"));
        m_pPanelInfo->Set_Text(TEXT("아이템을 구매 하시겠습니까?"));
    }

    const ITEM_DATA* pData = CClientInstance::GetInstance()->Get_Data<ITEM_DATA>(pDesc->iItemIndex);

    m_pSlot->Setting_Slot(pData);
    m_pItemName->Set_Text(pData->strName);
    m_pGold->Set_Text(IntToWstring(pData->iGold));

    m_Event = pDesc->Event;
    m_IsUpdate = true;
    m_pGameInstance->Change_InputType(INPUT_TYPE::POPUP);
    m_isOnStart = true;
    return S_OK;
}

HRESULT CPopup_Item::Ready_Componet()
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex_UI"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_UI_GuideTex"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
        return E_FAIL;

    return S_OK;
}

HRESULT CPopup_Item::Ready_Object()
{
    UIOBJECT_DESC Desc = {};
    Desc.fDepth = 2.2f;
    Desc.iUIType = ENUM_CLASS(UITYPE::TEXTURE);
    Desc.szName = "BackGround";
    Desc.vLocalSize = { g_iWinSizeX, g_iWinSizeY };
    Desc.vLocalPos = { 0, 0 };

    m_pBackGround = static_cast<CUI_BackGround*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_BackGround"), &Desc));
    if (m_pBackGround == nullptr)
        return E_FAIL;
    m_pBackGround->Setting_BG(CUI_BackGround::UIBGTYPE::END);
    m_pBackGround->Set_Color({ 0.0f, 0.0f, 0.0f, 0.6f });
    m_Children.push_back(m_pBackGround);
    Safe_AddRef(m_pBackGround);

    return S_OK;
}

_wstring CPopup_Item::IntToWstring(_int iValue)
{
    _wstring wstrValue = {};
    if (iValue > 999)
    {
        _int iTemp = iValue * 0.001;
        wstrValue = to_wstring(iTemp);
        wstrValue += TEXT(",");
        iTemp = iValue - (1000 * iTemp);
        if (iTemp <= 0)
            wstrValue += TEXT("000");
        else
            wstrValue += to_wstring(iTemp);
    }
    else
    {
        wstrValue = to_wstring(iValue);
    }
    return wstrValue;
}

CPopup_Item* CPopup_Item::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CPopup_Item* pInstance = new CPopup_Item(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed Created : CPopup_Item"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CPopup_Item::Clone(void* pArg)
{
    CPopup_Item* pInstance = new CPopup_Item(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed Cloned : CPopup_Item"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CPopup_Item::Free()
{
    __super::Free();

    Safe_Release(m_pBackGround);

    Safe_Release(m_pSlot);
    Safe_Release(m_pPanelName);
    Safe_Release(m_pPanelInfo);

    Safe_Release(m_pItemName);
    Safe_Release(m_pGold);

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
}
