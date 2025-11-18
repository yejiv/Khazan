#pragma once
#include "UI_ProgressBar.h"

NS_BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)
class CUI_PlayerHP_Gauge : public CUI_ProgressBar
{
private:
	CUI_PlayerHP_Gauge(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_PlayerHP_Gauge(const CUI_PlayerHP_Gauge& Prototype);
	virtual ~CUI_PlayerHP_Gauge() = default;

public:
	virtual HRESULT				Initialize_Prototype(_uint iLevel);
	virtual HRESULT				Initialize_Clone(void* pArg) override;
	virtual void				Priority_Update(_float fTimeDelta) override;
	virtual void				Update(_float fTimeDelta) override;
	virtual void				Late_Update(_float fTimeDelta) override;
	virtual HRESULT				Render() override;

	virtual HRESULT				Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg) override;
	virtual void				Update_Alpha(_float fAlpha);
private:
	CShader*					m_pShaderCom = { nullptr };
	CTexture*					m_pTextureCom = { nullptr };
	CVIBuffer_Rect*				m_pVIBufferCom = { nullptr };

    _float                     m_fDefaultValue = {};

	_float						m_fDeleyTime = {};
	_float						m_fAccTime = {};
	_bool						m_isDelay = {};
	
    const _float*               m_pMaxHp = { nullptr };
    const _float*               m_pCulHp = { nullptr };

	class CUI_Gague_Tip*		m_pUITip = {nullptr};
private:
	HRESULT						Ready_Component();
	HRESULT						Ready_Children();

	void						Update_Deley(_float fTimeDelta);
public:
	static CUI_PlayerHP_Gauge*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};
NS_END	
