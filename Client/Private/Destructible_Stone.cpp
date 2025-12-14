#include "Destructible_Stone.h"
#include "GameInstance.h"
#include "Transform.h"
#include "Model.h"
#include "Body.h"
#include "Chunk.h"


CDestructible_Stone::CDestructible_Stone(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CDestructible_Stone::CDestructible_Stone(const CDestructible_Stone& Prototype)
    : CGameObject{ Prototype }
{
}

HRESULT CDestructible_Stone::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CDestructible_Stone::Initialize_Clone(void* pArg)
{

    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;
    STONE_DESC* pDesc = static_cast<STONE_DESC*>(pArg);

    m_pTransformCom->Set_State(STATE::POSITION, pDesc->vPos);
    //m_iLevelIndex = ENUM_CLASS(pDesc->eLevel);

    if (FAILED(Ready_Components(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Collision(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Chunk(pArg)))
        return E_FAIL;


    return S_OK;
}

void CDestructible_Stone::Priority_Update(_float fTimeDelta)
{
    if (!m_isDestruct)
    {
        _vector vHitPosV = m_pTransformCom->Get_State(STATE::POSITION);
        _float3 vHitPos;
        XMStoreFloat3(&vHitPos, vHitPosV);

        _vector vForward = m_pTransformCom->Get_State(STATE::LOOK);
        vForward = XMVectorSetY(vForward, 0.f);
        vForward = XMVector3Normalize(vForward);

        if (XMVectorGetX(XMVector3LengthSq(vForward)) < 1e-6f)
            vForward = XMVectorSet(0.f, 0.f, 1.f, 0.f);

        const _float fSide = 18.f;
        const _float fUp = 22.f;
        _float fLifeTime = 2.0f;
        _float fImpulse = 0.f;
        _float fTorque = m_pGameInstance->Rand(30.f, 60.f);

        int i = 0;
        const int n = (int)m_Chunks.size();

        for (auto Chunk : m_Chunks)
        {
            _float t = (n > 0) ? ((_float)i / (_float)n) : 0.f;
            _float angle = XM_2PI * t;

            _matrix rotY = XMMatrixRotationY(angle);
            _vector vDirXZ = XMVector3TransformNormal(vForward, rotY);
            vDirXZ = XMVector3Normalize(vDirXZ);

            _vector vVelV = vDirXZ * fSide + XMVectorSet(0.f, 1.f, 0.f, 0.f) * fUp;

            _float3 vVel;
            XMStoreFloat3(&vVel, vVelV);

            Chunk->Destory(vVel, vHitPos, true, 1.5f, fImpulse, fTorque);

            ++i;
        }

        m_isDestruct = true;
    }

    for (auto Chunk : m_Chunks)
    {
        Chunk->Priority_Update(fTimeDelta);
    }
}

void CDestructible_Stone::Update(_float fTimeDelta)
{
    for (auto Chunk : m_Chunks)
    {
        Chunk->Update(fTimeDelta);
    }

    //if (m_isPlayDestroy)
    //{
    //    m_isPlayDestroy = false;

    //    _int iRand = m_pGameInstance->Rand(0, 5);
    //    switch (iRand)
    //    {
    //    case 0:
    //        m_pGameInstance->PlaySoundOnce(TEXT("DP_Destructible_Stone_Chaos_A.wav"), Get_Position(), nullptr, m_fDestroyVolume);
    //        break;
    //    case 1:
    //        m_pGameInstance->PlaySoundOnce(TEXT("DP_Destructible_Stone_Chaos_B.wav"), Get_Position(), nullptr, m_fDestroyVolume);
    //        break;
    //    case 2:
    //        m_pGameInstance->PlaySoundOnce(TEXT("DP_Destructible_Stone_Chaos_C.wav"), Get_Position(), nullptr, m_fDestroyVolume);
    //        break;
    //    case 3:
    //        m_pGameInstance->PlaySoundOnce(TEXT("DP_Destructible_Stone_Chaos_D.wav"), Get_Position(), nullptr, m_fDestroyVolume);
    //        break;
    //    case 4:
    //        m_pGameInstance->PlaySoundOnce(TEXT("DP_Destructible_Stone_Chaos_E.wav"), Get_Position(), nullptr, m_fDestroyVolume);
    //        break;
    //    default:
    //        m_pGameInstance->PlaySoundOnce(TEXT("DP_Destructible_Stone_Chaos_A.wav"), Get_Position(), nullptr, m_fDestroyVolume);
    //        break;
    //    }
    //}
}

void CDestructible_Stone::Late_Update(_float fTimeDelta)
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
        m_isDead = true;
        m_isActive = false;
    }


}

HRESULT CDestructible_Stone::Render()
{

    return S_OK;
}

void CDestructible_Stone::Set_Pos(_vector vPos)
{
    m_pTransformCom->Set_State(STATE::POSITION, vPos);

    for (auto Chunk : m_Chunks)
    {
        Chunk->Set_Pos(vPos);
    }
}


void CDestructible_Stone::Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{    

}

void CDestructible_Stone::Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{

}

void CDestructible_Stone::Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, COLLISION_DESC* pMyDesc)
{

}



HRESULT CDestructible_Stone::Ready_Components(void* pArg)
{

    return S_OK;
}

HRESULT CDestructible_Stone::Ready_Collision(void* pArg)
{


    return S_OK;
}

HRESULT CDestructible_Stone::Ready_Chunk(void* pArg)
{
    STONE_DESC* pDesc = static_cast<STONE_DESC*>(pArg);
    CHECK_NULLPTR(pDesc, E_FAIL);

    CChunk::CHUNK_DESC Desc{};
    Desc.iLevelIndex = m_iLevelIndex;
    Desc.fRotationPerSec = pDesc->fRotationPerSec;
    Desc.fRotationPerSec = pDesc->fRotationPerSec;
    Desc.vPos = m_pTransformCom->Get_State(STATE::POSITION);
    Desc.vScale = _float3(0.05f, 0.05f, 0.05f);
    Desc.strModelTag = TEXT("Prototype_Component_Model_Stone_Chunk_1");
    m_Chunks.push_back(dynamic_cast<CChunk*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, m_iLevelIndex, TEXT("Prototype_GameObject_Chunk"), &Desc)));

    Desc.strModelTag = TEXT("Prototype_Component_Model_Stone_Chunk_2");
    m_Chunks.push_back(dynamic_cast<CChunk*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, m_iLevelIndex, TEXT("Prototype_GameObject_Chunk"), &Desc)));

    Desc.strModelTag = TEXT("Prototype_Component_Model_Stone_Chunk_3");
    m_Chunks.push_back(dynamic_cast<CChunk*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, m_iLevelIndex, TEXT("Prototype_GameObject_Chunk"), &Desc)));

    Desc.strModelTag = TEXT("Prototype_Component_Model_Stone_Chunk_4");
    m_Chunks.push_back(dynamic_cast<CChunk*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, m_iLevelIndex, TEXT("Prototype_GameObject_Chunk"), &Desc)));

    Desc.strModelTag = TEXT("Prototype_Component_Model_Stone_Chunk_5");
    m_Chunks.push_back(dynamic_cast<CChunk*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, m_iLevelIndex, TEXT("Prototype_GameObject_Chunk"), &Desc)));

    Desc.strModelTag = TEXT("Prototype_Component_Model_Stone_Chunk_6");
    m_Chunks.push_back(dynamic_cast<CChunk*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, m_iLevelIndex, TEXT("Prototype_GameObject_Chunk"), &Desc)));

    Desc.strModelTag = TEXT("Prototype_Component_Model_Stone_Chunk_7");
    m_Chunks.push_back(dynamic_cast<CChunk*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, m_iLevelIndex, TEXT("Prototype_GameObject_Chunk"), &Desc)));

    Desc.strModelTag = TEXT("Prototype_Component_Model_Stone_Chunk_8");
    m_Chunks.push_back(dynamic_cast<CChunk*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, m_iLevelIndex, TEXT("Prototype_GameObject_Chunk"), &Desc)));

    return S_OK;
}

CDestructible_Stone* CDestructible_Stone::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CDestructible_Stone* pInstance = new CDestructible_Stone(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CDestructible_Stone"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CDestructible_Stone::Clone(void* pArg)
{
    CDestructible_Stone* pInstance = new CDestructible_Stone(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed To Cloned : CDestructible_Stone"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CDestructible_Stone::Free()
{
    __super::Free();
}
