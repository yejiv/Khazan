#pragma once
#include "UI_ProgressBar.h"

NS_BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)
class CUI_PlayerStamina_Gauge : public CUI_ProgressBar
{
private:
	CUI_PlayerStamina_Gauge(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_PlayerStamina_Gauge(const CUI_PlayerStamina_Gauge& Prototype);
	virtual ~CUI_PlayerStamina_Gauge() = default;

public:
	virtual HRESULT					Initialize_Prototype(_uint iLevel);
	virtual HRESULT					Initialize_Clone(void* pArg) override;
	virtual void					Priority_Update(_float fTimeDelta) override;
	virtual void					Update(_float fTimeDelta) override;
	virtual void					Late_Update(_float fTimeDelta) override;
	virtual HRESULT					Render() override;

	virtual void					Bubble_EventCall() override;
	virtual HRESULT					Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg) override;
private:
	CShader*						m_pShaderCom = { nullptr };
	CTexture*						m_pTextureCom = { nullptr };
	CVIBuffer_Rect*					m_pVIBufferCom = { nullptr };

private:
	HRESULT							Ready_Component();

public:
	static CUI_PlayerStamina_Gauge* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CGameObject*			Clone(void* pArg) override;
	virtual void					Free() override;
};
NS_END
