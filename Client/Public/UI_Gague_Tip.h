#pragma once
#include "UI_Texture.h"
#include "Client_Defines.h"

NS_BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)
class CUI_Gague_Tip final : public CUI_Texture
{
private:
	CUI_Gague_Tip(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Gague_Tip(const CUI_Gague_Tip& Prototype);
	virtual ~CUI_Gague_Tip() = default;

public:
	void						Update_Pos(_float2 vPos);

public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize_Clone(void* pArg) override;
	virtual void				Priority_Update(_float fTimeDelta) override;
	virtual void				Update(_float fTimeDelta) override;
	virtual void				Late_Update(_float fTimeDelta) override;
	virtual HRESULT				Render() override;

private:
	CShader*					m_pShaderCom = { nullptr };
	CTexture*					m_pTextureCom = { nullptr };
	CVIBuffer_Rect*				m_pVIBufferCom = { nullptr };

private:
	HRESULT						Ready_Component();

public:
	static CUI_Gague_Tip*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};

NS_END