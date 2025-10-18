#pragma once
#include "Editor_Defines.h"
#include "Effect_Element.h"

NS_BEGIN(Engine)
class CTexture;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Editor)

class CEffect_Sprite : public CEffect_Element
{
private:
	CEffect_Sprite(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CEffect_Sprite(const CEffect_Sprite& Prototype);
	virtual ~CEffect_Sprite() = default;

public:
	virtual HRESULT			Initialize_Clone(void* pArg) override;
	virtual void			Priority_Update(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			Late_Update(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

public:
	virtual void			Save_Data(ofstream& os);
	virtual void			Edit_Element() override;
	virtual void			RevertChanges() override;
	virtual void			Reset() override;

	void					SetSpriteData();

private:
	HRESULT					Ready_Component() override;
	HRESULT					Bind_ShaderResources() override;
	virtual void			Apply(void* pArg) override;

private:
	CTexture*				m_pTextureCom = { nullptr };
	CVIBuffer_Rect*			m_pVIBufferCom = { nullptr };

private :
	_float					m_pSpriteSpeed;
	_float					m_fCurTime;
	_uint					m_iUVIdx;
	_uint					m_iCol, m_iRow;
	
public:
	static CEffect_Sprite*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;

};

NS_END




