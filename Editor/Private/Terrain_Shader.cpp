#include "Terrain_Shader.h"
#include "GameInstance.h"

CTerrain_Shader::CTerrain_Shader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CTerrain_Shader::CTerrain_Shader(const CTerrain_Shader& Prototype)
    : CGameObject(Prototype)
{
}

HRESULT CTerrain_Shader::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CTerrain_Shader::Initialize_Clone(void* pArg)
{
    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CTerrain_Shader::Priority_Update(_float fTimeDelta)
{

}

void CTerrain_Shader::Update(_float fTimeDelta)
{
}

void CTerrain_Shader::Late_Update(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONBLEND, this)))
        return;
}

HRESULT CTerrain_Shader::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    m_pShaderCom->Begin(0);

    m_pVIBufferCom->Bind_Resources();

    m_pVIBufferCom->Render();

    return S_OK;
}

HRESULT CTerrain_Shader::Ready_Components()
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxNorTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Terrain"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::SHADER), TEXT("Prototype_Component_Texture_Terrain_Shader"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
        return E_FAIL;

    return S_OK;
}

HRESULT CTerrain_Shader::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_Shader_Resources(m_pShaderCom, "g_DiffuseTexture")))
        return E_FAIL;

    return S_OK;
}

CTerrain_Shader* CTerrain_Shader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CTerrain_Shader* pInstance = new CTerrain_Shader(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Create : CTerrain_Shader"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CTerrain_Shader::Clone(void* pArg)
{
    CTerrain_Shader* pInstance = new CTerrain_Shader(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Clone : CTerrain_Shader"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CTerrain_Shader::Free()
{
    __super::Free();

    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pShaderCom);
}
