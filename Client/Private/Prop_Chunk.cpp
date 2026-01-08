#include "Prop_Chunk.h"
#include "GameInstance.h"
#include "Transform.h"
#include "Model.h"
#include "Body.h"

CProp_Chunk::CProp_Chunk(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CProp{ pDevice, pContext }
{
}

CProp_Chunk::CProp_Chunk(const CProp_Chunk& Prototype)
    : CProp{ Prototype }
{
}

HRESULT CProp_Chunk::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CProp_Chunk::Initialize_Clone(void* pArg)
{
    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components(pArg)))
        return E_FAIL;

    PROP_CHUNK_DESC* pDesc = static_cast<PROP_CHUNK_DESC*>(pArg);
    CHECK_NULLPTR(pDesc, E_FAIL);

    m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&pDesc->WorldMatrix));
    //m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(pDesc->WorldMatrix._41, pDesc->WorldMatrix._42, pDesc->WorldMatrix._43, 1.f));
    m_pTransformCom->Scale(pDesc->vScale);

    if (FAILED(Ready_Collision(pArg)))
        return E_FAIL;

    m_pBodyCom->Set_Velocity(_float3(0.f, 0.f, 0.f));
    m_pBodyCom->Set_Gravity(0.f);

    return S_OK;
}

void CProp_Chunk::Priority_Update(_float fTimeDelta)
{

}

void CProp_Chunk::Update(_float fTimeDelta)
{
    //m_pBodyCom->Sync_Update(m_pTransformCom);
    if (m_isDestroyStart && !m_isDestruct)
    {
        m_pBodyCom->Set_Gravity(1.f);
        
        
        if (m_isHit)
        {
            m_pBodyCom->Set_Velocity(m_vVelocity);
            _vector vPos = m_pTransformCom->Get_State(STATE::POSITION);
            _vector vDiff = XMVectorSubtract(vPos, XMVectorSetW(XMLoadFloat3(&m_vHitPos), 1.f));
            _float fDist = XMVectorGetX(XMVector3Length(vDiff));
            m_pBodyCom->Add_Impulse(m_fImpulse + fDist);
            m_pBodyCom->Add_Torque(m_fTorque);
        }

        if (m_isExplode)
        {
            m_pBodyCom->ApplyExplosion(
                m_vExplosionPos,
                m_fImpulse,   // 선형 기본 세기
                m_fTorque     // 회전 기본 세기
            );
        }
            
        m_isDestruct = true;
    }

    if (m_isDestruct)
    {
        m_fLifeTime -= fTimeDelta;
        if (m_fLifeTime <= 0.f)
        {
            m_fTimeAcc += fTimeDelta;

            if (1.f <= m_fTimeAcc)
            {
                m_fDecreaseAlpha += fTimeDelta * 0.2f;
            }

            if (1.f <= m_fDecreaseAlpha)
            {
                m_pBodyCom->Collision_Active(false);
                m_isDead = true;
                m_isActive = false;
            }
                        
        }        
            
    }
    m_pBodyCom->Update(fTimeDelta, m_pTransformCom);    
}

void CProp_Chunk::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(RENDERGROUP::STATIC, this);
}

HRESULT CProp_Chunk::Render()
{
    CHECK_FAILED_MSG(Bind_ShaderResources(), TEXT("CProp_Object : Bind_ShaderResources 함수 E_FAIL"), E_FAIL);

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; ++i)
    {
        Bind_Materials(i);

        Bind_DissolveValues();

        if (true == isSnow()) CHECK_FAILED(Bind_ShaderResources_ForSnowMap(i), E_FAIL);

        CHECK_FAILED_ASSERT(m_pShaderCom->Begin(14), E_FAIL);

        CHECK_FAILED_ASSERT(m_pModelCom->Render(i), E_FAIL);
    }

    return S_OK;
}

void CProp_Chunk::Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{


}

void CProp_Chunk::Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{

}

void CProp_Chunk::Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, COLLISION_DESC* pMyDesc)
{

}

void CProp_Chunk::Destory(_float3 vVelocity, _float3 vHitPos, _bool isHit, _float fLifeTime, _float fImpulse, _float fTorque)
{
    m_isDestroyStart = true;
    m_vVelocity = vVelocity;
    m_vHitPos = vHitPos;
    m_fLifeTime = fLifeTime;
    m_isHit = isHit;
    m_fImpulse = fImpulse;
    m_fTorque = fTorque;
}

void CProp_Chunk::Explode(_float3 vExplosionPos, _float fImpulse, _float fTorque, _float fLifeTime)
{
    m_isDestroyStart = true;
    m_isExplode = true;
    m_vExplosionPos = vExplosionPos;
    m_fImpulse = fImpulse;
    m_fTorque = fTorque;
    m_fLifeTime = fLifeTime;
}

HRESULT CProp_Chunk::Ready_Components(void* pArg)
{
    PROP_CHUNK_DESC* pDesc = static_cast<PROP_CHUNK_DESC*>(pArg);
    CHECK_NULLPTR(pDesc, E_FAIL);

    LEVEL eLevel = pDesc->eLevel;
    CHECK_EQUAL(LEVEL::END, eLevel, E_FAIL);

    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr), E_FAIL);

    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(eLevel), pDesc->strModelTag,
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr), E_FAIL);

    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Dissolve"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pDissolveTextureCom), nullptr), E_FAIL);

    return S_OK;
}

HRESULT CProp_Chunk::Ready_Collision(void* pArg)
{
    CBody::BODY_CONVEXSHAPE_DESC BodyDesc{};
    BodyDesc.pModel = m_pModelCom;
    BodyDesc.pTransform = m_pTransformCom;
    BodyDesc.bIsTrigger = false;
    BodyDesc.bStartActive = false;
    BodyDesc.eMotion = EMotionType::Dynamic;
    BodyDesc.eQuality = EMotionQuality::Discrete;
    BodyDesc.eShapeType = SHAPE::CONVEX;
    BodyDesc.fFriction = 0.4f;
    BodyDesc.fMass = 30.0f;
    BodyDesc.fRestitution = 0.01f;
    BodyDesc.fAngularDamping = 0.2f;
    BodyDesc.fLinearDamping = 0.1f;
    BodyDesc.fGravity = 1.f;
    BodyDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::MAP_CHUNK);
    _float3 vPos{};
    XMStoreFloat3(&vPos, m_pTransformCom->Get_State(STATE::POSITION));
    _float4 vQuat{};
    XMStoreFloat4(&vQuat, m_pTransformCom->Get_Rotation_Quat());
    BodyDesc.vPos = vPos;
    BodyDesc.vQuat = vQuat;
    BodyDesc.vShapeOffset = _float3(0.f, 0.0f, 0.f);
    m_tCollisionDesc.pGameObject = this;
    //pCollDesc.pInfo = ?? // 작성하기
    BodyDesc.pCollisionDesc = &m_tCollisionDesc;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Body"),
        TEXT("Com_Body"), reinterpret_cast<CComponent**>(&m_pBodyCom), &BodyDesc)))
        return E_FAIL;

    return S_OK;
}


HRESULT CProp_Chunk::Bind_ShaderResources()
{
    // 월드 행렬 쉐이더에 바인딩
    CHECK_FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);

    // 뷰 행렬 쉐이더에 바인딩
    CHECK_FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW)), E_FAIL);

    // 투영 행렬 쉐이더에 바인딩
    CHECK_FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ)), E_FAIL);

    // 카메라 바인딩
    CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4)), E_FAIL);

    return S_OK;
}


HRESULT CProp_Chunk::Bind_Materials(_uint iMeshIndex)
{
    m_iMtrlFlags = 0;

    if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_DiffuseTexture", iMeshIndex, aiTextureType_DIFFUSE, 0)))
        m_iMtrlFlags |= M_DIFFUSE;
    if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_NormalTexture", iMeshIndex, aiTextureType_NORMALS, 0)))
        m_iMtrlFlags |= M_NORMAL;
    if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_EmissiveTexture", iMeshIndex, aiTextureType_EMISSIVE, 0)))
        m_iMtrlFlags |= M_EMISSIVE;
    if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_SpecularTexture", iMeshIndex, aiTextureType_SPECULAR, 0)))
        m_iMtrlFlags |= M_SPECULAR;

    m_iMtrlFlags &= ~M_EMISSIVE;
    m_iMtrlFlags &= ~M_SPECULAR;

    m_pShaderCom->Bind_RawValue("g_MtrlFlags", &m_iMtrlFlags, sizeof(_uint));

    return S_OK;
}

HRESULT CProp_Chunk::Bind_DissolveValues()
{
    CHECK_FAILED(m_pDissolveTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_DissolveTexture", 0), E_FAIL);

    _float fEdgeWidth = { 0.08f };    

    m_pShaderCom->Bind_RawValue("g_fDecreaseAlpha", &m_fDecreaseAlpha, sizeof(_float));
    m_pShaderCom->Bind_RawValue("g_fEdgeWidth", &fEdgeWidth, sizeof(_float));
    m_pShaderCom->Bind_RawValue("g_fEdgeColor", &m_vEdgeColor, sizeof(_float4));

    return S_OK;
}

CProp_Chunk* CProp_Chunk::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CProp_Chunk* pInstance = new CProp_Chunk(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CProp_Chunk"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CProp_Chunk::Clone(void* pArg)
{
    CProp_Chunk* pInstance = new CProp_Chunk(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed To Cloned : CProp_Chunk"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CProp_Chunk::Free()
{
    __super::Free();
    Safe_Release(m_pModelCom);
    Safe_Release(m_pBodyCom);
    Safe_Release(m_pDissolveTextureCom);
}
