#include "Body_Gomdol.h"
#include "GameInstance.h"
#include "Gomdol.h"
#include "AI_Controller.h"


_float4* CBody_Gomdol::Get_BonePointEX(const _char* pBoneName)
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

_matrix CBody_Gomdol::Get_BoneMatrix(const _char* pBoneName)
{
    _float4x4 BoneMatrix = *m_pModelCom->Get_BoneMatrix(pBoneName);
    _matrix BoneWorld = XMLoadFloat4x4(&BoneMatrix) * XMLoadFloat4x4(&m_CombinedWorldMatrix);

    return BoneWorld;
}

CBody_Gomdol::CBody_Gomdol(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CPartObject{pDevice,pContext}
{
}

CBody_Gomdol::CBody_Gomdol(const CBody_Gomdol& Prototype)
    :CPartObject{Prototype}
{
}

HRESULT CBody_Gomdol::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBody_Gomdol::Initialize_Clone(void* pArg)
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
        

    m_pLH_BodyCom->Activate(false);
    m_pRH_BodyCom->Activate(false);


    return S_OK;
}

void CBody_Gomdol::Priority_Update(_float fTimeDelta)
{
}

void CBody_Gomdol::Update(_float fTimeDelta)
{
    if (m_isOnAttackCollision_LH)
        m_pLH_BodyCom->Collision_Active(true);

    else if(m_isOnAttackCollision_RH)
        m_pRH_BodyCom->Collision_Active(true);

    else
    {
        m_pLH_BodyCom->Collision_Active(false);
        m_pRH_BodyCom->Collision_Active(false);

    }


    Update_CombinedMatrix();

    Carculate_Matrix(fTimeDelta);


}

void CBody_Gomdol::Late_Update(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::DYNAMIC, this)))
        return;
}

HRESULT CBody_Gomdol::Render()
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

void CBody_Gomdol::Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{
    COLLISION_LAYER eLayer = static_cast<COLLISION_LAYER>(iOtherObjectLayer);

    if (COLLISION_LAYER::PLAYER == eLayer)
    {
        m_pOwner->Get_Controller()->AI_React_Collision(pDesc, iOtherObjectLayer, m_pOwner);
    }
}

void CBody_Gomdol::Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{
}

void CBody_Gomdol::Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, COLLISION_DESC* pMyDesc)
{
}

HRESULT CBody_Gomdol::Ready_Components()
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::TRAINING), TEXT("Prototype_Component_Model_Gomdol"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr)))
        return E_FAIL;

    m_pModelCom->Set_OwnerTransform(&m_pOwnerTransform);

    return S_OK;
}

HRESULT CBody_Gomdol::Bind_ShaderResources()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;

    return S_OK;
}


void CBody_Gomdol::Carculate_Matrix(_float fTimeDelta)
{
    // 뼈 행렬을 가져온다.
    _float4x4 BoneMatrix = *m_pModelCom->Get_BoneMatrix("Bip001-R-Hand");
    // 오른쪽 뼈 행렬을 자체 행렬 * 뼈 로컬행렬  * 부모 행렬을 곱해서 최종 행렬을 만들어준다.
    XMStoreFloat4x4(&m_RightHandMatrix, m_pTransformCom->Get_WorldMatrix() * XMLoadFloat4x4(&BoneMatrix) * XMLoadFloat4x4(m_pParentMatrix));
    _vector vOutQuat, vOutPos;
    // 콜라이더를 갱신시킨다.
    m_pRH_BodyCom->Sync_Update(XMLoadFloat4x4(&m_RightHandMatrix));
    m_pRH_BodyCom->Update(fTimeDelta, XMLoadFloat4x4(&m_RightHandMatrix), vOutQuat, vOutPos);

    m_RightHandMatrix._41 = vOutPos.m128_f32[0];
    m_RightHandMatrix._42 = vOutPos.m128_f32[1];
    m_RightHandMatrix._43 = vOutPos.m128_f32[2];
    m_RightHandMatrix._44 = 1.f;


    BoneMatrix = *m_pModelCom->Get_BoneMatrix("Bip001-L-Hand");
    XMStoreFloat4x4(&m_LeftHandMatrix, m_pTransformCom->Get_WorldMatrix() * XMLoadFloat4x4(&BoneMatrix) * XMLoadFloat4x4(m_pParentMatrix));
    m_pLH_BodyCom->Sync_Update(XMLoadFloat4x4(&m_LeftHandMatrix));
    m_pLH_BodyCom->Update(fTimeDelta, XMLoadFloat4x4(&m_LeftHandMatrix), vOutQuat, vOutPos);

    m_LeftHandMatrix._41 = vOutPos.m128_f32[0];
    m_LeftHandMatrix._42 = vOutPos.m128_f32[1];
    m_LeftHandMatrix._43 = vOutPos.m128_f32[2];
    m_LeftHandMatrix._44 = vOutPos.m128_f32[3];

}

HRESULT CBody_Gomdol::Ready_Colliders()
{


    CBody::BODY_SPHERESHAPE_DESC BodyDesc{};

    // 오른손
    BodyDesc.fRadius = 1.5f;
    BodyDesc.eMotion = EMotionType::Kinematic;
    BodyDesc.eQuality = EMotionQuality::Discrete; // 기본 모드
    BodyDesc.eShapeType = SHAPE::SPHERE;
    BodyDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::MONSTERATTACK); // 추후에 Enum Monster attack 변경 할수도
    _float4x4 BoneMatrix = *m_pModelCom->Get_BoneMatrix("Bip001-R-Hand");
    XMStoreFloat4x4(&m_RightHandMatrix, m_pTransformCom->Get_WorldMatrix() * XMLoadFloat4x4(&BoneMatrix) * XMLoadFloat4x4(m_pParentMatrix));
    _vector vScale, vQuat, vTrans;
    XMMatrixDecompose(&vScale, &vQuat, &vTrans, XMLoadFloat4x4(&m_RightHandMatrix));
    BodyDesc.vPos = _float3(vTrans.m128_f32[0], vTrans.m128_f32[1], vTrans.m128_f32[2]);
    BodyDesc.vQuat = _float4(vQuat.m128_f32[0], vQuat.m128_f32[1], vQuat.m128_f32[2], vQuat.m128_f32[3]);
    BodyDesc.vShapeOffset = _float3(0.f, 0.f, 0.f);
    m_tCollision_RHDescGomdol.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::MONSTERATTACK);
    m_tCollision_RHDescGomdol.strName = TEXT("tCollision_RHDescGomdol");
    m_tCollision_RHDescGomdol.pGameObject = this;
    BodyDesc.pCollisionDesc = &m_tCollision_RHDescGomdol;
    BodyDesc.bIsTrigger = true;
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Body"),
        TEXT("Com_Body_RH"), reinterpret_cast<CComponent**>(&m_pRH_BodyCom), &BodyDesc)))
        return E_FAIL;

    BodyDesc.fRadius = 1.5f;
    BodyDesc.eMotion = EMotionType::Kinematic;
    BodyDesc.eQuality = EMotionQuality::Discrete; // 기본 모드
    BodyDesc.eShapeType = SHAPE::SPHERE;
    BodyDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::MONSTERATTACK);
    BoneMatrix = *m_pModelCom->Get_BoneMatrix("Bip001-L-Hand");
    XMStoreFloat4x4(&m_RightHandMatrix, m_pTransformCom->Get_WorldMatrix() *
        XMLoadFloat4x4(&BoneMatrix) * XMLoadFloat4x4(m_pParentMatrix));
    /* _vector vScale, vQuat, vTrans;*/
     // 쪼갠다.
    XMMatrixDecompose(&vScale, &vQuat, &vTrans, XMLoadFloat4x4(&m_LeftHandMatrix));
    // 위치값
    BodyDesc.vPos = _float3(vTrans.m128_f32[0], vTrans.m128_f32[1], vTrans.m128_f32[2]);
    // 쿼터니언
    BodyDesc.vQuat = _float4(vQuat.m128_f32[0], vQuat.m128_f32[1], vQuat.m128_f32[2], vQuat.m128_f32[3]);

    BodyDesc.vShapeOffset = _float3(0.f, 0.f, 0.f);
    m_tCollision_LHDescGomdol.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::MONSTERATTACK);
    m_tCollision_LHDescGomdol.strName = TEXT("tCollision_LHDescGomdol");
    m_tCollision_LHDescGomdol.pGameObject = this;
    BodyDesc.pCollisionDesc = &m_tCollision_LHDescGomdol;
    BodyDesc.bIsTrigger = true;
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Body"),
        TEXT("Com_Body_LH"), reinterpret_cast<CComponent**>(&m_pLH_BodyCom), &BodyDesc)))
        return E_FAIL;


    m_pLH_BodyCom->Activate(false);
    m_pRH_BodyCom->Activate(false);

    return S_OK;
}

CBody_Gomdol* CBody_Gomdol::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CBody_Gomdol* pInstance = new CBody_Gomdol(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Failed Create : CBody_Gomdol"));
    }

    return pInstance;
}

CGameObject* CBody_Gomdol::Clone(void* pArg)
{
    CBody_Gomdol* pInstance = new CBody_Gomdol(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Failed Clone : CBody_Gomdol"));
    }

    return pInstance;
}

void CBody_Gomdol::Free()
{
    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pOwnerTransform);

    Safe_Release(m_pRH_BodyCom);
    Safe_Release(m_pLH_BodyCom);

    __super::Free();
}
