#include "Prop_Interactive.h"

#include "GameInstance.h"

CProp_Interactive::CProp_Interactive(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CProp { pDevice, pContext }
{
}

CProp_Interactive::CProp_Interactive(const CProp_Interactive& Prototype)
    : CProp { Prototype }
{
}

HRESULT CProp_Interactive::Initialize_Prototype()
{
    CHECK_FAILED(__super::Initialize_Prototype(), E_FAIL);

    return S_OK;
}

HRESULT CProp_Interactive::Initialize_Clone(void* pArg)
{
    PROP_INTERACTIVE_DESC* pDesc = static_cast<PROP_INTERACTIVE_DESC*>(pArg);
    CHECK_NULLPTR(pDesc, E_FAIL);

    CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);

    memcpy(m_szInteractiveTag, pDesc->szInteractiveTag, MAX_PATH);

    // 맵 오브젝트의 월드 행렬 갱신 ( 파일 입출력 받은걸로 )
    m_pTransformCom->Set_WorldMatrix_4x4(pDesc->WorldMatrix);

    return S_OK;
}

void CProp_Interactive::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CProp_Interactive::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
}

void CProp_Interactive::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
}

HRESULT CProp_Interactive::Render()
{
    return S_OK;
}

HRESULT CProp_Interactive::Bind_ShaderResources()
{
    // 월드 행렬 쉐이더에 바인딩
    CHECK_FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);

    // 뷰 행렬 쉐이더에 바인딩
    CHECK_FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW)), E_FAIL);

    // 투영 행렬 쉐이더에 바인딩
    CHECK_FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ)), E_FAIL);

    // 맵 오브젝트 ID 쉐이더에 바인딩
    //CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_iMapObjectID", &m_iMapObjectID, sizeof(_uint)), E_FAIL);

    return S_OK;
}

HRESULT CProp_Interactive::Bind_Materials(_uint iMeshIndex)
{
    _bool isDiffuse = { false };
    _bool isNormal = { false };
    _bool isEmissive = { false };
    _bool isSpecular = { false };

    if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_DiffuseTexture", iMeshIndex, aiTextureType_DIFFUSE, 0)))
        isDiffuse = true;
    if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_NormalTexture", iMeshIndex, aiTextureType_NORMALS, 0)))
        isNormal = true;
    if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_EmissiveTexture", iMeshIndex, aiTextureType_EMISSIVE, 0)))
        isEmissive = true;
    if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_SpecularTexture", iMeshIndex, aiTextureType_SPECULAR, 0)))
        isSpecular = true;

    isSpecular = false;
    isEmissive = false;

    m_pShaderCom->Bind_RawValue("g_isDiffuse", &isDiffuse, sizeof(_bool));
    m_pShaderCom->Bind_RawValue("g_isNormal", &isNormal, sizeof(_bool));
    m_pShaderCom->Bind_RawValue("g_isEmissive", &isEmissive, sizeof(_bool));
    m_pShaderCom->Bind_RawValue("g_isSpecular", &isSpecular, sizeof(_bool));

    return S_OK;
}

void CProp_Interactive::Free()
{
    __super::Free();

    Safe_Release(m_pModelCom);
}
