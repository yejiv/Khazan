#include "UI_Slot_Over_Fx.h"
#include "GameInstance.h"
#include "ClientInstance.h"

CUI_Slot_Over_Fx::CUI_Slot_Over_Fx(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Panel{ pDevice, pContext }
{
}

CUI_Slot_Over_Fx::CUI_Slot_Over_Fx(const CUI_Slot_Over_Fx& Prototype)
    : CUI_Panel(Prototype)
{
}

void CUI_Slot_Over_Fx::Anim_On()
{
    if (m_eAnimState == UIANIMSTATE::ON)
        return;

    m_eAnimState = UIANIMSTATE::ON;
    m_fAccTime = 1.f;
}

HRESULT CUI_Slot_Over_Fx::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_Slot_Over_Fx::Initialize_Clone(void* pArg)
{
    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Component()))
        return E_FAIL;

    m_vColor = { 1.f, 1.f, 1.f, 0.7f };
    return S_OK;
}
void CUI_Slot_Over_Fx::Priority_Update(_float fTimeDelta)
{

}

void CUI_Slot_Over_Fx::Update(_float fTimeDelta)
{

    if (m_eAnimState == UIANIMSTATE::ON)
    {
        m_fAccTime -= fTimeDelta;
        if (m_fAccTime <= 0.f)
        {
            m_eAnimState = UIANIMSTATE::OFF;
        }
        m_fAlpha = m_fAccTime;
        Update_Scaling(1.f + ((1.f - m_fAccTime) * 0.5f));
    }
}

void CUI_Slot_Over_Fx::Late_Update(_float fTimeDelta)
{
    if (m_eAnimState == UIANIMSTATE::ON)
        CClientInstance::GetInstance()->Add_UIRender(UI_RENDER_TYPE::DEFAULT, this);
}

HRESULT CUI_Slot_Over_Fx::Render()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    if (FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_Texture", 3)))
        return E_FAIL;

    CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float)), E_FAIL);
    CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_float4)), E_FAIL);
    CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_fValue", &m_fAccTime, sizeof(_float)), E_FAIL);

    m_pShaderCom->Begin(8);
    m_pVIBufferCom->Bind_Resources();
    m_pVIBufferCom->Render();

    return S_OK;
}

HRESULT CUI_Slot_Over_Fx::Ready_Component()
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex_UI_Effect"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Tex_Cursor_FX"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
        return E_FAIL;

    return S_OK;
}

CUI_Slot_Over_Fx* CUI_Slot_Over_Fx::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUI_Slot_Over_Fx* pInstance = new CUI_Slot_Over_Fx(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed Created : CUI_Slot_Over_Fx"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_Slot_Over_Fx::Clone(void* pArg)
{
    CUI_Slot_Over_Fx* pInstance = new CUI_Slot_Over_Fx(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed Cloned : CUI_Slot_Over_Fx"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CUI_Slot_Over_Fx::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);

}
