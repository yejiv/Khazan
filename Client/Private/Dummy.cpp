#include "Dummy.h"
#include "GameInstance.h"
#include "RigidBody.h"
#include "ContainerObject.h"

CDummy::CDummy(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPool{ pDevice, pContext }
{

}

CDummy::CDummy(const CDummy& Prototype)
    : CPool{ Prototype }
{

}

HRESULT CDummy::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CDummy::Initialize_Clone(void* pArg)
{
    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    m_pTransformCom->Set_State(STATE::POSITION,
        XMVectorSet(
            m_pGameInstance->Rand(0.f, 10.f),
            0.f,
            m_pGameInstance->Rand(0.f, 10.f),
            1.f
        ));

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_pModelCom->Set_Animation(0, true);

    return S_OK;
}

void CDummy::Priority_Update(_float fTimeDelta)
{
    int a = 10;
}

void CDummy::Update(_float fTimeDelta)
{

    if (true == m_pModelCom->Play_Animation(fTimeDelta))
        int a = 10;


    m_pRigidBodyCom->Sync_Update(m_pTransformCom);
}

void CDummy::Late_Update(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONBLEND, this)))
        return;

}

HRESULT CDummy::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint           iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0)))
            return E_FAIL;
        /*if (FAILED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_NormalTexture", i, aiTextureType_DIFFUSE, 0)))
            return E_FAIL;        */

        if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
            return E_FAIL;

        m_pShaderCom->Begin(0);

        m_pModelCom->Render(i);
    }



    return S_OK;
}

HRESULT CDummy::Ready_Components()
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STAGE1), TEXT("Prototype_Component_Model_Fiona"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr)))
        return E_FAIL;


    CRigidBody::BOXSHAPE_DESC RigidDesc{};
    RigidDesc.vExtent = { 0.5f, 0.5f, 0.5f };
    RigidDesc.bIsTrigger = false;
    RigidDesc.bStartActive = true;
    RigidDesc.eMotion = EMotionType::Static;
    RigidDesc.eQuality = EMotionQuality::Discrete;
    RigidDesc.eShapeType = SHAPE::BOX;
    RigidDesc.fFriction = 0.8f;
    RigidDesc.fMass = 1.0f;
    RigidDesc.fRestitution = 0.0f;
    RigidDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::MONSTER);
    _float3 vPos{};
    XMStoreFloat3(&vPos, m_pTransformCom->Get_State(STATE::POSITION));
    _float4 vQuat{};
    XMStoreFloat4(&vQuat, m_pTransformCom->Get_Rotation_Quat());
    RigidDesc.vPos = vPos;
    RigidDesc.vQuat = vQuat;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_RigidBody"),
        TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBodyCom), &RigidDesc)))
        return E_FAIL;


    return S_OK;
}

HRESULT CDummy::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;


    return S_OK;
}

void CDummy::Reset()
{

}

CDummy* CDummy::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CDummy* pInstance = new CDummy(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CDummy"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CDummy::Clone(void* pArg)
{
    CDummy* pInstance = new CDummy(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CDummy"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CDummy::Free()
{
    __super::Free();

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pRigidBodyCom);
}
