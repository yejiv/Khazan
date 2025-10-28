#pragma once
#include "UI_Panel.h"
#include "Client_Defines.h"

NS_BEGIN(Client)
class CUI_Inven final : public CUI_Panel
{
public:
	enum class UIANIMSTATE { ON, OFF, END };
	enum class TapGroup {WEAPON, ARMOR, ACC, OTHER, QUICK, END};
	enum class ITEMTYPE { SPEAR, GREATE, HEAD, TOP, GLOVES, BOTTOM, SHOES, NECK, RING, ATIVE, COLLECTION, MATERIAL, QUICK_1, QUICK_2, QUICK_3, QUICK_4, QUICK_5, QUICK_6, END};
	enum class EVENT_TYPE { TAP, ITEM_SELETE, ITEM_EQUIP, SLOT_EQUIP, ITEM_RELEASE, END};
	
	enum class EQUIPSLOT_TYPE { WEAPON, HEAD, TOP, GLOVES, BOTTOM, SHOES, NECK, RING, QUICK_1, QUICK_2, QUICK_3, QUICK_4, QUICK_5, QUICK_6, SOULE, END };

	typedef struct tagInvenBubbleEventTag : public CUIObject::BUBBLEEVENT
	{
		EVENT_TYPE eBubbleType = EVENT_TYPE::END;
		_int iIndex = {};
		_int iTypeIndex = {};
		_int iItemType = {};
		_int iItemIndex = {};
		class CItem_Slot* pItem = { nullptr };
	}INVENBUBBLE_DESC;

	typedef struct tagInvenOpenTag
	{
		string szName;
		_bool isOpen;
		_bool isEquip;
	}INVEN_ONOFF_DESC;

private:
	CUI_Inven(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Inven(const CUI_Inven& Prototype);
	virtual ~CUI_Inven() = default;

public:
	void								On_Panel();
	void								Off_Panel();
	_bool								Add_Item(_uint iItemIndex);

public:
	virtual HRESULT						Initialize_Prototype(_uint iLevel);
	virtual HRESULT						Initialize_Clone(void* pArg) override;
	virtual void						Priority_Update(_float fTimeDelta) override;
	virtual void						Update(_float fTimeDelta) override;
	virtual void						Late_Update(_float fTimeDelta) override;
	virtual HRESULT						Render() override;

	virtual HRESULT						Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg) override;
	virtual void						Bubble_EventCall(BUBBLEEVENT* pArg) override;
	virtual	HRESULT						Update_Switch(void* pArg);

private:
	class CUI_BackGround*				m_pBackGround = { nullptr };
	class CUI_TextBox*					m_pUIText = { nullptr };

	UIANIMSTATE							m_eAnimState = { UIANIMSTATE::END };
	
	vector<vector<class CItem_Slot*>>	m_pItems;
	vector<vector<_int>>				m_UpdateGroup;
	vector<class CInven_Tap*>			m_pInvenTap;

	class CEquip_Panel*					m_pEquip_Panel = { nullptr };
	vector<class CEquip_Slot*>			m_pEquipSlot;

	_float								m_fAccTime = {};

	_int								m_iSeleteTap = {};
	_int								m_iTapGroupIndex = {};

	string								m_strReturnName = {};
	_bool								m_bIsEquip = {};
private:
	virtual	HRESULT						Ready_Prototype();
	HRESULT								Ready_Object();
	HRESULT								Ready_SlotSet();

	void								TapType_Mapping(string szName);
	void								Ready_Grouping();
	void								UI_Animation(_float fTimeDelta);
	void								Change_Tap(_int iSeleteINdex);

	ITEMTYPE							Convert_UIntToITEMTYPE(_uint iItemIndex);
	void								EquipSlot_Setting(class CEquip_Slot* pSlot, _int iIndex);

public:
	static CUI_Inven*					Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CGameObject*				Clone(void* pArg) override;
	virtual void						Free() override;
};

NS_END