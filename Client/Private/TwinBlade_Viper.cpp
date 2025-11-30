#include "TwinBlade_Viper.h"
#include "GameInstance.h"
#include "AI_Controller.h"
#include "Viper.h"
#include "Body_Viper.h"


_matrix CTwinBlade_Viper::Get_BoneMatrix(const _char* pBoneName)
{
    _float4x4 BoneMatrix = *m_pModelCom->Get_BoneMatrix(pBoneName);
    _matrix BoneWorld = XMLoadFloat4x4(&BoneMatrix) * XMLoadFloat4x4(&m_CombinedWorldMatrix);
    return BoneWorld;
}

CTwinBlade_Viper::CTwinBlade_Viper(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CPartObject{ pDevice,pContext }
{
}

CTwinBlade_Viper::CTwinBlade_Viper(const CTwinBlade_Viper& Prototype)
    :CPartObject{ Prototype }
{
}

HRESULT CTwinBlade_Viper::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CTwinBlade_Viper::Initialize_Clone(void* pArg)
{
    WEAPON_DESC* pDesc = static_cast<WEAPON_DESC*>(pArg);

    m_pOwnerTransform = pDesc->pOwnerTransform;
    if (!m_pOwnerTransform) return E_FAIL;
    Safe_AddRef(m_pOwnerTransform);

    m_pOwner = pDesc->pOwner;
    if (!m_pOwner) return E_FAIL;

    m_pSocketMatrix = pDesc->pSocketMatrix;

    if (FAILED(__super::Initialize_Clone(pArg))) return E_FAIL;
    if (FAILED(Ready_Components())) return E_FAIL;
    if (FAILED(Ready_Collision())) return E_FAIL;

    m_pTransformCom->Rotation(0.1f, 3.14f, 1.f);

    return S_OK;
}

void CTwinBlade_Viper::Priority_Update(_float fTimeDelta)
{
}

void CTwinBlade_Viper::Update(_float fTimeDelta)
{
 
    if (CViper::PHASE::PHASE1 == m_pOwner->Get_Phase() && m_isActive)
    {
        _matrix BoneMatrix = XMLoadFloat4x4(m_pSocketMatrix);

        for (uint32_t i = 0; i < 3; i++)
            BoneMatrix.r[i] = XMVector3Normalize(BoneMatrix.r[i]);

        XMStoreFloat4x4(
            &m_CombinedWorldMatrix,
            m_pTransformCom->Get_WorldMatrix() * BoneMatrix * XMLoadFloat4x4(m_pParentMatrix)
        );

        _matrix WeaponWorld = XMLoadFloat4x4(&m_CombinedWorldMatrix);

        _vector vScale, vQuat, vPos;
        XMMatrixDecompose(&vScale, &vQuat, &vPos, WeaponWorld);

        if (m_isLOnAttackCollision)
        {
            m_pLeftBodyComp->Collision_Active(true);
            m_pLeftBodyComp->Sync_Update(WeaponWorld);
            m_pLeftBodyComp->Update(fTimeDelta, WeaponWorld, vQuat, vPos);
            XMStoreFloat4(&m_vGrabPos, vPos);

        }
        else if (m_isROnAttackCollision)
        {
            m_pRightBodyComp->Collision_Active(true);
            m_pRightBodyComp->Sync_Update(WeaponWorld);
            m_pRightBodyComp->Update(fTimeDelta, WeaponWorld, vQuat, vPos);
        }
        else
        {
            m_pRightBodyComp->Collision_Active(false);
            m_pLeftBodyComp->Collision_Active(false);
        }



        XMStoreFloat4(&m_vTipPos, vPos);
    }


}

void CTwinBlade_Viper::Late_Update(_float fTimeDelta)
{
    if (CViper::PHASE::PHASE1 == m_pOwner->Get_Phase() && m_isActive)
    {
        if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::DYNAMIC, this)))
            return;
    }
}

HRESULT CTwinBlade_Viper::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    uint32_t iNumMeshes = m_pModelCom->Get_NumMeshes();

    _float fEdgeIntensity = 0.5f;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fEdgeIntensity", &fEdgeIntensity, sizeof(_float))))
        return E_FAIL;

    _float fShadeIntensity = 0.5f;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fShadeIntensity", &fShadeIntensity, sizeof(_float))))
        return E_FAIL;

    _float fEmissiveIntensity = 40.f;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fEmissiveIntensity", &fEmissiveIntensity, sizeof(_float))))
        return E_FAIL;

    for (uint32_t i = 0; i < iNumMeshes; i++)
    {
        m_pModelCom->Bind_Materials(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0);
        m_pModelCom->Bind_Materials(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS, 0);
        m_pModelCom->Bind_Materials(m_pShaderCom, "g_SpecularTexture", i, aiTextureType_SPECULAR, 0);
        m_pModelCom->Bind_Materials(m_pShaderCom, "g_MetalnessTexture", i, aiTextureType_METALNESS, 0);
        m_pModelCom->Bind_Materials(m_pShaderCom, "g_EmissiveTexture", i, aiTextureType_EMISSIVE, 0);

        m_pShaderCom->Begin(15);
        m_pModelCom->Render(i);
    }

    return S_OK;
}

void CTwinBlade_Viper::Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{
    COLLISION_LAYER eLayer = static_cast<COLLISION_LAYER>(iOtherObjectLayer);
    if (COLLISION_LAYER::PLAYER == eLayer)
    {
        m_pOwner->Get_Controller()->AI_React_Collision(pDesc, iOtherObjectLayer, m_pOwner);
    }


}

void CTwinBlade_Viper::Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{
}

void CTwinBlade_Viper::Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, COLLISION_DESC* pMyDesc)
{
}

HRESULT CTwinBlade_Viper::Ready_Components()
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        TEXT("Com_Shader"), (CComponent**)&m_pShaderCom, nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_Component_TwinBlade_Viper"),
        TEXT("Com_Model"), (CComponent**)&m_pModelCom, nullptr)))
        return E_FAIL;

    m_pModelCom->Set_OwnerTransform(&m_pOwnerTransform);

    return S_OK;
}

HRESULT CTwinBlade_Viper::Ready_Collision()
{
    CBody::BODY_SPHERESHAPE_DESC RigthBodyDesc{};
    RigthBodyDesc.fRadius = 2.f;
    RigthBodyDesc.eMotion = EMotionType::Kinematic;
    RigthBodyDesc.eQuality = EMotionQuality::Discrete;
    RigthBodyDesc.eShapeType = SHAPE::SPHERE;
    RigthBodyDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::MONSTERATTACK);
    RigthBodyDesc.bIsTrigger = true;

    _matrix BoneMatrix = XMLoadFloat4x4(m_pSocketMatrix);

    XMStoreFloat4x4(&m_CombinedWorldMatrix,
        m_pTransformCom->Get_WorldMatrix() * BoneMatrix * XMLoadFloat4x4(m_pParentMatrix));

    _vector vScale, vQuat, vPos;
    XMMatrixDecompose(&vScale, &vQuat, &vPos, XMLoadFloat4x4(&m_CombinedWorldMatrix));

    RigthBodyDesc.vPos = _float3(vPos.m128_f32[0], vPos.m128_f32[1], vPos.m128_f32[2]);
    RigthBodyDesc.vQuat = _float4(vQuat.m128_f32[0], vQuat.m128_f32[1], vQuat.m128_f32[2], vQuat.m128_f32[3]);

    RigthBodyDesc.vShapeOffset = _float3(0.f, 0.5f, 0.f);

    m_tCollisionDesc.pGameObject = this;
    RigthBodyDesc.pCollisionDesc = &m_tCollisionDesc;

    if (FAILED(CGameObject::Add_Component(
        ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Body"),
        TEXT("Com_Body_RH"), (CComponent**)&m_pRightBodyComp, &RigthBodyDesc)))
        return E_FAIL;



    CBody::BODY_SPHERESHAPE_DESC LeftBodyDesc{};
    LeftBodyDesc.fRadius = 2.f;
    LeftBodyDesc.eMotion = EMotionType::Kinematic;
    LeftBodyDesc.eQuality = EMotionQuality::Discrete;
    LeftBodyDesc.eShapeType = SHAPE::SPHERE;
    LeftBodyDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::MONSTERATTACK);
    LeftBodyDesc.bIsTrigger = true;


    XMStoreFloat4x4(&m_CombinedWorldMatrix,
        m_pTransformCom->Get_WorldMatrix() * BoneMatrix * XMLoadFloat4x4(m_pParentMatrix));

    XMMatrixDecompose(&vScale, &vQuat, &vPos, XMLoadFloat4x4(&m_CombinedWorldMatrix));

    LeftBodyDesc.vPos = _float3(vPos.m128_f32[0], vPos.m128_f32[1], vPos.m128_f32[2]);
    LeftBodyDesc.vQuat = _float4(vQuat.m128_f32[0], vQuat.m128_f32[1], vQuat.m128_f32[2], vQuat.m128_f32[3]);

    LeftBodyDesc.vShapeOffset = _float3(0.f, -0.5f, 0.f);

    m_tCollisionDesc.pGameObject = this;
    LeftBodyDesc.pCollisionDesc = &m_tCollisionDesc;

    if (FAILED(CGameObject::Add_Component(
        ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Body"),
        TEXT("Com_Body_LH"), (CComponent**)&m_pLeftBodyComp, &LeftBodyDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CTwinBlade_Viper::Bind_ShaderResources()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix",
        m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix",
        m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;

    return S_OK;
}

CTwinBlade_Viper* CTwinBlade_Viper::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CTwinBlade_Viper* pInstance = new CTwinBlade_Viper(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Failed Create : CTwinBlade_Viper"));
    }
    return pInstance;
}

CGameObject* CTwinBlade_Viper::Clone(void* pArg)
{
    CTwinBlade_Viper* pInstance = new CTwinBlade_Viper(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Failed Clone : CTwinBlade_Viper"));
    }

    return pInstance;
}

void CTwinBlade_Viper::Free()
{
    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pOwnerTransform);

    Safe_Release(m_pRightBodyComp);
    Safe_Release(m_pLeftBodyComp);


    __super::Free();
}
