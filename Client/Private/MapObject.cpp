#include "MapObject.h"

#include "GameInstance.h"

CMapObject::CMapObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CMapObject::CMapObject(const CMapObject& Prototype)
    : CGameObject{ Prototype }
{
}

HRESULT CMapObject::Initialize_Prototype()
{
    CHECK_FAILED(__super::Initialize_Prototype(), E_FAIL);

    return S_OK;
}

HRESULT CMapObject::Initialize_Clone(void* pArg)
{
    MAPOBJECT_DESC* pDesc = static_cast<MAPOBJECT_DESC*>(pArg);
    CHECK_NULLPTR(pDesc, E_FAIL);

    m_iMapObjectID = pDesc->iMapObjectID;

    CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);

    return S_OK;
}

void CMapObject::Priority_Update(_float fTimeDelta)
{
}

void CMapObject::Update(_float fTimeDelta)
{
}

void CMapObject::Late_Update(_float fTimeDelta)
{
}

HRESULT CMapObject::Render()
{
    return S_OK;
}

void CMapObject::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
}
