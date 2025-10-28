#include "UI_QuickSlot_Item_Panel.h"
#include "GameInstance.h"

#include "UI_QuickSlot_Item.h"
#include "UI_Atlas_Icon.h"

CUI_QuickSlot_Item_Panel::CUI_QuickSlot_Item_Panel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Panel{ pDevice, pContext }
{
}

CUI_QuickSlot_Item_Panel::CUI_QuickSlot_Item_Panel(const CUI_QuickSlot_Item_Panel& Prototype)
	: CUI_Panel(Prototype)
{
}

HRESULT CUI_QuickSlot_Item_Panel::Initialize_Prototype(_uint iLevel)
{
	m_iLevel = iLevel;

	if (FAILED(Ready_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_QuickSlot_Item_Panel::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;
	return S_OK;
}

void CUI_QuickSlot_Item_Panel::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUI_QuickSlot_Item_Panel::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CUI_QuickSlot_Item_Panel::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CUI_QuickSlot_Item_Panel::Render()
{
	return S_OK;
}

HRESULT CUI_QuickSlot_Item_Panel::Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg)
{
	__super::Load_UI(pInData, iPrototypeLevelID, pArg);

	for (_int i = 0; i < (_int)m_Children.size(); ++i)
		static_cast<CUI_QuickSlot_Item*>(m_Children[i])->Set_index(i);

	return S_OK;
}

HRESULT CUI_QuickSlot_Item_Panel::Ready_Prototype()
{
	CHECK_FAILED(m_pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_UI_Quick_Item_Slot"),
		CUI_QuickSlot_Item::Create(m_pDevice, m_pContext, m_iLevel)), E_FAIL);

	return S_OK;
}

CUI_QuickSlot_Item_Panel* CUI_QuickSlot_Item_Panel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CUI_QuickSlot_Item_Panel* pInstance = new CUI_QuickSlot_Item_Panel(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX(TEXT("Failed Created : CUI_QuickSlot_Item_Panel"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_QuickSlot_Item_Panel::Clone(void* pArg)
{
	CUI_QuickSlot_Item_Panel* pInstance = new CUI_QuickSlot_Item_Panel(*this);
	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CUI_QuickSlot_Item_Panel"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_QuickSlot_Item_Panel::Free()
{
	__super::Free();
}
