#include "UI_QuickSlot_Item.h"
#include "GameInstance.h"
#include "ClientInstance.h"

#include "UI_Atlas_Icon.h"
#include "UI_TextBox.h"
CUI_QuickSlot_Item::CUI_QuickSlot_Item(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Slot{ pDevice, pContext }
{
}

CUI_QuickSlot_Item::CUI_QuickSlot_Item(const CUI_QuickSlot_Item& Prototype)
    : CUI_Slot(Prototype)
{
}

void CUI_QuickSlot_Item::Set_index(_int iIndex)
{
    m_iIndex = iIndex;

    if (m_iIndex == 0)
    {
        m_iState = ENUM_CLASS(QUICKITMESLOTSTATE::ENABLE);
        m_pIcon->Set_Texture(CClientInstance::GetInstance()->Get_AtlasUV("T_Item_Potion_HP_1.png", 2), 2);
        Update_State();
    }
}

HRESULT CUI_QuickSlot_Item::Initialize_Prototype(_uint iLevel)
{
    m_iLevel = iLevel;

    return S_OK;
}

HRESULT CUI_QuickSlot_Item::Initialize_Clone(void* pArg)
{
    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    m_iState = ENUM_CLASS(QUICKITMESLOTSTATE::NONITEM);

    m_pGameInstance->Subscribe_Event<EVENT_HUD_QUICKSLOT>(ENUM_CLASS(EVENT_TYPE::UI_QUICK_SLOT), [&](const EVENT_HUD_QUICKSLOT& e) {Add_Item(e); });

    return S_OK;
}

void CUI_QuickSlot_Item::Priority_Update(_float fTimeDelta)
{
}

void CUI_QuickSlot_Item::Update(_float fTimeDelta)
{
    Input_KeyState();

    if (m_iItemCount != nullptr && *m_iItemCount > 0 && m_bIsItemZero)
    {
        m_bIsItemZero = false;
        Update_State();
    }
}

void CUI_QuickSlot_Item::Late_Update(_float fTimeDelta)
{
    CClientInstance::GetInstance()->Add_UIRender(UI_RENDER_TYPE::ATLAS, this);
    if (m_iState == ENUM_CLASS(QUICKITMESLOTSTATE::DISABLE))
        Update_DisableFX(fTimeDelta);

    if (m_iState != ENUM_CLASS(QUICKITMESLOTSTATE::NONITEM))
    {
        m_pIcon->Late_Update(fTimeDelta);
        m_pTextBox->Set_Text(to_wstring(*m_iItemCount));
        m_pTextBox->Late_Update(fTimeDelta);
    }

}

HRESULT CUI_QuickSlot_Item::Render()
{
    return S_OK;
}

HRESULT CUI_QuickSlot_Item::Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg)
{
    FAILED_CHECK_RETURN(__super::Load_UI(pInData, iPrototypeLevelID, pArg), E_FAIL);
    FAILED_CHECK_RETURN(Ready_Children(), E_FAIL);

    Update_State();
    return S_OK;
}

HRESULT CUI_QuickSlot_Item::Ready_Prototype()
{
    return S_OK;
}

HRESULT CUI_QuickSlot_Item::Ready_Children()
{
    
    CUI_Atlas_Icon::UIATLASICON_DESC AtlasDesc;

    AtlasDesc.fDepth = 7;
    AtlasDesc.iUIType = ENUM_CLASS(UITYPE::TEXTURE);
    AtlasDesc.szName = "Item_Icon";
    AtlasDesc.vLocalPos = _float2{ 0.f, 0.f };
    if (m_szName == "HUD_Item_Slot_0")
        AtlasDesc.vLocalSize = { m_vLocalSize.x * 0.9f, m_vLocalSize.y * 0.9f };
    else
        AtlasDesc.vLocalSize = { m_vLocalSize.x * 0.7f, m_vLocalSize.y * 0.7f };

    AtlasDesc.vUV = { 0.f, 0.f, 1.f, 1.f };
    AtlasDesc.iShaderPass = 0;
    AtlasDesc.iTexPass = 2;
    AtlasDesc.vColor = { 1.f, 1.f, 1.f, 1.f };
    m_pIcon = static_cast<CUI_Atlas_Icon*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Atlas_Icon"), &AtlasDesc));

    if (m_pIcon == nullptr)
        return E_FAIL;

    m_Children.push_back(m_pIcon);
    Safe_AddRef(m_pIcon);

    CUI_Atlas_Icon::UIATLASICON_DESC Desc;
    Desc.fDepth = 6;
    Desc.iUIType = ENUM_CLASS(UITYPE::TEXTURE);
    Desc.szName = "Quick_Slot_Item_Disable";
    Desc.vLocalPos = _float2{ 0.f, 0.f };
    Desc.vLocalSize = m_vLocalSize;

    Desc.vUV = CClientInstance::GetInstance()->Get_AtlasUV("T_Hud_Skill_Bg_Circle_Disable.png", 0);
    Desc.iShaderPass = 2;
    Desc.iTexPass = 0;
    Desc.vColor = {1.f, 0.15f, 0.15f, 0.5f};
    m_vFxColor = { 1.f, 0.15f, 0.15f, 0.5f };
    m_pDisableFX = static_cast<CUI_Atlas_Icon*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Atlas_Icon"), &Desc));

    if (m_pDisableFX == nullptr)
        return E_FAIL;

    m_Children.push_back(m_pDisableFX);
    Safe_AddRef(m_pDisableFX);

    CUIObject::UIOBJECT_DESC TextDesc = {};
    TextDesc.fDepth = 6.9f;
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

    if (m_szName == "HUD_Item_Slot_0")
    {
        TextSet.iPivotX = 30;
        TextSet.iPivotY = 45;
        TextSet.wstrTexttag = TEXT("Blade_Medium_22");
    }
    else
    {
        TextSet.iPivotX = 20;
        TextSet.iPivotY = 35;
        TextSet.wstrTexttag = TEXT("Blade_Medium_18");
    }
    TextSet.wstrText = TEXT("");
    TextSet.vColor = { 1.f, 1.f, 1.f, 1.f };
    m_pTextBox->Setting_Text(TextSet);
    m_Children.push_back(m_pTextBox);
    Safe_AddRef(m_pTextBox);
    return S_OK;
}

void CUI_QuickSlot_Item::Update_State()
{
    
    
    if (m_iItemIndex < 0 && m_iIndex != 0)
    {
        m_iState = ENUM_CLASS(QUICKITMESLOTSTATE::NONITEM);
        m_vColor.w = 0.5f;
    }
    else if (m_iItemCount != nullptr && (*m_iItemCount) == 0)
    {
        m_iState = ENUM_CLASS(QUICKITMESLOTSTATE::DISABLE);
        m_vColor.w = 1.f;
        m_vFxColor.w = 0.5f;
        m_pDisableFX->Update_Color(m_vFxColor);
        m_pIcon->Set_Color(_float4{1.f, 1.f, 1.f, 0.5f});
        m_pTextBox->Set_Color({ 1.f, 1.f,1.f,0.8f });
    }
    else
    {
        m_iState = ENUM_CLASS(QUICKITMESLOTSTATE::ENABLE);
        m_vColor.w = 1.f;
        m_pIcon->Set_Color(_float4{ 1.f, 1.f, 1.f, 1.f });
        m_pTextBox->Set_Color({ 1.f, 1.f,1.f,1.f });    
    }
}

void CUI_QuickSlot_Item::Update_DisableFX(_float fTimeDelta)
{
    if (m_vFxColor.w > 0.5f)
    {
        m_vFxColor.w -= fTimeDelta;
        m_pDisableFX->Update_Color(m_vFxColor);
        if (m_vFxColor.w <= 0.5f)
            m_vFxColor.w = 0.5f;
    }
    m_pDisableFX->Late_Update(fTimeDelta);
}

void CUI_QuickSlot_Item::Add_Item(EVENT_HUD_QUICKSLOT pItem)
{
    if (pItem.iIndex != m_iIndex)
        return;
    else if (0 == pItem.iIndex)
    {
        m_iItemCount = pItem.iItemCount;
        return;
    }


    if (!pItem.isEquip)
    {
        m_iState = ENUM_CLASS(QUICKITMESLOTSTATE::NONITEM);
        m_iItemIndex = -1;
        m_iItemCount = nullptr;
        Update_State();
    }
    else
    {
        m_iItemIndex = pItem.iItemIndex;
        m_iItemCount = pItem.iItemCount;

        ITEM_DATA ItemData = *CClientInstance::GetInstance()->Get_Data<ITEM_DATA>(m_iItemIndex);

        string strItemData = WStringToAnsi(ItemData.strIconName);
        _float4 vUV = CClientInstance::GetInstance()->Get_AtlasUV(strItemData.c_str(), ItemData.iTexPass);

        m_pIcon->Set_Texture(vUV, ItemData.iTexPass);
        Update_State();
    }
}

void CUI_QuickSlot_Item::Input_KeyState()
{
    if(m_iState == ENUM_CLASS(QUICKITMESLOTSTATE::NONITEM))
        return;

    _int iInputIndex = { -1 };

    if (m_pGameInstance->Key_Down(DIK_1))
        iInputIndex = 0;
    if (m_pGameInstance->Key_Down(DIK_2))
        iInputIndex = 1;
    if (m_pGameInstance->Key_Down(DIK_3))
        iInputIndex = 2;
    if (m_pGameInstance->Key_Down(DIK_4))
        iInputIndex = 3;
    if (m_pGameInstance->Key_Down(DIK_5))
        iInputIndex = 4;
    if (m_pGameInstance->Key_Down(DIK_6))
        iInputIndex = 5;
    if (m_pGameInstance->Key_Down(DIK_7))
        iInputIndex = 6;

    if (m_iIndex != iInputIndex)
        return;

    if (m_iItemCount != nullptr)
    {
        _bool isAticv = false;

        if(*m_iItemCount > 0)
            isAticv = true;

        if (!isAticv)
            m_vFxColor.w = 1.f;
        else
        {
            if (iInputIndex == 0)
            {
                m_pGameInstance->StopByKey(TEXT("UI_item_consume_assassinportion_01 (SFX).wav"));
                m_pGameInstance->PlaySoundOnce(TEXT("UI_item_consume_assassinportion_01 (SFX).wav"));
            }
            else if(m_iItemIndex >= 1001 && m_iItemIndex <= 1004)
            {
                _bool isAticv = CClientInstance::GetInstance()->Get_PlayerData().fCulHp < CClientInstance::GetInstance()->Get_PlayerData().fMaxHp;
                if (!isAticv)
                    return;

                --*m_iItemCount;
                if (*m_iItemCount <= 0)
                {
                    Update_State();
                    m_bIsItemZero = true;
                }
                if (m_iItemIndex == 1001 || m_iItemIndex == 1002)
                {
                    m_pGameInstance->StopByKey(TEXT("UI_item_tearsummons_01 (SFX).wav"));
                    m_pGameInstance->PlaySoundOnce(TEXT("UI_item_tearsummons_01 (SFX).wav"));
                }
                else
                {
                    m_pGameInstance->StopByKey(TEXT("UI_item_use_01 (SFX).wav"));
                    m_pGameInstance->PlaySoundOnce(TEXT("UI_item_use_01 (SFX).wav"));
                }

            }

            
        }
    }
}


CUI_QuickSlot_Item* CUI_QuickSlot_Item::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
    CUI_QuickSlot_Item* pInstance = new CUI_QuickSlot_Item(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype(iLevel)))
    {
        MSG_BOX(TEXT("Failed Created : CUI_QuickSlot_Item"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_QuickSlot_Item::Clone(void* pArg)
{
    CUI_QuickSlot_Item* pInstance = new CUI_QuickSlot_Item(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed Cloned : CUI_QuickSlot_Item"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CUI_QuickSlot_Item::Free()
{
    Safe_Release(m_pIcon);
    Safe_Release(m_pDisableFX);
    Safe_Release(m_pTextBox);
    __super::Free();
}
