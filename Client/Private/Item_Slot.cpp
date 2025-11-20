#include "Item_Slot.h"
#include "GameInstance.h"
#include "ClientInstance.h"

#include "UI_Atlas_Icon.h"
#include "UI_Inven.h"
#include "UI_TextBox.h"

#include "ItemInfo_Other.h"
#include "ItemInfo_Weapon.h"

#include "UI_Slot_Smoke.h"
#include "Amount.h"

CItem_Slot::CItem_Slot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Slot{ pDevice, pContext }
{
}

CItem_Slot::CItem_Slot(const CItem_Slot& Prototype)
    : CUI_Slot(Prototype)
{
}

_int* CItem_Slot::Get_ptrItemCount()
{
    return &m_iItemCount;
}

_bool CItem_Slot::Add_Item(_int iItemIndex)
{
    if (m_iItemIndex < 0)
    {
        m_iItemIndex = iItemIndex;
        ITEM_DATA ItemData = *CClientInstance::GetInstance()->Get_Data<ITEM_DATA>(m_iItemIndex);
        
        string strItemData = WStringToAnsi(ItemData.strIconName);
        _float4 vUV = CClientInstance::GetInstance()->Get_AtlasUV(strItemData.c_str(), 2);
        _uint iTexPass = ItemData.iTexPass;
        m_pIcon->Set_Texture(vUV, iTexPass);
        m_iItemMaxCount = ItemData.iMaxValue;
        ++m_iItemCount;
        Update_State(ItemData.iGrade);

        if (ItemData.iType >= 4)
        {
            m_fMainValue = CClientInstance::GetInstance()->Get_Data<EQUIPITEM_DATA>(ItemData.iEffect_ID)->iValue_1;

            Random_Effect_Setting(ItemData.iType);

        }
        return true;
    }
    else if (m_iItemIndex == iItemIndex && m_iItemCount < m_iItemMaxCount)
    {
        ++m_iItemCount;
        return true;
    }
    else
        return false;
}

void CItem_Slot::Update_Pos(_int iIndex, _float2 vPos, _float fOffSet, _int iMaxIndexX, _int iMaxIndexY)
{
    m_iIndex = iIndex;
    _int iCol = iIndex % iMaxIndexX;
    _int iRow = iIndex / iMaxIndexX;

    m_vWorldPos.x = vPos.x + iCol * fOffSet - (iMaxIndexX - 1) * fOffSet / 2;
    m_vWorldPos.y = vPos.y + iRow * fOffSet - (iMaxIndexY - 1) * fOffSet / 2;

    m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(m_vWorldPos.x - m_iWinSizeX * 0.5f, -m_vWorldPos.y + m_iWinSizeY * 0.5f, 0.f, 1.f));
    __super::Update_Transform(nullptr, m_vWorldPos);
}

_bool CItem_Slot::On_Selete()
{
    if (m_iItemIndex < 0)
        return false;

    m_bIsSelete = true;
    return true;
}

_bool CItem_Slot::Off_Selete()
{
    if (m_iItemIndex < 0)
        return false;
    m_bIsSelete = false;
    return true;
}

void CItem_Slot::is_Equip(_bool isEquip, _int iIndex)
{
    if (m_iItemIndex < 0)
        return;

    m_bIsEquip = isEquip;

    if (m_bIsEquip && m_iItemType == ENUM_CLASS(CUI_Inven::ITEMTYPE::ATIVE) && iIndex > 0)
    {
        _float4 vUV = {};
        if(iIndex == 1)
            vUV = CClientInstance::GetInstance()->Get_AtlasUV("T_Icon_ShortcutSlot_Equip_01.png", 1);
        if (iIndex == 2)
            vUV = CClientInstance::GetInstance()->Get_AtlasUV("T_Icon_ShortcutSlot_Equip_02.png", 1);
        if (iIndex == 3)
            vUV = CClientInstance::GetInstance()->Get_AtlasUV("T_Icon_ShortcutSlot_Equip_03.png", 1);
        if (iIndex == 4)
            vUV = CClientInstance::GetInstance()->Get_AtlasUV("T_Icon_ShortcutSlot_Equip_04.png", 1);
        if (iIndex == 5)
            vUV = CClientInstance::GetInstance()->Get_AtlasUV("T_Icon_ShortcutSlot_Equip_05.png", 1);
        if (iIndex == 6)
            vUV = CClientInstance::GetInstance()->Get_AtlasUV("T_Icon_ShortcutSlot_Equip_06.png", 1);

        m_pEquipIcon->Set_Texture(vUV, 1);
    }

}

void CItem_Slot::Is_UpItem(_float fValue)
{
    if (m_fMainValue > fValue)
        m_isUpIcon = true;
    else
        m_isUpIcon = false;
}

HRESULT CItem_Slot::Initialize_Prototype(_uint iLevel)
{
    m_iLevel = iLevel;

    return S_OK;
}

HRESULT CItem_Slot::Initialize_Clone(void* pArg)
{
    ITEMSLOT_DESC* pDesc = static_cast<ITEMSLOT_DESC*>(pArg);
    m_iItemType = pDesc->iItemType;
    m_iTexPass = 1;
    m_iShaderPass = 0;
    m_vColor = { 1.f,1.f,1.f,0.75f };

    _float4 vUV = CClientInstance::GetInstance()->Get_AtlasUV("T_Slot_Inven_01_Empty.png", m_iTexPass);
    m_vUV.push_back(vUV);

    for (_int i = 0; i < 2; i++)
    {
        _float4 vUV = CClientInstance::GetInstance()->Get_AtlasUV("T_Slot_Inven_03_UnCommon_Set.png", m_iTexPass);
        m_vUV.push_back(vUV);
    }

    CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);
    CHECK_FAILED(Ready_Children(), E_FAIL);

    __super::Update_Transform(this, m_vLocalPos);
    return S_OK;
}

void CItem_Slot::Priority_Update(_float fTimeDelta)
{
}

void CItem_Slot::Update(_float fTimeDelta)
{
    
}

void CItem_Slot::Late_Update(_float fTimeDelta)
{
    CClientInstance::GetInstance()->Add_UIRender(UI_RENDER_TYPE::ATLAS, this);
    
    if (m_fEquipTime > 0.f)
        m_fEquipTime -= fTimeDelta;
    else
        m_iEquipCount = 0;

    if (m_iItemIndex > -1 && ButtonClick(g_hWnd, false, true))
    {
        Selete_Item();
        ++m_iEquipCount;
        m_fEquipTime = 0.5f;
        
        
        if (!m_isSale)
        {
            if (m_iEquipCount >= 2)
            {
                Equip_Item();
                m_iEquipCount = 0;
            }
        }
        else if(!m_bIsEquip)
        {
            Sale_Item();
        }
    }
    if (m_iItemIndex > -1 && m_bIsSelete && m_pGameInstance->Key_Down(DIK_F, INPUT_TYPE::UI))
    {
        if(!m_isSale)
            Equip_Item();
        else if (!m_bIsEquip)
        {
            const ITEM_DATA* pData = CClientInstance::GetInstance()->Get_Data<ITEM_DATA>(m_iItemIndex);
            if (pData->iLachryma == -1 && pData->iGold == -1)
            {
            }
            else
                Sale_Item();
        }
    }
    if (m_iState == ENUM_CLASS(UISTATE::ENABLE))
    {
        if (ButtonOver(g_hWnd) && m_pGameInstance->Get_InputType() == INPUT_TYPE::UI)
        {
            m_pOverFx->Late_Update(fTimeDelta);
            Render_ItemInfo();
        }
        if (m_bIsSelete)
        {
            m_pSeleteFx->Late_Update(fTimeDelta);
            Render_ItemInfo();
        }
        m_pIcon->Late_Update(fTimeDelta);
        m_pSmoke_Fx->Late_Update(fTimeDelta);
    }

    if (m_bIsEquip)
    {
        m_pEquipIcon->Late_Update(fTimeDelta);
        if (m_iItemIndex < 0)
            return;

        const ITEM_DATA* pData = CClientInstance::GetInstance()->Get_Data<ITEM_DATA>(m_iItemIndex);

        if (m_iItemIndex > 0 && pData->iType > 3 && !m_isSale)
        {
            CItemInfo_Weapon::WEAPONINFO_DESC Desc = {};
            Desc.iItemIndex = m_iItemIndex;
            Desc.iOffsetPos = { 1230.f, 595.f };
            Desc.isEquip = m_bIsEquip;
            Desc.iEffect_Type = m_iRandomEffect_Type;
            Desc.iEffect_Value = m_iRandomEffect_Value;

            CClientInstance::GetInstance()->UI_UpdateSwitch(TEXT("ItemInfo_Equip"), &Desc);

        }
    }
    if (m_iItemIndex >= 0 && m_pTextBox != nullptr)
    {
        m_pTextBox->Set_Text(to_wstring(m_iItemCount));
        m_pTextBox->Late_Update(fTimeDelta);
    }
    if(m_pUpIcon != nullptr && m_iItemIndex >= 0 && m_isUpIcon)
        m_pUpIcon->Late_Update(fTimeDelta);
}

HRESULT CItem_Slot::Ready_Prototype()
{
    CHECK_FAILED(m_pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_UI_Item_Icon"),
        CItem_Slot::Create(m_pDevice, m_pContext, m_iLevel)), E_FAIL);

    return S_OK;
}

HRESULT CItem_Slot::Ready_Children()
{
    CUI_Atlas_Icon::UIATLASICON_DESC AtlasDesc;

    AtlasDesc.fDepth = m_fDepth - 1;
    AtlasDesc.iUIType = ENUM_CLASS(UITYPE::TEXTURE);
    AtlasDesc.szName = "Item_Over";
    AtlasDesc.vLocalPos = _float2{ 0.f, 0.f };
    AtlasDesc.vLocalSize = { 130.f, 130.f };

    AtlasDesc.vUV = CClientInstance::GetInstance()->Get_AtlasUV("T_Slot_Inven_Hover.png", 1);
    AtlasDesc.iShaderPass = 3;
    AtlasDesc.iTexPass = 1;
    AtlasDesc.vColor = { 1.f, 1.f, 1.f, 1.f };
    m_pOverFx = static_cast<CUI_Atlas_Icon*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Atlas_Icon"), &AtlasDesc));

    if (m_pOverFx == nullptr)
        return E_FAIL; 
    m_Children.push_back(m_pOverFx);
    Safe_AddRef(m_pOverFx);
    
    //장착
    if (m_iItemType <= ENUM_CLASS(CUI_Inven::ITEMTYPE::ATIVE))
    {
        AtlasDesc.fDepth = m_fDepth - 0.5;
        AtlasDesc.iUIType = ENUM_CLASS(UITYPE::TEXTURE);
        AtlasDesc.szName = "Item_Equip";
        AtlasDesc.vLocalPos = _float2{ -30.f, -30.f };

        if (m_iItemType < ENUM_CLASS(CUI_Inven::ITEMTYPE::ATIVE))
        {
            AtlasDesc.vLocalSize = { 40.f, 40.f };
            AtlasDesc.vUV = CClientInstance::GetInstance()->Get_AtlasUV("T_Icon_Inven_Equipped.png", 1);
        }
        else
        {
            AtlasDesc.vLocalSize = { 28.f, 28.f };
            AtlasDesc.vUV = CClientInstance::GetInstance()->Get_AtlasUV("T_Icon_Inven_Equipped_Alpha.png", 1);
        }
        AtlasDesc.iShaderPass = 0;
        AtlasDesc.iTexPass = 1;
        AtlasDesc.vColor = { 1.f, 1.f, 1.f, 1.f };
        m_pEquipIcon = static_cast<CUI_Atlas_Icon*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Atlas_Icon"), &AtlasDesc));

        if (m_pEquipIcon == nullptr)
            return E_FAIL;

        m_Children.push_back(m_pEquipIcon);
        Safe_AddRef(m_pEquipIcon);
    }
    //셀렉트
    AtlasDesc.fDepth = m_fDepth - 2;
    AtlasDesc.iUIType = ENUM_CLASS(UITYPE::TEXTURE);
    AtlasDesc.szName = "Item_Selet";
    AtlasDesc.vLocalPos = _float2{ 0.f, 0.f };
    AtlasDesc.vLocalSize = { 131.f, 131.f };

    AtlasDesc.vUV = CClientInstance::GetInstance()->Get_AtlasUV("T_Slot_Inven_Select.png", 1);
    AtlasDesc.iShaderPass = 3;
    AtlasDesc.iTexPass = 1;
    AtlasDesc.vColor = { 1.f, 0.7f, 0.6f, 1.f };
    m_pSeleteFx = static_cast<CUI_Atlas_Icon*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Atlas_Icon"), &AtlasDesc));

    if (m_pSeleteFx == nullptr)
        return E_FAIL;

    m_Children.push_back(m_pSeleteFx);
    Safe_AddRef(m_pSeleteFx);

    AtlasDesc.fDepth = m_fDepth - 1;
    AtlasDesc.iUIType = ENUM_CLASS(UITYPE::TEXTURE);
    AtlasDesc.szName = "Item_Icon";
    AtlasDesc.vLocalPos = _float2{ 0.f, 0.f };
    AtlasDesc.vLocalSize = { 93.f, 93.f };

    AtlasDesc.vUV = { 0.f, 0.f, 1.f, 1.f };
    AtlasDesc.iShaderPass = 0;
    AtlasDesc.iTexPass = 2;
    AtlasDesc.vColor = { 1.f, 1.f, 1.f, 1.f };
    m_pIcon = static_cast<CUI_Atlas_Icon*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Atlas_Icon"), &AtlasDesc));

    if (m_pIcon == nullptr)
        return E_FAIL;

    m_Children.push_back(m_pIcon);
    Safe_AddRef(m_pIcon);

    if (m_iItemType == ENUM_CLASS(CUI_Inven::ITEMTYPE::ATIVE) || m_iItemType == ENUM_CLASS(CUI_Inven::ITEMTYPE::MATERIAL))
    {
        CUIObject::UIOBJECT_DESC TextDesc = {};
        TextDesc.fDepth = m_fDepth - 1.f;
        TextDesc.iUIType = ENUM_CLASS(UITYPE::TEXT);
        TextDesc.szName = "Item_Count";
        TextDesc.vLocalPos = _float2{ 0.f, 0.f };
        TextDesc.vLocalSize = { m_vLocalSize.x, m_vLocalSize.y };
        TextDesc.vColor = { 1.f, 1.f, 1.f, 1.f };
        m_pTextBox = static_cast<CUI_TextBox*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_TextBox"), &TextDesc));

        if (m_pTextBox == nullptr)
            return E_FAIL;

        CUI_TextBox::TEXTBOX_DESC TextSet = {};
        TextSet.bIsTextBox = false;
        TextSet.eTextAlign = TEXT_ALIGN::RIGHT_BOTTOM;
        TextSet.fMaxWidth = 0;
        TextSet.fOffsetHeight = 0;
        TextSet.iPivotX = 40;
        TextSet.iPivotY = 50;
        TextSet.wstrTexttag = TEXT("Blade_Medium_18");
        TextSet.wstrText = TEXT("");
        TextSet.vColor = { 1.f, 1.f, 1.f, 1.f };
        m_pTextBox->Setting_Text(TextSet);
        m_Children.push_back(m_pTextBox);
        Safe_AddRef(m_pTextBox);
    }

    if (m_iItemType >= ENUM_CLASS(CUI_Inven::ITEMTYPE::SPEAR) && m_iItemType <= ENUM_CLASS(CUI_Inven::ITEMTYPE::RING))
    {
        AtlasDesc.fDepth = m_fDepth - 0.5;
        AtlasDesc.iUIType = ENUM_CLASS(UITYPE::TEXTURE);
        AtlasDesc.szName = "Item_Up";
        AtlasDesc.vLocalPos = _float2{ 32.f, 35.f };

        AtlasDesc.vLocalSize = { 24.f, 16.f };
        AtlasDesc.vUV = CClientInstance::GetInstance()->Get_AtlasUV("T_Icon_MetaStat_Arrow.png", 1);

        AtlasDesc.iShaderPass = 2;
        AtlasDesc.iTexPass = 1;
        AtlasDesc.vColor = { 0.302f, 0.584f, 1.f, 1.f };
        m_pUpIcon = static_cast<CUI_Atlas_Icon*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Atlas_Icon"), &AtlasDesc));

        if (m_pUpIcon == nullptr)
            return E_FAIL;

        m_Children.push_back(m_pUpIcon);
        Safe_AddRef(m_pUpIcon);
    }

    CUIObject::UIOBJECT_DESC TextDesc = {};
    TextDesc.fDepth = 2.9f;
    TextDesc.iUIType = ENUM_CLASS(UITYPE::TEXT);
    TextDesc.szName = "Smoke";
    TextDesc.vLocalPos = _float2{ 0.f, 0.f };
    TextDesc.vLocalSize = { 95.f,95.f };
    TextDesc.vColor = { 1.f, 1.f, 1.f, 1.f };
    m_pSmoke_Fx = static_cast<CUI_Slot_Smoke*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_Slot_Smoke"), &TextDesc));

    if (m_pSmoke_Fx == nullptr)
        return E_FAIL;
    
    m_Children.push_back(m_pSmoke_Fx);
    Safe_AddRef(m_pSmoke_Fx);

    return S_OK;
}

void CItem_Slot::Update_State(_uint iGrade)
{
    if (m_iItemIndex < 0)
    {
        m_iState = ENUM_CLASS(UISTATE::DISABLE);
        m_vColor.w = 0.75f;
        return;
    }
    else
    {
        m_iState = ENUM_CLASS(UISTATE::ENABLE);
        m_vColor.w = 1.f;
    }
    _float4 vUv = {};

    switch (iGrade)
    {
    case 0: m_vUV[ENUM_CLASS(UISTATE::ENABLE)] = CClientInstance::GetInstance()->Get_AtlasUV("T_Slot_Inven_02_Common.png", m_iTexPass);
        return;
    case 1: m_vUV[ENUM_CLASS(UISTATE::ENABLE)] = CClientInstance::GetInstance()->Get_AtlasUV("T_Slot_Inven_03_UnCommon.png", m_iTexPass);
        return;
    case 2: m_vUV[ENUM_CLASS(UISTATE::ENABLE)] = CClientInstance::GetInstance()->Get_AtlasUV("T_Slot_Inven_04_Rare.png", m_iTexPass);
        return;
    case 3: m_vUV[ENUM_CLASS(UISTATE::ENABLE)] = CClientInstance::GetInstance()->Get_AtlasUV("T_Slot_Inven_05_Unique.png", m_iTexPass);
        return;
    case 4: m_vUV[ENUM_CLASS(UISTATE::ENABLE)] = CClientInstance::GetInstance()->Get_AtlasUV("T_Slot_Inven_06_Legendary.png", m_iTexPass);
        return;
    case 5: m_vUV[ENUM_CLASS(UISTATE::ENABLE)] = CClientInstance::GetInstance()->Get_AtlasUV("T_Slot_Inven_07_Epic.png", m_iTexPass);
        return;
    }

}

void CItem_Slot::Selete_Item()
{
    m_bIsSelete = true;

    CUI_Inven::INVENBUBBLE_DESC Desc = {};
    Desc.eBubbleType = CUI_Inven::EVENT_TYPE::ITEM_SELETE;
    Desc.iTypeIndex = m_iItemType;
    Desc.iIndex = m_iIndex;

    __super::Bubble_EventCall(&Desc);
}

void CItem_Slot::Equip_Item()
{
    if (m_iItemType > ENUM_CLASS(CUI_Inven::ITEMTYPE::ATIVE))
        return;
    
    CUI_Inven::INVENBUBBLE_DESC Desc = {};
    
    if (!m_bIsEquip)
    {
        Desc.eBubbleType = CUI_Inven::EVENT_TYPE::ITEM_EQUIP;
        Desc.iTypeIndex = m_iItemType;
        Desc.iIndex = m_iIndex;
        Desc.iItemIndex = m_iItemIndex;
        Desc.pItem = this;

    }
    else
    {
        m_bIsEquip = false;
        Desc.eBubbleType = CUI_Inven::EVENT_TYPE::ITEM_UNEQUIP;
        Desc.iTypeIndex = m_iItemType;
        Desc.iIndex = m_iIndex;
        Desc.iItemIndex = m_iItemIndex;
        Desc.pItem = this;

    }
    __super::Bubble_EventCall(&Desc);
}

void CItem_Slot::Sale_Item()
{
    const ITEM_DATA* pData = CClientInstance::GetInstance()->Get_Data<ITEM_DATA>(m_iItemIndex);
    if(pData->iGold > 0)
        static_cast<CAmount*>(CClientInstance::GetInstance()->Get_RootUI(TEXT("Amount")))->Add_Value(CAmount::AMOUNT_TYPE::GOLD, pData->iGold * m_iItemCount, false);
    if (pData->iLachryma > 0)
        static_cast<CAmount*>(CClientInstance::GetInstance()->Get_RootUI(TEXT("Amount")))->Add_Value(CAmount::AMOUNT_TYPE::LACHRYMA, pData->iLachryma * m_iItemCount, false);
    Release_Item();

}

void CItem_Slot::Release_Item()
{
    m_iItemIndex = -1;
    m_iItemMaxCount = 0;
    m_iItemCount = 0;
    m_bIsSelete = false;
    m_bIsEquip = false;

    Update_State();

    CUI_Inven::INVENBUBBLE_DESC Desc = {};
    Desc.eBubbleType = CUI_Inven::EVENT_TYPE::ITEM_RELEASE;
    Desc.iIndex = m_iIndex;
    Desc.pItem = this;
   
    __super::Bubble_EventCall(&Desc);
}

void CItem_Slot::Render_ItemInfo()
{
    if (m_iItemIndex < 0)
        return;

    const ITEM_DATA* pData = CClientInstance::GetInstance()->Get_Data<ITEM_DATA>(m_iItemIndex);

    if (pData->iType <= 3)
    {
        CItemInfo_Other::OTHERINFO_DESC Desc = {};
        Desc.iItemIndex = m_iItemIndex;
        Desc.iOffsetPos = { 780.f, 511.f };
        Desc.isEquip = m_bIsEquip;
        Desc.iMaxItem = m_iItemMaxCount;
        Desc.iCurItem = m_iItemCount;
        CClientInstance::GetInstance()->UI_UpdateSwitch(TEXT("ItemInfo_Other"), &Desc);

    }
    else
    {
        CItemInfo_Weapon::WEAPONINFO_DESC Desc = {};
        Desc.iItemIndex = m_iItemIndex;
        Desc.iOffsetPos = { 780.f, 595.f };
        Desc.isEquip = m_bIsEquip;
        Desc.iEffect_Type = m_iRandomEffect_Type;
        Desc.iEffect_Value = m_iRandomEffect_Value;
        CClientInstance::GetInstance()->UI_UpdateSwitch(TEXT("ItemInfo_Weapon"), &Desc);
    }
}

void CItem_Slot::Random_Effect_Setting(_int iItemType)
{
    if (iItemType == 4)
        m_iRandomEffect_Type = m_pGameInstance->Rand(1, 3);
    else if (iItemType == 5)
        m_iRandomEffect_Type = 2 + m_pGameInstance->Rand(1, 5);
    else if (iItemType == 6)
        m_iRandomEffect_Type = 5 + m_pGameInstance->Rand(1, 2);

    if (m_iRandomEffect_Type == 1)
        m_iRandomEffect_Value = m_pGameInstance->Rand(5, 10);
    else if (m_iRandomEffect_Type == 2 || m_iRandomEffect_Type == 3)
        m_iRandomEffect_Value = m_pGameInstance->Rand(1, 3);
    else if (m_iRandomEffect_Type == 4)
        m_iRandomEffect_Value = m_pGameInstance->Rand(50, 150);
    else if (m_iRandomEffect_Type == 5)
        m_iRandomEffect_Value = m_pGameInstance->Rand(10, 20);
    else if (m_iRandomEffect_Type == 6)
        m_iRandomEffect_Value = m_pGameInstance->Rand(50, 300);
    else if (m_iRandomEffect_Type == 7)
        m_iRandomEffect_Value = 1;
}

CItem_Slot* CItem_Slot::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
    CItem_Slot* pInstance = new CItem_Slot(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype(iLevel)))
    {
        MSG_BOX(TEXT("Failed Created : CItem_Slot"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CItem_Slot::Clone(void* pArg)
{
    CItem_Slot* pInstance = new CItem_Slot(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed Cloned : CItem_Slot"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CItem_Slot::Free()
{
    __super::Free();
    Safe_Release(m_pEquipIcon);
    Safe_Release(m_pOverFx);
    Safe_Release(m_pSeleteFx);
    Safe_Release(m_pIcon);
    Safe_Release(m_pTextBox);
    Safe_Release(m_pUpIcon);
    Safe_Release(m_pSmoke_Fx);
}
