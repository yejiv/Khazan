#include "TwinBlade_R_Viper.h"
#include "GameInstance.h"
#include "Viper.h"
#include "AI_Controller.h"




_matrix CTwinBlade_R_Viper::Get_BoneMatrix(const _char* pBoneName)
{
    _float4x4 BoneMatrix = *m_pModelCom->Get_BoneMatrix(pBoneName);
    _matrix BoneWorld = XMLoadFloat4x4(&BoneMatrix) * XMLoadFloat4x4(&m_CombinedWorldMatrix);
    return BoneWorld;
}

CTwinBlade_R_Viper::CTwinBlade_R_Viper(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CPartObject{ pDevice,pContext }
{
}

CTwinBlade_R_Viper::CTwinBlade_R_Viper(const CTwinBlade_R_Viper& Prototype)
    :CPartObject{ Prototype }
{
}

HRESULT CTwinBlade_R_Viper::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CTwinBlade_R_Viper::Initialize_Clone(void* pArg)
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

    _matrix tempMat = XMMatrixRotationZ(XMConvertToRadians(90.0f)) * XMMatrixRotationX(XMConvertToRadians(90.0f)) * XMMatrixRotationX(XMConvertToRadians(90.0f));
    XMStoreFloat4x4(&m_matOffset, tempMat);

    return S_OK;
}

void CTwinBlade_R_Viper::Priority_Update(_float fTimeDelta)
{
}

void CTwinBlade_R_Viper::Update(_float fTimeDelta)
{

    if (m_pOwner->Get_Controller()->Get_BlackBoard()->Get_Value<_bool>(m_pOwner->Get_Name(), "isP2LockOn"))
    {
        // ¶ô¿Â Offset
        _matrix tempMat = XMMatrixRotationZ(XMConvertToRadians(180.0f)) * XMMatrixRotationX(XMConvertToRadians(60.0f)) * XMMatrixRotationX(XMConvertToRadians(90.0f));
        XMStoreFloat4x4(&m_matOffset, tempMat);
    }
    else if (m_pOwner->Get_Controller()->Get_BlackBoard()->Get_Value<_bool>(m_pOwner->Get_Name(), "isP2Cinematic_Walk"))
    {
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

        //m_vLocalOffset = { x = -0.833458841 y = 2.79396772e-06 z = 0.00000000 }
        //m_vLocalOffset = offset;
        m_vLocalOffset = _float3(-0.83, 0.f, 0.f);
    }
    else
    {
        _matrix tempMat = XMMatrixRotationZ(XMConvertToRadians(90.0f)) * XMMatrixRotationX(XMConvertToRadians(90.0f)) * XMMatrixRotationX(XMConvertToRadians(90.0f));
        XMStoreFloat4x4(&m_matOffset, tempMat);
        m_vLocalOffset = _float3(0.f, 0.f, 0.f);
    }


    if (CViper::PHASE::PHASE2 == m_pOwner->Get_Phase() && m_isActive)
    {
        _matrix BoneMatrix = XMLoadFloat4x4(m_pSocketMatrix);
        
        for (_uint i = 0; i < 3; i++)
        {
            BoneMatrix.r[i] = XMVector3Normalize(BoneMatrix.r[i]);
        }

        _matrix OffsetMatrix = XMMatrixTranslation(
            m_vLocalOffset.x,
            m_vLocalOffset.y,
            m_vLocalOffset.z
        );

      
        m_pModelCom->Update_BoneCombinedMatrices();

        XMStoreFloat4x4(&m_CombinedWorldMatrix, XMLoadFloat4x4(&m_matOffset) * OffsetMatrix * BoneMatrix * XMLoadFloat4x4(m_pParentMatrix));


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

   

}

void CTwinBlade_R_Viper::Late_Update(_float fTimeDelta)
{
    if (CViper::PHASE::PHASE2 == m_pOwner->Get_Phase() && m_isActive)
    {
        if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::DYNAMIC, this)))
            return;
    }
}

HRESULT CTwinBlade_R_Viper::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _float fEdgeIntensity = 0.5f;
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

        m_pShaderCom->Begin(18);
        m_pModelCom->Render(i);
    }

    return S_OK;
}

void CTwinBlade_R_Viper::Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{
    COLLISION_LAYER eLayer = static_cast<COLLISION_LAYER>(iOtherObjectLayer);
    if (COLLISION_LAYER::PLAYER == eLayer)
    {
        m_pOwner->Get_Controller()->AI_React_Collision(pDesc, iOtherObjectLayer, m_pOwner);
    }


}

void CTwinBlade_R_Viper::Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{
}

void CTwinBlade_R_Viper::Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, COLLISION_DESC* pMyDesc)
{
}

HRESULT CTwinBlade_R_Viper::Ready_Components()
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        TEXT("Com_Shader"), (CComponent**)&m_pShaderCom, nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_Component_TwinBlade_R"),
        TEXT("Com_Model"), (CComponent**)&m_pModelCom, nullptr)))
        return E_FAIL;

    m_pModelCom->Set_OwnerTransform(&m_pOwnerTransform);

    return S_OK;
}

HRESULT CTwinBlade_R_Viper::Ready_Collision()
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

HRESULT CTwinBlade_R_Viper::Bind_ShaderResources()
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

CTwinBlade_R_Viper* CTwinBlade_R_Viper::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CTwinBlade_R_Viper* pInstance = new CTwinBlade_R_Viper(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Failed Create : CTwinBlade_R_Viper"));
    }
    return pInstance;
}

CGameObject* CTwinBlade_R_Viper::Clone(void* pArg)
{
    CTwinBlade_R_Viper* pInstance = new CTwinBlade_R_Viper(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Failed Clone : CTwinBlade_R_Viper"));
    }

    return pInstance;
}

void CTwinBlade_R_Viper::Free()
{
    __super::Free();

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pOwnerTransform);
    Safe_Release(m_pBodyComp);

}
