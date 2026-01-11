#include "Body_Phase2_Viper.h"
#include "GameInstance.h"
#include "BlackBoard.h"
#include "Viper.h"
#include "AI_Controller.h"
#include "Body.h"
#include "ClothBody.h"
#include "AS_CutScene_2Phase_Viper.h"


_float3 CBody_Phase2_Viper::Get_BonePoint(const _char* BoneName)
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

_float4* CBody_Phase2_Viper::Get_BonePointEX(const _char* BoneName)
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

_matrix CBody_Phase2_Viper::Get_BoneMatrix(const _char* pBoneName)
{
    _float4x4 BoneMatrix = *m_pModelCom->Get_BoneMatrix(pBoneName);
    _matrix BoneWorld = XMLoadFloat4x4(&BoneMatrix) * XMLoadFloat4x4(&m_CombinedWorldMatrix);

    return BoneWorld;
}


_float4x4* CBody_Phase2_Viper::Get_BoneMatrix_Ptr(const _char* pBoneName)
{
    return m_pModelCom->Get_BoneMatrix(pBoneName);
}


CBody_Phase2_Viper::CBody_Phase2_Viper(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CWeaponObject{ pDevice,pContext }
{
}

CBody_Phase2_Viper::CBody_Phase2_Viper(const CBody_Phase2_Viper& Prototype)
    :CWeaponObject{ Prototype }
{
}

HRESULT CBody_Phase2_Viper::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBody_Phase2_Viper::Initialize_Clone(void* pArg)
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

     if (FAILED(Ready_Callback()))
         return E_FAIL;

    _matrix PreTransformMatrix = XMMatrixIdentity();
    PreTransformMatrix = XMMatrixScaling(0.00013f, 0.00013f, 0.00013f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
    _float4x4 TempMatrix = {};
    XMStoreFloat4x4(&TempMatrix, PreTransformMatrix);
    m_pModelCom->Set_PreTransformMatrix(TempMatrix);
    m_pModelCom->Set_Transform(&m_CombinedWorldMatrix);

    return S_OK;
}

void CBody_Phase2_Viper::Priority_Update(_float fTimeDelta)
{

}

void CBody_Phase2_Viper::Update(_float fTimeDelta)
{
    if (CViper::PHASE::PHASE2 == m_pOwner->Get_Phase())
    {
        if (m_isOnAttackCollision)
        {
            m_pLeftHandBody->Collision_Active(true);
            Carculate_Matrix(fTimeDelta);

        }
        else
        {
            m_pLeftHandBody->Collision_Active(false);
        }


        Update_CombinedMatrix();

        if (m_pOwner->Get_Phase2_Viper_CutSceneState()->Get_State() != P2CUTSCENE_STATE::PULLOUT)
        {
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
    }

    // Radial Blur 센터 설정
    if (m_pOwner->Get_Phase2_Viper_CutSceneState()->Get_State() == P2CUTSCENE_STATE::WALK)
        m_pGameInstance->Set_RadialBlurCenter(Get_BoneMatrix("Bone_tongue_04").r[3]);

    MOTIONTRAIL_CONFIG Config{};
    Config.vLifeTime = { 0.f, 0.3f };
    Config.vStartColor = { 1.f, 0.8f, 0.6f };
    Config.vTargetColor = { 0.6f, 0.6f, 0.6f };
    Config.fRimPower = 2.f;
    Config.fRimIntensity = 1.f;
    Config.fEmissiveIntensity = 3.f;
    Config.isIndividualColor = false;
    Config.fColorUpdateSpeed = 2500.f;
    Config.fInterval = 0.15f;
    Config.iMaxFrames = 10;
    m_pMotionTrailCom->Set_Config(Config);

    m_pMotionTrailCom->Update(fTimeDelta);

    if (m_pGameInstance->Key_Pressing(DIK_RCONTROL, fTimeDelta))
    {
        if (m_pGameInstance->Key_Down(DIK_NUMPAD0))
            m_isEnableEdge = false;

        if (m_pGameInstance->Key_Down(DIK_NUMPAD3))
            m_isEnableEdge = true;
    }
}

void CBody_Phase2_Viper::Late_Update(_float fTimeDelta)
{
    if (CViper::PHASE::PHASE2 == m_pOwner->Get_Phase())
    {
        if (m_pOwner->Get_Phase2_Viper_CutSceneState()->Get_State() != P2CUTSCENE_STATE::PULLOUT)
        {
            m_pFeelerBody->Late_Update(fTimeDelta);
        }
        if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::DYNAMIC, this)))
            return;
    }
}

HRESULT CBody_Phase2_Viper::Render()
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

    _float fEmissiveIntensity = 15.f;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fEmissiveIntensity", &fEmissiveIntensity, sizeof(_float))))
        return E_FAIL;

    _bool isEnableRimLight = false;
    if (FAILED(m_pShaderCom->Bind_Bool("g_isEnableRimLight", &isEnableRimLight)))
        return E_FAIL;

    //  _float fGreenIntensity = 0.9f;
    //  if (FAILED(m_pShaderCom->Bind_RawValue("g_fGreenIntensity", &fGreenIntensity, sizeof(_float))))
    //      return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Bool("g_isEnableEdge", &m_isEnableEdge)))
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

void CBody_Phase2_Viper::Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{
     COLLISION_LAYER eType = static_cast<COLLISION_LAYER>(iOtherObjectLayer);
     m_pOwner->Get_Controller()->AI_React_Collision(pDesc, iOtherObjectLayer, m_pOwner);
}

void CBody_Phase2_Viper::Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{

}

void CBody_Phase2_Viper::Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, COLLISION_DESC* pMyDesc)
{

}

void CBody_Phase2_Viper::Set_EnableMotionTrail(_bool isEnable)
{
    m_pMotionTrailCom->Set_Enable(isEnable);
}

HRESULT CBody_Phase2_Viper::Ready_Components()
{

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_Component_Model_Viper_Phase2"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr)))
        return E_FAIL;

    m_pModelCom->Set_OwnerTransform(&m_pOwnerTransform);

    m_tFeelerCollDesc.pGameObject = this;
    CClothBody::CLOTH_BODY_DESC ClothDesc;
    ClothDesc.pModel = m_pModelCom;
    vector<_int> RootBoneIndices;
    RootBoneIndices.push_back(m_pModelCom->Get_BoneIndex("Bone_Feeler01_R01"));
    RootBoneIndices.push_back(m_pModelCom->Get_BoneIndex("Bone_Feeler01_L01"));
    RootBoneIndices.push_back(m_pModelCom->Get_BoneIndex("Bone_Feeler3_L01"));
    RootBoneIndices.push_back(m_pModelCom->Get_BoneIndex("Bone_Feeler3_R01"));
    RootBoneIndices.push_back(m_pModelCom->Get_BoneIndex("Bone_Feeler2_R01"));
    RootBoneIndices.push_back(m_pModelCom->Get_BoneIndex("Bone_Feeler2_L01"));
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

    CMotionTrail::MOTIONTRAIL_DESC MTDesc{};
    MTDesc.pOwnerMasterModel = m_pModelCom;
    MTDesc.HasPartModels = false;
    MTDesc.Config.vLifeTime = { 0.f, 0.3f };
    MTDesc.Config.vStartColor = { 1.f, 0.8f, 0.6f };
    MTDesc.Config.vTargetColor = { 0.6f, 0.6f, 0.6f };
    MTDesc.Config.fRimPower = 2.f;
    MTDesc.Config.fRimIntensity = 1.f;
    MTDesc.Config.fEmissiveIntensity = 3.f;
    MTDesc.Config.isIndividualColor = false;
    MTDesc.Config.fColorUpdateSpeed = 2500.f;
    MTDesc.Config.fInterval = 0.2f;
    MTDesc.Config.iMaxFrames = 10;
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_MotionTrail"),
        TEXT("Com_MotionTrail"), reinterpret_cast<CComponent**>(&m_pMotionTrailCom), &MTDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CBody_Phase2_Viper::Bind_ShaderResources()
{

    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;


    return S_OK;
}

HRESULT CBody_Phase2_Viper::Ready_Callback()
{
    Set_JustGuardCallBack([this](_bool isJustGuard)
        {
            if (isJustGuard)
            {
                static_cast<CMonster*>(m_pOwner)->Consume_Stamina(m_pOwner->Get_MaxStamina() * 0.05f);
            }
        });
    return S_OK;
}

void CBody_Phase2_Viper::Carculate_Matrix(_float fTimeDelta)
{

    _float4x4 BoneMatrix = *m_pModelCom->Get_BoneMatrix("Bip001-L-Finger2");
    _vector vOutQuat, vOutPos;
    XMStoreFloat4x4(&m_LeftHandMatrix, m_pTransformCom->Get_WorldMatrix() * XMLoadFloat4x4(&BoneMatrix) * XMLoadFloat4x4(m_pParentMatrix));
    m_pLeftHandBody->Sync_Update(XMLoadFloat4x4(&m_LeftHandMatrix));
    m_pLeftHandBody->Update(fTimeDelta, XMLoadFloat4x4(&m_LeftHandMatrix), vOutQuat, vOutPos);

    m_LeftHandMatrix._41 = vOutPos.m128_f32[0];
    m_LeftHandMatrix._42 = vOutPos.m128_f32[1];
    m_LeftHandMatrix._43 = vOutPos.m128_f32[2];
    m_LeftHandMatrix._44 = vOutPos.m128_f32[3];


}


HRESULT CBody_Phase2_Viper::Ready_Colliders()
{
     CBody::BODY_SPHERESHAPE_DESC BodyDesc{};

     BodyDesc.fRadius = 2.2f;
     BodyDesc.eMotion = EMotionType::Kinematic;
     BodyDesc.eQuality = EMotionQuality::Discrete;
     BodyDesc.eShapeType = SHAPE::SPHERE;
     BodyDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::MONSTERATTACK);
     _float4x4 BoneMatrix = *m_pModelCom->Get_BoneMatrix("Bip001-L-Finger2");
     XMStoreFloat4x4(&m_LeftHandMatrix, m_pTransformCom->Get_WorldMatrix() * XMLoadFloat4x4(&BoneMatrix) * XMLoadFloat4x4(m_pParentMatrix));
     _vector vScale, vQuat, vTrans;
     XMMatrixDecompose(&vScale, &vQuat, &vTrans, XMLoadFloat4x4(&m_LeftHandMatrix));
     BodyDesc.vPos = _float3(vTrans.m128_f32[0], vTrans.m128_f32[1], vTrans.m128_f32[2]);
     BodyDesc.vQuat = _float4(vQuat.m128_f32[0], vQuat.m128_f32[1], vQuat.m128_f32[2], vQuat.m128_f32[3]);
     BodyDesc.vShapeOffset = _float3(0.f, -0.5f, 0.f);
     m_tPhase2CollisionDesc.pGameObject = this;
     m_tPhase2CollisionDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::MONSTERATTACK);
     m_tPhase2CollisionDesc.strName = TEXT("Phase2LeftHand");

     BodyDesc.pCollisionDesc = &m_tPhase2CollisionDesc;
     BodyDesc.bIsTrigger = true;
     if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Body"),
         TEXT("Com_Body_LH"), reinterpret_cast<CComponent**>(&m_pLeftHandBody), &BodyDesc)))
         return E_FAIL;

    return S_OK;
}


CBody_Phase2_Viper* CBody_Phase2_Viper::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CBody_Phase2_Viper* pInstance = new CBody_Phase2_Viper(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Failed Create : CBody_Phase2_Viper"));
    }

    return pInstance;
}

CGameObject* CBody_Phase2_Viper::Clone(void* pArg)
{
    CBody_Phase2_Viper* pInstance = new CBody_Phase2_Viper(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Failed Clone : CBody_Phase2_Viper"));
    }

    return pInstance;
}

void CBody_Phase2_Viper::Free()
{
    Safe_Release(m_pMotionTrailCom);
    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pOwnerTransform);
    Safe_Release(m_pLeftHandBody);
    Safe_Release(m_pClothBody);
    Safe_Release(m_pFeelerBody);

    __super::Free();

}
