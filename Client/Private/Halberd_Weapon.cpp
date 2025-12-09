#include "Halberd_Weapon.h"
#include "GameInstance.h"
#include "AI_Controller.h"

CHalberd_Weapon::CHalberd_Weapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CWeaponObject{ pDevice,pContext }
{
}

CHalberd_Weapon::CHalberd_Weapon(const CHalberd_Weapon& Prototype)
    :CWeaponObject(Prototype)
{
}

HRESULT CHalberd_Weapon::Initialize_Prototype(_int iLevel)
{
    m_iPrototypeIndex = iLevel;
    return S_OK;
}

HRESULT CHalberd_Weapon::Initialize_Clone(void* pArg)
{
    WEAPON_DESC* pDesc = static_cast<WEAPON_DESC*>(pArg);
    m_pData = pDesc->pData;
    m_pSocketMatrix = pDesc->pSocketMatrix;

    m_pOwnerTransform = pDesc->pOwnerTransform;
    CHECK_NULLPTR(m_pOwnerTransform, E_FAIL);
    Safe_AddRef(m_pOwnerTransform);

    CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);
    m_pTransformCom->Rotation(XMConvertToRadians(0.f), XMConvertToRadians(180.f), XMConvertToRadians(0.f));
    CHECK_FAILED(Ready_Components(), E_FAIL);
    _float4x4 PreTransformMatrix;
    XMStoreFloat4x4(&PreTransformMatrix, XMMatrixScaling(0.0001, 0.0001, 0.0001) * XMMatrixRotationY(XMConvertToRadians(180.0f)));
    m_pModelCom->Set_PreTransformMatrix(PreTransformMatrix);

    CHECK_FAILED(Ready_Collision(), E_FAIL);


    return S_OK;
}

void CHalberd_Weapon::Priority_Update(_float fTimeDelta)
{
}

void CHalberd_Weapon::Update(_float fTimeDelta)
{
    m_pTransformCom->Rotation(XMConvertToRadians(90.f), XMConvertToRadians(180.f), XMConvertToRadians(0.f));

    _matrix BoneMatrix = XMLoadFloat4x4(m_pSocketMatrix);

    for (uint32_t i = 0; i < 3; i++)
        BoneMatrix.r[i] = XMVector3Normalize(BoneMatrix.r[i]);

    XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrix() * BoneMatrix * XMLoadFloat4x4(m_pParentMatrix));

    m_pBodyComp->Collision_Active(m_pData->isAttack_Collinder);

    if (!m_pData->isAttack_Collinder)
        return;
    _matrix WeaponWorld = XMLoadFloat4x4(&m_CombinedWorldMatrix);

    _vector vScale, vQuat, vPos;
    XMMatrixDecompose(&vScale, &vQuat, &vPos, WeaponWorld);

    m_pBodyComp->Sync_Update(WeaponWorld);
    m_pBodyComp->Update(fTimeDelta, WeaponWorld, vQuat, vPos);

}

void CHalberd_Weapon::Late_Update(_float fTimeDelta)
{
    CHECK_FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::DYNAMIC, this),);
}

HRESULT CHalberd_Weapon::Render()
{
    CHECK_FAILED(Bind_ShaderResources(), E_FAIL);
    CHECK_FAILED(Bind_Dissolve(), E_FAIL);
    uint32_t iNumMeshes = m_pModelCom->Get_NumMeshes();

    _float fEdgeIntensity = 0.8f;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fEdgeIntensity", &fEdgeIntensity, sizeof(_float))))
        return E_FAIL;

    _float fShadeIntensity = 0.2f;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fShadeIntensity", &fShadeIntensity, sizeof(_float))))
        return E_FAIL;

    for (uint32_t i = 0; i < iNumMeshes; i++)
    {
        m_pModelCom->Bind_Materials(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0);
        m_pModelCom->Bind_Materials(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS, 0);
        m_pModelCom->Bind_Materials(m_pShaderCom, "g_SpecularTexture", i, aiTextureType_SPECULAR, 0);
        m_pModelCom->Bind_Materials(m_pShaderCom, "g_MetalnessTexture", i, aiTextureType_METALNESS, 0);

        m_pShaderCom->Begin(9);
        m_pModelCom->Render(i);
    }

    return S_OK;
}

void CHalberd_Weapon::Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{
    COLLISION_LAYER eLayer = static_cast<COLLISION_LAYER>(iOtherObjectLayer);
    if (COLLISION_LAYER::PLAYER == eLayer)
    {
        m_pData->pOwner->Get_Controller()->AI_React_Collision(pDesc, iOtherObjectLayer, m_pData->pOwner);
    }
}

void CHalberd_Weapon::Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{
}

void CHalberd_Weapon::Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, COLLISION_DESC* pMyDesc)
{
}

HRESULT CHalberd_Weapon::Ready_Components()
{
    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Dissolve"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr), E_FAIL);

    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom, nullptr), E_FAIL);
    CHECK_FAILED(CGameObject::Add_Component(m_iPrototypeIndex, TEXT("Prototype_Component_Weapon_Halberd"), TEXT("Com_Model"), (CComponent**)&m_pModelCom, nullptr), E_FAIL);

    m_pModelCom->Set_OwnerTransform(&m_pOwnerTransform);

    return S_OK;
}

HRESULT CHalberd_Weapon::Ready_Collision()
{
    m_tCollisionDesc.pGameObject = this;

    CBody::BODY_BOXSHAPE_DESC BodyDesc{};
    BodyDesc.vExtent = { 1.f, 2.f, 0.5f };
    BodyDesc.eMotion = EMotionType::Kinematic;
    BodyDesc.eQuality = EMotionQuality::Discrete;
    BodyDesc.eShapeType = SHAPE::BOX;
    BodyDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::MONSTERATTACK);
    BodyDesc.bIsTrigger = true;

    XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrix() * XMLoadFloat4x4(m_pSocketMatrix) * XMLoadFloat4x4(m_pParentMatrix));
    BodyDesc.vPos = { m_CombinedWorldMatrix._41, m_CombinedWorldMatrix._42, m_CombinedWorldMatrix._43 };
    XMStoreFloat4(&BodyDesc.vQuat, XMQuaternionRotationMatrix(XMLoadFloat4x4(&m_CombinedWorldMatrix)));

    BodyDesc.vShapeOffset = _float3(0.f, 1.8f, 0.f);
    BodyDesc.pCollisionDesc = &m_tCollisionDesc;

    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Body"), TEXT("Com_Body_RH"), (CComponent**)&m_pBodyComp, &BodyDesc), E_FAIL);


    return S_OK;
}

HRESULT CHalberd_Weapon::Bind_ShaderResources()
{
    CHECK_FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix), E_FAIL);
    CHECK_FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW)), E_FAIL);
    CHECK_FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ)), E_FAIL);

    return S_OK;
}

HRESULT CHalberd_Weapon::Bind_Dissolve()
{
    CHECK_FAILED(m_pTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_DissolveTexture", 0), E_FAIL);

    m_pShaderCom->Bind_RawValue("g_fDecreaseAlpha", &m_pData->fDecreaseAlpha, sizeof(_float));
    m_pShaderCom->Bind_RawValue("g_fEdgeWidth", &m_pData->fEdgeWidth, sizeof(_float));
    m_pShaderCom->Bind_RawValue("g_fEdgeColor", &m_pData->fEdgeColor, sizeof(_float4));

    return S_OK;
}

CHalberd_Weapon* CHalberd_Weapon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _int iLevel)
{
    CHalberd_Weapon* pInstance = new CHalberd_Weapon(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype(iLevel)))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Failed Create : CHalberd_Weapon"));
    }
    return pInstance;
}

CGameObject* CHalberd_Weapon::Clone(void* pArg)
{
    CHalberd_Weapon* pInstance = new CHalberd_Weapon(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Failed Clone : CHalberd_Weapon"));
    }

    return pInstance;
}

void CHalberd_Weapon::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);
    Safe_Release(m_pOwnerTransform);
    Safe_Release(m_pBodyComp);
    Safe_Release(m_pTextureCom);

}
