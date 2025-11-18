#include "UI_Slot_Selete_Fx.h"
#include "GameInstance.h"
#include "ClientInstance.h"

CUI_Slot_Selete_Fx::CUI_Slot_Selete_Fx(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Panel{ pDevice, pContext }
{
}

CUI_Slot_Selete_Fx::CUI_Slot_Selete_Fx(const CUI_Slot_Selete_Fx& Prototype)
    : CUI_Panel(Prototype)
{
}

HRESULT CUI_Slot_Selete_Fx::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_Slot_Selete_Fx::Initialize_Clone(void* pArg)
{
    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Component()))
        return E_FAIL;

    m_vColor = { 0.55f, 0.8f, 1.f, 1.f };
    return S_OK;
}
void CUI_Slot_Selete_Fx::Priority_Update(_float fTimeDelta)
{

}

void CUI_Slot_Selete_Fx::Update(_float fTimeDelta)
{

}

void CUI_Slot_Selete_Fx::Late_Update(_float fTimeDelta)
{
    m_fAccTime += fTimeDelta;
    if (m_fAccTime > 1000.f) 
        m_fAccTime -= 1000.f;

    if (m_isVisible)
        CClientInstance::GetInstance()->Add_UIRender(UI_RENDER_TYPE::DEFAULT, this);
}

HRESULT CUI_Slot_Selete_Fx::Render()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    if (FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_Texture", 6)))
        return E_FAIL;

    CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float)), E_FAIL);
    CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_float4)), E_FAIL);
    CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_fValue", &m_fAccTime, sizeof(_float)), E_FAIL);

    m_pShaderCom->Begin(9);
    m_pVIBufferCom->Bind_Resources();
    m_pVIBufferCom->Render();

    return S_OK;
}

HRESULT CUI_Slot_Selete_Fx::Ready_Component()
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex_UI_Effect"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_UI_FX_Mask"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
        return E_FAIL;

    return S_OK;
}

CUI_Slot_Selete_Fx* CUI_Slot_Selete_Fx::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUI_Slot_Selete_Fx* pInstance = new CUI_Slot_Selete_Fx(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed Created : CUI_Slot_Selete_Fx"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_Slot_Selete_Fx::Clone(void* pArg)
{
    CUI_Slot_Selete_Fx* pInstance = new CUI_Slot_Selete_Fx(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed Cloned : CUI_Slot_Selete_Fx"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CUI_Slot_Selete_Fx::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);

}
