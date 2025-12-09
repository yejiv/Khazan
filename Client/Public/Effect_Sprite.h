#pragma once
#include "Client_Defines.h"
#include "Effect_Element.h"

NS_BEGIN(Engine)
class CTexture;
class CVIBuffer_Point;
NS_END

NS_BEGIN(Client)

class CEffect_Sprite : public Engine::CEffect_Element
{
public:
	typedef struct tagSpriteDesc
	{
		_uint	IsLoop;
		_uint	iCol, iRow;
		_float	fSpriteSpeed;
		_float4 vColor;
		_float	fSize;
		_float	fSizeRatio;
		_float	ScalingValue;
		_uint	iTextureIdx;
		_uint	iMaskTextureIdx;
		_float	fMaskScrollSpeed;
		_uint	bIsScrollVertical;
		_uint	bIsScrollInverse;
		_float3	fOffset;
	}SPRITE_DESC;

private:
	CEffect_Sprite(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CEffect_Sprite(const CEffect_Sprite& Prototype);
	virtual ~CEffect_Sprite() = default;

public:
	virtual HRESULT			Initialize_Prototype(void* pArg);
	virtual HRESULT			Initialize_Clone();
	virtual void			Priority_Update(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			Late_Update(_float fTimeDelta) override;
	virtual HRESULT			Render() override;
    virtual void			SetLoopOff();

public:
	virtual void			Reset() override;
	void					Active() override;

private:
	HRESULT					Ready_Component() override;
	HRESULT					Bind_ShaderResources() override;
	//virtual void			Apply(void* pArg) override;

private:
	CTexture*				m_pTextureCom = { nullptr };
	CVIBuffer_Point*		m_pVIBufferCom = { nullptr };

private :
	_float					m_fCurTime;
	_uint					m_iUVIdx;
	SPRITE_DESC				m_sData;
	
    _bool                   m_bLoop;

public:
	static CEffect_Sprite*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	virtual CEffect_Element*	Clone();
	virtual void			Free() override;

};

NS_END




