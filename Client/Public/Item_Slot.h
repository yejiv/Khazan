#pragma once
#include "UI_Slot.h"
#include "Client_Defines.h"

NS_BEGIN(Client)
class CItem_Slot final : public CUI_Slot
{
public:
	typedef struct Item_Slot_Tag : public CUI_Slot::UISLOTDESC
	{
		_int iItemType;
	}ITEMSLOT_DESC;
private:
	CItem_Slot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CItem_Slot(const CItem_Slot& Prototype);
	virtual ~CItem_Slot() = default;
public:
	_int								Get_ItemCount() { return m_iItemCount; }
	_int*								Get_ptrItemCount();
	_bool								Add_Item(_int iItemIndex);
	void								Update_Pos(_int iIndex, _float2 vPos, _float fOffSet, _int iMaxIndexX, _int iMaxIndexY);
	_bool								Off_Selete();
	void								is_Equip(_bool isEquip, _int iindex = 0);
public:
	virtual HRESULT						Initialize_Prototype(_uint iLevel);
	virtual HRESULT						Initialize_Clone(void* pArg) override;
	virtual void						Priority_Update(_float fTimeDelta) override;
	virtual void						Update(_float fTimeDelta) override;
	virtual void						Late_Update(_float fTimeDelta) override;

private:
	class CUI_Atlas_Icon*				m_pIcon = { nullptr };
	class CUI_Atlas_Icon*				m_pOverFx = { nullptr };
	class CUI_Atlas_Icon*				m_pSeleteFx = { nullptr };
	class CUI_Atlas_Icon*				m_pEquipIcon = { nullptr };
	class CUI_TextBox*					m_pTextBox = { nullptr };

	_bool								m_bIsSelete = { false };
	_bool								m_bIsEquip = { false };
	_int								m_iItemIndex = { -1 };
	_int								m_iItemCount = {};
	_int								m_iItemMaxCount = {};
	_int								m_iItemType = {};

private:
	virtual	HRESULT						Ready_Prototype();
	virtual	HRESULT						Ready_Children();

	void								Update_State(_uint iGrade = 0);

	void								Selete_Item();
	void								Equip_Item();
	void								Release_Item();
public:
	static CItem_Slot*					Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CGameObject*				Clone(void* pArg) override;
	virtual void						Free() override;
};

NS_END