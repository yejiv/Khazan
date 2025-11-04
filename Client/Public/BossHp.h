#pragma once
#include "UI_Panel.h"
#include "Client_Defines.h"

NS_BEGIN(Client)
class CBossHp final : public CUI_Panel
{
public:
	enum class UIANIMSTATE { ON, OFF, END };

	typedef struct BossMonUpdateTag
	{
		_bool isOpen;
		const _float* pHpValue;
		const _float* pHpMaxValue;
		const _float* pStaminaCulValue;
		const _float* pStaminaMaxValue;
	}BOSSMON_UPDATE_DESC;

private:
	CBossHp(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBossHp(const CBossHp& Prototype);
	virtual ~CBossHp() = default;

public:
	virtual HRESULT				Initialize_Prototype(_uint iLevel);
	virtual HRESULT				Initialize_Clone(void* pArg) override;
	virtual void				Priority_Update(_float fTimeDelta) override;
	virtual void				Update(_float fTimeDelta) override;
	virtual void				Late_Update(_float fTimeDelta) override;
	virtual HRESULT				Render() override;

	virtual void				Bubble_EventCall();
	virtual HRESULT				Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg) override;
	virtual	HRESULT				Update_Switch(void* pArg);
private:
	UIANIMSTATE					m_eAnimState = { UIANIMSTATE::END };
	_float						m_fAccTime = {};

	class CBossHp_Gauge*		m_pHPGauge = { nullptr };
	class CBossHp_Gauge*		m_pStaminaGauge = { nullptr };

private:
	virtual	HRESULT				Ready_Prototype();
	void						UI_Animation(_float fTimeDelta);

public:
	static CBossHp* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void				Free() override;
};

NS_END