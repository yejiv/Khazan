#include "UI_State_Button.h"
#include "GameInstance.h"
#include "ClientInstance.h"

CUI_State_Button::CUI_State_Button(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Button{ pDevice, pContext }
{
}

CUI_State_Button::CUI_State_Button(const CUI_State_Button& Prototype)
    : CUI_Button(Prototype)
{
}

HRESULT CUI_State_Button::Initialize_Prototype(_uint iLevel)
{
    m_iLevel = iLevel;
    return S_OK;

}

HRESULT CUI_State_Button::Initialize_Clone(void* pArg)
{
    CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);
    CHECK_FAILED(Ready_Component(), E_FAIL);
    return S_OK;
}

void CUI_State_Button::Priority_Update(_float fTimeDelta)
{

}

void CUI_State_Button::Update(_float fTimeDelta)
{

    if (ButtonOver(g_hWnd))
        m_vColor.w = 1.f;
    else
        m_vColor.w = 0.7f;

    if (ButtonClick(g_hWnd, false, true))
    {
        CUI_State_List::STATE_LIST_BUBBLE Desc = {};
        Desc.eType = m_eType;
        Bubble_EventCall(&Desc);
        m_pGameInstance->PlaySoundOnce(TEXT("UI_item_select (SFX).wav"));
    }
}

void CUI_State_Button::Late_Update(_float fTimeDelta)
{
    CClientInstance::GetInstance()->Add_UIRender(UI_RENDER_TYPE::DEFAULT, this);
}

HRESULT CUI_State_Button::Render()
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
    CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_float4)), E_FAIL);

    m_pShaderCom->Begin(m_iShaderPass);
    m_pVIBufferCom->Bind_Resources();
    m_pVIBufferCom->Render();

    return S_OK;
}

HRESULT CUI_State_Button::Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg)
{
    string strTexTag = pInData.value("TexTag", "");
    wstring wstrTexTag = AnsiToWString(strTexTag);

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), wstrTexTag.c_str(),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
        return E_FAIL;

    if (FAILED(__super::Load_UI(pInData, iPrototypeLevelID, pArg)))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_State_Button::Ready_Component()
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex_UI"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
        return E_FAIL;

    return S_OK;
}

CUI_State_Button* CUI_State_Button::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
    CUI_State_Button* pInstance = new CUI_State_Button(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype(iLevel)))
    {
        MSG_BOX(TEXT("Failed Created : CUI_State_Button"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_State_Button::Clone(void* pArg)
{
    CUI_State_Button* pInstance = new CUI_State_Button(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed Cloned : CUI_State_Button"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CUI_State_Button::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);

}
