#include "SkySphere.h"
#include "GameInstance.h"

CSkySphere::CSkySphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject { pDevice, pContext }
{

}

CSkySphere::CSkySphere(const CSkySphere& Prototype)
    : CGameObject { Prototype }
{
}

HRESULT CSkySphere::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CSkySphere::Initialize_Clone(void* pArg)
{
    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components(pArg)))
        return E_FAIL;

    SKY_SPHERE_DESC* pDesc = static_cast<SKY_SPHERE_DESC*>(pArg);
    CHECK_NULLPTR(pDesc, E_FAIL);

    m_SkyDesc = pDesc->SkyDesc;

    m_pTransformCom->Scale(_float3(0.0001f, 0.0001f, 0.0001f));

    return S_OK;
}

void CSkySphere::Priority_Update(_float fTimeDelta)
{
    int a = 10;
}

void CSkySphere::Update(_float fTimeDelta)
{
    m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat4(m_pGameInstance->Get_CamPosition()));

    //m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta);

    m_fTimeAcc += fTimeDelta;
}

void CSkySphere::Late_Update(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::PRIORITY, this)))
        return;
}

HRESULT CSkySphere::Render()
{
    CHECK_FAILED_MSG(Bind_ShaderResources(), TEXT("CProp_Object : Bind_ShaderResources 함수 E_FAIL"), E_FAIL);

    m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4));

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; ++i)
    {
        CHECK_FAILED_ASSERT(Bind_Sky_ShaderResources(), E_FAIL);

        CHECK_FAILED_ASSERT(m_pShaderCom->Begin(1), E_FAIL);

        CHECK_FAILED_ASSERT(m_pModelCom->Render(i), E_FAIL);
    }

    return S_OK;
}

HRESULT CSkySphere::Ready_Components(void* pArg)
{
    SKY_SPHERE_DESC* pDesc = static_cast<SKY_SPHERE_DESC*>(pArg);
    CHECK_NULLPTR(pDesc, E_FAIL);

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxSphere"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(pDesc->eLevel), TEXT("Prototype_Component_Model_SkyMesh"),
        TEXT("Com_Model_Sky"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr)))
        return E_FAIL;

#pragma region 구 스카이 관련 텍스쳐들
    /* Prototype_Component_Texture_Sky_Nebula */
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(pDesc->eLevel), TEXT("Prototype_Component_Texture_Sky_Nebula"),
        TEXT("Com_Texture_Nebula"), reinterpret_cast<CComponent**>(&m_pTextureCom[NEBULA]), nullptr)))
        return E_FAIL;
    /* Prototype_Component_Texture_Sky_Star_Mask */
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(pDesc->eLevel), TEXT("Prototype_Component_Texture_Sky_Star_Mask"),
        TEXT("Com_Texture_Star_Mask"), reinterpret_cast<CComponent**>(&m_pTextureCom[STAR_MASK]), nullptr)))
        return E_FAIL;
    /* Prototype_Component_Texture_Moon */
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(pDesc->eLevel), TEXT("Prototype_Component_Texture_Moon"),
        TEXT("Com_Texture_Moon"), reinterpret_cast<CComponent**>(&m_pTextureCom[MOON]), nullptr)))
        return E_FAIL;
    /* Prototype_Component_Texture_Ring */
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(pDesc->eLevel), TEXT("Prototype_Component_Texture_Ring"),
        TEXT("Com_Texture_Ring"), reinterpret_cast<CComponent**>(&m_pTextureCom[RING]), nullptr)))
        return E_FAIL;
#pragma endregion

    return S_OK;
}

HRESULT CSkySphere::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;

    return S_OK;
}

HRESULT CSkySphere::Bind_Sky_ShaderResources()
{
    m_pTextureCom[NEBULA]->Bind_Shader_Resource(m_pShaderCom, "g_NebulaTexture", 0);
    m_pTextureCom[STAR_MASK]->Bind_Shader_Resource(m_pShaderCom, "g_StarMaskTexture", 0);
    m_pTextureCom[MOON]->Bind_Shader_Resource(m_pShaderCom, "g_MoonTexture", 0);
    m_pTextureCom[RING]->Bind_Shader_Resource(m_pShaderCom, "g_RingTexture", 0);

    m_pShaderCom->Bind_RawValue("g_vNebulaColorR", &m_SkyDesc.vNebulaColorR, sizeof(_float3));
    m_pShaderCom->Bind_RawValue("g_vNebulaColorG", &m_SkyDesc.vNebulaColorG, sizeof(_float3));
    m_pShaderCom->Bind_RawValue("g_vNebulaColorB", &m_SkyDesc.vNebulaColorB, sizeof(_float3));
    m_pShaderCom->Bind_RawValue("g_fStarStrength", &m_SkyDesc.fStarStrength, sizeof(_float));
    m_pShaderCom->Bind_RawValue("g_fMoonSize", &m_SkyDesc.fMoonSize, sizeof(_float));
    m_pShaderCom->Bind_RawValue("g_vMoonColor", &m_SkyDesc.vMoonColor, sizeof(_float3));
    m_pShaderCom->Bind_RawValue("g_vMoonDirection", &m_SkyDesc.vMoonDirection, sizeof(_float3));
    m_pShaderCom->Bind_RawValue("g_fMoonIntensity", &m_SkyDesc.fMoonIntensity, sizeof(_float));

    return S_OK;
}

CSkySphere* CSkySphere::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CSkySphere* pInstance = new CSkySphere(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CSkySphere"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CSkySphere::Clone(void* pArg)
{
    CSkySphere* pInstance = new CSkySphere(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CSkySphere"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CSkySphere::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);

    Safe_Release(m_pModelCom);

    for (auto& pTex : m_pTextureCom)
        Safe_Release(pTex);
}
