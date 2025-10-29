#pragma once
#include "UI_ProgressBar.h"

NS_BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)
class CMon_Gauge : public CUI_ProgressBar
{
public:
	typedef struct tagMonGaugeDesc : public CUIObject::UIOBJECT_DESC
	{
		_int iTexPass = {};
		_int iShaderPass = {};

	}MONGAUGE_DESC;
private:
	CMon_Gauge(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMon_Gauge(const CMon_Gauge& Prototype);
	virtual ~CMon_Gauge() = default;

public:
	void							Settin_Progress(const _int* pCulValue, const _int* pMaxValue);
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

	const _int*						m_pMaxValue = { nullptr };
	const _int*						m_pCulValue = { nullptr };


private:
	HRESULT							Ready_Component();

public:
	static CMon_Gauge*				Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*			Clone(void* pArg) override;
	virtual void					Free() override;
};
NS_END
