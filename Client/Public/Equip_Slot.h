#pragma once
#include "UI_Slot.h"
#include "Client_Defines.h"

NS_BEGIN(Client)
class CEquip_Slot final : public CUI_Slot
{
private:
	CEquip_Slot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEquip_Slot(const CEquip_Slot& Prototype);
	virtual ~CEquip_Slot() = default;
public:
	_bool								Add_Item(_int iItemIndex, class CItem_Slot* pItem);
	void								Release_Item(class CItem_Slot* pItem);

	void								Update_PosX(_int iIndex, _float2 vPos, _float fOffSetX, _float fOffSetY, CUIObject* pParent);
	_bool								Off_Selete();
public:
	virtual HRESULT						Initialize_Prototype(_uint iLevel);
	virtual HRESULT						Initialize_Clone(void* pArg) override;
	virtual void						Priority_Update(_float fTimeDelta) override;
	virtual void						Update(_float fTimeDelta) override;
	virtual void						Late_Update(_float fTimeDelta) override;

private:
	class CUI_Atlas_Icon*				m_pIcon = { nullptr };
	class CUI_Atlas_Icon*				m_pSeleteFx = { nullptr };
	class CItem_Slot*					m_pItem_Slot = { nullptr };
	class CUI_TextBox*					m_pTextBox = { nullptr };

	_bool								m_bIsSelete = { false };
	_int								m_iItemIndex = { -1 };

	_int								m_iSouleCount = {};
private:
	virtual	HRESULT						Ready_Prototype();
	virtual	HRESULT						Ready_Children();

	void								Update_State(_uint iGrade = 0);

	void								Equip_Item();
	void								Render_ItemInfo();
public:
	static CEquip_Slot* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void						Free() override;
};

NS_END