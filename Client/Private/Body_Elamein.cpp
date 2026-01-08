#include "Body_Elamein.h"
#include "GameInstance.h"
#include "ClothBody.h"
#include "Body.h"

CBody_Elamein::CBody_Elamein(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CWeaponObject{ pDevice,pContext }
{
}

CBody_Elamein::CBody_Elamein(const CBody_Elamein& Prototype)
    :CWeaponObject(Prototype)
{
}

_float4x4* CBody_Elamein::Get_BoneMatrix_Ptr(const _char* pBoneName)
{
    return m_pModelCom->Get_BoneMatrix(pBoneName);
}

_float CBody_Elamein::Get_CulTrack()
{
    return *m_pModelCom->Get_CurTrackPosition();
}

HRESULT CBody_Elamein::Initialize_Prototype(_int iLevel)
{
    m_iPrototypeIndex = iLevel;

    return S_OK;
}

HRESULT CBody_Elamein::Initialize_Clone(void* pArg)
{
    BODY_DESC* pDesc = static_cast<BODY_DESC*>(pArg);

    m_pOwnerTransform = pDesc->pOwnerTransform;
    CHECK_NULLPTR(m_pOwnerTransform, E_FAIL);
    Safe_AddRef(m_pOwnerTransform);

    m_pData = pDesc->pData;
    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_pModelCom->Set_OwnerTransform(&m_CombinedWorldMatrix);

    _float4x4 PreTransformMatrix;
    XMStoreFloat4x4(&PreTransformMatrix, XMMatrixScaling(0.00012f, 0.00012f, 0.00012f) * XMMatrixRotationY(XMConvertToRadians(180.0f)));
    m_pModelCom->Set_PreTransformMatrix(PreTransformMatrix);
    m_pModelCom->Play_Animation(0);

    Set_JustGuardCallBack([this](_bool isJustGuard) { *m_pData->pCulStamina -= *m_pData->pMaxStamina * 0.1f; });
    return S_OK;
}

void CBody_Elamein::Priority_Update(_float fTimeDelta)
{
}

void CBody_Elamein::Update(_float fTimeDelta)
{
    if (!m_pData->isSearch)
        return;

    if (m_iPreAnim != m_pData->iAnimIndex)
    {
        m_pModelCom->Set_Animation(m_pData->iAnimIndex);
        m_iPreAnim = m_pData->iAnimIndex;

        if (m_pData->fQuat != 0.f)
        {
            m_pModelCom->Set_BlendTime(0.f);
            _vector vQuat = m_pData->pOwner->Get_Transform()->Get_Rotation_Quat();
            _vector vAddQuat = XMQuaternionRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(m_pData->fQuat));
            vQuat = XMQuaternionNormalize(XMQuaternionMultiply(vAddQuat, vQuat));
            m_pData->pOwner->Get_Transform()->Set_Quaternion(vQuat);
            m_pData->fQuat = 0.f;
            m_pModelCom->Set_BlendTime(0.f);
        }
        else
            m_pModelCom->Set_BlendTime(0.25f);
    }

    Update_CombinedMatrix();
    m_pData->isAnimFinash = m_pModelCom->Play_Animation(fTimeDelta * m_pData->fDeltaSpeed);
    Update_Body(fTimeDelta);

//#ifdef NDEBUG
    _matrix BoneMatrix = XMLoadFloat4x4(m_pClothBodyMatrix);

    for (uint32_t i = 0; i < 3; i++)
        BoneMatrix.r[i] = XMVector3Normalize(BoneMatrix.r[i]);

    XMStoreFloat4x4(&m_pClothCombinedMatrix, m_pTransformCom->Get_WorldMatrix() * BoneMatrix * XMLoadFloat4x4(m_pParentMatrix));

    _matrix ClothWorld = XMLoadFloat4x4(&m_pClothCombinedMatrix);

    _vector vScale, vQuat, vPos;
    XMMatrixDecompose(&vScale, &vQuat, &vPos, ClothWorld);

    m_pClothBody->Sync_Update(ClothWorld);
    m_pClothBody->Update(fTimeDelta, ClothWorld, vQuat, vPos);
    m_pCapeBody->Priority_Update(fTimeDelta);
    m_pCapeBody->Update(fTimeDelta);
    //if (m_pData->iAnimIndex != 31 && m_pData->iAnimIndex != 35 && m_pData->iAnimIndex != 51 && m_pData->iAnimIndex != 95)
    //{
    //    m_pCapeBody->Priority_Update(fTimeDelta);
    //    m_pCapeBody->Update(fTimeDelta);
    //}     
//#endif
}

void CBody_Elamein::Late_Update(_float fTimeDelta)
{
    if (!m_pData->isSearch)
        return;
//#ifdef NDEBUG
    m_pCapeBody->Late_Update(fTimeDelta);

    //if (m_pData->iAnimIndex != 31 && m_pData->iAnimIndex != 35 && m_pData->iAnimIndex != 51 && m_pData->iAnimIndex != 95)
    //    m_pCapeBody->Late_Update(fTimeDelta);
//#endif
}

HRESULT CBody_Elamein::Render()
{
    CHECK_FAILED(Bind_ShaderResources(), E_FAIL);
    CHECK_FAILED(Bind_Dissolve(), E_FAIL);

    _uint           iNumMeshes = m_pModelCom->Get_NumMeshes();
    
    _float fEdgeIntensity = 1.f;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fEdgeIntensity", &fEdgeIntensity, sizeof(_float))))
        return E_FAIL;

    _float fShadeIntensity = 0.5f;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fShadeIntensity", &fShadeIntensity, sizeof(_float))))
        return E_FAIL;

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        m_pModelCom->Bind_Materials(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0);
        m_pModelCom->Bind_Materials(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS, 0);
        m_pModelCom->Bind_Materials(m_pShaderCom, "g_SpecularTexture", i, aiTextureType_SPECULAR, 0);
        m_pModelCom->Bind_Materials(m_pShaderCom, "g_MetalnessTexture", i, aiTextureType_METALNESS, 0);

        if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
            return E_FAIL;

        if (i == 5)
        { 
            _float fValue = 0.15f;
            m_pShaderCom->Bind_RawValue("g_fEmissiveValue", &fValue, sizeof(_float));
            m_pModelCom->Bind_Materials(m_pShaderCom, "g_EmissiveTexture", i, aiTextureType_EMISSIVE, 0);
            m_pShaderCom->Begin(15);
        }
        else  if (i == 3) 
            m_pShaderCom->Begin(21);
        else
            m_pShaderCom->Begin(17);

        m_pModelCom->Render(i);
    }

    return S_OK;
}

void CBody_Elamein::Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{
}

void CBody_Elamein::Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{
}

void CBody_Elamein::Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, COLLISION_DESC* pMyDesc)
{
}

HRESULT CBody_Elamein::Ready_Components()
{
    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Dissolve"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr), E_FAIL);

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(m_iPrototypeIndex, TEXT("Prototype_Component_Model_Elamein"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr)))
        return E_FAIL;

    m_pModelCom->Set_OwnerTransform(&m_pOwnerTransform);
//#ifdef NDEBUG
    m_tCapeCollDesc.pGameObject = this;
    CClothBody::CLOTH_BODY_DESC ClothDesc;
    ClothDesc.pModel = m_pModelCom;
    vector<_int> RootBoneIndices;
    RootBoneIndices.push_back(m_pModelCom->Get_BoneIndex("B_Cloth_04_01_R"));
    RootBoneIndices.push_back(m_pModelCom->Get_BoneIndex("B_Cloth_03_01_R"));
    RootBoneIndices.push_back(m_pModelCom->Get_BoneIndex("B_Cloth_03_01_L"));
    RootBoneIndices.push_back(m_pModelCom->Get_BoneIndex("B_Cloth_04_01_L"));
    ClothDesc.RootBoneIndices = RootBoneIndices;
    ClothDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::CLOTH);
    ClothDesc.pOwnerTransform = m_pOwnerTransform;
    ClothDesc.fGravity = 0.4f;
    ClothDesc.fLinearDamping = 0.3f;
    ClothDesc.fAngularDamping = 0.6f;
    ClothDesc.fMass = 0.05f;
    ClothDesc.fMinDistance = 1.f;
    ClothDesc.fMaxDistance = 1.0f;
    ClothDesc.fSpringFrequency = 4.f;
    ClothDesc.fSpringDamping = 2.f;
    ClothDesc.eType = CLOTHTYPE::CAPE;
    m_tCapeCollDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::CLOTH);
    ClothDesc.pCollisionDesc = &m_tCapeCollDesc;
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_ClothBody"),
        TEXT("Com_Cloth"), reinterpret_cast<CComponent**>(&m_pCapeBody), &ClothDesc)))
        return E_FAIL;

    // Bip001-Pelvis
    m_tClothBodyCollDesc.pGameObject = this;

    CBody::BODY_BOXSHAPE_DESC BodyDesc{};
    BodyDesc.vExtent = { 10.f, 0.3f, 10.f };
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
    m_tClothBodyCollDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::CLOTHBODY);
    BodyDesc.pCollisionDesc = &m_tClothBodyCollDesc;

    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Body"), TEXT("Com_ClothBody"), (CComponent**)&m_pClothBody, &BodyDesc), E_FAIL);    
//#endif

    m_pSocketMatrix = m_pModelCom->Get_BoneMatrix("Bip001-R-Foot");
    m_tCollisionDesc.pGameObject = this;
    CBody::BODY_BOXSHAPE_DESC CollBodyDesc{};
    CollBodyDesc.vExtent = { 1.5f, 0.5f, 0.5f };
    CollBodyDesc.eMotion = EMotionType::Kinematic;
    CollBodyDesc.eQuality = EMotionQuality::Discrete;
    CollBodyDesc.eShapeType = SHAPE::BOX;
    CollBodyDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::MONSTERATTACK);
    CollBodyDesc.bIsTrigger = true;

    XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrix() * XMLoadFloat4x4(m_pSocketMatrix) * XMLoadFloat4x4(m_pParentMatrix));
    CollBodyDesc.vPos = { m_CombinedWorldMatrix._41, m_CombinedWorldMatrix._42, m_CombinedWorldMatrix._43 };
    XMStoreFloat4(&CollBodyDesc.vQuat, XMQuaternionRotationMatrix(XMLoadFloat4x4(&m_CombinedWorldMatrix)));

    CollBodyDesc.vShapeOffset = _float3(0.4f, -0.f, 0.f);
    m_tCollisionDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::MONSTERATTACK);
    CollBodyDesc.pCollisionDesc = &m_tCollisionDesc;

    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Body"), TEXT("Com_LegBody"), (CComponent**)&m_pBodyComp, &CollBodyDesc), E_FAIL);
    return S_OK;
}

HRESULT CBody_Elamein::Bind_ShaderResources()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;

    return S_OK;
}

HRESULT CBody_Elamein::Bind_Dissolve()
{
    CHECK_FAILED(m_pTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_DissolveTexture", 0), E_FAIL);

    m_pShaderCom->Bind_RawValue("g_fDecreaseAlpha", &m_pData->fDecreaseAlpha, sizeof(_float));
    m_pShaderCom->Bind_RawValue("g_fEdgeWidth", &m_pData->fEdgeWidth, sizeof(_float));
    m_pShaderCom->Bind_RawValue("g_fEdgeColor", &m_pData->fEdgeColor, sizeof(_float4));

    return S_OK;
}

void CBody_Elamein::Update_Body(_float fTimeDelta)
{
    _matrix BoneMatrix = XMLoadFloat4x4(m_pSocketMatrix);

    for (uint32_t i = 0; i < 3; i++)
        BoneMatrix.r[i] = XMVector3Normalize(BoneMatrix.r[i]);
    _float4x4 CombinedWorldMatrix = {};

    XMStoreFloat4x4(&CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrix() * BoneMatrix * XMLoadFloat4x4(m_pParentMatrix));
    _bool isAttakc = m_pData->iAttackBody_State & (_uint)CElamein::ATTACK_BODY::RIGHT_LEG;
    m_pBodyComp->Collision_Active(isAttakc);

    if (!isAttakc)
        return;

    _matrix WeaponWorld = XMLoadFloat4x4(&CombinedWorldMatrix);

    _vector vScale, vQuat, vPos;
    XMMatrixDecompose(&vScale, &vQuat, &vPos, WeaponWorld);

    m_pBodyComp->Sync_Update(WeaponWorld);
    m_pBodyComp->Update(fTimeDelta, WeaponWorld, vQuat, vPos);
}

CBody_Elamein* CBody_Elamein::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _int iLevel)
{
    CBody_Elamein* pInstance = new CBody_Elamein(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype(iLevel)))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Failed Create : CBody_Elamein"));
    }
    return pInstance;
}

CGameObject* CBody_Elamein::Clone(void* pArg)
{
    CBody_Elamein* pInstance = new CBody_Elamein(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Failed Clone : CBody_Elamein"));
    }

    return pInstance;
}

void CBody_Elamein::Free()
{
    __super::Free();
    Safe_Release(m_pBodyComp);
    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pOwnerTransform);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pCapeBody);
    Safe_Release(m_pClothBody);
}
