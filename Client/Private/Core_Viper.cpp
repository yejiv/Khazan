#include "Core_Viper.h"
#include "GameInstance.h"
#include "AI_Controller.h"
#include "Viper.h"
#include "Body_Viper.h"


CCore_Viper::CCore_Viper(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CPartObject{ pDevice,pContext }
{
}

CCore_Viper::CCore_Viper(const CCore_Viper& Prototype)
    :CPartObject{ Prototype }
{
}

HRESULT CCore_Viper::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCore_Viper::Initialize_Clone(void* pArg)
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

    m_pTransformCom->Rotation(3.2f, 3.14f, 4.5f);
    //m_vLocalOffset = { x = -0.0333378017 y = -0.133596793 z = 0.00000000 }
    m_vLocalOffset = _float3(-0.03f, -0.13f, 0.f);
    Set_IsActive(false);


    return S_OK;
}

void CCore_Viper::Priority_Update(_float fTimeDelta)
{

}

void CCore_Viper::Update(_float fTimeDelta)
{

    if (CViper::PHASE::PHASE1 == m_pOwner->Get_Phase() && m_isActive)
    {
       
        _matrix BoneMatrix = XMLoadFloat4x4(m_pSocketMatrix);

        _float fMoveSpeed = 2.f;
        _float3 offset = m_vLocalOffset;

       if (m_pGameInstance->Key_Down(DIK_UP))
           offset.y += fMoveSpeed * fTimeDelta;
       if (m_pGameInstance->Key_Down(DIK_DOWN))
           offset.y -= fMoveSpeed * fTimeDelta;

       if (m_pGameInstance->Key_Down(DIK_LEFT))
           offset.x -= fMoveSpeed * fTimeDelta;

       if (m_pGameInstance->Key_Down(DIK_RIGHT))
           offset.x += fMoveSpeed * fTimeDelta;

       if (m_pGameInstance->Key_Down(DIK_L))
           offset.z += fMoveSpeed * fTimeDelta;

       if (m_pGameInstance->Key_Down(DIK_K))
           offset.z -= fMoveSpeed * fTimeDelta;

           m_vLocalOffset = offset;

           

        for (uint32_t i = 0; i < 3; i++)
            BoneMatrix.r[i] = XMVector3Normalize(BoneMatrix.r[i]);

        _matrix OffsetMatrix = XMMatrixTranslation(
            m_vLocalOffset.x,
            m_vLocalOffset.y,
            m_vLocalOffset.z
        );

        XMStoreFloat4x4(
            &m_CombinedWorldMatrix,
            m_pTransformCom->Get_WorldMatrix() * OffsetMatrix * BoneMatrix * XMLoadFloat4x4(m_pParentMatrix)
        );

        _matrix WeaponWorld = XMLoadFloat4x4(&m_CombinedWorldMatrix);

        _vector vScale, vQuat, vPos;
        XMMatrixDecompose(&vScale, &vQuat, &vPos, WeaponWorld);

        m_pBodyComp->Sync_Update(WeaponWorld);
        m_pBodyComp->Update(fTimeDelta, WeaponWorld, vQuat, vPos);
    }
}

void CCore_Viper::Late_Update(_float fTimeDelta)
{
    if (CViper::PHASE::PHASE1 == m_pOwner->Get_Phase() && m_isActive)
    {
        if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::DYNAMIC, this)))
            return;
    }
}

HRESULT CCore_Viper::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

   /* _float fEdgeIntensity = 0.5f;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fEdgeIntensity", &fEdgeIntensity, sizeof(_float))))
        return E_FAIL;

    _float fShadeIntensity = 0.5f;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fShadeIntensity", &fShadeIntensity, sizeof(_float))))
        return E_FAIL;

    _float fEmissiveIntensity = 40.f;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fEmissiveIntensity", &fEmissiveIntensity, sizeof(_float))))
        return E_FAIL;

    uint32_t iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (uint32_t i = 0; i < iNumMeshes; i++)
    {
        m_pModelCom->Bind_Materials(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0);
        m_pModelCom->Bind_Materials(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS, 0);
        m_pModelCom->Bind_Materials(m_pShaderCom, "g_SpecularTexture", i, aiTextureType_SPECULAR, 0);
        m_pModelCom->Bind_Materials(m_pShaderCom, "g_MetalnessTexture", i, aiTextureType_METALNESS, 0);
        m_pModelCom->Bind_Materials(m_pShaderCom, "g_EmissiveTexture", i, aiTextureType_EMISSIVE, 0);
        m_pShaderCom->Begin(0);
        m_pModelCom->Render(i);
    }*/

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

void CCore_Viper::Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{
    /*COLLISION_LAYER eLayer = static_cast<COLLISION_LAYER>(iOtherObjectLayer);
    if (COLLISION_LAYER::PLAYER == eLayer)
    {
        m_pOwner->Get_Controller()->AI_React_Collision(pDesc, iOtherObjectLayer, m_pOwner);
    }*/


}

void CCore_Viper::Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{
}

void CCore_Viper::Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, COLLISION_DESC* pMyDesc)
{
}

HRESULT CCore_Viper::Ready_Components()
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), (CComponent**)&m_pShaderCom, nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_Component_Core_Viper"),
        TEXT("Com_Model"), (CComponent**)&m_pModelCom, nullptr)))
        return E_FAIL;

    m_pModelCom->Set_OwnerTransform(&m_pOwnerTransform);

    return S_OK;
}

HRESULT CCore_Viper::Ready_Collision()
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

    BodyDesc.vShapeOffset = _float3(0.f, 0.f, 0.f);

    m_tCollisionDesc.pGameObject = this;
    BodyDesc.pCollisionDesc = &m_tCollisionDesc;

    if (FAILED(CGameObject::Add_Component(
        ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Body"),
        TEXT("Com_Body_RH"), (CComponent**)&m_pBodyComp, &BodyDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CCore_Viper::Bind_ShaderResources()
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

CCore_Viper* CCore_Viper::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CCore_Viper* pInstance = new CCore_Viper(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Failed Create : CCore_Viper"));
    }
    return pInstance;
}

CGameObject* CCore_Viper::Clone(void* pArg)
{
    CCore_Viper* pInstance = new CCore_Viper(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Failed Clone : CTwinBlade_Viper"));
    }

    return pInstance;
}

void CCore_Viper::Free()
{
    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pOwnerTransform);
    Safe_Release(m_pBodyComp);
    __super::Free();
}
