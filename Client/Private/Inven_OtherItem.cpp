#include "Inven_OtherItem.h"
#include "GameInstance.h"

#include "UI_Inven.h"
#include "Inven_Tap.h"

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

HRESULT CInven_OtherItem::Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg)
{
	__super::Load_UI(pInData, iPrototypeLevelID, pArg);

	for (auto pChild : m_Children)
	{
		m_pInvenTap.push_back(static_cast<CInven_Tap*>(pChild));
		Safe_AddRef(pChild);
	}

	m_pInvenTap[0]->Tap_Enable();
	return S_OK;
}

void CInven_OtherItem::Bubble_EventCall(BUBBLEEVENT* pArg)
{
	_int iOnTap = -1;
	if (pArg->szName == "Inven_OtherItem_Active")
	{
		iOnTap = 0;
	}
	else if (pArg->szName == "Inven_OtherItem_Collection")
	{
		iOnTap = 1;
	}
	else if (pArg->szName == "Inven_OtherItem_material")
	{
		iOnTap = 2;
	}
	
	if (iOnTap < 0)
		return;

	for (_int i = 0; i < (_int)m_pInvenTap.size(); ++i)
	{
		if (iOnTap == i)
			m_pInvenTap[i]->Tap_Enable();
		else
			m_pInvenTap[i]->Tap_Disable();
	}
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

	for (auto pTap : m_pInvenTap)
	{
		Safe_Release(pTap);
	}
	m_pInvenTap.clear();
}
