#pragma once
#include "UI_Texture.h"

NS_BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)
class CInteraction_Icon : public CUI_Texture
{
private:
	CInteraction_Icon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInteraction_Icon(const CInteraction_Icon& Prototype);
	virtual ~CInteraction_Icon() = default;

public:
	void							Anim_Start(_bool isStart);
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

	_float2							m_vDefaultPos = {};
	_bool							m_isAnim = { false };

private:
	HRESULT							Ready_Component();

public:
	static CInteraction_Icon*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*			Clone(void* pArg) override;
	virtual void					Free() override;
};
NS_END
