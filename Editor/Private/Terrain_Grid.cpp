#include "EditorPch.h"
#include "Terrain_Grid.h"
#include "GameInstance.h"

CTerrain_Grid::CTerrain_Grid(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CTerrain_Grid::CTerrain_Grid(const CTerrain_Grid& Prototype)
    : CGameObject(Prototype)
{
}

HRESULT CTerrain_Grid::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CTerrain_Grid::Initialize_Clone(void* pArg)
{
    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(-25.f, 0.f, -25.f, 1.f));

    return S_OK;
}

void CTerrain_Grid::Priority_Update(_float fTimeDelta)
{

}

void CTerrain_Grid::Update(_float fTimeDelta)
{
}

void CTerrain_Grid::Late_Update(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONBLEND, this)))
        return;
}

HRESULT CTerrain_Grid::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    m_pShaderCom->Begin(1);

    m_pVIBufferCom->Bind_Resources();

    m_pVIBufferCom->Render();

    return S_OK;
}

HRESULT CTerrain_Grid::Ready_Components()
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxNorTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::EFFECT), TEXT("Prototype_Component_VIBuffer_Terrain"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::EFFECT), TEXT("Prototype_Component_Texture_Terrain_Grid"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
        return E_FAIL;

    return S_OK;
}

HRESULT CTerrain_Grid::Bind_ShaderResources()
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

CTerrain_Grid* CTerrain_Grid::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CTerrain_Grid* pInstance = new CTerrain_Grid(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Create : CTerrain_Grid"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CTerrain_Grid::Clone(void* pArg)
{
    CTerrain_Grid* pInstance = new CTerrain_Grid(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Clone : CTerrain_Grid"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CTerrain_Grid::Free()
{
    __super::Free();

    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pShaderCom);
}
