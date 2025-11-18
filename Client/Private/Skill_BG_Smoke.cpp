#include "Skill_BG_Smoke.h"
#include "GameInstance.h"
#include "ClientInstance.h"

CSkill_BG_Smoke::CSkill_BG_Smoke(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Panel{ pDevice, pContext }
{
}

CSkill_BG_Smoke::CSkill_BG_Smoke(const CSkill_BG_Smoke& Prototype)
    : CUI_Panel(Prototype)
{
}

HRESULT CSkill_BG_Smoke::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CSkill_BG_Smoke::Initialize_Clone(void* pArg)
{
    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Component()))
        return E_FAIL;

    m_vColor = { 1.f, 1.f, 1.f, 0.3f };
    return S_OK;
}
void CSkill_BG_Smoke::Priority_Update(_float fTimeDelta)
{

}

void CSkill_BG_Smoke::Update(_float fTimeDelta)
{
}

void CSkill_BG_Smoke::Late_Update(_float fTimeDelta)
{
    m_fAccTime += fTimeDelta * 0.5f;
    if (m_isVisible)
        CClientInstance::GetInstance()->Add_UIRender(UI_RENDER_TYPE::DEFAULT, this);
}

HRESULT CSkill_BG_Smoke::Render()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    if (FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_Texture", 10)))
        return E_FAIL;

    CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float)), E_FAIL);
    CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_float4)), E_FAIL);
    CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_fValue", &m_fAccTime, sizeof(_float)), E_FAIL);

    m_pShaderCom->Begin(8);
    m_pVIBufferCom->Bind_Resources();
    m_pVIBufferCom->Render();

    return S_OK;
}

HRESULT CSkill_BG_Smoke::Ready_Component()
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

CSkill_BG_Smoke* CSkill_BG_Smoke::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CSkill_BG_Smoke* pInstance = new CSkill_BG_Smoke(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed Created : CSkill_BG_Smoke"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CSkill_BG_Smoke::Clone(void* pArg)
{
    CSkill_BG_Smoke* pInstance = new CSkill_BG_Smoke(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed Cloned : CSkill_BG_Smoke"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CSkill_BG_Smoke::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);

}
