#include "Body_Imp_Range.h"
#include "GameInstance.h"


_float4x4* CBody_Imp_Range::Get_BoneMatrix_Ptr(const _char* pBoneName)
{
    return m_pModelCom->Get_BoneMatrix(pBoneName);
}

_float4* CBody_Imp_Range::Get_BonePointEX(const _char* pBoneName)
{
    _float4x4 BoneMatrix = *m_pModelCom->Get_BoneMatrix(pBoneName);
    _matrix ConvertMatrix = XMLoadFloat4x4(&BoneMatrix);
    _matrix WorldMatrix = m_pOwnerTransform->Get_WorldMatrix();

    _matrix MulMatrix = ConvertMatrix * WorldMatrix;

    _float4x4 ThrowMatrix{};

    XMStoreFloat4x4(&ThrowMatrix, MulMatrix);

    m_vLockOnPoint.x = ThrowMatrix.m[3][0];
    m_vLockOnPoint.y = ThrowMatrix.m[3][1];
    m_vLockOnPoint.z = ThrowMatrix.m[3][2];

    return &m_vLockOnPoint;
}

_matrix CBody_Imp_Range::Get_BoneMatrix(const _char* pBoneName)
{
    _float4x4 BoneMatrix = *m_pModelCom->Get_BoneMatrix(pBoneName);
    _matrix BoneWorld = XMLoadFloat4x4(&BoneMatrix) * XMLoadFloat4x4(&m_CombinedWorldMatrix);

    return BoneWorld;
}

CBody_Imp_Range::CBody_Imp_Range(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CPartObject{pDevice,pContext}
{
}

CBody_Imp_Range::CBody_Imp_Range(const CBody_Imp_Range& Prototype)
    :CPartObject{Prototype}
{
}

HRESULT CBody_Imp_Range::Initialize_Prototype()
{

    return S_OK;
}

HRESULT CBody_Imp_Range::Initialize_Clone(void* pArg)
{

    BODY_DESC* pDesc = static_cast<BODY_DESC*>(pArg);

    m_pOwnerTransform = pDesc->pOwnerTransform;
    if (nullptr == m_pOwnerTransform)
        return E_FAIL;

    Safe_Release(m_pOwnerTransform);

    m_pOwner = pDesc->pOwner;
    if (nullptr == m_pOwner)
        return E_FAIL;

    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;


    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CBody_Imp_Range::Priority_Update(_float fTimeDelta)
{
}

void CBody_Imp_Range::Update(_float fTimeDelta)
{
    Update_CombinedMatrix();
}

void CBody_Imp_Range::Late_Update(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::DYNAMIC, this)))
        return;
}

HRESULT CBody_Imp_Range::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint           iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        m_pModelCom->Bind_Materials(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0);

        m_pModelCom->Bind_Materials(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS, 0);

        if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
            return E_FAIL;

        m_pShaderCom->Begin(0);

        m_pModelCom->Render(i);
    }

    return S_OK;
}

void CBody_Imp_Range::Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal)
{
}

void CBody_Imp_Range::Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal)
{
}

void CBody_Imp_Range::Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer)
{
}

HRESULT CBody_Imp_Range::Ready_Components()
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_Goblin_Range"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr)))
        return E_FAIL;

    m_pModelCom->Set_OwnerTransform(&m_pOwnerTransform);


    return S_OK;
}

HRESULT CBody_Imp_Range::Bind_ShaderResources()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;

    return S_OK;
}



CBody_Imp_Range* CBody_Imp_Range::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CBody_Imp_Range* pInstance = new CBody_Imp_Range(pDevice,pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Failed Create : CBody_Imp_Range"));
    }
    return pInstance;
}

CGameObject* CBody_Imp_Range::Clone(void* pArg)
{
    CBody_Imp_Range* pInstance = new CBody_Imp_Range(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Failed Clone : CBody_Imp_Range"));
    }

    return pInstance;
}

void CBody_Imp_Range::Free()
{
    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pOwnerTransform);

    __super::Free();

}
