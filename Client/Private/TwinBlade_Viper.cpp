#include "TwinBlade_Viper.h"
#include "GameInstance.h"
#include "AI_Controller.h"
#include "Viper.h"


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

    //m_pTransformCom->Rotation(1.5, 1.5, 0.5f);
    //m_vDebugRotation = _float3(-0, 3.22f, -2.6f);
    //XMStoreFloat3(&m_vLocalWeaponOffset, XMVectorSet(-0.4f, -0.6f, -0.5f, 1.f)) ;
    //m_vLocalWeaponOffset = {x=-0.400000006 y=-0.600000024 z=-0.500000000 }
    m_OffsetMatrix = XMMatrixRotationX(XMConvertToRadians(-90.f));
  
    return S_OK;
}

void CTwinBlade_Viper::Priority_Update(_float fTimeDelta)
{
}

void CTwinBlade_Viper::Update(_float fTimeDelta)
{

    if (m_pGameInstance->Key_Down(DIK_J))   // X축 +90도
    {
        m_vDebugRotation.x += XMConvertToRadians(5.f);
        cout << "Rotate X +90 : " << m_vDebugRotation.x << endl;
    }

    if (m_pGameInstance->Key_Down(DIK_K))   // Y축 +90도
    {
        m_vDebugRotation.y += XMConvertToRadians(5.f);
        cout << "Rotate Y +90 : " << m_vDebugRotation.y << endl;
    }

    if (m_pGameInstance->Key_Down(DIK_L))   // Z축 +90도
    {
        m_vDebugRotation.z += XMConvertToRadians(5.f);
        cout << "Rotate Z +90 : " << m_vDebugRotation.z << endl;
    }

    if (m_pGameInstance->Key_Down(DIK_B))   // X축 +90도
    {
        m_vDebugRotation.x -= XMConvertToRadians(5.f);
        cout << "Rotate X +90 : " << m_vDebugRotation.x << endl;
    }

    if (m_pGameInstance->Key_Down(DIK_N))   // Y축 +90도
    {
        m_vDebugRotation.y -= XMConvertToRadians(5.f);
        cout << "Rotate Y +90 : " << m_vDebugRotation.y << endl;
    }

    if (m_pGameInstance->Key_Down(DIK_M))   // Z축 +90도
    {
        m_vDebugRotation.z -= XMConvertToRadians(5.f);
        cout << "Rotate Z +90 : " << m_vDebugRotation.z << endl;
    }

    // 누적된 각도를 TransformCom에 반영
    //m_pTransformCom->Rotation(
    //    m_vDebugRotation.x,
    //    m_vDebugRotation.y,
    //    m_vDebugRotation.z
    //);



    //if (m_pGameInstance->Key_Down(DIK_J))
    //{
    //    m_vLocalWeaponOffset.x += 0.1f;
    //    cout << "Offset X: " << m_vLocalWeaponOffset.x << endl;
    //}
    //if (m_pGameInstance->Key_Down(DIK_B))
    //{
    //    m_vLocalWeaponOffset.x -= 0.1f;
    //    cout << "Offset X: " << m_vLocalWeaponOffset.x << endl;
    //}

    //// Y축 이동
    //if (m_pGameInstance->Key_Down(DIK_K))
    //{
    //    m_vLocalWeaponOffset.y += 0.1f;
    //    cout << "Offset Y: " << m_vLocalWeaponOffset.y << endl;
    //}
    //if (m_pGameInstance->Key_Down(DIK_N))
    //{
    //    m_vLocalWeaponOffset.y -= 0.1f;
    //    cout << "Offset Y: " << m_vLocalWeaponOffset.y << endl;
    //}

    //// Z축 이동
    //if (m_pGameInstance->Key_Down(DIK_L))
    //{
    //    m_vLocalWeaponOffset.z += 0.1f;
    //    cout << "Offset Z: " << m_vLocalWeaponOffset.z << endl;
    //}
    //if (m_pGameInstance->Key_Down(DIK_M))
    //{
    //    m_vLocalWeaponOffset.z -= 0.1f;
    //    cout << "Offset Z: " << m_vLocalWeaponOffset.z << endl;
    //}


    _matrix BoneMatrix = XMLoadFloat4x4(m_pSocketMatrix);
    //_matrix OffsetMatrix = XMMatrixTranslationFromVector(XMLoadFloat3(&m_vLocalWeaponOffset));

    for (uint32_t i = 0; i < 3; i++)
        BoneMatrix.r[i] = XMVector3Normalize(BoneMatrix.r[i]);

    m_pModelCom->Update_BoneCombinedMatrices();

    XMStoreFloat4x4(
        &m_CombinedWorldMatrix,
        /*m_pTransformCom->Get_WorldMatrix() **/ m_OffsetMatrix * BoneMatrix * XMLoadFloat4x4(m_pParentMatrix)
    );


  /*  _matrix WeaponWorld = XMLoadFloat4x4(&m_CombinedWorldMatrix);

    _vector vScale, vQuat, vPos;
    XMMatrixDecompose(&vScale, &vQuat, &vPos, WeaponWorld);
    XMStoreFloat4(&m_vTipPos, vPos);*/

}

void CTwinBlade_Viper::Late_Update(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::DYNAMIC, this)))
        return;
}

HRESULT CTwinBlade_Viper::Render()
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

void CTwinBlade_Viper::Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal)
{
    COLLISION_LAYER eLayer = static_cast<COLLISION_LAYER>(iOtherObjectLayer);
    if (COLLISION_LAYER::PLAYER == eLayer)
    {
        m_pOwner->Get_Controller()->AI_React_Collision(pDesc, iOtherObjectLayer, m_pOwner);
    }


}

void CTwinBlade_Viper::Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal)
{
}

void CTwinBlade_Viper::Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer)
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
    //CBody::BODY_SPHERESHAPE_DESC BodyDesc{};
    //BodyDesc.fRadius = 0.05f;
    //BodyDesc.eMotion = EMotionType::Kinematic;
    //BodyDesc.eQuality = EMotionQuality::Discrete;
    //BodyDesc.eShapeType = SHAPE::SPHERE;
    //BodyDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::MONSTERATTACK);
    //BodyDesc.bIsTrigger = true;

    //_matrix BoneMatrix = XMLoadFloat4x4(m_pSocketMatrix);

    //XMStoreFloat4x4(&m_CombinedWorldMatrix,
    //    m_pTransformCom->Get_WorldMatrix() * BoneMatrix * XMLoadFloat4x4(m_pParentMatrix));

    //_vector vScale, vQuat, vPos;
    //XMMatrixDecompose(&vScale, &vQuat, &vPos, XMLoadFloat4x4(&m_CombinedWorldMatrix));

    //BodyDesc.vPos = _float3(vPos.m128_f32[0], vPos.m128_f32[1], vPos.m128_f32[2]);
    //BodyDesc.vQuat = _float4(vQuat.m128_f32[0], vQuat.m128_f32[1], vQuat.m128_f32[2], vQuat.m128_f32[3]);

    //BodyDesc.vShapeOffset = _float3(0.f, 0.75f, 0.f);

    //m_tCollisionDesc.pGameObject = this;
    //BodyDesc.pCollisionDesc = &m_tCollisionDesc;

    //if (FAILED(CGameObject::Add_Component(
    //    ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Body"),
    //    TEXT("Com_Body_RH"), (CComponent**)&m_pBodyComp, &BodyDesc)))
    //    return E_FAIL;

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

    __super::Free();
}
