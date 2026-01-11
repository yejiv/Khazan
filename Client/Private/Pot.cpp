#include "Pot.h"
#include "GameInstance.h"
#include "Transform.h"
#include "Model.h"
#include "Body.h"
#include "Prop_Chunk.h"
#include "Body_Khazan_Spear.h"
#include "Body_Khazan_GS.h"

CPot::CPot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CProp_Destructible{ pDevice, pContext }
{
}

CPot::CPot(const CPot& Prototype)
    : CProp_Destructible{ Prototype }
{
}

HRESULT CPot::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CPot::Initialize_Clone(void* pArg)
{

    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;
    PROP_POT_DESC* pDesc = static_cast<PROP_POT_DESC*>(pArg);

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

void CPot::Priority_Update(_float fTimeDelta)
{
    for (auto Chunk : m_Chunks)
    {
        Chunk->Priority_Update(fTimeDelta);
    }
}

void CPot::Update(_float fTimeDelta)
{
    for (auto Chunk : m_Chunks)
    {
        Chunk->Update(fTimeDelta);
    }

    if (m_isPlayDestroy)
    {
        m_isPlayDestroy = false;

        _int iRand = m_pGameInstance->Rand(0, 5);
        switch (iRand)
        {
        case 0:
            m_pGameInstance->PlaySoundOnce(TEXT("DP_Pot_Chaos_A.wav"), Get_Position(), nullptr, m_fDestroyVolume);
            break;
        case 1:
            m_pGameInstance->PlaySoundOnce(TEXT("DP_Pot_Chaos_B.wav"), Get_Position(), nullptr, m_fDestroyVolume);
            break;
        case 2:
            m_pGameInstance->PlaySoundOnce(TEXT("DP_Pot_Chaos_C.wav"), Get_Position(), nullptr, m_fDestroyVolume);
            break;
        case 3:
            m_pGameInstance->PlaySoundOnce(TEXT("DP_Pot_Chaos_D.wav"), Get_Position(), nullptr, m_fDestroyVolume);
            break;
        case 4:
            m_pGameInstance->PlaySoundOnce(TEXT("DP_Pot_Chaos_E.wav"), Get_Position(), nullptr, m_fDestroyVolume);
            break;
        default:
            m_pGameInstance->PlaySoundOnce(TEXT("DP_Pot_Chaos_A.wav"), Get_Position(), nullptr, m_fDestroyVolume);
            break;
        }
    }
}

void CPot::Late_Update(_float fTimeDelta)
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

HRESULT CPot::Render()
{

    return S_OK;
}


void CPot::Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
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
                if (pKhazan) isAttack = pKhazan->Get_IsAttackCollisionActive();
            }
            else if (m_iLevelIndex == ENUM_CLASS(LEVEL::EMBARS))
            {
                CBody_Khazan_GS* pKhazan = dynamic_cast<CBody_Khazan_GS*>(pDesc->pGameObject);
                if (pKhazan) isAttack = pKhazan->IsAttackActive();
            }
            else  if (m_iLevelIndex == ENUM_CLASS(LEVEL::TRAINING))
            {
                CBody_Khazan_Spear* pKhazan = dynamic_cast<CBody_Khazan_Spear*>(pDesc->pGameObject);
                if (pKhazan) isAttack = pKhazan->Get_IsAttackCollisionActive();
            }

            if (isAttack)
            {
                _vector vDir = m_pTransformCom->Get_State(STATE::POSITION) - XMLoadFloat3(&vContactPoint);
                _float3 vVel = _float3(XMVector3Normalize(vDir).m128_f32[0], XMVector3Normalize(vDir).m128_f32[1], XMVector3Normalize(vDir).m128_f32[2]);
                for (auto Chunk : m_Chunks)
                {

                    Chunk->Destory(vVel, vContactPoint);
                }

                m_isPlayDestroy = true;
            }

        }

    }

}

void CPot::Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{

}

void CPot::Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, COLLISION_DESC* pMyDesc)
{

}



HRESULT CPot::Ready_Components(void* pArg)
{

    return S_OK;
}

HRESULT CPot::Ready_Collision(void* pArg)
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
    m_tCollisionDesc.isForceVaildation = true;
    BodyDesc.pCollisionDesc = &m_tCollisionDesc;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Body"),
        TEXT("Com_Body"), reinterpret_cast<CComponent**>(&m_pBodyCom), &BodyDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CPot::Ready_Chunk(void* pArg)
{
    PROP_POT_DESC* pDesc = static_cast<PROP_POT_DESC*>(pArg);
    CHECK_NULLPTR(pDesc, E_FAIL);

    CProp_Chunk::PROP_CHUNK_DESC Desc{};
    Desc.eLevel = pDesc->eLevel;
    Desc.fRotationPerSec = pDesc->fRotationPerSec;
    Desc.fRotationPerSec = pDesc->fRotationPerSec;
    Desc.iMapObjectID = pDesc->iMapObjectID;
    Desc.WorldMatrix = pDesc->WorldMatrix;
    Desc.vPos = m_pTransformCom->Get_State(STATE::POSITION);
    Desc.vScale = _float3(0.0001f, 0.0001f, 0.0001f);
    Desc.strModelTag = TEXT("Prototype_Component_Model_Pot_Chunk_1");
    m_Chunks.push_back(dynamic_cast<CProp_Chunk*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(Desc.eLevel), TEXT("Prototype_GameObject_Prop_Chunk"), &Desc)));

    Desc.strModelTag = TEXT("Prototype_Component_Model_Pot_Chunk_2");
    m_Chunks.push_back(dynamic_cast<CProp_Chunk*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(Desc.eLevel), TEXT("Prototype_GameObject_Prop_Chunk"), &Desc)));

    Desc.strModelTag = TEXT("Prototype_Component_Model_Pot_Chunk_3");
    m_Chunks.push_back(dynamic_cast<CProp_Chunk*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(Desc.eLevel), TEXT("Prototype_GameObject_Prop_Chunk"), &Desc)));

    Desc.strModelTag = TEXT("Prototype_Component_Model_Pot_Chunk_4");
    m_Chunks.push_back(dynamic_cast<CProp_Chunk*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(Desc.eLevel), TEXT("Prototype_GameObject_Prop_Chunk"), &Desc)));

    Desc.strModelTag = TEXT("Prototype_Component_Model_Pot_Chunk_5");
    m_Chunks.push_back(dynamic_cast<CProp_Chunk*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(Desc.eLevel), TEXT("Prototype_GameObject_Prop_Chunk"), &Desc)));

    return S_OK;
}

CPot* CPot::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CPot* pInstance = new CPot(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CPot"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CPot::Clone(void* pArg)
{
    CPot* pInstance = new CPot(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed To Cloned : CPot"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPot::Free()
{
    __super::Free();
    Safe_Release(m_pBodyCom);

}
