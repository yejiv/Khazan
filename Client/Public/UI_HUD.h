#pragma once
#include "UI_Panel.h"
#include "Client_Defines.h"

NS_BEGIN(Client)
class CUI_HUD final : public CUI_Panel
{
private:
	CUI_HUD(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_HUD(const CUI_HUD& Prototype);
	virtual ~CUI_HUD() = default;

public:
	void						Add_Item(_int iIndex);

public:
	virtual HRESULT				Initialize_Prototype(_uint iLevel);
	virtual HRESULT				Initialize_Clone(void* pArg) override;
	virtual void				Priority_Update(_float fTimeDelta) override;
	virtual void				Update(_float fTimeDelta) override;
	virtual void				Late_Update(_float fTimeDelta) override;
	virtual HRESULT				Render() override;

	virtual void				Bubble_EventCall();

private:
	deque<class CItemInfo_Hud*> m_pRender_ItemInfo;
	queue<class CItemInfo_Hud*> m_pItemInfo;
private:
	HRESULT						Ready_Prototype();
	HRESULT						Ready_Childeren();

public:
	static CUI_HUD*				Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};

NS_END