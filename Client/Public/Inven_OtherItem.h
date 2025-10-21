#pragma once
#include "UI_Panel.h"
#include "Client_Defines.h"

NS_BEGIN(Client)
class CInven_OtherItem final : public CUI_Panel
{
private:
	CInven_OtherItem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInven_OtherItem(const CInven_OtherItem& Prototype);
	virtual ~CInven_OtherItem() = default;

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
	vector<class CInven_Tap*>			m_pInvenTap;
	_int								m_iSeleteTap = {};
	
public:
	static CInven_OtherItem*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CGameObject*				Clone(void* pArg) override;
	virtual void						Free() override;
};

NS_END