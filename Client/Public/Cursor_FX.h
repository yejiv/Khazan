#pragma once
#include "UIObject.h"
#include "Client_Defines.h"

NS_BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)
class CCursor_FX final : public CUIObject
{
public:
	enum class MOUSE_MODE { UI_MODE, PLAY_MODE, END };
private:
	CCursor_FX(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCursor_FX(const CCursor_FX& Prototype);
	virtual ~CCursor_FX() = default;

public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize_Clone(void* pArg) override;
	virtual void				Priority_Update(_float fTimeDelta) override;
	virtual void				Update(_float fTimeDelta) override;
	virtual void				Late_Update(_float fTimeDelta) override;
	virtual HRESULT				Render() override;

public:
	virtual void				Reset() override;
private:
	CShader*					m_pShaderCom = { nullptr };
	CTexture*					m_pTextureCom = { nullptr };
	CVIBuffer_Rect*				m_pVIBufferCom = { nullptr };

	_float						m_fAccTime = {};

private:
	HRESULT						Ready_Prototype();
	HRESULT						Ready_Component();

public:
	static CCursor_FX*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};

NS_END
