#include "Illusion_Wall.h"

#include "GameInstance.h"

CIllusion_Wall::CIllusion_Wall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CProp_Interactive { pDevice, pContext }
{
}

CIllusion_Wall::CIllusion_Wall(const CIllusion_Wall& Prototype)
    : CProp_Interactive { Prototype }
{
}

HRESULT CIllusion_Wall::Initialize_Prototype()
{
    CHECK_FAILED(__super::Initialize_Prototype(), E_FAIL);

    return S_OK;
}

HRESULT CIllusion_Wall::Initialize_Clone(void* pArg)
{
    CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);

    CHECK_FAILED(Ready_Components(pArg), E_FAIL);

    m_pTransformCom->Scale(_float3(0.02f, 0.02f, 0.02f));

    return S_OK;
}

void CIllusion_Wall::Priority_Update(_float fTimeDelta)
{

}

void CIllusion_Wall::Update(_float fTimeDelta)
{
}

void CIllusion_Wall::Late_Update(_float fTimeDelta)
{
    CHECK_FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::STATIC, this), );
}

HRESULT CIllusion_Wall::Render()
{
    CHECK_FAILED_MSG(Bind_ShaderResources(), TEXT("CProp_Object : Bind_ShaderResources 함수 E_FAIL"), E_FAIL);

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; ++i)
    {
        Bind_Materials(i);

        CHECK_FAILED_ASSERT(m_pShaderCom->Begin(4), E_FAIL);

        CHECK_FAILED_ASSERT(m_pModelCom->Render(i), E_FAIL);
    }

    return S_OK;
}

HRESULT CIllusion_Wall::Ready_Components(void* pArg)
{
    ILLUSION_WALL_DESC* pDesc = static_cast<ILLUSION_WALL_DESC*>(pArg);
    CHECK_NULLPTR(pDesc, E_FAIL);

    LEVEL eLevel = pDesc->eLevel;
    CHECK_EQUAL_MSG(LEVEL::END, eLevel, TEXT("LAYER 함수에서 LEVEL 미입력"), E_FAIL);

    // 개별 쉐이더 생성할지 고민
    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr), E_FAIL);

    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(eLevel), m_szModelName,
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr), E_FAIL);

    return S_OK;
}

HRESULT CIllusion_Wall::Bind_Materials(_uint iMeshIndex)
{
    m_iMtrlFlags = 0;

    if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_DiffuseTexture", iMeshIndex, aiTextureType_DIFFUSE, 0)))
        m_iMtrlFlags |= M_DIFFUSE;
    if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_NormalTexture", iMeshIndex, aiTextureType_NORMALS, 0)))
        m_iMtrlFlags |= M_NORMAL;
    if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_EmissiveTexture", iMeshIndex, aiTextureType_EMISSIVE, 0)))
        m_iMtrlFlags |= M_EMISSIVE;
    if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_SpecularTexture", iMeshIndex, aiTextureType_SPECULAR, 0)))
        m_iMtrlFlags |= M_SPECULAR;
    if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_MetalicTexture", iMeshIndex, aiTextureType_METALNESS, 0)))
        m_iMtrlFlags |= M_METALIC;
    if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_RoughnessTexture", iMeshIndex, aiTextureType_SHININESS, 0)))
        m_iMtrlFlags |= M_ROUGHNESS;

    m_iMtrlFlags &= ~M_EMISSIVE;
    m_iMtrlFlags &= ~M_SPECULAR;

    m_pShaderCom->Bind_RawValue("g_MtrlFlags", &m_iMtrlFlags, sizeof(_uint));

    return S_OK;
}

CIllusion_Wall* CIllusion_Wall::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CIllusion_Wall* pInstance = new CIllusion_Wall(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CIllusion_Wall"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CIllusion_Wall::Clone(void* pArg)
{
    CIllusion_Wall* pInstance = new CIllusion_Wall(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CIllusion_Wall"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CIllusion_Wall::Free()
{
    __super::Free();


}
