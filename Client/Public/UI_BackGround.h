#pragma once
#include "UI_Texture.h"
#include "Client_Defines.h"

NS_BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)
class CUI_BackGround final : public CUI_Texture
{
public:
	enum class UIBGTYPE { MAIN, ITEM, END  };
private:
	CUI_BackGround(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_BackGround(const CUI_BackGround& Prototype);
	virtual ~CUI_BackGround() = default;

public:
	void					Setting_BG(UIBGTYPE eType);

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize_Clone(void* pArg) override;
	virtual void			Priority_Update(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			Late_Update(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

	virtual void			Bubble_EventCall();

private:
	CShader*				m_pShaderCom = { nullptr };
	CTexture*				m_pTextureCom = { nullptr };
	CVIBuffer_Rect*			m_pVIBufferCom = { nullptr };
	CTexture*				m_pMaskTextureCom = { nullptr };

	CShader*				m_pFontShaderCom = { nullptr };

	UIBGTYPE				m_eBGType = { UIBGTYPE ::END};
private:
	HRESULT					Ready_Prototype();
	HRESULT					Ready_Component();

	HRESULT					Bind_Mask();
public:
	static CUI_BackGround*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

NS_END