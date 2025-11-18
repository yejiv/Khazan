#include "UI_QuickSlot_Skill_Fx.h"
#include "GameInstance.h"
#include "ClientInstance.h"

CUI_QuickSlot_Skill_Fx::CUI_QuickSlot_Skill_Fx(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Texture{ pDevice, pContext }
{
}

CUI_QuickSlot_Skill_Fx::CUI_QuickSlot_Skill_Fx(const CUI_QuickSlot_Skill_Fx& Prototype)
    : CUI_Texture(Prototype)
{
}

void CUI_QuickSlot_Skill_Fx::On_Anim()
{
    if (m_eAnim == UIANIMSTATE::ON)
        return;

    m_eAnim = UIANIMSTATE::ON;
    m_fAccTime = 2.f;
    m_vColor = { 1.f,1.f,1.f,1.f };
}

HRESULT CUI_QuickSlot_Skill_Fx::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_QuickSlot_Skill_Fx::Initialize_Clone(void* pArg)
{
    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Component()))
        return E_FAIL;
    return S_OK;
}
void CUI_QuickSlot_Skill_Fx::Priority_Update(_float fTimeDelta)
{
}

void CUI_QuickSlot_Skill_Fx::Update(_float fTimeDelta)
{
    if (m_eAnim == UIANIMSTATE::ON)
    {
        m_fAccTime -= fTimeDelta * 3.f;

        if (m_fAccTime <= 0.f)
        {
            m_eAnim = UIANIMSTATE::OFF;
            m_fAccTime = 2.f;
        }
        m_vColor.w = m_fAccTime;
        Update_Scaling(1.f + ((2.f - m_fAccTime) * 0.5f));
    }
}

void CUI_QuickSlot_Skill_Fx::Late_Update(_float fTimeDelta)
{
    if (m_eAnim == UIANIMSTATE::ON)
        CClientInstance::GetInstance()->Add_UIRender(UI_RENDER_TYPE::DEFAULT, this);
}

HRESULT CUI_QuickSlot_Skill_Fx::Render()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    if (FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_Texture", 12)))
        return E_FAIL;

    CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float)), E_FAIL);
    CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_float4)), E_FAIL);

    m_pShaderCom->Begin(6);
    m_pVIBufferCom->Bind_Resources();
    m_pVIBufferCom->Render();

    return S_OK;
}

HRESULT CUI_QuickSlot_Skill_Fx::Ready_Component()
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

CUI_QuickSlot_Skill_Fx* CUI_QuickSlot_Skill_Fx::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUI_QuickSlot_Skill_Fx* pInstance = new CUI_QuickSlot_Skill_Fx(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed Created : CUI_QuickSlot_Skill_Fx"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_QuickSlot_Skill_Fx::Clone(void* pArg)
{
    CUI_QuickSlot_Skill_Fx* pInstance = new CUI_QuickSlot_Skill_Fx(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed Cloned : CUI_QuickSlot_Skill_Fx"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CUI_QuickSlot_Skill_Fx::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);

}
