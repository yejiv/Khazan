#include "Body_Yetuga.h"
#include "GameInstance.h"
#include "BlackBoard.h"
#include "Yetuga.h"
#include "AI_Controller.h"

_float3 CBody_Yetuga::Get_BonePoint(const _char* BoneName)
{
    _float4x4 BoneMatrix = *m_pModelCom->Get_BoneMatrix(BoneName);
    _matrix ConvertMatrix = XMLoadFloat4x4(&BoneMatrix);
    _matrix WorldMatrix = m_pOwnerTransform->Get_WorldMatrix();

    _matrix MulMatrix = ConvertMatrix * WorldMatrix ;

    _float4x4 ThrowMatrix{};

    XMStoreFloat4x4(&ThrowMatrix,MulMatrix);

    m_vThrowPoint.x = ThrowMatrix.m[3][0];
    m_vThrowPoint.y = ThrowMatrix.m[3][1];
    m_vThrowPoint.z = ThrowMatrix.m[3][2];

    return m_vThrowPoint;
}

_float4* CBody_Yetuga::Get_BonePointEX(const _char* BoneName)
{
    _float4x4 BoneMatrix = *m_pModelCom->Get_BoneMatrix(BoneName);
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

_matrix CBody_Yetuga::Get_BoneMatrix(const _char* pBoneName)
{
    _float4x4 BoneMatrix = *m_pModelCom->Get_BoneMatrix(pBoneName);
    _matrix BoneWorld = XMLoadFloat4x4(&BoneMatrix) * XMLoadFloat4x4(&m_CombinedWorldMatrix);

    return BoneWorld;
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
    if (nullptr == m_pOwnerTransform)
        return E_FAIL;

    Safe_AddRef(m_pOwnerTransform);

    m_pOwner = pDesc->pOwner;
    if (nullptr == m_pOwner)
        return E_FAIL;


    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

   if (FAILED(Ready_Colliders()))
        return E_FAIL;

    m_pTransformCom->Scale(_float3(1.3f,1.3f,1.3f));
    
    m_pLH_BodyCom->Collision_Active(false);
    m_pRH_BodyCom->Collision_Active(false);

    return S_OK;
}

void CBody_Yetuga::Priority_Update(_float fTimeDelta)
{

}

void CBody_Yetuga::Update(_float fTimeDelta)
{

    if (m_isOnAttackCollision)
    {

        m_pLH_BodyCom->Collision_Active(true);
        m_pRH_BodyCom->Collision_Active(true);
        Carculate_Matrix(fTimeDelta);
        //m_pGameInstance->Set_DrawFilter(ENUM_CLASS(COLLISION_LAYER::MONSTERATTACK));
    }
    else
    {
        m_pLH_BodyCom->Collision_Active(false);
        m_pRH_BodyCom->Collision_Active(false);
        //m_pGameInstance->Remove_DrawFilter(ENUM_CLASS(COLLISION_LAYER::MONSTERATTACK));
    }


    Update_CombinedMatrix();


}

void CBody_Yetuga::Late_Update(_float fTimeDelta)
{

    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::DYNAMIC, this)))
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
    COLLISION_LAYER eType = static_cast<COLLISION_LAYER>(iOtherObjectLayer);
    m_pOwner->Get_Controller()->AI_React_Collision(pDesc,iOtherObjectLayer,m_pOwner);
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

void CBody_Yetuga::Carculate_Matrix(_float fTimeDelta)
{

    _float4x4 BoneMatrix = *m_pModelCom->Get_BoneMatrix("Weapon_R");
    XMStoreFloat4x4(&m_RightHandMatrix, m_pTransformCom->Get_WorldMatrix() * XMLoadFloat4x4(&BoneMatrix) * XMLoadFloat4x4(m_pParentMatrix));
    _vector vOutQuat, vOutPos;
    m_pRH_BodyCom->Sync_Update(XMLoadFloat4x4(&m_RightHandMatrix));
    m_pRH_BodyCom->Update(fTimeDelta, XMLoadFloat4x4(&m_RightHandMatrix), vOutQuat, vOutPos);

    m_RightHandMatrix._41 = vOutPos.m128_f32[0];
    m_RightHandMatrix._42 = vOutPos.m128_f32[1];
    m_RightHandMatrix._43 = vOutPos.m128_f32[2];
    m_RightHandMatrix._44 = 1.f;


    BoneMatrix = *m_pModelCom->Get_BoneMatrix("Weapon_L");
    XMStoreFloat4x4(&m_LeftHandMatrix, m_pTransformCom->Get_WorldMatrix() * XMLoadFloat4x4(&BoneMatrix) * XMLoadFloat4x4(m_pParentMatrix));
    m_pLH_BodyCom->Sync_Update(XMLoadFloat4x4(&m_LeftHandMatrix));
    m_pLH_BodyCom->Update(fTimeDelta, XMLoadFloat4x4(&m_LeftHandMatrix), vOutQuat, vOutPos);

    m_LeftHandMatrix._41 = vOutPos.m128_f32[0];
    m_LeftHandMatrix._42 = vOutPos.m128_f32[1];
    m_LeftHandMatrix._43 = vOutPos.m128_f32[2];
    m_LeftHandMatrix._44 = vOutPos.m128_f32[3];

}

HRESULT CBody_Yetuga::Ready_Colliders()
{
    CBody::BODY_SPHERESHAPE_DESC BodyDesc{};

    // ������
    BodyDesc.fRadius = 2.f;
    BodyDesc.eMotion = EMotionType::Kinematic;
    BodyDesc.eQuality = EMotionQuality::Discrete; // �⺻ ���
    BodyDesc.eShapeType = SHAPE::SPHERE;
    BodyDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::MONSTERATTACK); // ���Ŀ� Enum Monster attack ���� �Ҽ���
    _float4x4 BoneMatrix = *m_pModelCom->Get_BoneMatrix("Weapon_R");
    XMStoreFloat4x4(&m_RightHandMatrix, m_pTransformCom->Get_WorldMatrix() * XMLoadFloat4x4(&BoneMatrix) * XMLoadFloat4x4(m_pParentMatrix));
    _vector vScale, vQuat, vTrans;
    XMMatrixDecompose(&vScale, &vQuat, &vTrans, XMLoadFloat4x4(&m_RightHandMatrix));
    BodyDesc.vPos = _float3(vTrans.m128_f32[0], vTrans.m128_f32[1], vTrans.m128_f32[2]);
    BodyDesc.vQuat = _float4(vQuat.m128_f32[0], vQuat.m128_f32[1], vQuat.m128_f32[2], vQuat.m128_f32[3]);
    BodyDesc.vShapeOffset = _float3(0.f, 0.f, 0.f);
    m_tCollisionDesc.pGameObject = this;
    BodyDesc.pCollisionDesc = &m_tCollisionDesc;
    BodyDesc.bIsTrigger = true;
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Body"),
        TEXT("Com_Body_RH"), reinterpret_cast<CComponent**>(&m_pRH_BodyCom), &BodyDesc)))
        return E_FAIL;

    BodyDesc.fRadius = 2.f;
    BodyDesc.eMotion = EMotionType::Kinematic;
    BodyDesc.eQuality = EMotionQuality::Discrete; // �⺻ ���
    BodyDesc.eShapeType = SHAPE::SPHERE;
    BodyDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::MONSTERATTACK);
    BoneMatrix = *m_pModelCom->Get_BoneMatrix("Weapon_L");
    XMStoreFloat4x4(&m_RightHandMatrix, m_pTransformCom->Get_WorldMatrix() * 
        XMLoadFloat4x4(&BoneMatrix) * XMLoadFloat4x4(m_pParentMatrix));
   /* _vector vScale, vQuat, vTrans;*/
    // �ɰ���.
    XMMatrixDecompose(&vScale, &vQuat, &vTrans, XMLoadFloat4x4(&m_RightHandMatrix));
    // ��ġ��
    BodyDesc.vPos = _float3(vTrans.m128_f32[0], vTrans.m128_f32[1], vTrans.m128_f32[2]);
    // ���ʹϾ�
    BodyDesc.vQuat = _float4(vQuat.m128_f32[0], vQuat.m128_f32[1], vQuat.m128_f32[2], vQuat.m128_f32[3]);

    BodyDesc.vShapeOffset = _float3(0.f, 0.f, 0.f);
    m_tCollisionDesc.pGameObject = this;
    BodyDesc.pCollisionDesc = &m_tCollisionDesc;
    BodyDesc.bIsTrigger = true;
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Body"),
        TEXT("Com_Body_LH"), reinterpret_cast<CComponent**>(&m_pLH_BodyCom), &BodyDesc)))
        return E_FAIL;

   


 //   // ��
 //   BodyDesc.fRadius = 15.f;
 //   BodyDesc.eMotion = EMotionType::Kinematic;
 //   BodyDesc.eQuality = EMotionQuality::Discrete; // �⺻ ���
 //   BodyDesc.eShapeType = SHAPE::SPHERE;
 //   BodyDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::MONSTER); // ���Ŀ� Enum Monster attack ���� �Ҽ���
 ///*   vPosition.x = m_pModelCom->Get_BoneMatrix("B_Spine2_12_01_S")->m[3][0];
 //   vPosition.y = m_pModelCom->Get_BoneMatrix("B_Spine2_12_01_S")->m[3][1];
 //   vPosition.z = m_pModelCom->Get_BoneMatrix("B_Spine2_12_01_S")->m[3][2];*/
 //   vPosition = Get_BonePoint("B_Spine2_12_01_S");
 //   XMStoreFloat4(&vQuat, m_pTransformCom->Get_Rotation_Quat());
 //   BodyDesc.vPos = vPosition;
 //   BodyDesc.vQuat = vQuat;
 //   BodyDesc.vShapeOffset = _float3(0.f, 0.f, 0.f);
 //   m_tCollisionDesc.pGameObject = this;
 //   BodyDesc.pCollisionDesc = &m_tCollisionDesc;

 //   if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Body"),
 //       TEXT("Com_Body_Back"), reinterpret_cast<CComponent**>(&m_pBack_BodyCom), &BodyDesc)))
 //       return E_FAIL;


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

    Safe_Release(m_pRH_BodyCom);
    Safe_Release(m_pLH_BodyCom);
    Safe_Release(m_pBack_BodyCom);

    __super::Free();

}
