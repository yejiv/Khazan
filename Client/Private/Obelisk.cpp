#include "Obelisk.h"
#include "GameInstance.h"
#include "Transform.h"
#include "Model.h"
#include "Body.h"
#include "Prop_Chunk.h"

CObelisk::CObelisk(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CProp_Destructible{ pDevice, pContext }
{
}

CObelisk::CObelisk(const CObelisk& Prototype)
    : CProp_Destructible{ Prototype }
{
}

HRESULT CObelisk::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CObelisk::Initialize_Clone(void* pArg)
{

    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;
    PROP_FENCE_DESC* pDesc = static_cast<PROP_FENCE_DESC*>(pArg);

    m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&pDesc->WorldMatrix));
    m_iLevelIndex = ENUM_CLASS(pDesc->eLevel);

    if (FAILED(Ready_Components(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Chunk(pArg)))
        return E_FAIL;

    m_DestoryQueue.push(14);
    m_DestoryQueue.push(18);
    m_DestoryQueue.push(15);
    m_DestoryQueue.push(6);
    m_DestoryQueue.push(30);

    m_DestoryQueue.push(1);
    m_DestoryQueue.push(20);
    m_DestoryQueue.push(8);
    m_DestoryQueue.push(19);
    m_DestoryQueue.push(4);

    m_DestoryQueue.push(5);
    m_DestoryQueue.push(26);
    m_DestoryQueue.push(13);
    m_DestoryQueue.push(16);
    m_DestoryQueue.push(24);

    m_DestoryQueue.push(25);
    m_DestoryQueue.push(2);
    m_DestoryQueue.push(22);
    m_DestoryQueue.push(21);
    m_DestoryQueue.push(23);

    m_DestoryQueue.push(27);
    m_DestoryQueue.push(28);
    m_DestoryQueue.push(29);
    m_DestoryQueue.push(3);
    m_DestoryQueue.push(7);

    m_DestoryQueue.push(17);
    m_DestoryQueue.push(9);
    m_DestoryQueue.push(10);
    m_DestoryQueue.push(11);
    m_DestoryQueue.push(12);


    return S_OK;
}

void CObelisk::Priority_Update(_float fTimeDelta)
{
    if (m_pGameInstance->Key_Down(DIK_7))
    {
        Destory();
    }

    for (auto Chunk : m_Chunks)
    {
        Chunk->Priority_Update(fTimeDelta);
    }
}

void CObelisk::Update(_float fTimeDelta)
{
    for (auto Chunk : m_Chunks)
    {
        Chunk->Update(fTimeDelta);
    }
}

void CObelisk::Late_Update(_float fTimeDelta)
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

HRESULT CObelisk::Render()
{

    return S_OK;
}

void CObelisk::Destory()
{
    for (auto Chunk : m_Chunks)
    {
        Chunk->Explode(
            _float3(-32.365f, -29.5f, 198.409f),
            m_pGameInstance->Rand(100000.f, 150000.f),
            m_pGameInstance->Rand(100000.f, 150000.f));
    }
}


void CObelisk::Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal)
{

}

void CObelisk::Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal)
{

}

void CObelisk::Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer)
{

}



HRESULT CObelisk::Ready_Components(void* pArg)
{

    return S_OK;
}

HRESULT CObelisk::Ready_Chunk(void* pArg)
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
    Desc.vScale = _float3(0.004f, 0.004f, 0.004f);


    _wstring strBase = TEXT("Prototype_Component_Model_Obelisk_Chunk_");
    for (size_t i = 1; i < 31; i++)
    {
        Desc.strModelTag = strBase + to_wstring(i);
        m_Chunks.push_back(dynamic_cast<CProp_Chunk*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, m_iLevelIndex, TEXT("Prototype_GameObject_Prop_Chunk"), &Desc)));
    }
    
    return S_OK;
}

CObelisk* CObelisk::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CObelisk* pInstance = new CObelisk(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CObelisk"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CObelisk::Clone(void* pArg)
{
    CObelisk* pInstance = new CObelisk(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed To Cloned : CObelisk"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CObelisk::Free()
{
    __super::Free();

}
