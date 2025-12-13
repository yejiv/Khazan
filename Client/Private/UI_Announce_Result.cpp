#include "UI_Announce_Result.h"
#include "GameInstance.h"
#include "ClientInstance.h"

#include "UI_Announce_Symbol.h"
#include "UI_BackGround.h"
#include "UI_Default_Tex.h"

CUI_Announce_Result::CUI_Announce_Result(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Panel(pDevice, pContext)
{
}

CUI_Announce_Result::CUI_Announce_Result(const CUI_Announce_Result& Prototype)
    : CUI_Panel(Prototype)
{
}

HRESULT CUI_Announce_Result::Initialize_Prototype(_int iLevel)
{
    m_iLevel = iLevel;
    CHECK_FAILED(Ready_Prototype(), E_FAIL);
    return S_OK;
}

HRESULT CUI_Announce_Result::Initialize_Clone(void* pArg)
{
    __super::Initialize_Clone(pArg);
    CHECK_FAILED(Ready_Children(), E_FAIL);

    Update_Transform(nullptr, m_vLocalPos);

    m_pGameInstance->Subscribe_Event<EVENT_ANNOUNCE_RESULT>(ENUM_CLASS(EVENT_TYPE::ANNOUNCE_RESULT), [&](const EVENT_ANNOUNCE_RESULT& e)
        { Start_Anim(e); });

    return S_OK;
}

void CUI_Announce_Result::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CUI_Announce_Result::Update(_float fTimeDelta)
{
    if (m_eAnimState == UIANIMSTATE::END)
        return;
    else if (m_eAnimState == UIANIMSTATE::START)
    {
        m_fAcctime += fTimeDelta;
        if (m_fAcctime > 0.5f)
        {
            m_pSymbol->Set_Dissovle(true);
           
        }
        if (m_fAcctime > 1.f)
        {
            m_fAcctime = 1.f;
            m_fDelaytime = 4.f;
            m_eAnimState = UIANIMSTATE::EFFECT_1;

        }
        Update_Alpha(m_fAcctime);
    }
    else if (m_eAnimState == UIANIMSTATE::EFFECT_1)
    {
        m_fDelaytime -= fTimeDelta;
        if (m_fDelaytime < 3.5f)
        {
            m_pSymbol->Set_SubTexScalling(true);
        }
        if (m_fDelaytime < 2.5f)
        {
            m_fAlpha = 1.f;
            m_eAnimState = UIANIMSTATE::EFFECT_2;
        }
    }
    else if (m_eAnimState == UIANIMSTATE::EFFECT_2)
    {
        m_fDelaytime -= fTimeDelta;
        m_fAlpha -= fTimeDelta;

        m_pColorBg->Set_Color({ 1.f, 1.f, 1.f, m_fAlpha });
        if (m_fDelaytime < 1.0f)
        {
            m_eAnimState = UIANIMSTATE::EFFECT_3;
        }
    }
    else if (m_eAnimState == UIANIMSTATE::EFFECT_3)
    {
        m_fDelaytime -= fTimeDelta;
        m_fAlpha -= fTimeDelta;

        m_pColorBg->Set_Color({ 1.f, 1.f, 1.f, m_fAlpha });
        if (m_fDelaytime < 0.0f)
        {
            m_fAcctime = 1.f;;
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

void CUI_Announce_Result::Late_Update(_float fTimeDelta)
{
    if (m_eAnimState == UIANIMSTATE::END)
        return;

    __super::Late_Update(fTimeDelta);
}

HRESULT CUI_Announce_Result::Render()
{
    return S_OK;
}

HRESULT CUI_Announce_Result::Ready_Prototype()
{
    CHECK_FAILED(m_pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_Component_Tex_Annouce_Result_Text"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Announce/T_Hud_Result_%d.png"), 4)), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_Component_Tex_Annouce_Icon"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Announce/T_Hud_Deco_Symbol_01.png"), 1)), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_Component_Tex_Annouce_Color_Bg"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Announce/T_Hud_BG_MissionCompleted_%d.png"), 2)), E_FAIL);


    CHECK_FAILED(m_pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Announce_Symbol"),
        CUI_Announce_Symbol::Create(m_pDevice, m_pContext)), E_FAIL);


    return S_OK;
}

HRESULT CUI_Announce_Result::Ready_Children()
{
    CUIObject::UIOBJECT_DESC Desc = {};
    Desc.vLocalSize = { 550.f, 550.f };
    Desc.vLocalPos = { 0, -260 };
    Desc.iUIType = ENUM_CLASS(UITYPE::TEXTURE);
    Desc.szName = "Symbol";
    Desc.fDepth = 2.1f;

    m_pSymbol = static_cast<CUI_Announce_Symbol*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Announce_Symbol"), &Desc));
    if (!m_pSymbol)
        return E_FAIL;

    m_Children.push_back(m_pSymbol);
    Safe_AddRef(m_pSymbol);

    Desc.fDepth = 2.3f;
    Desc.iUIType = ENUM_CLASS(UITYPE::TEXTURE);
    Desc.szName = "BackGround";
    Desc.vLocalSize = { g_iWinSizeX, g_iWinSizeY };
    Desc.vLocalPos = { 0,0 };

    m_pBackGround = static_cast<CUI_BackGround*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_BackGround"), &Desc));
    if (m_pBackGround == nullptr)
        return E_FAIL;
    m_pBackGround->Setting_BG(CUI_BackGround::UIBGTYPE::TOP);
    m_pBackGround->Set_Color({ 0.0f, 0.0f, 0.0f, 1.f });
    m_Children.push_back(m_pBackGround);
    Safe_AddRef(m_pBackGround);

    Desc.vLocalSize = { 2680.f, 1240.f };
    Desc.vLocalPos = { 0,  0};
    Desc.iUIType = ENUM_CLASS(UITYPE::TEXTURE);
    Desc.szName = "MainText";
    Desc.fDepth = 2.2f;

    m_pColorBg = static_cast<CUI_Default_Tex*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_Default_Tex"), &Desc));
    if (!m_pColorBg)
        return E_FAIL;

    m_Children.push_back(m_pColorBg);
    Safe_AddRef(m_pColorBg);

    m_pColorBg->Set_Texture(TEXT("Prototype_Component_Tex_Annouce_Color_Bg"), 1);
    m_pColorBg->Set_ShaderPass(1);
    m_pColorBg->Set_Color({ 1.f, 1.f, 1.f, 1.f });
    return S_OK;
}

void CUI_Announce_Result::Start_Anim(const EVENT_ANNOUNCE_RESULT& e)
{
    m_isElite = e.isElite;
    m_eAnimState = UIANIMSTATE::START;
    m_fAcctime = 0.f;
    m_fAlpha = 1.f;
    m_pColorBg->Set_Color({ 1.f, 1.f, 1.f, m_fAlpha });
    m_pSymbol->Set_Dissovle(false);

    m_pSymbol->Set_SubTexScalling(false);
    m_pSymbol->Set_Elite(m_isElite);
    m_pGameInstance->PlaySoundOnce(TEXT("UI_mission_complete (SFX).wav"));
}

CUI_Announce_Result* CUI_Announce_Result::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _int iLevel)
{
    CUI_Announce_Result* pInstance = new CUI_Announce_Result(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype(iLevel)))
    {
        MSG_BOX(TEXT("Failed Created : CUI_Announce_Result"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_Announce_Result::Clone(void* pArg)
{
    CUI_Announce_Result* pInstance = new CUI_Announce_Result(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed Cloned : CUI_Announce_Result"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CUI_Announce_Result::Free()
{
    __super::Free();

    Safe_Release(m_pSymbol);
    Safe_Release(m_pBackGround);
    Safe_Release(m_pColorBg);
}
