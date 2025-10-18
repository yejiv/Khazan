#pragma once
#include "Client_Defines.h"
#include "UIObject.h"

NS_BEGIN(Engine)
class CShader;
class CTexture_Atlas;
class CVIBuffer_Instance_UI;
NS_END

NS_BEGIN(Client)
class CAtlas_RenderGroup final : public CUIObject
{
public:
	typedef struct AtlasRenderGrouptag
	{
		_float fDepth = {};
		_int  iShdaerPass = {};
	}ATLASGROUP_DESC;
private:
	CAtlas_RenderGroup(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CAtlas_RenderGroup(const CAtlas_RenderGroup& Prototype);
	virtual ~CAtlas_RenderGroup() = default;

public:
	virtual HRESULT					Initialize_Prototype() override;
	virtual HRESULT					Initialize_Clone(void* pArg) override;
	virtual void					Priority_Update(_float fTimeDelta) override;
	virtual void					Update(_float fTimeDelta) override;
	virtual void					Late_Update(_float fTimeDelta) override;
	virtual HRESULT					Render() override;

public:
	HRESULT							Add_UIInstance(const VTXINSTANCE_UI* pUIData);

private:
	CShader*						m_pShaderCom = { nullptr };
	CVIBuffer_Instance_UI*			m_pVIBufferCom = { nullptr };
	CTexture_Atlas*					m_pTextureCom = { nullptr };

	_bool							m_isRender = { false };
private:
	HRESULT							Ready_Component();

public:
	static CAtlas_RenderGroup*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*			Clone(void* pArg);
	virtual void					Free() override;
};

NS_END