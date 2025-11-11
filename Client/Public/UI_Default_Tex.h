#pragma once
#include "UI_Texture.h"
#include "Client_Defines.h"

NS_BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)
class CUI_Default_Tex final : public CUI_Texture
{
private:
	CUI_Default_Tex(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Default_Tex(const CUI_Default_Tex& Prototype);
	virtual ~CUI_Default_Tex() = default;

public:
	HRESULT					Set_Texture(const _wstring& strPrototypeTag, _int iTexPass = 0);
    void                    Tex_Scaling(_float fSizeX, _float fSizeY);
public:
	virtual HRESULT			Initialize_Prototype();
	virtual HRESULT			Initialize_Clone(void* pArg) override;
	virtual void			Priority_Update(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			Late_Update(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

	virtual HRESULT			Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg) override;

private:
	CShader*				m_pShaderCom = { nullptr };
	CTexture*				m_pTextureCom = { nullptr };
	CVIBuffer_Rect*			m_pVIBufferCom = { nullptr };

private:
	HRESULT					Ready_Component();

public:
	static CUI_Default_Tex* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

NS_END