#include "Inven_OtherItem.h"
#include "GameInstance.h"

CInven_OtherItem::CInven_OtherItem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Panel{ pDevice, pContext }
{
}

CInven_OtherItem::CInven_OtherItem(const CInven_OtherItem& Prototype)
	: CUI_Panel(Prototype)
{
}

HRESULT CInven_OtherItem::Initialize_Prototype(_uint iLevel)
{
	m_iLevel = iLevel;

	return S_OK;
}

HRESULT CInven_OtherItem::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	return S_OK;
}

void CInven_OtherItem::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CInven_OtherItem::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CInven_OtherItem::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CInven_OtherItem::Render()
{
	return S_OK;
}

void CInven_OtherItem::Bubble_EventCall()
{
}

CInven_OtherItem* CInven_OtherItem::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CInven_OtherItem* pInstance = new CInven_OtherItem(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX(TEXT("Failed Created : CInven_OtherItem"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CInven_OtherItem::Clone(void* pArg)
{
	CInven_OtherItem* pInstance = new CInven_OtherItem(*this);
	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CInven_OtherItem"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CInven_OtherItem::Free()
{
	__super::Free();
}
