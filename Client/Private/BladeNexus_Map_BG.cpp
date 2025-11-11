#include "BladeNexus_Map_BG.h"
#include "GameInstance.h"
#include "ClientInstance.h"

#include "UI_Default_Tex.h"


CBladeNexus_Map_BG::CBladeNexus_Map_BG(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Panel(pDevice, pContext)
{
}

CBladeNexus_Map_BG::CBladeNexus_Map_BG(const CBladeNexus_Map_BG& Prototype)
    : CUI_Panel(Prototype)
{
}

HRESULT CBladeNexus_Map_BG::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBladeNexus_Map_BG::Initialize_Clone(void* pArg)
{
    __super::Initialize_Clone(pArg);
    CHECK_FAILED(Ready_Component(), E_FAIL);

    return S_OK;
}

void CBladeNexus_Map_BG::Priority_Update(_float fTimeDelta)
{
}

void CBladeNexus_Map_BG::Update(_float fTimeDelta)
{

}

void CBladeNexus_Map_BG::Late_Update(_float fTimeDelta)
{
    CClientInstance::GetInstance()->Add_UIRender(UI_RENDER_TYPE::DEFAULT, this);
    __super::Late_Update(fTimeDelta);
}

HRESULT CBladeNexus_Map_BG::Render()
{
    CHECK_FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix), E_FAIL);
    CHECK_FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix), E_FAIL);
    CHECK_FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);

    CHECK_FAILED(m_pTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_Texture", m_iTexPass), E_FAIL);

    CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float)), E_FAIL);
    CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_float4)), E_FAIL);

    CHECK_FAILED(m_pDissolveTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_MaskTexture", 5), E_FAIL);

    m_pShaderCom->Begin(10);
    m_pVIBufferCom->Bind_Resources();
    m_pVIBufferCom->Render();
    return S_OK;
}

HRESULT CBladeNexus_Map_BG::Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg)
{
    CHECK_FAILED(__super::Load_UI(pInData, iPrototypeLevelID, pArg), E_FAIL);
    m_vColor = { 1.f, 1.f, 1.f, 0.8f };
    return S_OK;
}

HRESULT CBladeNexus_Map_BG::Ready_Component()
{
    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex_UI_Mask"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr), E_FAIL);

    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_UI_FX_Mask"),
        TEXT("Com_DissoveTex"), reinterpret_cast<CComponent**>(&m_pDissolveTextureCom), nullptr), E_FAIL);

    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_UI_TS_BG"),
        TEXT("Com_Tex"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr), E_FAIL);

    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuuferCom"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr), E_FAIL);

    return S_OK;
}

CBladeNexus_Map_BG* CBladeNexus_Map_BG::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CBladeNexus_Map_BG* pInstance = new CBladeNexus_Map_BG(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed Created : CBladeNexus_Map_BG"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CBladeNexus_Map_BG::Clone(void* pArg)
{
    CBladeNexus_Map_BG* pInstance = new CBladeNexus_Map_BG(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed Cloned : CBladeNexus_Map_BG"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CBladeNexus_Map_BG::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pDissolveTextureCom);
    Safe_Release(m_pVIBufferCom);
}
