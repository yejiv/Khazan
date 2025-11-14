#pragma once
#include "Client_Defines.h"
#include "Effect_Element.h"
#include "VIBuffer_Point_Instance.h"

NS_BEGIN(Engine)
class CTexture;
NS_END

NS_BEGIN(Client)

class CEffect_Point_Instance : public Engine::CEffect_Element
{
public:
	typedef struct tagParticlePrototypeDesc : public CVIBuffer_Point_Instance::POINT_INSTANCE_DESC
	{
		_float4			vColor;
		_uint			iTextureIdx;
		_float2			iScrollSpeed;
		_uint			bGravity = false;
		_uint			iMaskTextureIdx;
		_float			fMaskScrollSpeed;
		_uint			bIsScrollVertical;		
		_uint			bIsScrollInverse;		
		_uint			iTurbulenceTextureIdx;	
		_uint			bIsTurbulence;			
		_uint			iCol, iRow;
		_float			fSpriteSpeed;
		DISSOLVE_DATA	sDissolveData;
	}PARTICLE_DESC;

private:
	CEffect_Point_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CEffect_Point_Instance(const CEffect_Point_Instance& Prototype);
	virtual ~CEffect_Point_Instance() = default;

public:
	virtual HRESULT					Initialize_Prototype(void* pArg);
	virtual HRESULT					Initialize_Clone();
	virtual void					Priority_Update(_float fTimeDelta) override;
	virtual void					Update(_float fTimeDelta) override;
	virtual void					Late_Update(_float fTimeDelta) override;
	virtual HRESULT					Render() override;
    virtual void				    SetLoopOff();

public:
	virtual void					Reset() override;

	virtual void					SetSpreadData(void* pArg) override;
	virtual void					SetRotateData(void* pArg) override;
	virtual void					SetTwinkleData(void* pArg) override;
	virtual void					SetUpwardData(void* pArg) override;
	virtual void					SetScrollData(void* pArg) override;

private:
	HRESULT							Ready_Component() override;
	HRESULT							Bind_ShaderResources() override;
	//virtual void					Apply(void* pArg) override;

private:
	CTexture*						m_pTextureCom = { nullptr };
	CTexture*						m_pSpriteTextureCom = { nullptr };
	CTexture*						m_pMaskTextureCom = { nullptr };
	CTexture*						m_pDissolveTextureCom = { nullptr };
	CVIBuffer_Point_Instance*		m_pVIBufferCom = { nullptr };

private :
	PARTICLE_DESC					m_sData;
	_float							m_fAccTime;
	_float							m_fSpriteTime;
	_uint							m_iUVIdx;

public:
	static CEffect_Point_Instance*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	virtual CEffect_Element*		Clone();
	virtual void					Free() override;

};

NS_END




