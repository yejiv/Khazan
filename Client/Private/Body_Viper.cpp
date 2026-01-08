#include "Body_Viper.h"
#include "GameInstance.h"
#include "BlackBoard.h"
#include "Viper.h"
#include "AI_Controller.h"
#include "Body.h"
#include "ClothBody.h"
#include "AS_CutScene_Start_Viper.h"

_float3 CBody_Viper::Get_BonePoint(const _char* BoneName)
{
    _float4x4 BoneMatrix = *m_pModelCom->Get_BoneMatrix(BoneName);
    _matrix ConvertMatrix = XMLoadFloat4x4(&BoneMatrix);
    _matrix WorldMatrix = m_pOwnerTransform->Get_WorldMatrix();

    _matrix MulMatrix = ConvertMatrix * WorldMatrix;

    _float4x4 ThrowMatrix{};

    XMStoreFloat4x4(&ThrowMatrix, MulMatrix);

    m_vThrowPoint.x = ThrowMatrix.m[3][0];
    m_vThrowPoint.y = ThrowMatrix.m[3][1];
    m_vThrowPoint.z = ThrowMatrix.m[3][2];

    return m_vThrowPoint;
}

_float4* CBody_Viper::Get_BonePointEX(const _char* BoneName)
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

_matrix CBody_Viper::Get_BoneMatrix(const _char* pBoneName)
{
    _float4x4 BoneMatrix = *m_pModelCom->Get_BoneMatrix(pBoneName);
    _matrix BoneWorld = XMLoadFloat4x4(&BoneMatrix) * XMLoadFloat4x4(&m_CombinedWorldMatrix);

    return BoneWorld;
}


_float4x4* CBody_Viper::Get_BoneMatrix_Ptr(const _char* pBoneName)
{
    return m_pModelCom->Get_BoneMatrix(pBoneName);
}


CBody_Viper::CBody_Viper(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CPartObject{ pDevice,pContext }
{
}

CBody_Viper::CBody_Viper(const CBody_Viper& Prototype)
    :CPartObject{ Prototype }
{
}

HRESULT CBody_Viper::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBody_Viper::Initialize_Clone(void* pArg)
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

   /* if (FAILED(Ready_Colliders()))
        return E_FAIL;*/

    m_fRimDuration = 3.f;
    m_vRimColor = _float3(1.f, 0.8f, 0.6f);

    return S_OK;
}

void CBody_Viper::Priority_Update(_float fTimeDelta)
{

}

void CBody_Viper::Update(_float fTimeDelta)
{
    if (CViper::PHASE::PHASE1 == m_pOwner->Get_Phase())
    {
        Update_CombinedMatrix();

        _matrix BoneMatrix = XMLoadFloat4x4(m_pClothBodyMatrix);

        for (uint32_t i = 0; i < 3; i++)
            BoneMatrix.r[i] = XMVector3Normalize(BoneMatrix.r[i]);

        XMStoreFloat4x4(&m_pClothCombinedMatrix, m_pTransformCom->Get_WorldMatrix() * BoneMatrix * XMLoadFloat4x4(m_pParentMatrix));

        _matrix ClothWorld = XMLoadFloat4x4(&m_pClothCombinedMatrix);

        _vector vScale, vQuat, vPos;
        XMMatrixDecompose(&vScale, &vQuat, &vPos, ClothWorld);

        m_pClothBody->Sync_Update(ClothWorld);
        m_pClothBody->Update(fTimeDelta, ClothWorld, vQuat, vPos);

        m_pFeelerBody->Priority_Update(fTimeDelta);

        m_pFeelerBody->Update(fTimeDelta);

    }

    if (true == m_isEnableRimLight)
    {
        m_fTimeAcc += fTimeDelta;

        _float fRatio = m_fTimeAcc / m_fRimDuration;

        if (fRatio >= 1.f)
            fRatio = 1.f;

        m_fRimIntensity = Lerp(m_fRimIntensity, m_fMaxRimIntensity, fRatio);
    }

    if (true == m_isBlinkRimLight)
        m_vRimColor = _float3(1.f, 0.6f, 0.4f);

    // Radial Blur 센터 설정
    if (m_pOwner->Get_Viper_CutSceneState()->Get_State() == CUTSCENE_STATE::STAND)
        m_pGameInstance->Set_RadialBlurCenter(Get_BoneMatrix("Bone_tongue_04").r[3]);
}

void CBody_Viper::Late_Update(_float fTimeDelta)
{
    if (CViper::PHASE::PHASE1 == m_pOwner->Get_Phase())
    {
        m_pFeelerBody->Late_Update(fTimeDelta);

        if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::DYNAMIC, this)))
            return;
    }
}

HRESULT CBody_Viper::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    _float fEdgeIntensity = 0.5f;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fEdgeIntensity", &fEdgeIntensity, sizeof(_float))))
        return E_FAIL;

    _float fShadeIntensity = 0.5f;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fShadeIntensity", &fShadeIntensity, sizeof(_float))))
        return E_FAIL;

    _float fEmissiveIntensity = 10.f;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fEmissiveIntensity", &fEmissiveIntensity, sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Bool("g_isEnableRimLight", &m_isEnableRimLight)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Bool("g_isBlinkRimLight", &m_isBlinkRimLight)))
        return E_FAIL;

    _float fRimPower = 2.f;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fRimPower", &fRimPower, sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_fRimLightIntensity", &m_fRimIntensity, sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vRimColor", &m_vRimColor, sizeof(_float3))))
        return E_FAIL;

    _float fRimEmissive = 3.f;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fRimEmissive", &fRimEmissive, sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_fTimeDelta", &m_fTimeAcc, sizeof(_float))))
        return E_FAIL;

    _float fCycleSpeed = 30.f;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fCycleSpeed", &fCycleSpeed, sizeof(_float))))
        return E_FAIL;

    m_fMinRimIntensity = 0.7f;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fMinRimIntensity", &m_fMinRimIntensity, sizeof(_float))))
        return E_FAIL;

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        _bool HasEmissive = true;

        m_pModelCom->Bind_Materials(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0);
        m_pModelCom->Bind_Materials(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS, 0);
        m_pModelCom->Bind_Materials(m_pShaderCom, "g_SpecularTexture", i, aiTextureType_SPECULAR, 0);
        m_pModelCom->Bind_Materials(m_pShaderCom, "g_MetalnessTexture", i, aiTextureType_METALNESS, 0);
        if (FAILED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_EmissiveTexture", i, aiTextureType_EMISSIVE, 0)))
            HasEmissive = false;

        if (FAILED(m_pShaderCom->Bind_Bool("g_HasEmissive", &HasEmissive)))
            return E_FAIL;

        if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
            return E_FAIL;

        m_pShaderCom->Begin(18);

        m_pModelCom->Render(i);
    }

    return S_OK;
}

void CBody_Viper::Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{
   /* COLLISION_LAYER eType = static_cast<COLLISION_LAYER>(iOtherObjectLayer);
    m_pOwner->Get_Controller()->AI_React_Collision(pDesc, iOtherObjectLayer, m_pOwner);*/
}

void CBody_Viper::Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{

}

void CBody_Viper::Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, COLLISION_DESC* pMyDesc)
{

}


HRESULT CBody_Viper::Ready_Components()
{

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_Component_Model_Viper_Phase1"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr)))
        return E_FAIL;

    m_pModelCom->Set_OwnerTransform(&m_pOwnerTransform);

    m_tFeelerCollDesc.pGameObject = this;
    CClothBody::CLOTH_BODY_DESC ClothDesc;
    ClothDesc.pModel = m_pModelCom;
    vector<_int> RootBoneIndices;
    RootBoneIndices.push_back(m_pModelCom->Get_BoneIndex("Bone_Feeler01_R01"));
    RootBoneIndices.push_back(m_pModelCom->Get_BoneIndex("Bone_Feeler01_L01"));
    ClothDesc.RootBoneIndices = RootBoneIndices;
    ClothDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::CLOTH);
    ClothDesc.pOwnerTransform = m_pOwnerTransform;
    ClothDesc.fGravity = 0.4f;
    ClothDesc.fLinearDamping = 1.0f;
    ClothDesc.fAngularDamping = 2.0f;
    ClothDesc.fMass = 0.03f;
    ClothDesc.fMinDistance = 0.98f;
    ClothDesc.fMaxDistance = 1.02f;
    ClothDesc.fSpringFrequency = 3.0f;
    ClothDesc.fSpringDamping = 3.0f;
    ClothDesc.eType = CLOTHTYPE::FEELER;
    ClothDesc.pCollisionDesc = &m_tFeelerCollDesc;
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_ClothBody"),
        TEXT("Com_Cloth"), reinterpret_cast<CComponent**>(&m_pFeelerBody), &ClothDesc)))
        return E_FAIL;

    // Bip001-Pelvis
    m_tClothBodyCollDesc.pGameObject = this;

    CBody::BODY_BOXSHAPE_DESC BodyDesc{};
    BodyDesc.vExtent = { 6.f, 0.3f, 6.f };
    BodyDesc.eMotion = EMotionType::Kinematic;
    BodyDesc.eQuality = EMotionQuality::Discrete;
    BodyDesc.eShapeType = SHAPE::BOX;
    BodyDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::CLOTHBODY);
    BodyDesc.bIsTrigger = false;

    m_pClothBodyMatrix = m_pModelCom->Get_BoneMatrix("Bip001-Pelvis");
    XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrix() * XMLoadFloat4x4(m_pClothBodyMatrix) * XMLoadFloat4x4(m_pParentMatrix));
    BodyDesc.vPos = { m_CombinedWorldMatrix._41, m_CombinedWorldMatrix._42, m_CombinedWorldMatrix._43 };
    XMStoreFloat4(&BodyDesc.vQuat, XMQuaternionRotationMatrix(XMLoadFloat4x4(&m_CombinedWorldMatrix)));

    BodyDesc.vShapeOffset = _float3(0.f, 0.7f, 0.f);
    BodyDesc.pCollisionDesc = &m_tClothBodyCollDesc;

    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Body"), TEXT("Com_ClothBody"), (CComponent**)&m_pClothBody, &BodyDesc), E_FAIL);

    return S_OK;

}



HRESULT CBody_Viper::Bind_ShaderResources()
{

    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;


    return S_OK;
}

void CBody_Viper::Carculate_Matrix(_float fTimeDelta)
{

    //_float4x4 BoneMatrix = *m_pModelCom->Get_BoneMatrix("Weapon_R");
    //XMStoreFloat4x4(&m_RightHandMatrix, m_pTransformCom->Get_WorldMatrix() * XMLoadFloat4x4(&BoneMatrix) * XMLoadFloat4x4(m_pParentMatrix));
    //_vector vOutQuat, vOutPos;
    //m_pRH_BodyCom->Sync_Update(XMLoadFloat4x4(&m_RightHandMatrix));
    //m_pRH_BodyCom->Update(fTimeDelta, XMLoadFloat4x4(&m_RightHandMatrix), vOutQuat, vOutPos);

    //m_RightHandMatrix._41 = vOutPos.m128_f32[0];
    //m_RightHandMatrix._42 = vOutPos.m128_f32[1];
    //m_RightHandMatrix._43 = vOutPos.m128_f32[2];
    //m_RightHandMatrix._44 = 1.f;


    //BoneMatrix = *m_pModelCom->Get_BoneMatrix("Weapon_L");
    //XMStoreFloat4x4(&m_LeftHandMatrix, m_pTransformCom->Get_WorldMatrix() * XMLoadFloat4x4(&BoneMatrix) * XMLoadFloat4x4(m_pParentMatrix));
    //m_pLH_BodyCom->Sync_Update(XMLoadFloat4x4(&m_LeftHandMatrix));
    //m_pLH_BodyCom->Update(fTimeDelta, XMLoadFloat4x4(&m_LeftHandMatrix), vOutQuat, vOutPos);

    //m_LeftHandMatrix._41 = vOutPos.m128_f32[0];
    //m_LeftHandMatrix._42 = vOutPos.m128_f32[1];
    //m_LeftHandMatrix._43 = vOutPos.m128_f32[2];
    //m_LeftHandMatrix._44 = vOutPos.m128_f32[3];


} 

HRESULT CBody_Viper::Ready_Colliders()
{
   /* CBody::BODY_SPHERESHAPE_DESC BodyDesc{};

    BodyDesc.fRadius = 2.f;
    BodyDesc.eMotion = EMotionType::Kinematic;
    BodyDesc.eQuality = EMotionQuality::Discrete;
    BodyDesc.eShapeType = SHAPE::SPHERE;
    BodyDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::MONSTERATTACK);
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
        TEXT("Com_Body_RH"), reinterpret_cast<CComponent**>(&m_pBodyCom), &BodyDesc)))
        return E_FAIL;*/

    return S_OK;
}


CBody_Viper* CBody_Viper::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CBody_Viper* pInstance = new CBody_Viper(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Failed Create : CBody_Viper"));
    }

    return pInstance;
}

CGameObject* CBody_Viper::Clone(void* pArg)
{
    CBody_Viper* pInstance = new CBody_Viper(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Failed Clone : CBody_Viper"));
    }

    return pInstance;
}

void CBody_Viper::Free()
{
    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pOwnerTransform);
    Safe_Release(m_pClothBody);
    Safe_Release(m_pFeelerBody);

    __super::Free();

}
