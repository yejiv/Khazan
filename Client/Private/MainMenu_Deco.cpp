#include "MainMenu_Deco.h"
#include "GameInstance.h"
#include "ClientInstance.h"

CMainMune_Deco::CMainMune_Deco(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Texture{ pDevice, pContext }
{
}

CMainMune_Deco::CMainMune_Deco(const CMainMune_Deco& Prototype)
    : CUI_Texture(Prototype)
{
}

HRESULT CMainMune_Deco::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CMainMune_Deco::Initialize_Clone(void* pArg)
{
    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Component()))
        return E_FAIL;

    return S_OK;
}

void CMainMune_Deco::Priority_Update(_float fTimeDelta)
{
}

void CMainMune_Deco::Update(_float fTimeDelta)
{
}

void CMainMune_Deco::Late_Update(_float fTimeDelta)
{
    CClientInstance::GetInstance()->Add_UIRender(UI_RENDER_TYPE::DEFAULT, this);
}

HRESULT CMainMune_Deco::Render()
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

    m_pShaderCom->Begin(1);
    m_pVIBufferCom->Bind_Resources();
    m_pVIBufferCom->Render();

    return S_OK;
}

HRESULT CMainMune_Deco::Ready_Component()
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex_UI"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_UI_Common_MenuList"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
        return E_FAIL;

    return S_OK;
}

CMainMune_Deco* CMainMune_Deco::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CMainMune_Deco* pInstance = new CMainMune_Deco(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed Created : CMainMune_Deco"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CMainMune_Deco::Clone(void* pArg)
{
    CMainMune_Deco* pInstance = new CMainMune_Deco(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed Cloned : CMainMune_Deco"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CMainMune_Deco::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
}
