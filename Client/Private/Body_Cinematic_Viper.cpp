#include "Body_Cinematic_Viper.h"
#include "GameInstance.h"
#include "BlackBoard.h"
#include "Viper.h"
#include "AI_Controller.h"


_matrix CBody_Cinematic_Viper::Get_BoneMatrix(const _char* pBoneName)
{
    _float4x4 BoneMatrix = *m_pModelCom->Get_BoneMatrix(pBoneName);
    _matrix BoneWorld = XMLoadFloat4x4(&BoneMatrix) * XMLoadFloat4x4(&m_CombinedWorldMatrix);

    return BoneWorld;
}


_float4x4* CBody_Cinematic_Viper::Get_BoneMatrix_Ptr(const _char* pBoneName)
{
    return m_pModelCom->Get_BoneMatrix(pBoneName);
}


CBody_Cinematic_Viper::CBody_Cinematic_Viper(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CPartObject{ pDevice,pContext }
{
}

CBody_Cinematic_Viper::CBody_Cinematic_Viper(const CBody_Cinematic_Viper& Prototype)
    :CPartObject{ Prototype }
{
}

HRESULT CBody_Cinematic_Viper::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBody_Cinematic_Viper::Initialize_Clone(void* pArg)
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

    /* if (FAILED(Ready_Colliders()))
         return E_FAIL;*/

    m_vRimColor = _float3(1.f, 0.8f, 0.6f);
    m_fRimEmissive = 3.f;
    m_fMaxRimEmissive = 7.f;
    m_fRimIntensity = 1.f;
    m_fRimDuration = 20.f;

    return S_OK;
}

void CBody_Cinematic_Viper::Priority_Update(_float fTimeDelta)
{

}

void CBody_Cinematic_Viper::Update(_float fTimeDelta)
{
    if (true == m_isAccRimEmissive)
    {
        m_fTimeAcc += fTimeDelta;

        _float fRatio = m_fTimeAcc / m_fRimDuration;

        if (fRatio >= 1.f)
            fRatio = 1.f;

        m_fRimEmissive = Lerp(m_fRimEmissive, m_fMaxRimEmissive, fRatio);
    }

    if (CViper::PHASE::CINEMATIC == m_pOwner->Get_Phase())
        Update_CombinedMatrix();
}

void CBody_Cinematic_Viper::Late_Update(_float fTimeDelta)
{

    if (CViper::PHASE::CINEMATIC == m_pOwner->Get_Phase())
    {
        if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::DYNAMIC, this)))
            return;
    }
}

HRESULT CBody_Cinematic_Viper::Render()
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

    _float fEmissiveIntensity = 10.f;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fEmissiveIntensity", &fEmissiveIntensity, sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Bool("g_isEnableRimLight", &m_isEnableRimLight)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Bool("g_isBlinkRimLight", &m_isBlinkRimLight)))
        return E_FAIL;

    _float fRimPower = 2.f;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fRimPower", &fRimPower, sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_fRimLightIntensity", &m_fRimIntensity, sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vRimColor", &m_vRimColor, sizeof(_float3))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_fRimEmissive", &m_fRimEmissive, sizeof(_float))))
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

void CBody_Cinematic_Viper::Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{
    /* COLLISION_LAYER eType = static_cast<COLLISION_LAYER>(iOtherObjectLayer);
     m_pOwner->Get_Controller()->AI_React_Collision(pDesc, iOtherObjectLayer, m_pOwner);*/
}

void CBody_Cinematic_Viper::Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{

}

void CBody_Cinematic_Viper::Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, COLLISION_DESC* pMyDesc)
{

}


HRESULT CBody_Cinematic_Viper::Ready_Components()
{

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::VIPER), TEXT("Prototype_Component_Model_Viper_Cinematic"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr)))
        return E_FAIL;

    m_pModelCom->Set_OwnerTransform(&m_pOwnerTransform);

    return S_OK;

}



HRESULT CBody_Cinematic_Viper::Bind_ShaderResources()
{

    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;


    return S_OK;
}

void CBody_Cinematic_Viper::Carculate_Matrix(_float fTimeDelta)
{

    //_float4x4 BoneMatrix = *m_pModelCom->Get_BoneMatrix("Weapon_R");
    //XMStoreFloat4x4(&m_RightHandMatrix, m_pTransformCom->Get_WorldMatrix() * XMLoadFloat4x4(&BoneMatrix) * XMLoadFloat4x4(m_pParentMatrix));
    //_vector vOutQuat, vOutPos;
    //m_pRH_BodyCom->Sync_Update(XMLoadFloat4x4(&m_RightHandMatrix));
    //m_pRH_BodyCom->Update(fTimeDelta, XMLoadFloat4x4(&m_RightHandMatrix), vOutQuat, vOutPos);

    //m_RightHandMatrix._41 = vOutPos.m128_f32[0];
    //m_RightHandMatrix._42 = vOutPos.m128_f32[1];
    //m_RightHandMatrix._43 = vOutPos.m128_f32[2];
    //m_RightHandMatrix._44 = 1.f;


    //BoneMatrix = *m_pModelCom->Get_BoneMatrix("Weapon_L");
    //XMStoreFloat4x4(&m_LeftHandMatrix, m_pTransformCom->Get_WorldMatrix() * XMLoadFloat4x4(&BoneMatrix) * XMLoadFloat4x4(m_pParentMatrix));
    //m_pLH_BodyCom->Sync_Update(XMLoadFloat4x4(&m_LeftHandMatrix));
    //m_pLH_BodyCom->Update(fTimeDelta, XMLoadFloat4x4(&m_LeftHandMatrix), vOutQuat, vOutPos);

    //m_LeftHandMatrix._41 = vOutPos.m128_f32[0];
    //m_LeftHandMatrix._42 = vOutPos.m128_f32[1];
    //m_LeftHandMatrix._43 = vOutPos.m128_f32[2];
    //m_LeftHandMatrix._44 = vOutPos.m128_f32[3];


}


HRESULT CBody_Cinematic_Viper::Ready_Colliders()
{
    /* CBody::BODY_SPHERESHAPE_DESC BodyDesc{};

     BodyDesc.fRadius = 2.f;
     BodyDesc.eMotion = EMotionType::Kinematic;
     BodyDesc.eQuality = EMotionQuality::Discrete;
     BodyDesc.eShapeType = SHAPE::SPHERE;
     BodyDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::MONSTERATTACK);
     _float4x4 BoneMatrix = *m_pModelCom->Get_BoneMatrix("Weapon_R");
     XMStoreFloat4x4(&m_RightHandMatrix, m_pTransformCom->Get_WorldMatrix() * XMLoadFloat4x4(&BoneMatrix) * XMLoadFloat4x4(m_pParentMatrix));
     _vector vScale, vQuat, vTrans;
     XMMatrixDecompose(&vScale, &vQuat, &vTrans, XMLoadFloat4x4(&m_RightHandMatrix));
     BodyDesc.vPos = _float3(vTrans.m128_f32[0], vTrans.m128_f32[1], vTrans.m128_f32[2]);
     BodyDesc.vQuat = _float4(vQuat.m128_f32[0], vQuat.m128_f32[1], vQuat.m128_f32[2], vQuat.m128_f32[3]);
     BodyDesc.vShapeOffset = _float3(0.f, 0.f, 0.f);
     m_tCollisionDesc.pGameObject = this;
     BodyDesc.pCollisionDesc = &m_tCollisionDesc;
     BodyDesc.bIsTrigger = true;
     if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Body"),
         TEXT("Com_Body_RH"), reinterpret_cast<CComponent**>(&m_pBodyCom), &BodyDesc)))
         return E_FAIL;*/

    return S_OK;
}


CBody_Cinematic_Viper* CBody_Cinematic_Viper::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CBody_Cinematic_Viper* pInstance = new CBody_Cinematic_Viper(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Failed Create : CBody_Cinematic_Viper"));
    }

    return pInstance;
}

CGameObject* CBody_Cinematic_Viper::Clone(void* pArg)
{
    CBody_Cinematic_Viper* pInstance = new CBody_Cinematic_Viper(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Failed Clone : CBody_Cinematic_Viper"));
    }

    return pInstance;
}

void CBody_Cinematic_Viper::Free()
{
    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pOwnerTransform);

    __super::Free();

}
