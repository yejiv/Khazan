#include "UI_Announce_Symbol.h"
#include "GameInstance.h"
#include "ClientInstance.h"

#include "UI_Default_Tex.h"


CUI_Announce_Symbol::CUI_Announce_Symbol(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Panel(pDevice, pContext)
{
}

CUI_Announce_Symbol::CUI_Announce_Symbol(const CUI_Announce_Symbol& Prototype)
    : CUI_Panel(Prototype)
{
}

void CUI_Announce_Symbol::Set_Dissovle(_bool isDissovel)
{
    if (isDissovel == m_isDissovle)
        return;

    m_isDissovle = isDissovel;

    if (!m_isDissovle)
        m_fDissolveTime = 0.f;
}

void CUI_Announce_Symbol::Set_SubTexScalling(_bool isScaling)
{
    if (m_isScaling == isScaling)
        return;
    m_isScaling = isScaling;

    if (m_isScaling)
    {
        m_pSubText->Update_Visible(true);
    }
    else
    {
        m_pSubText->Update_Visible(false);
        m_fScalingSize = 0.93f;
        m_fSetTexAlpha = 0.6f;
        m_pSubText->Set_Color({ 1.f, 1.f, 1.f,m_fSetTexAlpha });
    }

}

void CUI_Announce_Symbol::Set_Elite(_bool isElite)
{
    m_isElite = isElite;
    if (m_isElite)
    {
        m_pMainText->Set_TexPass(2);
        m_pSubText->Set_TexPass(3);
    }
    else
    {
        m_pMainText->Set_TexPass(0);
        m_pSubText->Set_TexPass(1);

    }
}

HRESULT CUI_Announce_Symbol::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_Announce_Symbol::Initialize_Clone(void* pArg)
{
    __super::Initialize_Clone(pArg);
    CHECK_FAILED(Ready_Component(), E_FAIL);

    CHECK_FAILED(Ready_Children(), E_FAIL);
    m_iShaderPass = 1;
    m_fDissolveTime = 0.f;
    m_fAlpha = 1.f;
    Update_Transform(nullptr, m_vLocalPos);
    m_vColor = { 1.f, 1.f, 1.f, 1.f };
    m_fScalingSize = 0.95f;
    m_fSetTexAlpha = 0.6f;

    return S_OK;
}

void CUI_Announce_Symbol::Priority_Update(_float fTimeDelta)
{
}

void CUI_Announce_Symbol::Update(_float fTimeDelta)
{
}

void CUI_Announce_Symbol::Late_Update(_float fTimeDelta)
{
    if (m_isDissovle && m_fDissolveTime < 1.f)
    {
        m_fDissolveTime += fTimeDelta * 0.5f;
    }

    if (m_isScaling && m_fScalingSize < 1.2f)
    {
        m_fScalingSize += fTimeDelta * 0.04f;
        if (m_fScalingSize >= 1.2f)
        {
            m_fScalingSize = 1.2f;
        }
    }

    if (m_fScalingSize >= 1.05f)
    {
        m_fSetTexAlpha -= fTimeDelta;
        m_pSubText->Set_Color({ 1.f, 1.f, 1.f,m_fSetTexAlpha });
    }
    m_pSubText->Tex_Scaling(m_fScalingSize, 1.f);
    if(!m_isElite)
        CClientInstance::GetInstance()->Add_UIRender(UI_RENDER_TYPE::DEFAULT, this);
    __super::Late_Update(fTimeDelta);
}

HRESULT CUI_Announce_Symbol::Render()
{
    CHECK_FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix), E_FAIL);
    CHECK_FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix), E_FAIL);
    CHECK_FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);

    CHECK_FAILED(m_pTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_Texture", m_iTexPass), E_FAIL);
    
    CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float)), E_FAIL);
    CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_float4)), E_FAIL);

    CHECK_FAILED(m_pDissolveTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_MaskTexture", 0), E_FAIL);
    CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_fMaskValue", &m_fDissolveTime, sizeof(_float)), E_FAIL);

    m_pShaderCom->Begin(8);
    m_pVIBufferCom->Bind_Resources();
    m_pVIBufferCom->Render();
    return S_OK;
}

HRESULT CUI_Announce_Symbol::Ready_Component()
{
    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex_UI_Mask"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr), E_FAIL);

    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_UI_FX_Mask"),
        TEXT("Com_DissoveTex"), reinterpret_cast<CComponent**>(&m_pDissolveTextureCom), nullptr), E_FAIL);

    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Tex_Annouce_Icon"),
        TEXT("Com_Tex"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr), E_FAIL);

    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuuferCom"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr), E_FAIL);

    return S_OK;
}

HRESULT CUI_Announce_Symbol::Ready_Children()
{
    CUIObject::UIOBJECT_DESC Desc = {};
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

    m_pMainText->Set_Texture(TEXT("Prototype_Component_Tex_Annouce_Result_Text"), 0);
    m_pMainText->Set_ShaderPass(1);
    m_pMainText->Set_Color({ 1.f, 1.f, 1.f, 1.f });

    Desc.vLocalSize = { 1024.f, 150.f };
    Desc.vLocalPos = { 0, 0 };
    Desc.iUIType = ENUM_CLASS(UITYPE::TEXTURE);
    Desc.szName = "MainText";
    Desc.fDepth = 1.8f;

    m_pSubText = static_cast<CUI_Default_Tex*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_Default_Tex"), &Desc));
    if (!m_pSubText)
        return E_FAIL;

    m_Children.push_back(m_pSubText);
    Safe_AddRef(m_pSubText);

    m_pSubText->Set_Texture(TEXT("Prototype_Component_Tex_Annouce_Result_Text"), 1);
    m_pSubText->Set_ShaderPass(1);
    m_pSubText->Set_Color({ 1.f, 1.f, 1.f, 0.6f });
    m_pSubText->Update_Visible(false);
    
    return S_OK;
}

CUI_Announce_Symbol* CUI_Announce_Symbol::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUI_Announce_Symbol* pInstance = new CUI_Announce_Symbol(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed Created : CUI_Announce_Symbol"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_Announce_Symbol::Clone(void* pArg)
{
    CUI_Announce_Symbol* pInstance = new CUI_Announce_Symbol(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed Cloned : CUI_Announce_Symbol"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CUI_Announce_Symbol::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pDissolveTextureCom);
    Safe_Release(m_pVIBufferCom);

    Safe_Release(m_pMainText);
    Safe_Release(m_pSubText);
}
