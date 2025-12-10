#include "Body_Pet_Danjinjar.h"
#include "GameInstance.h"
#include "DanjinJar.h"
CBody_Pet_Danjinjar::CBody_Pet_Danjinjar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CPartObject{ pDevice,pContext }
{
}

CBody_Pet_Danjinjar::CBody_Pet_Danjinjar(const CBody_Pet_Danjinjar& Prototype)
    :CPartObject(Prototype)
{
}

_float4x4* CBody_Pet_Danjinjar::Get_BoneMatrix_Ptr(const _char* pBoneName)
{
    return m_pModelCom->Get_BoneMatrix(pBoneName);
}

HRESULT CBody_Pet_Danjinjar::Initialize_Prototype(_int iLevel)
{
    m_iPrototypeIndex = iLevel;

    return S_OK;
}

HRESULT CBody_Pet_Danjinjar::Initialize_Clone(void* pArg)
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
    return S_OK;
}

void CBody_Pet_Danjinjar::Priority_Update(_float fTimeDelta)
{
}

void CBody_Pet_Danjinjar::Update(_float fTimeDelta)
{
    if (m_iPreAnim != m_pData->iAnimIndex)
    {
        m_pModelCom->Set_Animation(m_pData->iAnimIndex);
        m_iPreAnim = m_pData->iAnimIndex;

        if (m_pData->fQuat != 0.f)
        {
            if (!m_pData->isBland)
            {
                m_pModelCom->Set_BlendTime(0.f);
                m_pData->isBland = true;
            }
            else
                m_pModelCom->Set_BlendTime(0.25f);

            _vector vQuat = m_pData->pOwner->Get_Transform()->Get_Rotation_Quat();
            _vector vAddQuat = XMQuaternionRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(m_pData->fQuat));
            vQuat = XMQuaternionNormalize(XMQuaternionMultiply(vAddQuat, vQuat));
            m_pData->pOwner->Get_Transform()->Set_Quaternion(vQuat);
            m_pData->fQuat = 0.f;

        }
        else
            m_pModelCom->Set_BlendTime(0.25f);

    }

    Update_CombinedMatrix();
    if (m_pData->isTPDanjin)
    {
        *m_pModelCom->Get_CurTrackPosition() = 73.f;
        m_pData->isAnimFinash = m_pModelCom->Play_Animation(fTimeDelta);
    }
    else
    {
        m_pData->isAnimFinash = m_pModelCom->Play_Animation(fTimeDelta);
    }
}

void CBody_Pet_Danjinjar::Late_Update(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::DYNAMIC, this)))
        return;
}

HRESULT CBody_Pet_Danjinjar::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    CHECK_FAILED(Bind_Dissolve(), E_FAIL);

    _uint           iNumMeshes = m_pModelCom->Get_NumMeshes();

    // 카메라 위치 바인딩 (자체 림 라이트)
    CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float3)), E_FAIL);

    _float fEdgeIntensity = 0.5f;
    CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_fEdgeIntensity", &fEdgeIntensity, sizeof(_float)), E_FAIL);

    _float fShadeIntensity = 0.5f;
    CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_fShadeIntensity", &fShadeIntensity, sizeof(_float)), E_FAIL);

    _float fDiffusePower = 2.f;
    CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_fDiffusePower", &fDiffusePower, sizeof(_float)), E_FAIL);

    _float fRimPower = 1.f;
    CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_fRimPower", &fRimPower, sizeof(_float)), E_FAIL);

    _float fRimIntensity = 0.5f;
    CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_fRimLightIntensity", &fRimIntensity, sizeof(_float)), E_FAIL);

    _float3 vRimColor = _float3(0.9f, 1.f, 0.8f);
    CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_vRimColor", &vRimColor, sizeof(_float3)), E_FAIL);

    _float fRimEmissive = 2.f;
    CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_fRimEmissive", &fRimEmissive, sizeof(_float)), E_FAIL);

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        if (Skip_Mesh(i))
            continue;

        _uint iMtrlFlags = 0;

        if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0)))
            iMtrlFlags |= M_DIFFUSE;
        if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS, 0)))
            iMtrlFlags |= M_NORMAL;
        if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_EmissiveTexture", i, aiTextureType_EMISSIVE, 0)))
            iMtrlFlags |= M_EMISSIVE;
        if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_SpecularTexture", i, aiTextureType_SPECULAR, 0)))
            iMtrlFlags |= M_SPECULAR;
        if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_MetalicTexture", i, aiTextureType_METALNESS, 0)))
            iMtrlFlags |= M_METALIC;
        if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_RoughnessTexture", i, aiTextureType_SHININESS, 0)))
            iMtrlFlags |= M_ROUGHNESS;

        CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_MtrlFlags", &iMtrlFlags, sizeof(_uint)), E_FAIL);

        if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
            return E_FAIL;

        m_pShaderCom->Begin(20);

        m_pModelCom->Render(i);
    }

    return S_OK;
}

void CBody_Pet_Danjinjar::Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{
}

void CBody_Pet_Danjinjar::Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{
}

void CBody_Pet_Danjinjar::Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, COLLISION_DESC* pMyDesc)
{
}

_bool CBody_Pet_Danjinjar::Skip_Mesh(_uint iMeshIndex)
{
    switch ((ANIM_STATE)m_pData->iAnimIndex)
    {
    case ANIM_STATE::DANCE1_ACTIVE:
    case ANIM_STATE::DANCE1_LOOP:
        if (MESH_CENTER == iMeshIndex)
            return true;
        break;
    case ANIM_STATE::FLIPPING_ACTIVE:
    case ANIM_STATE::FLIPPING_LOOP:
        if (MESH_RIGHT == iMeshIndex)
            return true;
        break;
    case ANIM_STATE::LIE_ACTIVE:
    case ANIM_STATE::LIE_LOOP:
        if (MESH_RIGHT == iMeshIndex || MESH_LEFT == iMeshIndex)
            return true;
        break;
    case ANIM_STATE::WALK_ACTIVE:
    case ANIM_STATE::WALK_LOOP:
        if (MESH_CENTER == iMeshIndex)
            return true;
        break;
    case ANIM_STATE::LEAN_ACTIVE:
    case ANIM_STATE::LEAN_LOOP:
        if (MESH_CENTER == iMeshIndex)
            return true;
        break;
    case ANIM_STATE::SHADOWBOXING_ACTIVE:
    case ANIM_STATE::SHADOWBOXING_LOOP:
        if (MESH_RIGHT == iMeshIndex)
            return true;
        break;
    case ANIM_STATE::DANCE2_ACTIVE:
    case ANIM_STATE::DANCE2_LOOP:
        if (MESH_CENTER == iMeshIndex)
            return true;
        break;
    case ANIM_STATE::DANCE3_ACTIVE:
    case ANIM_STATE::DANCE3_LOOP:
        if (MESH_RIGHT == iMeshIndex || MESH_LEFT == iMeshIndex)
            return true;
        break;
    case ANIM_STATE::DRSTRANGE_ACTIVE:
    case ANIM_STATE::DRSTRANGE_LOOP:
        break;
    case ANIM_STATE::DEACTIVE:
        if (MESH_BODY == iMeshIndex || MESH_CENTER == iMeshIndex || MESH_RIGHT == iMeshIndex || MESH_LEFT == iMeshIndex)
            return true;
        break;
    case ANIM_STATE::DEACTIVE_IDLE:
        return true;
        break;
    }

    return false;
}

HRESULT CBody_Pet_Danjinjar::Ready_Components()
{
    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Dissolve"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr), E_FAIL);

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::EMBARS), TEXT("Prototype_Component_Model_NPC_DanjinJar_A"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr)))
        return E_FAIL;

    m_pModelCom->Set_OwnerTransform(&m_pOwnerTransform);

    return S_OK;
}

HRESULT CBody_Pet_Danjinjar::Bind_ShaderResources()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;

    return S_OK;
}

HRESULT CBody_Pet_Danjinjar::Bind_Dissolve()
{
    CHECK_FAILED(m_pTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_DissolveTexture", 0), E_FAIL);

    m_pShaderCom->Bind_RawValue("g_fDecreaseAlpha", &m_pData->fDecreaseAlpha, sizeof(_float));
    m_pShaderCom->Bind_RawValue("g_fEdgeWidth", &m_pData->fEdgeWidth, sizeof(_float));
    m_pShaderCom->Bind_RawValue("g_fEdgeColor", &m_pData->fEdgeColor, sizeof(_float4));

    return S_OK;
}

CBody_Pet_Danjinjar* CBody_Pet_Danjinjar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _int iLevel)
{
    CBody_Pet_Danjinjar* pInstance = new CBody_Pet_Danjinjar(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype(iLevel)))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Failed Create : CBody_Pet_Danjinjar"));
    }
    return pInstance;
}

CGameObject* CBody_Pet_Danjinjar::Clone(void* pArg)
{
    CBody_Pet_Danjinjar* pInstance = new CBody_Pet_Danjinjar(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Failed Clone : CBody_Pet_Danjinjar"));
    }

    return pInstance;
}

void CBody_Pet_Danjinjar::Free()
{
    __super::Free();

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pOwnerTransform);
    Safe_Release(m_pTextureCom);
}
