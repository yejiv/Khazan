#include "Map_Light.h"

#include "GameInstance.h"

CMap_Light::CMap_Light(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject { pDevice, pContext }
{
}

CMap_Light::CMap_Light(const CMap_Light& Prototype)
    : CGameObject { Prototype }
{
}

HRESULT CMap_Light::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CMap_Light::Initialize_Clone(void* pArg)
{
    CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);

    CHECK_FAILED(Ready_Components(pArg), E_FAIL);

    MAP_LIGHT_DESC* pDesc = static_cast<MAP_LIGHT_DESC*>(pArg);
    CHECK_NULLPTR(pDesc, E_FAIL);

    m_pLightDesc = pDesc->pLightDesc;

    m_pTransformCom->Scale(_float3(0.1f, 0.1f, 0.1f));

    return S_OK;
}

void CMap_Light::Priority_Update(_float fTimeDelta)
{
}

void CMap_Light::Update(_float fTimeDelta)
{
    if (LIGHT_DESC::TYPE::POINT == m_pLightDesc->eType)
        m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat4(&m_pLightDesc->vPosition), 1.f));
}

void CMap_Light::Late_Update(_float fTimeDelta)
{
    if (LIGHT_DESC::TYPE::POINT == m_pLightDesc->eType)
        m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONLIGHT, this);
}

HRESULT CMap_Light::Render()
{
    CHECK_FAILED_MSG(Bind_ShaderResources(), TEXT("CProp_Object : Bind_ShaderResources 함수 E_FAIL"), E_FAIL);

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; ++i)
    {
        Bind_Materials(i);

        m_pShaderCom->Bind_RawValue("g_vColor", &m_pLightDesc->vDiffuse, sizeof(_float4));

        CHECK_FAILED_ASSERT(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS::SOLIDFRAME)), E_FAIL);

        CHECK_FAILED_ASSERT(m_pModelCom->Render(i), E_FAIL);
    }

    return S_OK;
}

HRESULT CMap_Light::Ready_Components(void* pArg)
{
    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr), E_FAIL);

    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_Component_Model_Cube"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr), E_FAIL);

    return S_OK;
}

HRESULT CMap_Light::Bind_ShaderResources()
{
    // 월드 행렬 쉐이더에 바인딩
    CHECK_FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);

    // 뷰 행렬 쉐이더에 바인딩
    CHECK_FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW)), E_FAIL);

    // 투영 행렬 쉐이더에 바인딩
    CHECK_FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ)), E_FAIL);
}

HRESULT CMap_Light::Bind_Materials(_uint iMeshIndex)
{
    return S_OK;
}

CMap_Light* CMap_Light::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CMap_Light* pInstance = new CMap_Light(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CMap_Light"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CMap_Light::Clone(void* pArg)
{
    CMap_Light* pInstance = new CMap_Light(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CMap_Light"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMap_Light::Free()
{
    __super::Free();

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
}
