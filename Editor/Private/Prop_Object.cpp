#include "Prop_Object.h"

#include "GameInstance.h"

#include "Editor_Model.h"

CProp_Object::CProp_Object(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CProp { pDevice, pContext }
{
}

CProp_Object::CProp_Object(const CProp_Object& Prototype)
    : CProp { Prototype }
{
}

HRESULT CProp_Object::Initialize_Prototype()
{
    CHECK_FAILED(__super::Initialize_Prototype(), E_FAIL);

    return S_OK;
}

HRESULT CProp_Object::Initialize_Clone(void* pArg)
{
    CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);

    CHECK_FAILED(Ready_Components(pArg), E_FAIL);

    PROP_OBJECT_DESC* pDesc = static_cast<PROP_OBJECT_DESC*>(pArg);

    if (true == pDesc->isIndependentObject)
    {
        _float3 vScale = {};

        XMStoreFloat3(&vScale, XMLoadFloat3(&pDesc->vScale));

        //_float3 vRotation = _float3(XMConvertToRadians(pDesc->vRotation.x), XMConvertToRadians(pDesc->vRotation.y), XMConvertToRadians(pDesc->vRotation.z));
        _float3 vRotation = _float3(pDesc->vRotation.x, pDesc->vRotation.y, pDesc->vRotation.z);
        _float3 vPosition = pDesc->vPosition;

        m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&vPosition), 1.f));
        m_pTransformCom->Scale(vScale);
        m_pTransformCom->Rotation(vRotation.x, vRotation.y, vRotation.z);

        _vector test = m_pTransformCom->Get_Rotation_Quat();

        int a = 10;
    }

    return S_OK;
}

void CProp_Object::Priority_Update(_float fTimeDelta)
{
}

void CProp_Object::Update(_float fTimeDelta)
{
}

void CProp_Object::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONBLEND, this);
}

HRESULT CProp_Object::Render()
{
    CHECK_FAILED_MSG(Bind_ShaderResources(), TEXT("CProp_Object : Bind_ShaderResources 함수 E_FAIL"), E_FAIL);

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; ++i)
    {
        Bind_Materials(i);

        CHECK_FAILED_ASSERT(m_pShaderCom->Begin(0), E_FAIL);

        CHECK_FAILED_ASSERT(m_pModelCom->Render(i), E_FAIL);
    }

    return S_OK;
}

HRESULT CProp_Object::Ready_Components(void* pArg)
{
    PROP_OBJECT_DESC* pDesc = static_cast<PROP_OBJECT_DESC*>(pArg);

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::MAP), m_szModelName,
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr)))
        return E_FAIL;

    return S_OK;
}

HRESULT CProp_Object::Bind_ShaderResources()
{
    // 월드 행렬 쉐이더에 바인딩
    CHECK_FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);

    // 뷰 행렬 쉐이더에 바인딩
    CHECK_FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW)), E_FAIL);

    // 투영 행렬 쉐이더에 바인딩
    CHECK_FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ)), E_FAIL);

    return S_OK;
}

HRESULT CProp_Object::Bind_Materials(_uint iMeshIndex)
{
    m_pModelCom->Bind_Materials(m_pShaderCom, "g_DiffuseTexture", iMeshIndex, aiTextureType_DIFFUSE, 0);
    m_pModelCom->Bind_Materials(m_pShaderCom, "g_NormalTexture", iMeshIndex, aiTextureType_NORMALS, 0);
    m_pModelCom->Bind_Materials(m_pShaderCom, "g_EmissiveTexture", iMeshIndex, aiTextureType_EMISSIVE, 0);
    m_pModelCom->Bind_Materials(m_pShaderCom, "g_SpecularTexture", iMeshIndex, aiTextureType_SPECULAR, 0);

    return S_OK;
}

CProp_Object* CProp_Object::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CProp_Object* pInstance = new CProp_Object(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CProp_Object"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CProp_Object::Clone(void* pArg)
{
    CProp_Object* pInstance = new CProp_Object(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CProp_Object"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CProp_Object::Free()
{
    __super::Free();

    Safe_Release(m_pModelCom);
}
