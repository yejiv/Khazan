#include "CloudSphere.h"
#include "GameInstance.h"

CCloudSphere::CCloudSphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject { pDevice, pContext }
{

}

CCloudSphere::CCloudSphere(const CCloudSphere& Prototype)
    : CGameObject { Prototype }
{
}

HRESULT CCloudSphere::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCloudSphere::Initialize_Clone(void* pArg)
{
    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components(pArg)))
        return E_FAIL;

    CLOUD_SPHERE_DESC* pDesc = static_cast<CLOUD_SPHERE_DESC*>(pArg);
    CHECK_NULLPTR(pDesc, E_FAIL);

    m_CloudDesc = pDesc->CloudDesc;

    m_pTransformCom->Scale(_float3(0.0001f, 0.0001f, 0.0001f));

    return S_OK;
}

void CCloudSphere::Priority_Update(_float fTimeDelta)
{
}

void CCloudSphere::Update(_float fTimeDelta)
{
    m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat4(m_pGameInstance->Get_CamPosition()));

    m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta);

    m_fTimeAcc += fTimeDelta;
}

void CCloudSphere::Late_Update(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::PRIORITY, this)))
        return;
}

HRESULT CCloudSphere::Render()
{
    CHECK_FAILED_MSG(Bind_ShaderResources(), TEXT("CProp_Object : Bind_ShaderResources 함수 E_FAIL"), E_FAIL);

    m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4));

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < 0/*iNumMeshes*/; ++i)
    {
        CHECK_FAILED_ASSERT(Bind_Cloud_ShaderResources(), E_FAIL);

        m_pShaderCom->Bind_RawValue("g_fTime", &m_fTimeAcc, sizeof(_float));

        CHECK_FAILED_ASSERT(m_pShaderCom->Begin(2), E_FAIL);

        CHECK_FAILED_ASSERT(m_pModelCom->Render(i), E_FAIL);
    }

    return S_OK;
}

HRESULT CCloudSphere::Ready_Components(void* pArg)
{
    CLOUD_SPHERE_DESC* pDesc = static_cast<CLOUD_SPHERE_DESC*>(pArg);
    CHECK_NULLPTR(pDesc, E_FAIL);

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxSphere"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(pDesc->eLevel), TEXT("Prototype_Component_Model_CloudMesh"),
        TEXT("Com_Model_Cloud"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr)))
        return E_FAIL;

#pragma region 구 스카이 관련 텍스쳐들
    /* Prototype_Component_Texture_Cloud_LookUp */
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(pDesc->eLevel), TEXT("Prototype_Component_Texture_Cloud_LookUp"),
        TEXT("Com_Texture_Gradation"), reinterpret_cast<CComponent**>(&m_pTextureCom[DISTANCE_GRADATION]), nullptr)))
        return E_FAIL;
    /* Prototype_Component_Texture_Cloud_LookUp */
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(pDesc->eLevel), TEXT("Prototype_Component_Texture_Cloud_LookUp"),
        TEXT("Com_Texture_LookUp"), reinterpret_cast<CComponent**>(&m_pTextureCom[LOOKUP]), nullptr)))
        return E_FAIL;
    /* Prototype_Component_Texture_Cloud_Normal */
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(pDesc->eLevel), TEXT("Prototype_Component_Texture_Cloud_Normal"),
        TEXT("Com_Texture_Normal"), reinterpret_cast<CComponent**>(&m_pTextureCom[NORMAL]), nullptr)))
        return E_FAIL;
    /* Prototype_Component_Texture_Cloud_Distortion */
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(pDesc->eLevel), TEXT("Prototype_Component_Texture_Cloud_Distortion"),
        TEXT("Com_Texture_Distortion"), reinterpret_cast<CComponent**>(&m_pTextureCom[DISTORTION]), nullptr)))
        return E_FAIL;
#pragma endregion

    return S_OK;
}

HRESULT CCloudSphere::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;

    return S_OK;
}

HRESULT CCloudSphere::Bind_Cloud_ShaderResources()
{
    m_pTextureCom[DISTANCE_GRADATION]->Bind_Shader_Resource(m_pShaderCom, "g_GradationTexture", 0);
    m_pTextureCom[LOOKUP]->Bind_Shader_Resource(m_pShaderCom, "g_LookUpTexture", 0);
    m_pTextureCom[NORMAL]->Bind_Shader_Resource(m_pShaderCom, "g_NormalTexture", 0);
    m_pTextureCom[DISTORTION]->Bind_Shader_Resource(m_pShaderCom, "g_DistortionTexture", 0);

    return S_OK;
}

CCloudSphere* CCloudSphere::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CCloudSphere* pInstance = new CCloudSphere(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CCloudSphere"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CCloudSphere::Clone(void* pArg)
{
    CCloudSphere* pInstance = new CCloudSphere(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CCloudSphere"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCloudSphere::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);

    Safe_Release(m_pModelCom);

    for (auto& pTex : m_pTextureCom)
        Safe_Release(pTex);
}
