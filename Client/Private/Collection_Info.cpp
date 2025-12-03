#include "Collection_Info.h"
#include "ClientInstance.h"
#include "GameInstance.h"

#include "UI_TextBox.h"
#include "UI_Atlas_Icon.h"
#include "UI_BackGround.h"

CCollection_Info::CCollection_Info(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Panel{ pDevice, pContext }
{
}

CCollection_Info::CCollection_Info(const CCollection_Info& Prototype)
    : CUI_Panel(Prototype)
{
}

HRESULT CCollection_Info::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCollection_Info::Initialize_Clone(void* pArg)
{
    CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);
    CHECK_FAILED(Ready_Object(), E_FAIL);

    return S_OK;
}

void CCollection_Info::Priority_Update(_float fTimeDelta)
{
    if (m_isChangeInputType)
    {
        m_pGameInstance->Change_InputType(INPUT_TYPE::UI);
        m_isChangeInputType = false;
    }
}

void CCollection_Info::Update(_float fTimeDelta)
{
    if (!m_IsUpdate)
        return;

    if (m_isOnStart)
    {
        m_isOnStart = false;
        return;
    }

    else if (m_pGameInstance->Key_Down(DIK_ESCAPE, INPUT_TYPE::POPUP))
    {
        m_IsUpdate = false;
        m_isChangeInputType = true;
        m_pGameInstance->PlaySoundOnce(TEXT("UI_inherit_open (SFX).wav"));
    }
}

void CCollection_Info::Late_Update(_float fTimeDelta)
{
    if (!m_IsUpdate)
        return;
    m_pBackGround->Setting_BG(CUI_BackGround::UIBGTYPE::DEFAULT);
    __super::Late_Update(fTimeDelta);
}

HRESULT CCollection_Info::Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg)
{
    __super::Load_UI(pInData, iPrototypeLevelID, pArg);

    for (auto pChild : m_Children)
    {
        string strName = pChild->Get_Name();
        if (strName == "Item_Icon")
        {
            m_pAtlasIcon = static_cast<CUI_Atlas_Icon*>(pChild);
            Safe_AddRef(m_pAtlasIcon);
        }
        else if (strName == "Item_Name")
        {
            m_pItemName = static_cast<CUI_TextBox*>(pChild);
            Safe_AddRef(m_pItemName);
        }
        else if (strName == "Item_Info")
        {
            m_pItemInfo = static_cast<CUI_TextBox*>(pChild);
            Safe_AddRef(m_pItemInfo);
        }
        else if (strName == "Icon_Bg")
        {
            pChild->Set_ShaderPass(21);
        }
    }

    return S_OK;
}

HRESULT CCollection_Info::Update_Switch(void* pArg)
{
    COLLECTIONINFO_DESC* pDesc = static_cast<COLLECTIONINFO_DESC*>(pArg);

    const ITEM_DATA* pData = CClientInstance::GetInstance()->Get_Data<ITEM_DATA>(pDesc->iItemIndex);
    
    m_pAtlasIcon->Set_Texture(CClientInstance::GetInstance()->Get_AtlasUV(WStringToAnsi(pData->strIconName), 2), 2);
    m_pItemName->Set_Text(pData->strName);
    m_pItemInfo->Set_Text(CClientInstance::GetInstance()->Get_Data<OTHERITEM_DATA>(pData->iEffect_ID)->strText);

    m_IsUpdate = true;
    m_pGameInstance->Change_InputType(INPUT_TYPE::POPUP);
    m_isOnStart = true;

    m_pGameInstance->PlaySoundOnce(TEXT("UI_inherit_close (SFX).wav"));
    return S_OK;
}


HRESULT CCollection_Info::Ready_Object()
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
    m_pBackGround->Setting_BG(CUI_BackGround::UIBGTYPE::STATE);
    m_pBackGround->Set_Color({ 0.0f, 0.0f, 0.0f, 0.9f });
    m_Children.push_back(m_pBackGround);
    Safe_AddRef(m_pBackGround);

    return S_OK;
}

CCollection_Info* CCollection_Info::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CCollection_Info* pInstance = new CCollection_Info(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed Created : CCollection_Info"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CCollection_Info::Clone(void* pArg)
{
    CCollection_Info* pInstance = new CCollection_Info(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed Cloned : CCollection_Info"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CCollection_Info::Free()
{
    __super::Free();

    Safe_Release(m_pBackGround);
    Safe_Release(m_pAtlasIcon);
    Safe_Release(m_pItemName);
    Safe_Release(m_pItemInfo);
}
