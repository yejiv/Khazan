#include "Body_Yetuga.h"
#include "GameInstance.h"


_float3 CBody_Yetuga::Get_ThrowPoint()
{
    _float4x4 BoneMatrix = *m_pModelCom->Get_BoneMatrix("Weapon_R");
    _matrix ConvertMatrix = XMLoadFloat4x4(&BoneMatrix);
    _matrix WorldMatrix = m_pOwnerTransform->Get_WorldMatrix();

    _matrix MulMatrix = ConvertMatrix * WorldMatrix;

    _float4x4 ThrowMatrix{};

    XMStoreFloat4x4(&ThrowMatrix,MulMatrix);

  
    m_vThrowPoint.x = ThrowMatrix.m[3][0];
    m_vThrowPoint.y = ThrowMatrix.m[3][1];
    m_vThrowPoint.z = ThrowMatrix.m[3][2];

    return m_vThrowPoint;
}


CBody_Yetuga::CBody_Yetuga(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CPartObject{pDevice,pContext}
{
}

CBody_Yetuga::CBody_Yetuga(const CBody_Yetuga& Prototype)
    :CPartObject{ Prototype }
{
}

HRESULT CBody_Yetuga::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBody_Yetuga::Initialize_Clone(void* pArg)
{   
    BODY_DESC* pDesc = static_cast<BODY_DESC*>(pArg);
    
    m_pOwnerTransform = pDesc->pOwnerTransform;
    Safe_AddRef(m_pOwnerTransform);
    if (nullptr == m_pOwnerTransform)
        return E_FAIL;

    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

   /* if (FAILED(Ready_Colliders()))
        return E_FAIL;*/

    return S_OK;
}

void CBody_Yetuga::Priority_Update(_float fTimeDelta)
{

}

void CBody_Yetuga::Update(_float fTimeDelta)
{
    Update_CombinedMatrix();

   /* m_pRH_BodyCom->Sync_Update(m_pTransformCom);
    m_pLH_BodyCom->Sync_Update(m_pTransformCom);
    m_pBack_BodyCom->Sync_Update(m_pTransformCom);

    m_pRH_BodyCom->Update(fTimeDelta, m_pTransformCom);
    m_pLH_BodyCom->Update(fTimeDelta, m_pTransformCom);
    m_pBack_BodyCom->Update(fTimeDelta, m_pTransformCom);*/
}

void CBody_Yetuga::Late_Update(_float fTimeDelta)
{

    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONBLEND, this)))
        return;

}

HRESULT CBody_Yetuga::Render()
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

void CBody_Yetuga::Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal)
{

}

void CBody_Yetuga::Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal)
{

}

void CBody_Yetuga::Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer)
{
    
}


HRESULT CBody_Yetuga::Ready_Components()
{

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;
 
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_Model_Yetuga"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr)))
        return E_FAIL;

    m_pModelCom->Set_OwnerTransform(&m_pOwnerTransform);

    return S_OK;

}

HRESULT CBody_Yetuga::Bind_ShaderResources()
{

    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;


    return S_OK;
}

HRESULT CBody_Yetuga::Ready_Colliders()
{
    CBody::BODY_SPHERESHAPE_DESC BodyDesc{};

    // 오른손
    BodyDesc.fRadius = 6.f;
    BodyDesc.eMotion = EMotionType::Kinematic;
    BodyDesc.eQuality = EMotionQuality::Discrete; // 기본 모드
    BodyDesc.eShapeType = SHAPE::SPHERE;
    BodyDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::MONSTER); // 추후에 Enum Monster attack 변경 할수도
    _float3 vPosition{};
    vPosition.x = m_pModelCom->Get_BoneMatrix("Weapon_R")->m[3][0];
    vPosition.y = m_pModelCom->Get_BoneMatrix("Weapon_R")->m[3][1];
    vPosition.z = m_pModelCom->Get_BoneMatrix("Weapon_R")->m[3][2];
    _float4 vQuat{};
    XMStoreFloat4(&vQuat, m_pTransformCom->Get_Rotation_Quat());
    BodyDesc.vPos = vPosition;
    BodyDesc.vQuat = vQuat;
    BodyDesc.vShapeOffset = _float3(0.f, 0.f, 0.f);
    m_tCollisionDesc.pGameObject = this;
    BodyDesc.pCollisionDesc = &m_tCollisionDesc;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Body"),
        TEXT("Com_Body_LH"), reinterpret_cast<CComponent**>(&m_pRH_BodyCom), &BodyDesc)))
        return E_FAIL;

    // 왼손
    BodyDesc.fRadius = 6.f;
    BodyDesc.eMotion = EMotionType::Kinematic;
    BodyDesc.eQuality = EMotionQuality::Discrete; // 기본 모드
    BodyDesc.eShapeType = SHAPE::SPHERE;
    BodyDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::MONSTER); // 추후에 Enum Monster attack 변경 할수도
    vPosition.x = m_pModelCom->Get_BoneMatrix("Weapon_L")->m[3][0];
    vPosition.y = m_pModelCom->Get_BoneMatrix("Weapon_L")->m[3][1];
    vPosition.z = m_pModelCom->Get_BoneMatrix("Weapon_L")->m[3][2];
    XMStoreFloat4(&vQuat, m_pTransformCom->Get_Rotation_Quat());
    BodyDesc.vPos = vPosition;
    BodyDesc.vQuat = vQuat;
    BodyDesc.vShapeOffset = _float3(0.f, 0.f, 0.f);
    m_tCollisionDesc.pGameObject = this;
    BodyDesc.pCollisionDesc = &m_tCollisionDesc;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Body"),
        TEXT("Com_Body_RH"), reinterpret_cast<CComponent**>(&m_pLH_BodyCom), &BodyDesc)))
        return E_FAIL;

    // 등
    BodyDesc.fRadius = 15.f;
    BodyDesc.eMotion = EMotionType::Kinematic;
    BodyDesc.eQuality = EMotionQuality::Discrete; // 기본 모드
    BodyDesc.eShapeType = SHAPE::SPHERE;
    BodyDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::MONSTER); // 추후에 Enum Monster attack 변경 할수도
    vPosition.x = m_pModelCom->Get_BoneMatrix("B_Spine2_12_01_S")->m[3][0];
    vPosition.y = m_pModelCom->Get_BoneMatrix("B_Spine2_12_01_S")->m[3][1];
    vPosition.z = m_pModelCom->Get_BoneMatrix("B_Spine2_12_01_S")->m[3][2];
    XMStoreFloat4(&vQuat, m_pTransformCom->Get_Rotation_Quat());
    BodyDesc.vPos = vPosition;
    BodyDesc.vQuat = vQuat;
    BodyDesc.vShapeOffset = _float3(0.f, 0.f, 0.f);
    m_tCollisionDesc.pGameObject = this;
    BodyDesc.pCollisionDesc = &m_tCollisionDesc;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Body"),
        TEXT("Com_Body_Back"), reinterpret_cast<CComponent**>(&m_pBack_BodyCom), &BodyDesc)))
        return E_FAIL;


    return S_OK;
}





CBody_Yetuga* CBody_Yetuga::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CBody_Yetuga* pInstance = new CBody_Yetuga(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Failed Create : CBody_Yetuga"));
    }

    return pInstance;
}

CGameObject* CBody_Yetuga::Clone(void* pArg)
{
    CBody_Yetuga* pInstance = new CBody_Yetuga(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Failed Clone : CBody_Yetuga"));
    }

    return pInstance;
}

void CBody_Yetuga::Free()
{
    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pOwnerTransform);

    __super::Free();

}
