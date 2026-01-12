#include "Equip_Slot.h"
#include "GameInstance.h"
#include "ClientInstance.h"

#include "UI_Atlas_Icon.h"
#include "UI_Inven.h"
#include "Item_Slot.h"
#include "UI_TextBox.h"

#include "ItemInfo_Other.h"
#include "ItemInfo_Weapon.h"

CEquip_Slot::CEquip_Slot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Slot{ pDevice, pContext }
{
}

CEquip_Slot::CEquip_Slot(const CEquip_Slot& Prototype)
    : CUI_Slot(Prototype)
{
}

_bool CEquip_Slot::Add_Item(_int iItemIndex, CItem_Slot* pItem, _bool isSetting)
{
    if (m_pItem_Slot == pItem)
    {
        return true;
    }
    else if (iItemIndex >= 0)
    {
        if (m_pItem_Slot != nullptr)
        {
            const ITEM_DATA* pData = CClientInstance::GetInstance()->Get_Data<ITEM_DATA>(m_iItemIndex);

            if (pData->iType > 3)
            {
                EQUIPITEM_DATA pEquipData = *CClientInstance::GetInstance()->Get_Data<EQUIPITEM_DATA>(pData->iEffect_ID);
                Setting_UnEquipData(pEquipData);
            }
            m_pItem_Slot->is_Equip(false);
            Safe_Release(m_pItem_Slot);
        }
        m_iItemIndex = iItemIndex;
        ITEM_DATA ItemData = *CClientInstance::GetInstance()->Get_Data<ITEM_DATA>(m_iItemIndex);

        string strItemData = WStringToAnsi(ItemData.strIconName);
        _float4 vUV = CClientInstance::GetInstance()->Get_AtlasUV(strItemData.c_str(), ItemData.iTexPass);
        _uint iTexPass = ItemData.iTexPass;

        m_pIcon->Set_Texture(vUV, iTexPass);
        Update_State(ItemData.iGrade);

        _int iQuickSlotIndex = m_iIndex - ENUM_CLASS(CUI_Inven::EQUIPSLOT_TYPE::QUICK_1) + 1;
        m_pItem_Slot = pItem;
        m_pItem_Slot->is_Equip(true, iQuickSlotIndex);
        Safe_AddRef(m_pItem_Slot);
        if (ENUM_CLASS(CUI_Inven::EQUIPSLOT_TYPE::QUICK_1) <= m_iIndex)
        {
            EVENT_HUD_QUICKSLOT EventDesc = {};
            EventDesc.isEquip = true;
            EventDesc.iItemIndex = m_iItemIndex;
            EventDesc.iIndex = iQuickSlotIndex;
            EventDesc.iItemCount = m_pItem_Slot->Get_ptrItemCount();
            m_pGameInstance->Emit_Event<EVENT_HUD_QUICKSLOT>(ENUM_CLASS(EVENT_TYPE::UI_QUICK_SLOT), EventDesc);
        }
        else
        {
            EQUIPITEM_DATA pEquipData = *CClientInstance::GetInstance()->Get_Data<EQUIPITEM_DATA>(ItemData.iEffect_ID);
            Setting_EquipData(pEquipData);
            if (isSetting)
                if (pEquipData.iType >= ENUM_CLASS(EQUIPMENTTYPE::SPEAR) && pEquipData.iType <= ENUM_CLASS(EQUIPMENTTYPE::SHOES))
                    CClientInstance::GetInstance()->Change_PlayerEquipment((EQUIPMENTTYPE)pEquipData.iType, m_iItemIndex);
        }
        return true;
    }
    return false;
}

void CEquip_Slot::Release_Item(CItem_Slot* pItem)
{
    if (m_pItem_Slot == nullptr || pItem == nullptr)
        return;

    if (m_pItem_Slot != pItem)
        return;

    ITEM_DATA ItemData = *CClientInstance::GetInstance()->Get_Data<ITEM_DATA>(m_iItemIndex);
    m_iItemIndex = -1;

    Update_State(0);

    if (ENUM_CLASS(CUI_Inven::EQUIPSLOT_TYPE::QUICK_1) <= m_iIndex)
    {
        _int iQuickSlotIndex = m_iIndex - ENUM_CLASS(CUI_Inven::EQUIPSLOT_TYPE::QUICK_1) + 1;

        EVENT_HUD_QUICKSLOT EventDesc = {};
        EventDesc.isEquip = false;
        EventDesc.iIndex = iQuickSlotIndex;

        m_pGameInstance->Emit_Event<EVENT_HUD_QUICKSLOT>(ENUM_CLASS(EVENT_TYPE::UI_QUICK_SLOT), EventDesc);
    }
    else
    {
        EQUIPITEM_DATA pEquipData = *CClientInstance::GetInstance()->Get_Data<EQUIPITEM_DATA>(ItemData.iEffect_ID);
        Setting_UnEquipData(pEquipData);

        if (pEquipData.iType == ENUM_CLASS(EQUIPMENTTYPE::HEAD))
            CClientInstance::GetInstance()->Change_PlayerEquipment((EQUIPMENTTYPE)pEquipData.iType, 21);
        else if (pEquipData.iType == ENUM_CLASS(EQUIPMENTTYPE::TORSO))
            CClientInstance::GetInstance()->Change_PlayerEquipment((EQUIPMENTTYPE)pEquipData.iType, 30);
        else if (pEquipData.iType == ENUM_CLASS(EQUIPMENTTYPE::ARM))
            CClientInstance::GetInstance()->Change_PlayerEquipment((EQUIPMENTTYPE)pEquipData.iType, 40);
        else if (pEquipData.iType == ENUM_CLASS(EQUIPMENTTYPE::LEG))
            CClientInstance::GetInstance()->Change_PlayerEquipment((EQUIPMENTTYPE)pEquipData.iType, 50);
        else if (pEquipData.iType == ENUM_CLASS(EQUIPMENTTYPE::SHOES))
            CClientInstance::GetInstance()->Change_PlayerEquipment((EQUIPMENTTYPE)pEquipData.iType, 60);
    }
    Safe_Release(m_pItem_Slot);
    m_pItem_Slot = nullptr;
}

void CEquip_Slot::Update_PosX(_int iIndex, _float2 vPos, _float fOffSetX, _float fOffSetY, CUIObject* pParent)
{
    m_vLocalPos.x = vPos.x + iIndex * fOffSetX;
    m_vLocalPos.y = vPos.y + fOffSetY;

    __super::Update_Transform(pParent, m_vWorldPos);
}

_bool CEquip_Slot::Off_Selete()
{
    if (m_iItemIndex < 0)
        return false;
    m_bIsSelete = false;
    return true;
}

HRESULT CEquip_Slot::Initialize_Prototype(_uint iLevel)
{
    m_iLevel = iLevel;

    return S_OK;
}

HRESULT CEquip_Slot::Initialize_Clone(void* pArg)
{
    UISLOTDESC* pDesc = static_cast<UISLOTDESC*>(pArg);
    m_iIndex = pDesc->iIndex;
    m_iTexPass = 1;
    m_iShaderPass = 0;

    m_vColor = { 1.f,1.f,1.f,0.75f };
    m_iSouleCount = 3;

    _float4 vUV = CClientInstance::GetInstance()->Get_AtlasUV("T_Slot_Inven_02_Common.png", m_iTexPass);
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

void CEquip_Slot::Priority_Update(_float fTimeDelta)
{
}

void CEquip_Slot::Update(_float fTimeDelta)
{

}

void CEquip_Slot::Late_Update(_float fTimeDelta)
{
    if(m_iIndex != ENUM_CLASS(CUI_Inven::EQUIPSLOT_TYPE::SOUL))
        CClientInstance::GetInstance()->Add_UIRender(UI_RENDER_TYPE::ATLAS, this);

    if (ButtonClick(g_hWnd, false, true))
    {
        m_pGameInstance->PlaySoundOnce(TEXT("UI_common_click2 (SFX).wav"));

        CUI_Inven::INVENBUBBLE_DESC Desc = {};
        Desc.eBubbleType = CUI_Inven::EVENT_TYPE::SLOT_EQUIP;
        Desc.iIndex = m_iIndex;
        if (m_iIndex == ENUM_CLASS(CUI_Inven::EQUIPSLOT_TYPE::WEAPON) && m_iItemIndex >= 0)
        {
            _int iID = CClientInstance::GetInstance()->Get_Data<ITEM_DATA>(m_iItemIndex)->iEffect_ID;
            Desc.iItemType = CClientInstance::GetInstance()->Get_Data <EQUIPITEM_DATA>(iID)->iType;
        }
        __super::Bubble_EventCall(&Desc);
    }
    m_pIcon->Late_Update(fTimeDelta);
    if (ButtonOver(g_hWnd) && m_iIndex != ENUM_CLASS(CUI_Inven::EQUIPSLOT_TYPE::SOUL))
    {
        if (!m_isOver)
        {
            m_isOver = true;
            _int iRand = m_pGameInstance->Rand(1, 4);
            _wstring wstrSound = TEXT("UI_common_mouse_over_0") + std::to_wstring(iRand) + TEXT(" (SFX).wav");
            m_pGameInstance->PlaySoundOnce(wstrSound.c_str());
        }
        m_pSeleteFx->Late_Update(fTimeDelta);
        Render_ItemInfo();
    }
    else
        m_isOver = false;

    if (m_iIndex == ENUM_CLASS(CUI_Inven::EQUIPSLOT_TYPE::SOUL))
    {
        m_pTextBox->Set_Text(to_wstring(m_iSouleCount));
        m_pTextBox->Late_Update(fTimeDelta);
    }

    if (m_pItem_Slot != nullptr && m_pTextBox != nullptr && m_iItemIndex >= 0)
    {
        m_pTextBox->Set_Text(to_wstring(m_pItem_Slot->Get_ItemCount()));
        m_pTextBox->Late_Update(fTimeDelta);
    }
}

HRESULT CEquip_Slot::Ready_Prototype()
{
    CHECK_FAILED(m_pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_UI_Item_Icon"),
        CEquip_Slot::Create(m_pDevice, m_pContext, m_iLevel)), E_FAIL);

    return S_OK;
}

HRESULT CEquip_Slot::Ready_Children()
{
    CUI_Atlas_Icon::UIATLASICON_DESC AtlasDesc;

    AtlasDesc.fDepth = m_fDepth - 1;
    AtlasDesc.iUIType = ENUM_CLASS(UITYPE::TEXTURE);
    AtlasDesc.szName = "Item_Selet";
    AtlasDesc.vLocalPos = _float2{ 0.f, 0.f };
    if (m_iIndex >= ENUM_CLASS(CUI_Inven::EQUIPSLOT_TYPE::QUICK_1) && m_iIndex <= ENUM_CLASS(CUI_Inven::EQUIPSLOT_TYPE::QUICK_6))
        AtlasDesc.vLocalSize = { 111.f, 111.f };
    else
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

    AtlasDesc.fDepth = m_fDepth - 2;
    AtlasDesc.iUIType = ENUM_CLASS(UITYPE::TEXTURE);
    AtlasDesc.szName = "Item_Icon";
    AtlasDesc.vLocalPos = _float2{ 0.f, 0.f };
    AtlasDesc.vLocalSize = { m_vLocalSize.x - 10.f, m_vLocalSize.y - 10.f };

    AtlasDesc.vUV = { 0.f, 0.f, 1.f, 1.f };
    AtlasDesc.iShaderPass = 0;
    AtlasDesc.iTexPass = 2;
    AtlasDesc.vColor = { 1.f, 1.f, 1.f, 1.f };
    m_pIcon = static_cast<CUI_Atlas_Icon*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Atlas_Icon"), &AtlasDesc));

    if (m_pIcon == nullptr)
        return E_FAIL;

    m_Children.push_back(m_pIcon);
    Safe_AddRef(m_pIcon);
   
    if (m_iIndex >= ENUM_CLASS(CUI_Inven::EQUIPSLOT_TYPE::QUICK_1))
    {
        CUIObject::UIOBJECT_DESC TextDesc = {};
        TextDesc.fDepth = m_fDepth - 1.5f;
        TextDesc.iUIType = ENUM_CLASS(UITYPE::TEXT);
        TextDesc.szName = "Item_Count";
        TextDesc.vLocalPos = _float2{ 0.f, 0.f };
        TextDesc.vLocalSize = { m_vLocalSize.x, m_vLocalSize.y};
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
        if(m_iIndex == ENUM_CLASS(CUI_Inven::EQUIPSLOT_TYPE::SOUL))
            TextSet.wstrTexttag = TEXT("Blade_Medium_22");
        else
            TextSet.wstrTexttag = TEXT("Blade_Medium_18");
        TextSet.wstrText = TEXT("");
        TextSet.vColor = { 1.f, 1.f, 1.f, 1.f };
        m_pTextBox->Setting_Text(TextSet);
        m_Children.push_back(m_pTextBox);
        Safe_AddRef(m_pTextBox);
    }
    Update_State(0);

    if (m_iIndex == ENUM_CLASS(CUI_Inven::EQUIPSLOT_TYPE::SOUL))
    {
        EVENT_HUD_QUICKSLOT EventDesc = {};
        EventDesc.isEquip = true;
        EventDesc.iIndex = 0;
        EventDesc.iItemCount = &CClientInstance::GetInstance()->Get_ptrPlayerData().iSouleCount;
        m_pGameInstance->Emit_Event<EVENT_HUD_QUICKSLOT>(ENUM_CLASS(EVENT_TYPE::UI_QUICK_SLOT), EventDesc);
    }
    return S_OK;
}

void CEquip_Slot::Update_State(_uint iGrade)
{
    if (m_iItemIndex < 0)
    {
        _float4 vUV = {};
        CUI_Inven::EQUIPSLOT_TYPE eType = static_cast<CUI_Inven::EQUIPSLOT_TYPE>(m_iIndex);

        _int iTexPass = 1;
        _int iShaderPass = 2;
        if (eType == CUI_Inven::EQUIPSLOT_TYPE::WEAPON)
            vUV = CClientInstance::GetInstance()->Get_AtlasUV("T_Icon_SubTab_Weapon.png", 1);
        else if (eType == CUI_Inven::EQUIPSLOT_TYPE::HEAD)
            vUV = CClientInstance::GetInstance()->Get_AtlasUV("T_Icon_SubTab_ArmorHari.png", 1);
        else if (eType == CUI_Inven::EQUIPSLOT_TYPE::TOP)
            vUV = CClientInstance::GetInstance()->Get_AtlasUV("T_Icon_SubTab_ArmorTorso.png", 1);
        else if (eType == CUI_Inven::EQUIPSLOT_TYPE::GLOVES)
            vUV = CClientInstance::GetInstance()->Get_AtlasUV("T_Icon_SubTab_ArmorArm.png", 1);
        else if (eType == CUI_Inven::EQUIPSLOT_TYPE::BOTTOM)
            vUV = CClientInstance::GetInstance()->Get_AtlasUV("T_Icon_SubTab_ArmorLeg.png", 1);
        else if (eType == CUI_Inven::EQUIPSLOT_TYPE::SHOES)
            vUV = CClientInstance::GetInstance()->Get_AtlasUV("T_Icon_SubTab_ArmorShoes.png", 1);
        else if (eType == CUI_Inven::EQUIPSLOT_TYPE::NECK)
            vUV = CClientInstance::GetInstance()->Get_AtlasUV("T_Icon_SubTab_AccNeck.png", 1);
        else if (eType == CUI_Inven::EQUIPSLOT_TYPE::RING)
            vUV = CClientInstance::GetInstance()->Get_AtlasUV("T_Icon_SubTab_AccRing.png", 1);
        else if (eType == CUI_Inven::EQUIPSLOT_TYPE::SOUL)
        {
            vUV = CClientInstance::GetInstance()->Get_AtlasUV("T_Item_Potion_HP_1.png", 2);
            iTexPass = 2;
            iShaderPass = 1;
        }
        else
            vUV = CClientInstance::GetInstance()->Get_AtlasUV("T_Icon_ShortcutEmptySlot_UI.png", 1);

        m_pIcon->Set_Texture(vUV, iTexPass);
        m_pIcon->Set_Color({ 0.259f, 0.267f, 0.278f, 1.0f });
        m_pIcon->Set_Shader(iShaderPass);
        m_iState = ENUM_CLASS(UISTATE::DISABLE);
        m_vColor.w = 0.75f;
        return;
    }
    else
    {
        m_pIcon->Set_Shader(0);
        m_pIcon->Set_Color({ 1.f, 1.f, 1.f, 1.0f });
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

void CEquip_Slot::Equip_Item()
{
    m_bIsSelete ? m_bIsSelete = false : m_bIsSelete = true;

    CUI_Inven::INVENBUBBLE_DESC Desc = {};
    Desc.eBubbleType = CUI_Inven::EVENT_TYPE::ITEM_EQUIP;
    Desc.iIndex = m_iIndex;

    __super::Bubble_EventCall(&Desc);
}

void CEquip_Slot::Render_ItemInfo()
{
    if (m_iItemIndex < 0)
        return;

    const ITEM_DATA* pData = CClientInstance::GetInstance()->Get_Data<ITEM_DATA>(m_iItemIndex);

    if (pData->iType <= 3)
    {
        CItemInfo_Other::OTHERINFO_DESC Desc = {};
        Desc.iItemIndex = m_iItemIndex;
        Desc.iOffsetPos = { 890.f, 511.f };
        Desc.isEquip = true;
        Desc.iMaxItem = m_pItem_Slot->Get_MaxItemCount();
        Desc.iCurItem = m_pItem_Slot->Get_ItemCount();
        CClientInstance::GetInstance()->UI_UpdateSwitch(TEXT("ItemInfo_Other"), &Desc);

    }
    else
    {
        CItemInfo_Weapon::WEAPONINFO_DESC Desc = {};
        Desc.iItemIndex = m_iItemIndex;
        Desc.iOffsetPos = { 890.f, 595.f };
        Desc.isEquip = true;
        Desc.iEffect_Type = m_pItem_Slot->Get_Random_Type();
        Desc.iEffect_Value = m_pItem_Slot->Get_Random_Value();

        CClientInstance::GetInstance()->UI_UpdateSwitch(TEXT("ItemInfo_Weapon"), &Desc);
    }
}

void CEquip_Slot::Setting_EquipData(EQUIPITEM_DATA pData)
{
    if (pData.iValue_Type_1 == 1)
        CClientInstance::GetInstance()->Get_ptrPlayerData().fDamage += pData.iValue_1;
    else if (pData.iValue_Type_1 == 2)
        CClientInstance::GetInstance()->Get_ptrPlayerData().fGuard += pData.iValue_1;
    else if (pData.iValue_Type_1 == 3)
        CClientInstance::GetInstance()->Get_ptrPlayerData().fMaxHp += pData.iValue_1;
    else if (pData.iValue_Type_1 == 4)
        CClientInstance::GetInstance()->Get_ptrPlayerData().fMaxStamina += pData.iValue_1;

    if (pData.iValue_Type_2 == 1)
        CClientInstance::GetInstance()->Get_ptrPlayerData().fDamage += pData.iValue_2;
    else if (pData.iValue_Type_2 == 2)
        CClientInstance::GetInstance()->Get_ptrPlayerData().fGuard += pData.iValue_2;
    else if (pData.iValue_Type_2 == 3)
        CClientInstance::GetInstance()->Get_ptrPlayerData().fMaxHp += pData.iValue_2;
    else if (pData.iValue_Type_2 == 4)
        CClientInstance::GetInstance()->Get_ptrPlayerData().fMaxStamina += pData.iValue_2;

    if (pData.iValue_Type_3 == 1)
        CClientInstance::GetInstance()->Get_ptrPlayerData().fDamage += pData.iValue_3;
    else if (pData.iValue_Type_3 == 2)
        CClientInstance::GetInstance()->Get_ptrPlayerData().fGuard += pData.iValue_3;
    else if (pData.iValue_Type_3 == 3)
        CClientInstance::GetInstance()->Get_ptrPlayerData().fMaxHp += pData.iValue_3;
    else if (pData.iValue_Type_3 == 4)
        CClientInstance::GetInstance()->Get_ptrPlayerData().fMaxStamina += pData.iValue_3;

    if (pData.iClother_Type == 1)
        CClientInstance::GetInstance()->Get_ptrPlayerData().fWeight += pData.fWeight;

    if (m_pItem_Slot->Get_Random_Type() == 1)
        CClientInstance::GetInstance()->Get_ptrPlayerData().fDamage += m_pItem_Slot->Get_Random_Value();
    else if (m_pItem_Slot->Get_Random_Type() == 2)
        CClientInstance::GetInstance()->Get_ptrPlayerData().fDamage += (_int)((pData.iValue_1 * 0.01f) * m_pItem_Slot->Get_Random_Value());
    else if (m_pItem_Slot->Get_Random_Type() == 3)
        CClientInstance::GetInstance()->Get_ptrPlayerData().fStaminaRegen += m_pItem_Slot->Get_Random_Value();
    else if (m_pItem_Slot->Get_Random_Type() == 4)
        CClientInstance::GetInstance()->Get_ptrPlayerData().fGuard += m_pItem_Slot->Get_Random_Value();
    else if (m_pItem_Slot->Get_Random_Type() == 5)
        CClientInstance::GetInstance()->Get_ptrPlayerData().fMaxStamina += m_pItem_Slot->Get_Random_Value();
    else if (m_pItem_Slot->Get_Random_Type() == 6)
        CClientInstance::GetInstance()->Get_ptrPlayerData().fMaxHp += m_pItem_Slot->Get_Random_Value();
    else if (m_pItem_Slot->Get_Random_Type() == 7)
        CClientInstance::GetInstance()->Get_ptrPlayerData().iMaxDoggednessCount += m_pItem_Slot->Get_Random_Value();
}

void CEquip_Slot::Setting_UnEquipData(EQUIPITEM_DATA pData)
{
    if (pData.iValue_Type_1 == 1)
        CClientInstance::GetInstance()->Get_ptrPlayerData().fDamage -= pData.iValue_1;
    else if (pData.iValue_Type_1 == 2)
        CClientInstance::GetInstance()->Get_ptrPlayerData().fGuard -= pData.iValue_1;
    else if (pData.iValue_Type_1 == 3)
        CClientInstance::GetInstance()->Get_ptrPlayerData().fMaxHp -= pData.iValue_1;
    else if (pData.iValue_Type_1 == 4)
        CClientInstance::GetInstance()->Get_ptrPlayerData().fMaxStamina -= pData.iValue_1;

    if (pData.iValue_Type_2 == 1)
        CClientInstance::GetInstance()->Get_ptrPlayerData().fDamage -= pData.iValue_2;
    else if (pData.iValue_Type_2 == 2)
        CClientInstance::GetInstance()->Get_ptrPlayerData().fGuard -= pData.iValue_2;
    else if (pData.iValue_Type_2 == 3)
        CClientInstance::GetInstance()->Get_ptrPlayerData().fMaxHp -= pData.iValue_2;
    else if (pData.iValue_Type_2 == 4)
        CClientInstance::GetInstance()->Get_ptrPlayerData().fMaxStamina -= pData.iValue_2;

    if (pData.iValue_Type_3 == 1)
        CClientInstance::GetInstance()->Get_ptrPlayerData().fDamage -= pData.iValue_3;
    else if (pData.iValue_Type_3 == 2)
        CClientInstance::GetInstance()->Get_ptrPlayerData().fGuard -= pData.iValue_3;
    else if (pData.iValue_Type_3 == 3)
        CClientInstance::GetInstance()->Get_ptrPlayerData().fMaxHp -= pData.iValue_3;
    else if (pData.iValue_Type_3 == 4)
        CClientInstance::GetInstance()->Get_ptrPlayerData().fMaxStamina -= pData.iValue_3;

    if (pData.iClother_Type == 1)
        CClientInstance::GetInstance()->Get_ptrPlayerData().fWeight -= pData.fWeight;

    if (m_pItem_Slot->Get_Random_Type() == 1)
        CClientInstance::GetInstance()->Get_ptrPlayerData().fDamage -= m_pItem_Slot->Get_Random_Value();
    else if (m_pItem_Slot->Get_Random_Type() == 2)
        CClientInstance::GetInstance()->Get_ptrPlayerData().fDamage -= (_int)((pData.iValue_1 * 0.01f) * m_pItem_Slot->Get_Random_Value());
    else if (m_pItem_Slot->Get_Random_Type() == 3)
        CClientInstance::GetInstance()->Get_ptrPlayerData().fStaminaRegen -= m_pItem_Slot->Get_Random_Value();
    else if (m_pItem_Slot->Get_Random_Type() == 4)
        CClientInstance::GetInstance()->Get_ptrPlayerData().fGuard -= m_pItem_Slot->Get_Random_Value();
    else if (m_pItem_Slot->Get_Random_Type() == 5)
        CClientInstance::GetInstance()->Get_ptrPlayerData().fMaxStamina -= m_pItem_Slot->Get_Random_Value();
    else if (m_pItem_Slot->Get_Random_Type() == 6)
        CClientInstance::GetInstance()->Get_ptrPlayerData().fMaxHp -= m_pItem_Slot->Get_Random_Value();
    else if (m_pItem_Slot->Get_Random_Type() == 7)
        CClientInstance::GetInstance()->Get_ptrPlayerData().iMaxDoggednessCount -= m_pItem_Slot->Get_Random_Value();
}

CEquip_Slot* CEquip_Slot::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
    CEquip_Slot* pInstance = new CEquip_Slot(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype(iLevel)))
    {
        MSG_BOX(TEXT("Failed Created : CEquip_Slot"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CEquip_Slot::Clone(void* pArg)
{
    CEquip_Slot* pInstance = new CEquip_Slot(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed Cloned : CEquip_Slot"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CEquip_Slot::Free()
{
    __super::Free();
    Safe_Release(m_pSeleteFx);
    Safe_Release(m_pIcon);
    Safe_Release(m_pItem_Slot);
    Safe_Release(m_pTextBox);
}
