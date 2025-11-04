#pragma once
#include "UI_ProgressBar.h"

NS_BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)
class CBossHp_Gauge : public CUI_ProgressBar
{
private:
	CBossHp_Gauge(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBossHp_Gauge(const CBossHp_Gauge& Prototype);
	virtual ~CBossHp_Gauge() = default;

public:
	void							Setting_Progress(const _float* pCulValue, const _float* pMaxValue);
	void							Reset_Progress();
public:
	virtual HRESULT					Initialize_Prototype();
	virtual HRESULT					Initialize_Clone(void* pArg) override;
	virtual void					Priority_Update(_float fTimeDelta) override;
	virtual void					Update(_float fTimeDelta) override;
	virtual void					Late_Update(_float fTimeDelta) override;
	virtual HRESULT					Render() override;

private:
	CShader*						m_pShaderCom = { nullptr };
	CTexture*						m_pTextureCom = { nullptr };
	CVIBuffer_Rect*					m_pVIBufferCom = { nullptr };

	const _float*						m_pMaxValue = { nullptr };
	const _float*						m_pCulValue = { nullptr };

	_float							m_fDeleyTime = {};
	_float							m_fAccTime = {};
	_bool							m_isDelay = {};
private:
	HRESULT							Ready_Component();

	void							Update_Deley(_float fTimeDelta);
public:
	static CBossHp_Gauge*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*			Clone(void* pArg) override;
	virtual void					Free() override;
};
NS_END
