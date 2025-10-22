#include "UI_Inven.h"
#include "GameInstance.h"
#include "ClientInstance.h"

#include "Inven_Panel.h"
#include "Inven_Tap.h"

#include "UI_BackGround.h"
#include "UI_Panel.h"

#include "Item_Slot.h"

CUI_Inven::CUI_Inven(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Panel{ pDevice, pContext }
{
}

CUI_Inven::CUI_Inven(const CUI_Inven& Prototype)
	: CUI_Panel(Prototype)
{
}

void CUI_Inven::On_Panel()
{
	m_IsUpdate ? m_eAnimState = UIANIMSTATE::OFF : m_eAnimState = UIANIMSTATE::ON;
	m_eAnimState == UIANIMSTATE::ON ? m_fAccTime = 0.f : m_fAccTime = 1.f;
	m_IsUpdate = true;
}

HRESULT CUI_Inven::Initialize_Prototype(_uint iLevel)
{
	m_iLevel = iLevel;

	CHECK_FAILED(Ready_Prototype(), E_FAIL);
	
	return S_OK;
}

HRESULT CUI_Inven::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	return S_OK;
}

void CUI_Inven::Priority_Update(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down(DIK_8))
		On_Panel();

	if (m_eAnimState == UIANIMSTATE::ON)
	{
		m_fAccTime += fTimeDelta;
		__super::Update_Alpha(m_fAccTime);

		if (m_fAccTime >= 1.f)
		{
			m_fAccTime = 1.f;
			m_eAnimState = UIANIMSTATE::END;
		}
	}
	else if (m_eAnimState == UIANIMSTATE::OFF)
	{
		m_fAccTime -= fTimeDelta * 2.f;
		__super::Update_Alpha(m_fAccTime);

		if (m_fAccTime <= 0.f)
		{
			m_fAccTime = 0.f;
			m_eAnimState = UIANIMSTATE::END;
			m_IsUpdate = false;
		}
	}
	if (!m_IsUpdate)
		return;
	m_pBackGround->Priority_Update(fTimeDelta);
	for (auto Tap : m_pInvenTap)
		Tap->Priority_Update(fTimeDelta);

	if (m_iSeleteTap == 0)
	{
		for (auto Item : m_pActiveItem)
			Item->Priority_Update(fTimeDelta);
	}
	else if (m_iSeleteTap == 1)
	{
		for (auto Item : m_pCollection)
			Item->Priority_Update(fTimeDelta);
	}
	else if (m_iSeleteTap == 2)
	{
		for (auto Item : m_pMaterial)
			Item->Priority_Update(fTimeDelta);
	}
}

void CUI_Inven::Update(_float fTimeDelta)
{
	if (!m_IsUpdate)
		return;
	m_pBackGround->Update(fTimeDelta);
	for (auto Tap : m_pInvenTap)
		Tap->Update(fTimeDelta);

	if (m_iSeleteTap == 0)
	{
		for (auto Item : m_pActiveItem)
			Item->Update(fTimeDelta);
	}
	else if (m_iSeleteTap == 1)
	{
		for (auto Item : m_pCollection)
			Item->Update(fTimeDelta);
	}
	else if (m_iSeleteTap == 2)
	{
		for (auto Item : m_pMaterial)
			Item->Update(fTimeDelta);
	}
}

void CUI_Inven::Late_Update(_float fTimeDelta)
{
	if (!m_IsUpdate)
		return;
	m_pBackGround->Late_Update(fTimeDelta);
	for (auto Tap : m_pInvenTap)
		Tap->Late_Update(fTimeDelta);

	if (m_iSeleteTap == 0)
	{
		for (auto Item : m_pActiveItem)
			Item->Late_Update(fTimeDelta);
	}
	else if (m_iSeleteTap == 1)
	{
		for (auto Item : m_pCollection)
			Item->Late_Update(fTimeDelta);
	}
	else if (m_iSeleteTap == 2)
	{
		for (auto Item : m_pMaterial)
			Item->Late_Update(fTimeDelta);
	}
}

HRESULT CUI_Inven::Render()
{
	return S_OK;
}

HRESULT CUI_Inven::Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg)
{
	__super::Load_UI(pInData, iPrototypeLevelID, pArg);

	for (auto pChild : m_Children)
	{
		m_pInvenTap.push_back(static_cast<CInven_Tap*>(pChild));
		Safe_AddRef(pChild);
	}

	m_pInvenTap[0]->Tap_Enable();
	CHECK_FAILED(Ready_Object(), E_FAIL);
	return S_OK;
}

void CUI_Inven::Bubble_EventCall(BUBBLEEVENT* pArg)
{
	m_iSeleteTap = -1;
	if (pArg->szName == "Inven_OtherItem_Active")
	{
		m_iSeleteTap = 0;
	}
	else if (pArg->szName == "Inven_OtherItem_Collection")
	{
		m_iSeleteTap = 1;
	}
	else if (pArg->szName == "Inven_OtherItem_material")
	{
		m_iSeleteTap = 2;
	}

	if (m_iSeleteTap < 0)
		return;

	for (_int i = 0; i < (_int)m_pInvenTap.size(); ++i)
	{
		if (m_iSeleteTap == i)
			m_pInvenTap[i]->Tap_Enable();
		else
			m_pInvenTap[i]->Tap_Disable();
	}
}


HRESULT CUI_Inven::Ready_Prototype()
{
	CHECK_FAILED(m_pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_UI_Inven_Panel"),
		CInven_Panel::Create(m_pDevice, m_pContext, m_iLevel)), E_FAIL);
	CHECK_FAILED(m_pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_UI_Inven_Tap"),
		CInven_Tap::Create(m_pDevice, m_pContext, m_iLevel)), E_FAIL);

	return S_OK;
}

HRESULT CUI_Inven::Ready_Object()
{
	UIOBJECT_DESC Desc = {};
	Desc.fDepth = 3;
	Desc.iUIType = ENUM_CLASS(UITYPE::TEXTURE);
	Desc.szName = "BackGround";
	Desc.vLocalSize = { g_iWinSizeX, g_iWinSizeY };
	Desc.vLocalPos = { g_iWinSizeX >> 1, g_iWinSizeY >> 1 };

	m_pBackGround = static_cast<CUI_BackGround*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC),TEXT("Prototype_GameObject_UI_BackGround"), &Desc));
	if (m_pBackGround == nullptr)
		return E_FAIL;
	m_pBackGround->Setting_BG(CUI_BackGround::UIBGTYPE::ITEM);
	m_Children.push_back(m_pBackGround);
	Safe_AddRef(m_pBackGround);

	Ready_SlotSet();
	return S_OK;
}

HRESULT CUI_Inven::Ready_SlotSet()
{
	CUIObject::UIOBJECT_DESC Desc = {};
	
	_float2 vPos = { 320.f , 577.f };
	Desc.iUIType = ENUM_CLASS(UITYPE::PANEL);
	Desc.szName = "Item";
	Desc.vLocalPos = vPos;
	Desc.vLocalSize = { 103.f , 103.f };

	for (_int i = 0; i < 28; ++i)
	{
		CItem_Slot* pActiveItemSlot = static_cast<CItem_Slot*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_Item"), &Desc));
		CItem_Slot* pCollectionSlot = static_cast<CItem_Slot*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_Item"), &Desc));
		CItem_Slot* pMaterialSlot = static_cast<CItem_Slot*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_Item"), &Desc));

		if (pActiveItemSlot == nullptr || pCollectionSlot == nullptr || pMaterialSlot == nullptr)
			return E_FAIL;

		m_pActiveItem.push_back(pActiveItemSlot);
		m_pCollection.push_back(pCollectionSlot);
		m_pMaterial.push_back(pMaterialSlot);

		m_Children.push_back(pActiveItemSlot);
		m_Children.push_back(pCollectionSlot);
		m_Children.push_back(pMaterialSlot);

		Safe_AddRef(pActiveItemSlot);
		Safe_AddRef(pCollectionSlot);
		Safe_AddRef(pMaterialSlot);

		m_pActiveItem[i]->Update_Pos(i, vPos, 110.f, 4, 7);
		m_pCollection[i]->Update_Pos(i, vPos, 110.f, 4, 7);
		m_pMaterial[i]->Update_Pos(i, vPos, 110.f, 4, 7);
	}


	return S_OK;
}

CUI_Inven* CUI_Inven::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CUI_Inven* pInstance = new CUI_Inven(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX(TEXT("Failed Created : CUI_Inven"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_Inven::Clone(void* pArg)
{
	CUI_Inven* pInstance = new CUI_Inven(*this);
	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CUI_Inven"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_Inven::Free()
{
	__super::Free();

	for (auto pSlot : m_pActiveItem)
		Safe_Release(pSlot);
	m_pActiveItem.clear();

	for (auto pSlot : m_pMaterial)
		Safe_Release(pSlot);
	m_pMaterial.clear();

	for (auto pSlot : m_pCollection)
		Safe_Release(pSlot);
	m_pCollection.clear();

	for (auto pTap : m_pInvenTap)
		Safe_Release(pTap);
	m_pInvenTap.clear();

	Safe_Release(m_pBackGround);
}
