#pragma once
#include "UI_Slot.h"
#include "Client_Defines.h"

NS_BEGIN(Client)
class CItem_Slot final : public CUI_Slot
{
private:
	CItem_Slot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CItem_Slot(const CItem_Slot& Prototype);
	virtual ~CItem_Slot() = default;
public:
	void								Input_Slot();
	void								Update_Pos(_int iIndex, _float2 vPos, _float fOffSet, _int iMaxIndexX, _int iMaxIndexY);
public:
	virtual HRESULT						Initialize_Prototype(_uint iLevel);
	virtual HRESULT						Initialize_Clone(void* pArg) override;
	virtual void						Priority_Update(_float fTimeDelta) override;
	virtual void						Update(_float fTimeDelta) override;
	virtual void						Late_Update(_float fTimeDelta) override;

private:
	virtual	HRESULT						Ready_Prototype();
	virtual	HRESULT						Ready_Childer();

	class CUI_Atlas_Icon*				m_pIcon = { nullptr };
	class CUI_Atlas_Icon*				m_pOverFx = { nullptr };
	class CUI_Atlas_Icon*				m_pSeleteFx = { nullptr };

	_bool								m_bIsSelete = { false };

public:
	static CItem_Slot*					Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CGameObject*				Clone(void* pArg) override;
	virtual void						Free() override;
};

NS_END