#include "Map_Trigger.h"

CMap_Trigger::CMap_Trigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject { pDevice, pContext }
{
}

CMap_Trigger::CMap_Trigger(const CMap_Trigger& Prototype)
    : CGameObject { Prototype }
{
}

HRESULT CMap_Trigger::Initialize_Prototype()
{
    return E_NOTIMPL;
}

HRESULT CMap_Trigger::Initialize_Clone(void* pArg)
{
    return E_NOTIMPL;
}

void CMap_Trigger::Priority_Update(_float fTimeDelta)
{
}

void CMap_Trigger::Update(_float fTimeDelta)
{
}

void CMap_Trigger::Late_Update(_float fTimeDelta)
{
}

HRESULT CMap_Trigger::Render()
{
    return E_NOTIMPL;
}

HRESULT CMap_Trigger::Ready_Components()
{
    return E_NOTIMPL;
}

HRESULT CMap_Trigger::Bind_ShaderResources()
{
    return E_NOTIMPL;
}

CMap_Trigger* CMap_Trigger::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    return nullptr;
}

CGameObject* CMap_Trigger::Clone(void* pArg)
{
    return nullptr;
}

void CMap_Trigger::Free()
{
}
