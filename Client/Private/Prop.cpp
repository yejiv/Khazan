#include "Prop.h"

#include "GameInstance.h"

CProp::CProp(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CMapObject { pDevice, pContext }
{
}

CProp::CProp(const CProp& Prototype)
    : CMapObject { Prototype }
{
}

HRESULT CProp::Initialize_Prototype()
{
    CHECK_FAILED(__super::Initialize_Prototype(), E_FAIL);

    return S_OK;
}

HRESULT CProp::Initialize_Clone(void* pArg)
{
    PROP_DESC* pDesc = static_cast<PROP_DESC*>(pArg);
    CHECK_NULLPTR(pDesc, E_FAIL);

    CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);

    memcpy(m_szModelName, pDesc->szModelName, MAX_PATH);

    m_Properties = pDesc->Properties;

    return S_OK;
}

void CProp::Priority_Update(_float fTimeDelta)
{
}

void CProp::Update(_float fTimeDelta)
{
}

void CProp::Late_Update(_float fTimeDelta)
{
}

HRESULT CProp::Render()
{
    return S_OK;
}

HRESULT CProp::Bind_ShaderResources_ForSnowMap(_uint iMeshIndex)
{
    CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_fSnowAmount", &m_fSnowAmount, sizeof(_float)), E_FAIL);

    CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_vSnowColor", &m_vSnowColor, sizeof(_float3)), E_FAIL);

    return S_OK;
}

HRESULT CProp::Deferred_Bind_ShaderResources_ForSnowMap(_uint iMeshIndex, CDeferredShader* pDeferredShader)
{
    CHECK_FAILED(pDeferredShader->Bind_RawValue("g_fSnowAmount", &m_fSnowAmount, sizeof(_float)), E_FAIL);

    CHECK_FAILED(pDeferredShader->Bind_RawValue("g_vSnowColor", &m_vSnowColor, sizeof(_float3)), E_FAIL);

    return S_OK;
}

HRESULT CProp::Bind_Instance_Materials(CModel_Instance* pModelCom, _uint iMeshIndex)
{
    _bool isDiffuse = { false };
    _bool isNormal = { false };
    _bool isEmissive = { false };
    _bool isSpecular = { false };

    if (SUCCEEDED(pModelCom->Bind_Materials(m_pShaderCom, "g_DiffuseTexture", iMeshIndex, aiTextureType_DIFFUSE, 0)))
        isDiffuse = true;
    if (SUCCEEDED(pModelCom->Bind_Materials(m_pShaderCom, "g_NormalTexture", iMeshIndex, aiTextureType_NORMALS, 0)))
        isNormal = true;
    if (SUCCEEDED(pModelCom->Bind_Materials(m_pShaderCom, "g_EmissiveTexture", iMeshIndex, aiTextureType_EMISSIVE, 0)))
        isEmissive = true;
    if (SUCCEEDED(pModelCom->Bind_Materials(m_pShaderCom, "g_SpecularTexture", iMeshIndex, aiTextureType_SPECULAR, 0)))
        isSpecular = true;

    isSpecular = false;
    isEmissive = false;

    m_pShaderCom->Bind_RawValue("g_isDiffuse", &isDiffuse, sizeof(_bool));
    m_pShaderCom->Bind_RawValue("g_isNormal", &isNormal, sizeof(_bool));
    m_pShaderCom->Bind_RawValue("g_isEmissive", &isEmissive, sizeof(_bool));
    m_pShaderCom->Bind_RawValue("g_isSpecular", &isSpecular, sizeof(_bool));

    return S_OK;
}

HRESULT CProp::Deferred_Bind_Instance_Materials(CModel_Instance* pModelCom, _uint iMeshIndex, CDeferredShader* pDeferredShader)
{
    _bool isDiffuse = { false };
    _bool isNormal = { false };
    _bool isEmissive = { false };
    _bool isSpecular = { false };

    if (SUCCEEDED(pModelCom->Bind_Materials(pDeferredShader, "g_DiffuseTexture", iMeshIndex, aiTextureType_DIFFUSE, 0)))
        isDiffuse = true;
    if (SUCCEEDED(pModelCom->Bind_Materials(pDeferredShader, "g_NormalTexture", iMeshIndex, aiTextureType_NORMALS, 0)))
        isNormal = true;
    if (SUCCEEDED(pModelCom->Bind_Materials(pDeferredShader, "g_EmissiveTexture", iMeshIndex, aiTextureType_EMISSIVE, 0)))
        isEmissive = true;
    if (SUCCEEDED(pModelCom->Bind_Materials(pDeferredShader, "g_SpecularTexture", iMeshIndex, aiTextureType_SPECULAR, 0)))
        isSpecular = true;

    isSpecular = false;
    isEmissive = false;

    pDeferredShader->Bind_RawValue("g_isDiffuse", &isDiffuse, sizeof(_bool));
    pDeferredShader->Bind_RawValue("g_isNormal", &isNormal, sizeof(_bool));
    pDeferredShader->Bind_RawValue("g_isEmissive", &isEmissive, sizeof(_bool));
    pDeferredShader->Bind_RawValue("g_isSpecular", &isSpecular, sizeof(_bool));

    return S_OK;
}


void CProp::Free()
{
    __super::Free();
}
