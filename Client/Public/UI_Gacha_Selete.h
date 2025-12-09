#pragma once
#include "UI_Texture.h"

NS_BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)
class CUI_Gacha_Selete : public CUI_Texture
{
private:
	CUI_Gacha_Selete(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Gacha_Selete(const CUI_Gacha_Selete& Prototype);
	virtual ~CUI_Gacha_Selete() = default;

public:
	virtual HRESULT					Initialize_Prototype();
	virtual HRESULT					Initialize_Clone(void* pArg) override;
	virtual void					Late_Update(_float fTimeDelta, _vector vPos);
	virtual HRESULT					Render() override;

private:
	CShader*						m_pShaderCom = { nullptr };
	CTexture*						m_pTextureCom = { nullptr };
	CVIBuffer_Rect*					m_pVIBufferCom = { nullptr };

	_float2							m_vDefaultPos = {};

private:
	HRESULT							Ready_Component();
    void						    Update_WorldPos(_vector vPos);
public:
	static CUI_Gacha_Selete*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*			Clone(void* pArg) override;
	virtual void					Free() override;
};
NS_END
