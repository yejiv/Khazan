
#include "Imp_Sword.h"
#include "Model.h"
#include "GameInstance.h"
#include "Imp_Melee.h"
#include "AI_Controller.h"


_float4* CImp_Sword::Get_BonePointEX(const _char* pBoneName)
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

_matrix CImp_Sword::Get_BoneMatrix(const _char* pBoneName)
{
    _float4x4 BoneMatrix = *m_pModelCom->Get_BoneMatrix(pBoneName);
    _matrix BoneWorld = XMLoadFloat4x4(&BoneMatrix) * XMLoadFloat4x4(&m_CombinedWorldMatrix);
    return BoneWorld;
}

CImp_Sword::CImp_Sword(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CPartObject{ pDevice,pContext }
{
}

CImp_Sword::CImp_Sword(const CImp_Sword& Prototype)
    :CPartObject{ Prototype }
{
}

HRESULT CImp_Sword::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CImp_Sword::Initialize_Clone(void* pArg)
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

    m_pTransformCom->Rotation(XMConvertToRadians(-90.f), 0.f, 0.f);

    return S_OK;
}

void CImp_Sword::Priority_Update(_float fTimeDelta)
{
}

void CImp_Sword::Update(_float fTimeDelta)
{
    _matrix BoneMatrix = XMLoadFloat4x4(m_pSocketMatrix);

    for (uint32_t i = 0; i < 3; i++)
        BoneMatrix.r[i] = XMVector3Normalize(BoneMatrix.r[i]);

    XMStoreFloat4x4(
        &m_CombinedWorldMatrix,
        m_pTransformCom->Get_WorldMatrix() * BoneMatrix * XMLoadFloat4x4(m_pParentMatrix)
    );


    m_pBodyComp->Collision_Active(m_isOnAttackCollision);
    if (m_isOnAttackCollision)
    {
        _matrix WeaponWorld = XMLoadFloat4x4(&m_CombinedWorldMatrix);

        _vector vScale, vQuat, vPos;
        XMMatrixDecompose(&vScale, &vQuat, &vPos, WeaponWorld);

        m_pBodyComp->Sync_Update(WeaponWorld);
        m_pBodyComp->Update(fTimeDelta, WeaponWorld, vQuat, vPos);
    }
   
    //XMStoreFloat4(&m_vTipPos,vPos);
    
}

void CImp_Sword::Late_Update(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::DYNAMIC, this)))
        return;
}

HRESULT CImp_Sword::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    uint32_t iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (uint32_t i = 0; i < iNumMeshes; i++)
    {
        m_pModelCom->Bind_Materials(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0);
        m_pModelCom->Bind_Materials(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS, 0);

        m_pShaderCom->Begin(0);
        m_pModelCom->Render(i);
    }

    return S_OK;
}

void CImp_Sword::Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{
    COLLISION_LAYER eLayer = static_cast<COLLISION_LAYER>(iOtherObjectLayer);
    if (COLLISION_LAYER::PLAYER == eLayer)
    {
        m_pOwner->Get_Controller()->AI_React_Collision(pDesc,iOtherObjectLayer,m_pOwner);
    }


}

void CImp_Sword::Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{
}

void CImp_Sword::Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, COLLISION_DESC* pMyDesc)
{
}

HRESULT CImp_Sword::Ready_Components()
{
    //if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxMesh"),
    //    TEXT("Com_Shader"), (CComponent**)&m_pShaderCom, nullptr)))
    //    return E_FAIL;

   /* if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_ImpSword"),
        TEXT("Com_Model"), (CComponent**)&m_pModelCom, nullptr)))
        return E_FAIL;*/

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        TEXT("Com_Shader"), (CComponent**)&m_pShaderCom, nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Component_TwinBlade_Viper"),
        TEXT("Com_Model"), (CComponent**)&m_pModelCom, nullptr)))
        return E_FAIL;


    m_pModelCom->Set_OwnerTransform(&m_pOwnerTransform);

    return S_OK;
}

HRESULT CImp_Sword::Ready_Collision()
{
    CBody::BODY_SPHERESHAPE_DESC BodyDesc{};
    BodyDesc.fRadius = 0.05f;
    BodyDesc.eMotion = EMotionType::Kinematic;
    BodyDesc.eQuality = EMotionQuality::Discrete;
    BodyDesc.eShapeType = SHAPE::SPHERE;
    BodyDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::MONSTERATTACK);
    BodyDesc.bIsTrigger = true;

    _matrix BoneMatrix = XMLoadFloat4x4(m_pSocketMatrix);

    XMStoreFloat4x4(&m_CombinedWorldMatrix,
        m_pTransformCom->Get_WorldMatrix() * BoneMatrix * XMLoadFloat4x4(m_pParentMatrix));

    _vector vScale, vQuat, vPos;
    XMMatrixDecompose(&vScale, &vQuat, &vPos, XMLoadFloat4x4(&m_CombinedWorldMatrix));

    BodyDesc.vPos = _float3(vPos.m128_f32[0], vPos.m128_f32[1], vPos.m128_f32[2]);
    BodyDesc.vQuat = _float4(vQuat.m128_f32[0], vQuat.m128_f32[1], vQuat.m128_f32[2], vQuat.m128_f32[3]);

    BodyDesc.vShapeOffset = _float3(0.f, 0.75f, 0.f);

    m_tCollisionDesc.pGameObject = this;
    BodyDesc.pCollisionDesc = &m_tCollisionDesc;

    if (FAILED(CGameObject::Add_Component(
        ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Body"),
        TEXT("Com_Body_RH"), (CComponent**)&m_pBodyComp, &BodyDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CImp_Sword::Bind_ShaderResources()
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

CImp_Sword* CImp_Sword::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CImp_Sword* pInstance = new CImp_Sword(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Failed Create : CImp_Sword"));
    }
    return pInstance;
}

CGameObject* CImp_Sword::Clone(void* pArg)
{
    CImp_Sword* pInstance = new CImp_Sword(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Failed Clone : CImp_Sword"));
    }

    return pInstance;
}

void CImp_Sword::Free()
{
    __super::Free();

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pOwnerTransform);
    Safe_Release(m_pBodyComp);
    
}
