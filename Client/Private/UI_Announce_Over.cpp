#include "UI_Announce_Over.h"
#include "GameInstance.h"
#include "ClientInstance.h"

#include "UI_Announce_Symbol.h"
#include "UI_BackGround.h"
#include "UI_Default_Tex.h"

CUI_Announce_Over::CUI_Announce_Over(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Panel(pDevice, pContext)
{
}

CUI_Announce_Over::CUI_Announce_Over(const CUI_Announce_Over& Prototype)
    : CUI_Panel(Prototype)
{
}

HRESULT CUI_Announce_Over::Initialize_Prototype(_int iLevel)
{
    m_iLevel = iLevel;
    CHECK_FAILED(Ready_Prototype(), E_FAIL);
    return S_OK;
}

HRESULT CUI_Announce_Over::Initialize_Clone(void* pArg)
{
    __super::Initialize_Clone(pArg);
    CHECK_FAILED(Ready_Children(), E_FAIL);

    Update_Transform(nullptr, m_vLocalPos);

    m_pGameInstance->Subscribe_Event<EVENT_ANNOUNCE_RESULT>(ENUM_CLASS(EVENT_TYPE::ANNOUNCE_OVER), [&](const EVENT_ANNOUNCE_RESULT& e)
        { Start_Anim(); });

    return S_OK;
}

void CUI_Announce_Over::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CUI_Announce_Over::Update(_float fTimeDelta)
{
    if (m_eAnimState == UIANIMSTATE::END)
        return;
    else if (m_eAnimState == UIANIMSTATE::START)
    {
        m_fAcctime += fTimeDelta;

        if (m_fAcctime > 1.f)
        {
            m_fAcctime = 1.f;
            m_fDelaytime = 3.f;
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

void CUI_Announce_Over::Late_Update(_float fTimeDelta)
{
    if (m_eAnimState == UIANIMSTATE::END)
        return;
    __super::Late_Update(fTimeDelta);
}

HRESULT CUI_Announce_Over::Render()
{
    return S_OK;
}

HRESULT CUI_Announce_Over::Ready_Prototype()
{
    CHECK_FAILED(m_pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_Component_Tex_Annouce_Result_Over"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Announce/T_Hud_Over_%d.png"), 2)), E_FAIL);

    return S_OK;
}

HRESULT CUI_Announce_Over::Ready_Children()
{
    CUIObject::UIOBJECT_DESC Desc = {};

    Desc.fDepth = 2.3f;
    Desc.iUIType = ENUM_CLASS(UITYPE::TEXTURE);
    Desc.szName = "BackGround";
    Desc.vLocalSize = { g_iWinSizeX, g_iWinSizeY };
    Desc.vLocalPos = { 0,0 };

    m_pBackGround = static_cast<CUI_BackGround*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_BackGround"), &Desc));
    if (m_pBackGround == nullptr)
        return E_FAIL;
    m_pBackGround->Setting_BG(CUI_BackGround::UIBGTYPE::CIRCLE);
    m_pBackGround->Set_Color({ 0.0f, 0.0f, 0.0f, 1.f });
    m_Children.push_back(m_pBackGround);
    Safe_AddRef(m_pBackGround);

    Desc.vLocalSize = { 1024.f, 150.f };
    Desc.vLocalPos = { 0, 0 };
    Desc.iUIType = ENUM_CLASS(UITYPE::TEXTURE);
    Desc.szName = "MainText";
    Desc.fDepth = 1.7f;

    m_pMainText = static_cast<CUI_Default_Tex*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_Default_Tex"), &Desc));
    if (!m_pMainText)
        return E_FAIL;

    m_Children.push_back(m_pMainText);
    Safe_AddRef(m_pMainText);

    m_pMainText->Set_Texture(TEXT("Prototype_Component_Tex_Annouce_Result_Over"), 0);
    m_pMainText->Set_ShaderPass(1);
    m_pMainText->Set_Color({ 1.f, 1.f, 1.f, 1.f });

    return S_OK;
}

void CUI_Announce_Over::Start_Anim()
{
    m_eAnimState = UIANIMSTATE::START;
    m_fAcctime = 0.f;
    m_fAlpha = 1.f;

}

CUI_Announce_Over* CUI_Announce_Over::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _int iLevel)
{
    CUI_Announce_Over* pInstance = new CUI_Announce_Over(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype(iLevel)))
    {
        MSG_BOX(TEXT("Failed Created : CUI_Announce_Over"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_Announce_Over::Clone(void* pArg)
{
    CUI_Announce_Over* pInstance = new CUI_Announce_Over(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed Cloned : CUI_Announce_Over"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CUI_Announce_Over::Free()
{
    __super::Free();

    Safe_Release(m_pBackGround);
    Safe_Release(m_pMainText);
}
