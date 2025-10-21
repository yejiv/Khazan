#include "UI_QuickSlot_Item.h"
#include "GameInstance.h"
#include "ClientInstance.h"

#include "UI_Atlas_Icon.h"

CUI_QuickSlot_Item::CUI_QuickSlot_Item(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Slot{ pDevice, pContext }
{
}

CUI_QuickSlot_Item::CUI_QuickSlot_Item(const CUI_QuickSlot_Item& Prototype)
    : CUI_Slot(Prototype)
{
}

void CUI_QuickSlot_Item::Input_Slot()
{
    if (m_iState == ENUM_CLASS(QUICKITMESLOTSTATE::DISABLE))
    {
        m_vFxColor.w = 1.f;
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
    return S_OK;
}

void CUI_QuickSlot_Item::Priority_Update(_float fTimeDelta)
{
}

void CUI_QuickSlot_Item::Update(_float fTimeDelta)
{
    //Test
    if (m_pGameInstance->Key_Pressing(DIK_1, 0))
    {
        m_iState = ENUM_CLASS(QUICKITMESLOTSTATE::NONITEM);
        m_vColor.w = 0.5f;
    }
    else if (m_pGameInstance->Key_Pressing(DIK_2, 0))
    {
        m_iState = ENUM_CLASS(QUICKITMESLOTSTATE::DISABLE);
        m_vColor.w = 1.f;
        m_vFxColor.w = 0.5f;
        m_pDisableFX->Update_Color(m_vFxColor);
    }
    else if (m_pGameInstance->Key_Pressing(DIK_3, 0))
    {
        m_iState = ENUM_CLASS(QUICKITMESLOTSTATE::ENABLE);
        m_vColor.w = 1.f;
    }
    else if (m_pGameInstance->Key_Down(DIK_Q) && m_iState == ENUM_CLASS(QUICKITMESLOTSTATE::DISABLE))
    {
        m_vFxColor.w = 1.f;
    }
 
}

void CUI_QuickSlot_Item::Late_Update(_float fTimeDelta)
{
    CClientInstance::GetInstance()->Add_UIRender(UI_RENDER_TYPE::ATLAS, this);
    if (m_iState == ENUM_CLASS(QUICKITMESLOTSTATE::DISABLE))
        Update_DisableFX(fTimeDelta);
}

HRESULT CUI_QuickSlot_Item::Render()
{
    return S_OK;
}

HRESULT CUI_QuickSlot_Item::Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg)
{
    FAILED_CHECK_RETURN(__super::Load_UI(pInData, iPrototypeLevelID, pArg), E_FAIL);
    FAILED_CHECK_RETURN(Ready_Childer(), E_FAIL);

    Update_State();
    return S_OK;
}

HRESULT CUI_QuickSlot_Item::Ready_Prototype()
{
    return S_OK;
}

HRESULT CUI_QuickSlot_Item::Ready_Childer()
{
    
    CUI_Atlas_Icon::UIATLASICON_DESC Desc;
    Desc.fDepth = m_fDepth;
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
    return S_OK;
}

void CUI_QuickSlot_Item::Update_State()
{
    if (m_iItemIndex < 0)
    {
        m_iState = ENUM_CLASS(QUICKITMESLOTSTATE::NONITEM);
        m_vColor.w = 0.5f;
    }
    else if (m_iItemValue = 0)
    {
        m_iState = ENUM_CLASS(QUICKITMESLOTSTATE::DISABLE);
        m_vColor.w = 1.f;
        m_vFxColor.w = 0.5f;
        m_pDisableFX->Update_Color(m_vFxColor);
    }
    else
    {
        m_iState = ENUM_CLASS(QUICKITMESLOTSTATE::ENABLE);
        m_vColor.w = 1.f;
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
    Safe_Release(m_pDisableFX);
    __super::Free();
}
