#include "EnginePch.h"
#include "GameObject.h"

#include "GameInstance.h"
#include "Pool.h"

CPool::CPool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CPool::CPool(const CPool& Prototype)
	: CGameObject{ Prototype }
{

}

HRESULT CPool::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPool::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	return S_OK;
}

void CPool::Priority_Update(_float fTimeDelta)
{
}

void CPool::Update(_float fTimeDelta)
{
}

void CPool::Late_Update(_float fTimeDelta)
{
}

HRESULT CPool::Render()
{
	return S_OK;
}

void CPool::Free()
{
	__super::Free();
}
