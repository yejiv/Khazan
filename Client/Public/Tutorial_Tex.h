#pragma once
#include "UI_Texture.h"
#include "Client_Defines.h"

NS_BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)
class CTutorial_Tex final : public CUI_Texture
{
private:
	enum MOVIE_TYPE { GUARD_1, GUARD_2, DODGE_1, DODGE_2, BRUTALATTACK, END };
private:
	CTutorial_Tex(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTutorial_Tex(const CTutorial_Tex& Prototype);
	virtual ~CTutorial_Tex() = default;

public:
	void								Setting_Tex(GUIDE_TYPE eType, _int iPage);
public:
	virtual HRESULT						Initialize_Prototype();
	virtual HRESULT						Initialize_Clone(void* pArg) override;
	virtual void						Priority_Update(_float fTimeDelta) override;
	virtual void						Update(_float fTimeDelta) override;
	virtual void						Late_Update(_float fTimeDelta) override;
	virtual HRESULT						Render() override;

private:
	CShader*							m_pShaderCom = { nullptr };
	CTexture*							m_pTextureCom = { nullptr };
	vector<CTexture*>					m_pMovieCom = { nullptr };
	CVIBuffer_Rect*						m_pVIBufferCom = { nullptr };

	_bool								m_isMovie = { false };
	MOVIE_TYPE							m_eMovieType = {};

private:
	HRESULT								Ready_Component();
	
public:
	static CTutorial_Tex*				Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*				Clone(void* pArg) override;
	virtual void						Free() override;
};

NS_END