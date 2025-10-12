#include "Prop_Static.h"

#include "GameInstance.h"

CProp_Static::CProp_Static(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CProp { pDevice, pContext }
{
}

CProp_Static::CProp_Static(const CProp_Static& Prototype)
    : CProp { Prototype }
{
}

HRESULT CProp_Static::Initialize_Prototype()
{
    CHECK_FAILED(__super::Initialize_Prototype(), E_FAIL);

    return S_OK;
}

HRESULT CProp_Static::Initialize_Clone(void* pArg)
{
    CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);

    CHECK_FAILED(Ready_Components(pArg), E_FAIL);

    return S_OK;
}

void CProp_Static::Priority_Update(_float fTimeDelta)
{
}

void CProp_Static::Update(_float fTimeDelta)
{
}

void CProp_Static::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONBLEND, this);
}

HRESULT CProp_Static::Render()
{
    CHECK_FAILED_MSG(Bind_ShaderResources(), TEXT("CProp_Static : Bind_ShaderResources 함수 E_FAIL"), E_FAIL);

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; ++i)
    {
        CHECK_FAILED_ASSERT(Bind_Instance_Materials(m_pModelCom, i), S_OK);

        CHECK_FAILED_ASSERT(m_pShaderCom->Begin(0), E_FAIL);

        CHECK_FAILED_ASSERT(m_pModelCom->Render(i), E_FAIL);
    }

    return S_OK;
}

void CProp_Static::Add_Instance(MESH_INSTANCE_DATA InstanceData)
{
    m_pModelCom->Add_Instance(InstanceData);
}

void CProp_Static::Fix_Instance(MESH_INSTANCE_DATA InstanceData, _uint InstanceIndex)
{
    m_pModelCom->Fix_Instance(InstanceData, InstanceIndex);
}

const _uint CProp_Static::Get_NumInstances() const
{
    return m_pModelCom->Get_NumInstances();
}

HRESULT CProp_Static::Ready_Components(void* pArg)
{
    PROP_STATIC_DESC* pDesc = static_cast<PROP_STATIC_DESC*>(pArg);

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_Component_Shader_MeshInstance"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::MAP), pDesc->szModelName,
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr)))
        return E_FAIL;

    return S_OK;
}

HRESULT CProp_Static::Bind_ShaderResources()
{
    // 월드 행렬 쉐이더에 바인딩
    CHECK_FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);

    // 뷰 행렬 쉐이더에 바인딩
    CHECK_FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW)), E_FAIL);

    // 투영 행렬 쉐이더에 바인딩
    CHECK_FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ)), E_FAIL);

    return S_OK;
}

HRESULT CProp_Static::Bind_Materials(_uint iMeshIndex)
{

    return S_OK;
}

CProp_Static* CProp_Static::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CProp_Static* pInstance = new CProp_Static(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CProp_Static"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CProp_Static::Clone(void* pArg)
{
    CProp_Static* pInstance = new CProp_Static(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CProp_Static"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CProp_Static::Free()
{
    __super::Free();

    Safe_Release(m_pModelCom);
}
