#include "UI_Inven.h"
#include "GameInstance.h"
#include "ClientInstance.h"

#include "UI_TextBox.h"
#include "Inven_Tap.h"

#include "UI_BackGround.h"
#include "UI_Panel.h"

#include "Item_Slot.h"
#include "Equip_Panel.h"
#include "Equip_Slot.h"

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
	if (m_IsUpdate)
		return;
	m_eAnimState = UIANIMSTATE::ON;
	m_fAccTime = 0.f;
	m_IsUpdate = true;
}

void CUI_Inven::Off_Panel()
{
	if (m_strReturnName == "")
	{
		m_eAnimState = UIANIMSTATE::OFF;
		m_fAccTime = 1.f;
	}
	else
	{
		m_IsUpdate = false;
		CClientInstance::GetInstance()->UI_UpdateSwitch(AnsiToWString(m_strReturnName));
		m_strReturnName = "";
	}
}

_bool CUI_Inven::Add_Item(_uint iItemIndex)
{
	ITEMTYPE eType = Convert_UIntToITEMTYPE(iItemIndex);
	
	for (auto Item : m_pItems[ENUM_CLASS(eType)])
	{
		if (Item->Add_Item(iItemIndex))
			return true;
	}
	
	return false;
}

HRESULT CUI_Inven::Initialize_Prototype(_uint iLevel)
{
	m_iLevel = iLevel;
	CHECK_FAILED(Ready_Prototype(), E_FAIL);
	return S_OK;
}

HRESULT CUI_Inven::Initialize_Clone(void* pArg)
{
	m_pItems.resize(ENUM_CLASS(ITEMTYPE::END));
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;
	Ready_Grouping();
	return S_OK;
}

void CUI_Inven::Priority_Update(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down(DIK_K))
	{
		Add_Item(1010);
		Add_Item(1011);
		Add_Item(1012);
		Add_Item(2030);
		Add_Item(3020);
		Add_Item(4001);
		Add_Item(5001);
		Add_Item(6001);
	}
	if (!m_IsUpdate)
		return;

	if (m_pGameInstance->Key_Down(DIK_ESCAPE))
	{
		if (ENUM_CLASS(TapGroup::OTHER) == m_iTapGroupIndex || m_bIsEquip)
			Off_Panel();
		else
			m_bIsEquip = true;
	}
	UI_Animation(fTimeDelta);
	m_pBackGround->Priority_Update(fTimeDelta);
	m_pUIText->Priority_Update(fTimeDelta);

	if (!m_bIsEquip)
	{
		for (auto TapIndex : m_UpdateGroup[m_iTapGroupIndex])
		{
			if (m_iTapGroupIndex == ENUM_CLASS(TapGroup::QUICK) && TapIndex == ENUM_CLASS(ITEMTYPE::ATIVE))
				continue;

			m_pInvenTap[TapIndex]->Priority_Update(fTimeDelta);
		}
		
		for (auto Item : m_pItems[m_iSeleteTap])
			Item->Priority_Update(fTimeDelta);
	}
	else
	{
		m_pEquip_Panel->Priority_Update(fTimeDelta);
		for (auto Slot : m_pEquipSlot)
			Slot->Priority_Update(fTimeDelta);
	}
}

void CUI_Inven::Update(_float fTimeDelta)
{
	if (!m_IsUpdate)
		return;

	m_pBackGround->Update(fTimeDelta);
	m_pUIText->Update(fTimeDelta);

	if (!m_bIsEquip)
	{
		for (auto TapIndex : m_UpdateGroup[m_iTapGroupIndex])
		{
			if (m_iTapGroupIndex == ENUM_CLASS(TapGroup::QUICK) && TapIndex == ENUM_CLASS(ITEMTYPE::ATIVE))
				continue;

			m_pInvenTap[TapIndex]->Update(fTimeDelta);
		}

		for (auto Item : m_pItems[m_iSeleteTap])
			Item->Update(fTimeDelta);
	}
	else
	{
		m_pEquip_Panel->Update(fTimeDelta);
		for (auto Slot : m_pEquipSlot)
			Slot->Update(fTimeDelta);
	}
}

void CUI_Inven::Late_Update(_float fTimeDelta)
{
	if (!m_IsUpdate)
		return;

	m_pBackGround->Late_Update(fTimeDelta);
	m_pUIText->Late_Update(fTimeDelta);

	if (!m_bIsEquip)
	{
		for (auto TapIndex : m_UpdateGroup[m_iTapGroupIndex])
		{
			if (m_iTapGroupIndex == ENUM_CLASS(TapGroup::QUICK) && TapIndex == ENUM_CLASS(ITEMTYPE::ATIVE))
				continue;

			m_pInvenTap[TapIndex]->Late_Update(fTimeDelta);
		}
		for (auto Item : m_pItems[m_iSeleteTap])
			Item->Late_Update(fTimeDelta);
	}
	else
	{
		m_pEquip_Panel->Late_Update(fTimeDelta);
		for (auto Slot : m_pEquipSlot)
			Slot->Late_Update(fTimeDelta);
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
		if (ENUM_CLASS(UITYPE::TAP) == pChild->Get_UIType())
		{
			m_pInvenTap.push_back(static_cast<CInven_Tap*>(pChild));
			Safe_AddRef(pChild);
		}

		if (ENUM_CLASS(UITYPE::TEXT) == pChild->Get_UIType())
		{
			m_pUIText = static_cast<CUI_TextBox*>(pChild);
			Safe_AddRef(m_pUIText);
		}

		if (ENUM_CLASS(UITYPE::PANEL) == pChild->Get_UIType())
		{
			m_pEquip_Panel = static_cast<CEquip_Panel*>(pChild);
			Safe_AddRef(m_pEquip_Panel);
		}
	}

	m_pInvenTap[0]->Tap_Enable();
	CHECK_FAILED(Ready_Object(), E_FAIL);
	return S_OK;
}

void CUI_Inven::Bubble_EventCall(BUBBLEEVENT* pArg)
{
	INVENBUBBLE_DESC* pDesc = static_cast<INVENBUBBLE_DESC*>(pArg);

	if (pDesc->eBubbleType == EVENT_TYPE::TAP)
	{
		TapType_Mapping(pArg->szName);
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
	else if (pDesc->eBubbleType == EVENT_TYPE::ITEM_SELETE)
	{
		for (_int i = 0; i < (_int)m_pItems[pDesc->iTypeIndex].size(); ++i)
		{
			if (i == pDesc->iIndex)
				continue;
			if (m_pItems[pDesc->iTypeIndex][i]->Off_Selete() == false)
				break;
		}
	}
	else if (pDesc->eBubbleType == EVENT_TYPE::ITEM_EQUIP)
	{
		for (_int i = 0; i < (_int)m_pItems[pDesc->iTypeIndex].size(); ++i)
		{
			if (i == pDesc->iIndex)
				continue;
			else if(m_pItems[pDesc->iTypeIndex][i]->Off_Equip() == false)
				break;
		}

		if (pDesc->iTypeIndex >= 0)
		{
			if(pDesc->iTypeIndex <= ENUM_CLASS(ITEMTYPE::GREATE))
				m_pEquipSlot[ENUM_CLASS(EQUIPSLOT_TYPE::WEAPON)]->Add_Item(pDesc->iItemIndex);
			else if (pDesc->iTypeIndex == ENUM_CLASS(ITEMTYPE::HEAD))
				m_pEquipSlot[ENUM_CLASS(EQUIPSLOT_TYPE::HEAD)]->Add_Item(pDesc->iItemIndex);
			else if (pDesc->iTypeIndex == ENUM_CLASS(ITEMTYPE::TOP))
				m_pEquipSlot[ENUM_CLASS(EQUIPSLOT_TYPE::TOP)]->Add_Item(pDesc->iItemIndex);
			else if (pDesc->iTypeIndex == ENUM_CLASS(ITEMTYPE::GLOVES))
				m_pEquipSlot[ENUM_CLASS(EQUIPSLOT_TYPE::GLOVES)]->Add_Item(pDesc->iItemIndex);
			else if (pDesc->iTypeIndex == ENUM_CLASS(ITEMTYPE::BOTTOM))
				m_pEquipSlot[ENUM_CLASS(EQUIPSLOT_TYPE::BOTTOM)]->Add_Item(pDesc->iItemIndex);
			else if (pDesc->iTypeIndex == ENUM_CLASS(ITEMTYPE::SHOES))
				m_pEquipSlot[ENUM_CLASS(EQUIPSLOT_TYPE::SHOES)]->Add_Item(pDesc->iItemIndex);
			else if (pDesc->iTypeIndex == ENUM_CLASS(ITEMTYPE::NECK))
				m_pEquipSlot[ENUM_CLASS(EQUIPSLOT_TYPE::NECK)]->Add_Item(pDesc->iItemIndex);
			else if (pDesc->iTypeIndex == ENUM_CLASS(ITEMTYPE::RING))
				m_pEquipSlot[ENUM_CLASS(EQUIPSLOT_TYPE::RING)]->Add_Item(pDesc->iItemIndex);
			//else if (pDesc->iItemType <= ENUM_CLASS(ITEMTYPE::ATIVE))
			//	m_pEquipSlot[ENUM_CLASS(EQUIPSLOT_TYPE::SHOES)]->Add_Item(pDesc->iItemIndex);
		}
	}
	else if (pDesc->eBubbleType == EVENT_TYPE::SLOT_EQUIP)
	{
		EQUIPSLOT_TYPE eType = static_cast<EQUIPSLOT_TYPE>(pDesc->iIndex);
		if (eType == EQUIPSLOT_TYPE::WEAPON)
		{
			m_iTapGroupIndex = ENUM_CLASS(TapGroup::WEAPON);
			pDesc->iItemType == 2 ? Change_Tap(1) : Change_Tap(0);
			m_bIsEquip = false;
		}
		else if (eType >= EQUIPSLOT_TYPE::HEAD && eType <= EQUIPSLOT_TYPE::SHOES)
		{
			m_iTapGroupIndex = ENUM_CLASS(TapGroup::ARMOR);
			Change_Tap(ENUM_CLASS(eType) - ENUM_CLASS(EQUIPSLOT_TYPE::HEAD));
			m_bIsEquip = false;
		}
		else if (eType == EQUIPSLOT_TYPE::NECK || eType == EQUIPSLOT_TYPE::RING)
		{
			m_iTapGroupIndex = ENUM_CLASS(TapGroup::ACC);
			Change_Tap(ENUM_CLASS(eType) - ENUM_CLASS(EQUIPSLOT_TYPE::NECK));
			m_bIsEquip = false;
		}
		else
		{
			m_iTapGroupIndex = ENUM_CLASS(TapGroup::QUICK);
			Change_Tap(0);
			m_bIsEquip = false;
		}
	}
}

HRESULT CUI_Inven::Update_Switch(void* pArg)
{
	if (pArg == nullptr)
		return E_FAIL;

	INVEN_ONOFF_DESC* pDesc = static_cast<INVEN_ONOFF_DESC*>(pArg);
	if (pDesc->isOpen)
	{
		On_Panel();

		m_bIsEquip = pDesc->isEquip;
		
		if (!m_bIsEquip)
		{
			m_iTapGroupIndex = ENUM_CLASS(TapGroup::OTHER);
			m_pUIText->Set_Text(TEXT("소지품"));
			Change_Tap(0);
		}
		else
		{
			m_pUIText->Set_Text(TEXT("장비"));
		}
		m_strReturnName = pDesc->szName;
	}
	else
	{

	}

	return S_OK;
}


HRESULT CUI_Inven::Ready_Prototype()
{
	CHECK_FAILED(m_pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_UI_Inven_Tap"),
		CInven_Tap::Create(m_pDevice, m_pContext, m_iLevel)), E_FAIL);
	CHECK_FAILED(m_pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_UI_Inven_Equip"),
		CEquip_Panel::Create(m_pDevice, m_pContext, m_iLevel)), E_FAIL);
	CHECK_FAILED(m_pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_UI_Inven_Equip_Slot"),
		CEquip_Slot::Create(m_pDevice, m_pContext, m_iLevel)), E_FAIL);

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
	CItem_Slot::ITEMSLOT_DESC Desc = {};
	
	_float2 vPos = { 320.f , 590.f };
	Desc.iUIType = ENUM_CLASS(UITYPE::PANEL);
	Desc.szName = "Item";
	Desc.vLocalPos = vPos;
	Desc.vLocalSize = { 103.f , 103.f };

	for (_int i = 0; i < ENUM_CLASS(ITEMTYPE::END); ++i)
	{
		if (i < ENUM_CLASS(ITEMTYPE::QUICK_1))
		{
			vector<CItem_Slot*> pItemGroup;
			Desc.iItemType = i;
			for (_int j = 0; j < 28; ++j)
			{
				Desc.iIndex = j;
				CItem_Slot* pItemSlot = static_cast<CItem_Slot*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_Item"), &Desc));

				if (pItemSlot == nullptr)
					return E_FAIL;

				m_Children.push_back(pItemSlot);

				Safe_AddRef(pItemSlot);
				pItemSlot->Insert_Bubble([this](BUBBLEEVENT* pArg) {this->Bubble_EventCall(pArg); });
				pItemGroup.push_back(pItemSlot);
				pItemGroup[j]->Update_Pos(j, vPos, 110.f, 4, 7);
			}
			m_pItems[i] = pItemGroup;
		}
		else
		{
			for (_int j = 0; j < 28; ++j)
			{
				m_pItems[i].push_back(m_pItems[ENUM_CLASS(ITEMTYPE::ATIVE)][j]);
				Safe_AddRef(m_pItems[ENUM_CLASS(ITEMTYPE::ATIVE)][j]);
			}
		}

	}

	CEquip_Slot::UISLOTDESC EquipDesc = {};

	EquipDesc.iUIType = ENUM_CLASS(UITYPE::PANEL);
	EquipDesc.szName = "Equip";
	EquipDesc.vLocalPos = { 0.f, 0.f };
	
	for (_int i = 0; i < ENUM_CLASS(EQUIPSLOT_TYPE::END); ++i)
	{
		if (i >= ENUM_CLASS(EQUIPSLOT_TYPE::QUICK_1) && i <= ENUM_CLASS(EQUIPSLOT_TYPE::QUICK_6))
			EquipDesc.vLocalSize = {90.f , 90.f };
		else
			EquipDesc.vLocalSize = { 103.f ,103.f };

		EquipDesc.iIndex = i;
		CEquip_Slot* pEquipSlot = static_cast<CEquip_Slot*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_Inven_Equip_Slot"), &EquipDesc));

		if (pEquipSlot == nullptr)
			return E_FAIL;

		m_Children.push_back(pEquipSlot);

		Safe_AddRef(pEquipSlot);
		pEquipSlot->Insert_Bubble([this](BUBBLEEVENT* pArg) {this->Bubble_EventCall(pArg); });
		m_pEquipSlot.push_back(pEquipSlot);

		EquipSlot_Setting(pEquipSlot, i);
	}


	return S_OK;
}

void CUI_Inven::TapType_Mapping(string szName)
{
	m_iSeleteTap = -1;
	if (szName == "SPEAR")
		m_iSeleteTap = ENUM_CLASS(ITEMTYPE::SPEAR);
	else if (szName == "GREATE")
		m_iSeleteTap = ENUM_CLASS(ITEMTYPE::GREATE);
	else if (szName == "HEAD")
		m_iSeleteTap = ENUM_CLASS(ITEMTYPE::HEAD);
	else if (szName == "TOP")
		m_iSeleteTap = ENUM_CLASS(ITEMTYPE::TOP);
	else if (szName == "GLOVES")
		m_iSeleteTap = ENUM_CLASS(ITEMTYPE::GLOVES);
	else if (szName == "BOTTOM")
		m_iSeleteTap = ENUM_CLASS(ITEMTYPE::BOTTOM);
	else if (szName == "SHOES")
		m_iSeleteTap = ENUM_CLASS(ITEMTYPE::SHOES);
	else if (szName == "NECK")
		m_iSeleteTap = ENUM_CLASS(ITEMTYPE::NECK);
	else if (szName == "RING")
		m_iSeleteTap = ENUM_CLASS(ITEMTYPE::RING);
	else if (szName == "ATIVE")
		m_iSeleteTap = ENUM_CLASS(ITEMTYPE::ATIVE);
	else if (szName == "COLLECTION")
		m_iSeleteTap = ENUM_CLASS(ITEMTYPE::COLLECTION);
	else if (szName == "MATERIAL")
		m_iSeleteTap = ENUM_CLASS(ITEMTYPE::MATERIAL);
	else if (szName == "QUICK_1")
		m_iSeleteTap = ENUM_CLASS(ITEMTYPE::QUICK_1);
	else if (szName == "QUICK_2")
		m_iSeleteTap = ENUM_CLASS(ITEMTYPE::QUICK_2);
	else if (szName == "QUICK_3")
		m_iSeleteTap = ENUM_CLASS(ITEMTYPE::QUICK_3);
	else if (szName == "QUICK_4")
		m_iSeleteTap = ENUM_CLASS(ITEMTYPE::QUICK_4);
	else if (szName == "QUICK_5")
		m_iSeleteTap = ENUM_CLASS(ITEMTYPE::QUICK_5);
	else if (szName == "QUICK_6")
		m_iSeleteTap = ENUM_CLASS(ITEMTYPE::QUICK_6);
	if (m_iSeleteTap < 0)
		return;
}

void CUI_Inven::Ready_Grouping()
{
	m_UpdateGroup.resize(ENUM_CLASS(TapGroup::END));

	m_UpdateGroup[ENUM_CLASS(TapGroup::WEAPON)].push_back(ENUM_CLASS(ITEMTYPE::SPEAR));
	m_UpdateGroup[ENUM_CLASS(TapGroup::WEAPON)].push_back(ENUM_CLASS(ITEMTYPE::GREATE));

	m_UpdateGroup[ENUM_CLASS(TapGroup::ARMOR)].push_back(ENUM_CLASS(ITEMTYPE::HEAD));
	m_UpdateGroup[ENUM_CLASS(TapGroup::ARMOR)].push_back(ENUM_CLASS(ITEMTYPE::TOP));
	m_UpdateGroup[ENUM_CLASS(TapGroup::ARMOR)].push_back(ENUM_CLASS(ITEMTYPE::GLOVES));
	m_UpdateGroup[ENUM_CLASS(TapGroup::ARMOR)].push_back(ENUM_CLASS(ITEMTYPE::BOTTOM));
	m_UpdateGroup[ENUM_CLASS(TapGroup::ARMOR)].push_back(ENUM_CLASS(ITEMTYPE::SHOES));

	m_UpdateGroup[ENUM_CLASS(TapGroup::ACC)].push_back(ENUM_CLASS(ITEMTYPE::NECK));
	m_UpdateGroup[ENUM_CLASS(TapGroup::ACC)].push_back(ENUM_CLASS(ITEMTYPE::RING));

	m_UpdateGroup[ENUM_CLASS(TapGroup::OTHER)].push_back(ENUM_CLASS(ITEMTYPE::ATIVE));
	m_UpdateGroup[ENUM_CLASS(TapGroup::OTHER)].push_back(ENUM_CLASS(ITEMTYPE::COLLECTION));
	m_UpdateGroup[ENUM_CLASS(TapGroup::OTHER)].push_back(ENUM_CLASS(ITEMTYPE::MATERIAL));

	m_UpdateGroup[ENUM_CLASS(TapGroup::QUICK)].push_back(ENUM_CLASS(ITEMTYPE::QUICK_1));
	m_UpdateGroup[ENUM_CLASS(TapGroup::QUICK)].push_back(ENUM_CLASS(ITEMTYPE::QUICK_2));
	m_UpdateGroup[ENUM_CLASS(TapGroup::QUICK)].push_back(ENUM_CLASS(ITEMTYPE::QUICK_3));
	m_UpdateGroup[ENUM_CLASS(TapGroup::QUICK)].push_back(ENUM_CLASS(ITEMTYPE::QUICK_4));
	m_UpdateGroup[ENUM_CLASS(TapGroup::QUICK)].push_back(ENUM_CLASS(ITEMTYPE::QUICK_5));
	m_UpdateGroup[ENUM_CLASS(TapGroup::QUICK)].push_back(ENUM_CLASS(ITEMTYPE::QUICK_6));

}

void CUI_Inven::UI_Animation(_float fTimeDelta)
{
	if (m_eAnimState == UIANIMSTATE::ON)
	{
		m_fAccTime += fTimeDelta * 3.f;
		__super::Update_Alpha(m_fAccTime);

		if (m_fAccTime >= 1.f)
		{
			m_fAccTime = 1.f;
			m_eAnimState = UIANIMSTATE::END;
		}
	}
	else if (m_eAnimState == UIANIMSTATE::OFF)
	{
		m_fAccTime -= fTimeDelta * 3.f;
		__super::Update_Alpha(m_fAccTime);

		if (m_fAccTime <= 0.f)
		{
			m_fAccTime = 0.f;
			m_eAnimState = UIANIMSTATE::END;
			m_IsUpdate = false;
		}
	}
}

void CUI_Inven::Change_Tap(_int iSeleteINdex)
{
	for (_int i = 0; i < (_int)m_UpdateGroup[m_iTapGroupIndex].size(); ++i)
	{
		if (i == iSeleteINdex)
		{
			m_iSeleteTap = m_UpdateGroup[m_iTapGroupIndex][i];
			for (_int i = 0; i < (_int)m_pInvenTap.size(); ++i)
			{
				if (m_iSeleteTap == i)
					m_pInvenTap[i]->Tap_Enable();
				else
					m_pInvenTap[i]->Tap_Disable();
			}
		}
		
		m_pInvenTap[m_UpdateGroup[m_iTapGroupIndex][i]]->Update_Pos(i, { 185.f, 135.f }, 80.f, (_int)m_UpdateGroup[m_iTapGroupIndex].size());
	}
}

CUI_Inven::ITEMTYPE CUI_Inven::Convert_UIntToITEMTYPE(_uint iItemIndex)
{
	_int iType = CClientInstance::GetInstance()->Get_Data<ITEM_DATA>(iItemIndex)->iType;
	_int iEffectID = CClientInstance::GetInstance()->Get_Data<ITEM_DATA>(iItemIndex)->iEffect_ID;
	_int iSubType = -1;
	
	if (iType < 4)
	{
		switch (iType)
		{
		case 1: return ITEMTYPE::ATIVE;
		case 2: return ITEMTYPE::COLLECTION;
		case 3: return ITEMTYPE::MATERIAL;
		}
	}
	else
	{
		iSubType = CClientInstance::GetInstance()->Get_Data<EQUIPITEM_DATA>(iEffectID)->iType;
		
		switch (iSubType)
		{
		case 1: return ITEMTYPE::SPEAR;
		case 2: return ITEMTYPE::GREATE;
		case 3: return ITEMTYPE::HEAD;
		case 4: return ITEMTYPE::TOP;
		case 5: return ITEMTYPE::GLOVES;
		case 6: return ITEMTYPE::BOTTOM;
		case 7: return ITEMTYPE::SHOES;
		case 8: return ITEMTYPE::NECK;
		case 9: return ITEMTYPE::RING;
		}
	}

	return ITEMTYPE::END;
}

void CUI_Inven::EquipSlot_Setting(CEquip_Slot* pSlot, _int iIndex)
{
	_float2 vPos = { -804.f , -318.f + 51.f };
	if(iIndex == ENUM_CLASS(EQUIPSLOT_TYPE::WEAPON))
		pSlot->Update_PosX(0, vPos, 130.f, 0, this);
	else if(iIndex >= ENUM_CLASS(EQUIPSLOT_TYPE::HEAD) && iIndex <= ENUM_CLASS(EQUIPSLOT_TYPE::SHOES))
		pSlot->Update_PosX(iIndex - ENUM_CLASS(EQUIPSLOT_TYPE::HEAD), vPos, 115.f, 155.f * 1, this);
	else if (iIndex >= ENUM_CLASS(EQUIPSLOT_TYPE::NECK) && iIndex <= ENUM_CLASS(EQUIPSLOT_TYPE::RING))
		pSlot->Update_PosX(iIndex - ENUM_CLASS(EQUIPSLOT_TYPE::NECK), vPos, 115.f, 155.f * 2, this);
	else if (iIndex >= ENUM_CLASS(EQUIPSLOT_TYPE::QUICK_1) && iIndex <= ENUM_CLASS(EQUIPSLOT_TYPE::QUICK_6))
	{
		vPos = { -810.f , -318.f + 51.f };
		pSlot->Update_PosX(iIndex - ENUM_CLASS(EQUIPSLOT_TYPE::QUICK_1), vPos, 95.f, 155.f * 3, this);
	}
	else if (iIndex >= ENUM_CLASS(EQUIPSLOT_TYPE::SOULE))
		pSlot->Update_PosX(0, vPos, 113.f, 155.f * 4, this);
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

	Safe_Release(m_pEquip_Panel);
	Safe_Release(m_pUIText);

	for (auto pSlot : m_pEquipSlot)
		Safe_Release(pSlot);
	m_pEquipSlot.clear();

	for (auto pTap : m_pItems)
	{
		for (auto pItem : pTap)
			Safe_Release(pItem);
		pTap.clear();
	}
	m_pItems.clear();

	for (auto pTap : m_pInvenTap)
		Safe_Release(pTap);
	m_pInvenTap.clear();

	Safe_Release(m_pBackGround);
}
