#include "Body_Halberd.h"
#include "GameInstance.h"
#include "ClothBody.h"
#include "Body.h"

CBody_Halberd::CBody_Halberd(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CPartObject{ pDevice,pContext }
{
}

CBody_Halberd::CBody_Halberd(const CBody_Halberd& Prototype)
    :CPartObject(Prototype)
{
}

_float4x4* CBody_Halberd::Get_BoneMatrix_Ptr(const _char* pBoneName)
{
    return m_pModelCom->Get_BoneMatrix(pBoneName);
}

_float CBody_Halberd::Get_CulTrack()
{
    return *m_pModelCom->Get_CurTrackPosition();
}

HRESULT CBody_Halberd::Initialize_Prototype(_int iLevel)
{
    m_iPrototypeIndex = iLevel;

    return S_OK;
}

HRESULT CBody_Halberd::Initialize_Clone(void* pArg)
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
    _float4x4 PreTransformMatrix;
    XMStoreFloat4x4(&PreTransformMatrix, XMMatrixScaling(0.0001, 0.0001, 0.0001) * XMMatrixRotationY(XMConvertToRadians(180.0f)));
    m_pModelCom->Set_PreTransformMatrix(PreTransformMatrix);

    m_pModelCom->Set_OwnerTransform(&m_CombinedWorldMatrix);

    return S_OK;
}

void CBody_Halberd::Priority_Update(_float fTimeDelta)
{
}

void CBody_Halberd::Update(_float fTimeDelta)
{
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
}

void CBody_Halberd::Late_Update(_float fTimeDelta)
{
    m_pCapeBody->Late_Update(fTimeDelta);
    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::DYNAMIC, this)))
        return;
}

HRESULT CBody_Halberd::Render()
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

        if (i == 0)
            m_pShaderCom->Begin(22);
        else
            m_pShaderCom->Begin(17);
         
        m_pModelCom->Render(i);
    }

    return S_OK;
}

void CBody_Halberd::Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{
}

void CBody_Halberd::Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{
}

void CBody_Halberd::Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, COLLISION_DESC* pMyDesc)
{
}

HRESULT CBody_Halberd::Ready_Components()
{
    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Dissolve"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr), E_FAIL);

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(m_iPrototypeIndex, TEXT("Prototype_Component_Model_Halberd"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr)))
        return E_FAIL;

    m_pModelCom->Set_OwnerTransform(&m_pOwnerTransform);


    m_tCapeCollDesc.pGameObject = this;
    CClothBody::CLOTH_BODY_DESC ClothDesc;
    ClothDesc.pModel = m_pModelCom;
    vector<_int> RootBoneIndices;
    RootBoneIndices.push_back(m_pModelCom->Get_BoneIndex("B_Cape_01_01_Spine2"));
    RootBoneIndices.push_back(m_pModelCom->Get_BoneIndex("B_Cape_02_01_Spine2"));
    RootBoneIndices.push_back(m_pModelCom->Get_BoneIndex("B_Cape_03_01_Spine2"));
    ClothDesc.RootBoneIndices = RootBoneIndices;
    ClothDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::CLOTH);
    ClothDesc.pOwnerTransform = m_pOwnerTransform;
    ClothDesc.fGravity = 0.4f;
    ClothDesc.fLinearDamping = 0.3f;
    ClothDesc.fAngularDamping = 0.6f;
    ClothDesc.fMass = 0.05f;
    ClothDesc.fMinDistance = 0.89f;
    ClothDesc.fMaxDistance = 1.02f;
    ClothDesc.fSpringFrequency = 1.5f;
    ClothDesc.fSpringDamping = 2.f;
    ClothDesc.eType = CLOTHTYPE::CAPE;
    ClothDesc.pCollisionDesc = &m_tCapeCollDesc;
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_ClothBody"),
        TEXT("Com_Cloth"), reinterpret_cast<CComponent**>(&m_pCapeBody), &ClothDesc)))
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

HRESULT CBody_Halberd::Bind_ShaderResources()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;

    return S_OK;
}

HRESULT CBody_Halberd::Bind_Dissolve()
{
    CHECK_FAILED(m_pTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_DissolveTexture", 0), E_FAIL);

    m_pShaderCom->Bind_RawValue("g_fDecreaseAlpha", &m_pData->fDecreaseAlpha, sizeof(_float));
    m_pShaderCom->Bind_RawValue("g_fEdgeWidth", &m_pData->fEdgeWidth, sizeof(_float));
    m_pShaderCom->Bind_RawValue("g_fEdgeColor", &m_pData->fEdgeColor, sizeof(_float4));

    return S_OK;
}

CBody_Halberd* CBody_Halberd::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _int iLevel)
{
    CBody_Halberd* pInstance = new CBody_Halberd(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype(iLevel)))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Failed Create : CBody_Halberd"));
    }
    return pInstance;
}

CGameObject* CBody_Halberd::Clone(void* pArg)
{
    CBody_Halberd* pInstance = new CBody_Halberd(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Failed Clone : CBody_Halberd"));
    }

    return pInstance;
}

void CBody_Halberd::Free()
{
    __super::Free();

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pOwnerTransform);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pCapeBody);
    Safe_Release(m_pClothBody);
}
