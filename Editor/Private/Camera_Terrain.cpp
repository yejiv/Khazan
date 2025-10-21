#include "Camera_Terrain.h"
#include "GameInstance.h"

CCamera_Terrain::CCamera_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CCamera_Terrain::CCamera_Terrain(const CCamera_Terrain& Prototype)
    : CGameObject(Prototype)
{
}

HRESULT CCamera_Terrain::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCamera_Terrain::Initialize_Clone(void* pArg)
{
    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CCamera_Terrain::Priority_Update(_float fTimeDelta)
{

}

void CCamera_Terrain::Update(_float fTimeDelta)
{
}

void CCamera_Terrain::Late_Update(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONBLEND, this)))
        return;
}

HRESULT CCamera_Terrain::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    m_pShaderCom->Begin(1);

    m_pVIBufferCom->Bind_Resources();

    m_pVIBufferCom->Render();

    return S_OK;
}

HRESULT CCamera_Terrain::Ready_Components()
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxNorTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::CAMERA), TEXT("Prototype_Component_VIBuffer_Terrain"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::CAMERA), TEXT("Prototype_Component_Texture_Terrain_Camera"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
        return E_FAIL;

    return S_OK;
}

HRESULT CCamera_Terrain::Bind_ShaderResources()
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

CCamera_Terrain* CCamera_Terrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CCamera_Terrain* pInstance = new CCamera_Terrain(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Create : CCamera_Terrain"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CCamera_Terrain::Clone(void* pArg)
{
    CCamera_Terrain* pInstance = new CCamera_Terrain(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Clone : CCamera_Terrain"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCamera_Terrain::Free()
{
    __super::Free();

    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pShaderCom);
}
