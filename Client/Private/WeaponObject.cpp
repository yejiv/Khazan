#include "WeaponObject.h"

CWeaponObject::CWeaponObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPartObject{ pDevice, pContext }
{
}

CWeaponObject::CWeaponObject(const CWeaponObject& Prototype)
    : CPartObject{ Prototype }
{
}
HRESULT CWeaponObject::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CWeaponObject::Initialize_Clone(void* pArg)
{

    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    return S_OK;
}

void CWeaponObject::Priority_Update(_float fTimeDelta)
{
}

void CWeaponObject::Update(_float fTimeDelta)
{
}

void CWeaponObject::Late_Update(_float fTimeDelta)
{
}

HRESULT CWeaponObject::Render()
{
    return S_OK;
}


void CWeaponObject::Free()
{
    __super::Free();
}
