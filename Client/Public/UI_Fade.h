#pragma once
#include "UI_Texture.h"
#include "Client_Defines.h"

NS_BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)
class CUI_Fade final : public CUI_Texture
{
private:
	enum class FADE_STATE{FADE_IN, FADE_OUT, END};
private:
	CUI_Fade(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Fade(const CUI_Fade& Prototype);
	virtual ~CUI_Fade() = default;

public:
	void								Fade_In(function<void()> FadeEvent);
	void								Fade_Out(function<void()> FadeEvent);
	_bool								Fade_End() { return m_eFadeType == FADE_STATE::END; }
public:
	virtual HRESULT						Initialize_Prototype();
	virtual HRESULT						Initialize_Clone(void* pArg) override;
	virtual void						Priority_Update(_float fTimeDelta) override;
	virtual void						Update(_float fTimeDelta) override;
	virtual void						Late_Update(_float fTimeDelta) override;
	virtual HRESULT						Render() override;

private:
	CShader*							m_pShaderCom = { nullptr };
	CVIBuffer_Rect*						m_pVIBufferCom = { nullptr };

	FADE_STATE							m_eFadeType = { FADE_STATE::END };
	function<void()>					m_FadeEvent;
private:
	HRESULT								Ready_Component();

public:
	static CUI_Fade*					Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*				Clone(void* pArg) override;
	virtual void						Free() override;
};

NS_END