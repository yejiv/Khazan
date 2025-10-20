#include "Prop.h"

#include "GameInstance.h"

#include "Editor_Model.h"
#include "Editor_Model_Instance.h"

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

    memcpy(m_szModelName, pDesc->szModelName, MAX_PATH);
    m_isSnow = pDesc->isSnow;
    m_isCollider = pDesc->isCollider;
    m_isBlended = pDesc->isBlended;

    CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);

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

void CProp::Set_ShaderPass(_uint iShaderPass)
{
    m_eShaderPass = static_cast<SHADER_PASS>(iShaderPass);
}

HRESULT CProp::Bind_ShaderResources_ForSnowMap(CTexture* pTextureCom, _uint iMeshIndex)
{
    CHECK_FAILED(pTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_SnowTexture", 0), E_FAIL);

    CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_fSnowAmount", &m_fSnowAmount, sizeof(_float)), E_FAIL);

    CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_vSnowColor", &m_vSnowColor, sizeof(_float3)), E_FAIL);

    return S_OK;
}

HRESULT CProp::Bind_Instance_Materials(CModel_Instance* pModelCom, _uint iMeshIndex)
{
    pModelCom->Bind_Materials(m_pShaderCom, "g_DiffuseTexture", iMeshIndex, aiTextureType_DIFFUSE, 0);
    pModelCom->Bind_Materials(m_pShaderCom, "g_NormalTexture", iMeshIndex, aiTextureType_NORMALS, 0);
    pModelCom->Bind_Materials(m_pShaderCom, "g_EmissiveTexture", iMeshIndex, aiTextureType_EMISSIVE, 0);
    pModelCom->Bind_Materials(m_pShaderCom, "g_SpecularTexture", iMeshIndex, aiTextureType_SPECULAR, 0);

    return S_OK;
}

void CProp::Free()
{
    __super::Free();
}
