#include "Fence.h"
#include "GameInstance.h"
#include "Transform.h"
#include "Model.h"
#include "Body.h"
#include "Prop_Chunk.h"
#include "Body_Khazan_Spear.h"

CFence::CFence(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CProp_Destructible{ pDevice, pContext }
{
}

CFence::CFence(const CFence& Prototype)
    : CProp_Destructible{ Prototype }
{
}

HRESULT CFence::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CFence::Initialize_Clone(void* pArg)
{

    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;
    PROP_FENCE_DESC* pDesc = static_cast<PROP_FENCE_DESC*>(pArg);

    m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&pDesc->WorldMatrix));
    m_iLevelIndex = ENUM_CLASS(pDesc->eLevel);

    if (FAILED(Ready_Components(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Collision(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Chunk(pArg)))
        return E_FAIL;


    return S_OK;
}

void CFence::Priority_Update(_float fTimeDelta)
{
    for (auto Chunk : m_Chunks)
    {
        Chunk->Priority_Update(fTimeDelta);
    }
}

void CFence::Update(_float fTimeDelta)
{
    for (auto Chunk : m_Chunks)
    {
        Chunk->Update(fTimeDelta);
    }
}

void CFence::Late_Update(_float fTimeDelta)
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

HRESULT CFence::Render()
{

    return S_OK;
}


void CFence::Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal)
{
    if (iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::PLAYER_ATTACK) ||
        iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::MONSTERATTACK))
    {
        if (iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::PLAYER_ATTACK))
        {
            _bool isAttack = false;
            if (m_iLevelIndex == ENUM_CLASS(LEVEL::HEINMACH))
            {
                CBody_Khazan_Spear* pKhazan = dynamic_cast<CBody_Khazan_Spear*>(pDesc->pGameObject);
                isAttack = pKhazan->Get_IsAttackCollisionActive();
            }

            if (isAttack)
            {
                _vector vDir = m_pTransformCom->Get_State(STATE::POSITION) - XMLoadFloat3(&vContactPoint);
                _float3 vVel = _float3(XMVector3Normalize(vDir).m128_f32[0], XMVector3Normalize(vDir).m128_f32[1], XMVector3Normalize(vDir).m128_f32[2]);
                for (auto Chunk : m_Chunks)
                {

                    Chunk->Destory(vVel, vContactPoint);
                }
            }
            
        }

    }

}

void CFence::Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal)
{

}

void CFence::Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer)
{

}



HRESULT CFence::Ready_Components(void* pArg)
{

    return S_OK;
}

HRESULT CFence::Ready_Collision(void* pArg)
{
    CBody::BODY_SPHERESHAPE_DESC BodyDesc{};
    BodyDesc.fRadius = 1.f;
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

HRESULT CFence::Ready_Chunk(void* pArg)
{
    PROP_FENCE_DESC* pDesc = static_cast<PROP_FENCE_DESC*>(pArg);
    CHECK_NULLPTR(pDesc, E_FAIL);

    CProp_Chunk::PROP_CHUNK_DESC Desc{};
    Desc.eLevel = pDesc->eLevel;
    Desc.fRotationPerSec = pDesc->fRotationPerSec;
    Desc.fRotationPerSec = pDesc->fRotationPerSec;
    Desc.iMapObjectID = pDesc->iMapObjectID;
    Desc.WorldMatrix = pDesc->WorldMatrix;
    Desc.vPos = m_pTransformCom->Get_State(STATE::POSITION);
    Desc.vScale = _float3(0.00015f, 0.00015f, 0.00015f);
    Desc.strModelTag = TEXT("Prototype_Component_Model_Fence_Chunk_1");
    m_Chunks.push_back(dynamic_cast<CProp_Chunk*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_GameObject_Prop_Chunk"), &Desc)));

    Desc.strModelTag = TEXT("Prototype_Component_Model_Fence_Chunk_2");
    m_Chunks.push_back(dynamic_cast<CProp_Chunk*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_GameObject_Prop_Chunk"), &Desc)));

    Desc.strModelTag = TEXT("Prototype_Component_Model_Fence_Chunk_3");
    m_Chunks.push_back(dynamic_cast<CProp_Chunk*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_GameObject_Prop_Chunk"), &Desc)));

    Desc.strModelTag = TEXT("Prototype_Component_Model_Fence_Chunk_4");
    m_Chunks.push_back(dynamic_cast<CProp_Chunk*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_GameObject_Prop_Chunk"), &Desc)));

    Desc.strModelTag = TEXT("Prototype_Component_Model_Fence_Chunk_5");
    m_Chunks.push_back(dynamic_cast<CProp_Chunk*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_GameObject_Prop_Chunk"), &Desc)));

    return S_OK;
}

CFence* CFence::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CFence* pInstance = new CFence(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CFence"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CFence::Clone(void* pArg)
{
    CFence* pInstance = new CFence(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed To Cloned : CFence"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CFence::Free()
{
    __super::Free();
    Safe_Release(m_pBodyCom);
}
