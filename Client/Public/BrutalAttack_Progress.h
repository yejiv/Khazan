#pragma once
#include "UI_ProgressBar.h"

NS_BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)
class CBrutalAttack_Progress : public CUI_ProgressBar
{
public:
	typedef struct tagGuideGaugeDesc : public CUIObject::UIOBJECT_DESC
	{
		const _float* pMaxValue = { nullptr };
		const _float* pCulValue = { nullptr };

	}GUIDEGAUGE_DESC;
private:
	CBrutalAttack_Progress(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBrutalAttack_Progress(const CBrutalAttack_Progress& Prototype);
	virtual ~CBrutalAttack_Progress() = default;

public:
	virtual HRESULT					Initialize_Prototype();
	virtual HRESULT					Initialize_Clone(void* pArg) override;
	virtual void					Priority_Update(_float fTimeDelta) override;
	virtual void					Update(_float fTimeDelta) override;
	virtual void					Late_Update(_float fTimeDelta) override;
	virtual HRESULT					Render() override;

private:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

	const _float* m_pMaxValue = { nullptr };
	const _float* m_pCulValue = { nullptr };


private:
	HRESULT							Ready_Component();

public:
	static CBrutalAttack_Progress* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void					Free() override;
};
NS_END
