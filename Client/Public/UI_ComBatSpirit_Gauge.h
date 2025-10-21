#pragma once
#include "UI_ProgressBar.h"

NS_BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)
class CUI_CombatSpirit_Gauge : public CUI_ProgressBar
{
private:
	CUI_CombatSpirit_Gauge(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_CombatSpirit_Gauge(const CUI_CombatSpirit_Gauge& Prototype);
	virtual ~CUI_CombatSpirit_Gauge() = default;

public:
	void							Setting_Gauge(_float* pCulGauge);
public:
	virtual HRESULT					Initialize_Prototype(_uint iLevel);
	virtual HRESULT					Initialize_Clone(void* pArg) override;
	virtual void					Priority_Update(_float fTimeDelta) override;
	virtual void					Update(_float fTimeDelta) override;
	virtual void					Late_Update(_float fTimeDelta) override;
	virtual HRESULT					Render() override;

	virtual void					Bubble_EventCall();

private:
	CShader*						m_pShaderCom = { nullptr };
	CTexture*						m_pTextureCom = { nullptr };
	CVIBuffer_Rect*					m_pVIBufferCom = { nullptr };

	_float*							m_fCulGauge = { nullptr };

private:
	HRESULT							Ready_Component();

public:
	static CUI_CombatSpirit_Gauge*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CGameObject*			Clone(void* pArg) override;
	virtual void					Free() override;
};
NS_END
