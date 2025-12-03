#include "UI_Announce_Warning.h"
#include "GameInstance.h"
#include "ClientInstance.h"

#include "UI_TextBox.h"
#include "UI_Default_Tex.h"

CUI_Annouce_Warning::CUI_Annouce_Warning(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Panel(pDevice, pContext)
{
}

CUI_Annouce_Warning::CUI_Annouce_Warning(const CUI_Annouce_Warning& Prototype)
    : CUI_Panel(Prototype)
{
}

HRESULT CUI_Annouce_Warning::Initialize_Prototype(_int iLevel)
{
    m_iLevel = iLevel;
    return S_OK;
}

HRESULT CUI_Annouce_Warning::Initialize_Clone(void* pArg)
{
    __super::Initialize_Clone(pArg);
    CHECK_FAILED(Ready_Children(), E_FAIL);

    Update_Transform(nullptr, m_vLocalPos);

    m_pGameInstance->Subscribe_Event<EVENT_ANNOUNCE_WARNING>(ENUM_CLASS(EVENT_TYPE::ANNOUNCE_WARNING), [&](const EVENT_ANNOUNCE_WARNING& e)
        { Start_Anim(e); });

    return S_OK;
}

void CUI_Annouce_Warning::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CUI_Annouce_Warning::Update(_float fTimeDelta)
{

    if (m_eAnimState == UIANIMSTATE::END)
        return;
    else if (m_eAnimState == UIANIMSTATE::START)
    {
        m_fAcctime += fTimeDelta;

        if (m_fAcctime > 1.f)
        {
            m_fAcctime = 1.f;
            m_fDelaytime = 2.f;
            m_eAnimState = UIANIMSTATE::EFFECT_1;

        }
        Update_Alpha(m_fAcctime);
    }
    else if (m_eAnimState == UIANIMSTATE::EFFECT_1)
    {
        m_fDelaytime -= fTimeDelta;

        if (m_fDelaytime < 0.f)
        {
            m_fAlpha = 1.f;
            m_eAnimState = UIANIMSTATE::FINASH;
        }
    }
    else if (m_eAnimState == UIANIMSTATE::FINASH)
    {
        m_fAcctime -= fTimeDelta;
        if (m_fAcctime < 0.f)
        {
            m_fAcctime = 1.f;
            m_eAnimState = UIANIMSTATE::END;
        }
        Update_Alpha(m_fAcctime);
    }
    __super::Update(fTimeDelta);
}

void CUI_Annouce_Warning::Late_Update(_float fTimeDelta)
{
    if (m_eAnimState == UIANIMSTATE::END)
        return;
    __super::Late_Update(fTimeDelta);
}

HRESULT CUI_Annouce_Warning::Ready_Children()
{
    CUIObject::UIOBJECT_DESC Desc = {};

    Desc.vLocalSize = { 375.f, 55.f };
    Desc.vLocalPos = { 0.f, 180.f };
    Desc.iUIType = ENUM_CLASS(UITYPE::TEXTURE);
    Desc.szName = "MainText";
    Desc.fDepth = 1.f;

    m_pBG = static_cast<CUI_Default_Tex*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_Default_Tex"), &Desc));
    if (!m_pBG)
        return E_FAIL;

    m_Children.push_back(m_pBG);
    Safe_AddRef(m_pBG);

    m_pBG->Set_Texture(TEXT("Prototype_Component_UI_Common_MenuList"), 1);
    m_pBG->Set_ShaderPass(22);
    m_pBG->Set_Color({ 0.20f, 0.27f, 0.33f, 0.6f });

    CUIObject::UIOBJECT_DESC TextDesc = {};
    TextDesc.fDepth = 0.9f;
    TextDesc.iUIType = ENUM_CLASS(UITYPE::TEXT);
    TextDesc.szName = "Text";
    Desc.vLocalSize = { 375.f, 55.f };
    Desc.vLocalPos = { 0.f, 180.f };
    TextDesc.vColor = { 1.f, 1.f, 1.f, 1.f };
    m_pTextBox = static_cast<CUI_TextBox*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_TextBox"), &TextDesc));

    if (m_pTextBox == nullptr)
        return E_FAIL;

    CUI_TextBox::TEXTBOX_DESC TextSet = {};
    TextSet.bIsTextBox = false;
    TextSet.eTextAlign = TEXT_ALIGN::CENTER;
    TextSet.fMaxWidth = 0;
    TextSet.fOffsetHeight = 0;
    TextSet.iPivotX = 0;
    TextSet.iPivotY = 195;
    TextSet.wstrTexttag = TEXT("Blade_Medium_18");
    TextSet.wstrText = TEXT("");
    TextSet.vColor = { 1.f, 1.f, 1.f, 1.f };
    m_pTextBox->Setting_Text(TextSet);
    m_Children.push_back(m_pTextBox);
    Safe_AddRef(m_pTextBox);

    return S_OK;
}

void CUI_Annouce_Warning::Start_Anim(const EVENT_ANNOUNCE_WARNING& e)
{
    m_eAnimState = UIANIMSTATE::START;
    m_fAcctime = 0.f;
    m_fAlpha = 1.f;

    m_pTextBox->Set_Text(e.strText);

}

CUI_Annouce_Warning* CUI_Annouce_Warning::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _int iLevel)
{
    CUI_Annouce_Warning* pInstance = new CUI_Annouce_Warning(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype(iLevel)))
    {
        MSG_BOX(TEXT("Failed Created : CUI_Annouce_Warning"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_Annouce_Warning::Clone(void* pArg)
{
    CUI_Annouce_Warning* pInstance = new CUI_Annouce_Warning(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed Cloned : CUI_Annouce_Warning"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CUI_Annouce_Warning::Free()
{
    __super::Free();

    Safe_Release(m_pBG);
    Safe_Release(m_pTextBox);
}
