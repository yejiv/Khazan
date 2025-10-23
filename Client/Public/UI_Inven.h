#pragma once
#include "UI_Panel.h"
#include "Client_Defines.h"

NS_BEGIN(Client)
class CUI_Inven final : public CUI_Panel
{
public:
	enum class UIANIMSTATE { ON, OFF, END };
	enum class TapGroup {WEAPON, ARMOR, ACC, OTHER, END};
	enum class ITEMTYPE { SPEAR, GREATE, HEAD, TOP, GLOVES, BOTTOM, SHOES, NECK, RING, ATIVE, COLLECTION, MATERIAL, END};
	enum class EVENT_TYPE { TAP, ITEM_EQUIP, ITEM_RELEASE, END};
	typedef struct tagInvenBubbleEventTag : public CUIObject::BUBBLEEVENT
	{
		EVENT_TYPE eBubbleType = EVENT_TYPE::END;
		_int iIndex = {};
		_int iTypeIndex = {};
	}INVENBUBBLE_DESC;

private:
	CUI_Inven(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Inven(const CUI_Inven& Prototype);
	virtual ~CUI_Inven() = default;

public:
	void								On_Panel();
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

private:
	class CUI_BackGround*				m_pBackGround = { nullptr };

	_float								m_fAccTime = {};
	UIANIMSTATE							m_eAnimState = { UIANIMSTATE::END };
	vector<vector<class CItem_Slot*>>	m_pItems;

	vector<vector<_int>>				m_UpdateGroup;

	vector<class CInven_Tap*>			m_pInvenTap;
	_int								m_iSeleteTap = {};

	_int								m_iTapGroupIndex = {};

	_bool								m_IsText = {false};
private:
	virtual	HRESULT						Ready_Prototype();
	HRESULT								Ready_Object();
	HRESULT								Ready_SlotSet();

	void								TapType_Mapping(string szName);
	void								Ready_Grouping();
	void								UI_Animation(_float fTimeDelta);
	void								Change_Tap();

	ITEMTYPE							Convert_UIntToITEMTYPE(_uint iItemIndex);
public:
	static CUI_Inven*					Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CGameObject*				Clone(void* pArg) override;
	virtual void						Free() override;
};

NS_END