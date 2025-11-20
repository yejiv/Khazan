#include "Prop_Destructible.h"
#include "GameInstance.h"
#include "Transform.h"
#include "Model.h"
#include "Body.h"
#include "Prop_Chunk.h"

CProp_Destructible::CProp_Destructible(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CProp{ pDevice, pContext }
{
}

CProp_Destructible::CProp_Destructible(const CProp_Destructible& Prototype)
    : CProp{ Prototype }
{
}

HRESULT CProp_Destructible::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CProp_Destructible::Initialize_Clone(void* pArg)
{

    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(2.f, 0.5f, 2.f, 1.f));

    if (FAILED(Ready_Components(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Collision(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Chunk(pArg)))
        return E_FAIL;


    return S_OK;
}

void CProp_Destructible::Priority_Update(_float fTimeDelta)
{
    for (auto Chunk : m_Chunks)
    {
        Chunk->Priority_Update(fTimeDelta);
    }
}

void CProp_Destructible::Update(_float fTimeDelta)
{
    for (auto Chunk : m_Chunks)
    {
        Chunk->Update(fTimeDelta);
    }
}

void CProp_Destructible::Late_Update(_float fTimeDelta)
{
    _bool isAllDead = true;
    for (auto Chunk : m_Chunks)
    {
        Chunk->Late_Update(fTimeDelta);

        if (!Chunk->Get_IsDead())
            isAllDead = false;
    }

    if (isAllDead)
    {
        m_pBodyCom->Collision_Active(false);
        m_isDead = true;
        m_isActive = false;
    }
        

}

HRESULT CProp_Destructible::Render()
{

    return S_OK;
}


void CProp_Destructible::Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal)
{
    if (iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::PLAYER_ATTACK) ||
        iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::MONSTERATTACK))
    {
        _vector vDir = m_pTransformCom->Get_State(STATE::POSITION) - XMLoadFloat3(&vContactPoint);
        _float3 vVel = _float3(XMVector3Normalize(vDir).m128_f32[0], XMVector3Normalize(vDir).m128_f32[1], XMVector3Normalize(vDir).m128_f32[2]);
        for (auto Chunk : m_Chunks)
        {
            
            Chunk->Destory(vVel, vContactPoint);
        }
    }

}

void CProp_Destructible::Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal)
{

}

void CProp_Destructible::Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer)
{

}



HRESULT CProp_Destructible::Ready_Components(void* pArg)
{
    PROP_DEST_DESC* pDesc = static_cast<PROP_DEST_DESC*>(pArg);
    CHECK_NULLPTR(pDesc, E_FAIL);

    LEVEL eLevel = pDesc->eLevel;
    CHECK_EQUAL(LEVEL::END, eLevel, E_FAIL);

    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr), E_FAIL);

    return S_OK;
}

HRESULT CProp_Destructible::Ready_Collision(void* pArg)
{
    CBody::BODY_SPHERESHAPE_DESC BodyDesc{};
    BodyDesc.fRadius = 0.5f;
    BodyDesc.bIsTrigger = true;
    BodyDesc.bStartActive = true;
    BodyDesc.eMotion = EMotionType::Kinematic;
    BodyDesc.eQuality = EMotionQuality::Discrete;
    BodyDesc.eShapeType = SHAPE::SPHERE;
    BodyDesc.fFriction = 0.8f;
    BodyDesc.fMass = 1.0f;
    BodyDesc.fRestitution = 0.0f;
    BodyDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::MAP_DEST);
    _float3 vPos{};

    XMStoreFloat3(&vPos, m_pTransformCom->Get_State(STATE::POSITION));
    _float4 vQuat{};
    XMStoreFloat4(&vQuat, m_pTransformCom->Get_Rotation_Quat());
    BodyDesc.vPos = vPos;
    BodyDesc.vQuat = vQuat;
    BodyDesc.vShapeOffset = _float3(0.f, 0.5f, 0.f);
    m_tCollisionDesc.pGameObject = this;
    BodyDesc.pCollisionDesc = &m_tCollisionDesc;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Body"),
        TEXT("Com_Body"), reinterpret_cast<CComponent**>(&m_pBodyCom), &BodyDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CProp_Destructible::Ready_Chunk(void* pArg)
{
    PROP_DEST_DESC* pDesc = static_cast<PROP_DEST_DESC*>(pArg);
    CHECK_NULLPTR(pDesc, E_FAIL);

    CProp_Chunk::PROP_CHUNK_DESC Desc{};
    Desc.eLevel = pDesc->eLevel;
    Desc.fRotationPerSec = pDesc->fRotationPerSec;
    Desc.fRotationPerSec = pDesc->fRotationPerSec;
    Desc.iMapObjectID = pDesc->iMapObjectID;    
    Desc.WorldMatrix = pDesc->WorldMatrix;
    Desc.strModelTag = TEXT("Prototype_Component_Model_OakChunk_1");
    m_Chunks.push_back(dynamic_cast<CProp_Chunk*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_GameObject_Prop_Chunk"), &Desc)));

    Desc.strModelTag = TEXT("Prototype_Component_Model_OakChunk_2");
    m_Chunks.push_back(dynamic_cast<CProp_Chunk*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_GameObject_Prop_Chunk"), &Desc)));

    Desc.strModelTag = TEXT("Prototype_Component_Model_OakChunk_3");
    m_Chunks.push_back(dynamic_cast<CProp_Chunk*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_GameObject_Prop_Chunk"), &Desc)));

    Desc.strModelTag = TEXT("Prototype_Component_Model_OakChunk_4");
    m_Chunks.push_back(dynamic_cast<CProp_Chunk*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_GameObject_Prop_Chunk"), &Desc)));

    Desc.strModelTag = TEXT("Prototype_Component_Model_OakChunk_5");
    m_Chunks.push_back(dynamic_cast<CProp_Chunk*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_GameObject_Prop_Chunk"), &Desc)));

    return S_OK;
}

CProp_Destructible* CProp_Destructible::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CProp_Destructible* pInstance = new CProp_Destructible(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CProp_Destructible"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CProp_Destructible::Clone(void* pArg)
{
    CProp_Destructible* pInstance = new CProp_Destructible(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed To Cloned : CProp_Destructible"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CProp_Destructible::Free()
{
    __super::Free();

    for (auto Chunk : m_Chunks)
        Safe_Release(Chunk);

    m_pBodyCom->Collision_Active(false);
    Safe_Release(m_pBodyCom);


}
