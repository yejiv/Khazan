#include "Store_List.h"
#include "ClientInstance.h"
#include "GameInstance.h"

#include "UI_TextBox.h"
#include "UI_Default_Tex.h"
#include "Store_Slot.h"

#include "ItemInfo_Other.h"
#include "ItemInfo_Weapon.h"

#include "UI_Inven.h"
#include "Amount.h"
#include "Popup_Item.h"
CStore_List::CStore_List(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Tap{ pDevice, pContext }
{
}

CStore_List::CStore_List(const CStore_List& Prototype)
    : CUI_Tap(Prototype)
{
}

void CStore_List::Update_Pos(_int iIndex, _float2 vPos, _float fOffSetY, CUIObject* pParet)
{
    m_vLocalPos.x = vPos.x;
    m_vLocalPos.y = vPos.y + iIndex * fOffSetY;

    __super::Update_Transform(pParet, m_vWorldPos);
}

void CStore_List::Set_Selete(_bool isSelete)
{
    m_bIsSelete = isSelete;

    m_pHover->Update_Visible(m_bIsSelete);
        
}

void CStore_List::Setting_List(_int iItemIndex)
{
    if (iItemIndex <= 0)
    {
        m_IsUpdate = false;
    }
    else
    {
        m_iItemIndex = iItemIndex;
        m_IsUpdate = true;
        const ITEM_DATA* pData = CClientInstance::GetInstance()->Get_Data<ITEM_DATA>(iItemIndex);
        if(pData == nullptr)
            return;
        m_pItemName->Set_Text(pData->strName);
        m_iAdd_Gold = pData->iGold;
        m_pGold->Set_Text(IntToWstring(pData->iGold));
        m_pSlot->Setting_Slot(pData);
    }
}

HRESULT CStore_List::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CStore_List::Initialize_Clone(void* pArg)
{
    CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);
    CHECK_FAILED(Ready_Componet(), E_FAIL);

    m_pPlayerGold = &CClientInstance::GetInstance()->Get_PlayerData().iGold;
    return S_OK;
}

void CStore_List::Priority_Update(_float fTimeDelta)
{
}

void CStore_List::Update(_float fTimeDelta)
{
    if (!m_IsUpdate)
        return;

    if (m_bIsSelete)
    {
        Render_ItemInfo();
        if (m_pGameInstance->Key_Down(DIK_F, INPUT_TYPE::UI) && *m_pPlayerGold >= m_iAdd_Gold)
        {
            CPopup_Item::POPUP_ITEM_DESC Desc;
            Desc.isSale = false;
            Desc.iItemIndex = m_iItemIndex;
            Desc.Event = [this]() {
                static_cast<CUI_Inven*>(CClientInstance::GetInstance()->Get_RootUI(TEXT("Inven")))->Add_Item(m_iItemIndex);
                static_cast<CAmount*>(CClientInstance::GetInstance()->Get_RootUI(TEXT("Amount")))->Add_Value(CAmount::AMOUNT_TYPE::GOLD, -m_iAdd_Gold);
                };
            CClientInstance::GetInstance()->UI_UpdateSwitch(TEXT("Popup_Item"), &Desc);
        }
    }
    if (*m_pPlayerGold >= m_iAdd_Gold)
    {
        m_pItemName->Set_Color({ 1.f,1.f,1.f, 1.f });
        m_pGold->Set_Color({ 1.f,1.f,1.f,1.f });
    }
    else
    {
        m_pItemName->Set_Color({ 1.f,1.f,1.f, 0.6f });
        m_pGold->Set_Color({ 1.f,0.f,0.f,0.6f });
    }
}

void CStore_List::Late_Update(_float fTimeDelta)
{
    if (!m_IsUpdate)
        return;

    m_iState = ENUM_CLASS(UISTATE::ENABLE);
    CClientInstance::GetInstance()->Add_UIRender(UI_RENDER_TYPE::DEFAULT, this);

    __super::Late_Update(fTimeDelta);
}

HRESULT CStore_List::Render()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    if (FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_Texture", m_iTexPass)))
        return E_FAIL;

    CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float)), E_FAIL);
    CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_fValue", &m_fAccTime, sizeof(_float)), E_FAIL);

    m_pShaderCom->Begin(m_iShaderPass);
    m_pVIBufferCom->Bind_Resources();
    m_pVIBufferCom->Render();

    return S_OK;
}

HRESULT CStore_List::Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg)
{
    __super::Load_UI(pInData, iPrototypeLevelID, pArg);
    m_iShaderPass = 6;
    m_iState = ENUM_CLASS(UISTATE::ENABLE);

    for (auto pChild : m_Children)
    {
        string strName = pChild->Get_Name();
        if (strName == "ButtonHover")
        {
            m_pHover = static_cast<CUI_Default_Tex*>(pChild);
            Safe_AddRef(m_pHover);
        }
        else if (strName == "Item_Slot")
        {
            m_pSlot = static_cast<CStore_Slot*>(pChild);
            Safe_AddRef(m_pSlot);
        }
        else if (strName == "Item_Name_Text")
        {
            m_pItemName = static_cast<CUI_TextBox*>(pChild);
            Safe_AddRef(m_pItemName);
        }
        else if (strName == "Gold_Text")
        {
            m_pGold = static_cast<CUI_TextBox*>(pChild);
            Safe_AddRef(m_pGold);
        }
    }

    return S_OK;
}

HRESULT CStore_List::Ready_Componet()
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex_UI"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_UI_State_Button"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
        return E_FAIL;

    return S_OK;
}

_wstring CStore_List::IntToWstring(_int iValue)
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

void CStore_List::Render_ItemInfo()
{
    if (m_iItemIndex < 0)
        return;

    const ITEM_DATA* pData = CClientInstance::GetInstance()->Get_Data<ITEM_DATA>(m_iItemIndex);

    if (pData->iType <= 3)
    {
        CItemInfo_Other::OTHERINFO_DESC Desc = {};
        Desc.iItemIndex = m_iItemIndex;
        Desc.iOffsetPos = { 800.f, 468.f };
        Desc.isEquip = false;
        Desc.iMaxItem = pData->iMaxValue;
        Desc.iCurItem = 1;
        CClientInstance::GetInstance()->UI_UpdateSwitch(TEXT("ItemInfo_Other"), &Desc);

    }
    else
    {
        CItemInfo_Weapon::WEAPONINFO_DESC Desc = {};
        Desc.iItemIndex = m_iItemIndex;
        Desc.iOffsetPos = { 800.f, 553.f };
        Desc.isEquip = false;
        Desc.iEffect_Type = 0;
        Desc.iEffect_Value = 0;
        CClientInstance::GetInstance()->UI_UpdateSwitch(TEXT("ItemInfo_Weapon"), &Desc);
    }
}

CStore_List* CStore_List::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CStore_List* pInstance = new CStore_List(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed Created : CStore_List"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CStore_List::Clone(void* pArg)
{
    CStore_List* pInstance = new CStore_List(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed Cloned : CStore_List"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CStore_List::Free()
{
    __super::Free();
    Safe_Release(m_pSlot);
    Safe_Release(m_pHover);
    Safe_Release(m_pItemName);
    Safe_Release(m_pGold);

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
}
