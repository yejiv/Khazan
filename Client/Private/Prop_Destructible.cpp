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

    if (FAILED(Ready_Components(pArg)))
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
