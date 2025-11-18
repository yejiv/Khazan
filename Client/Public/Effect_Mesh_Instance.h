#pragma once
#include "Client_Defines.h"
#include "Effect_Element.h"
#include "VIBuffer_Mesh_Instance.h"

NS_BEGIN(Engine)
class CTexture;
class CVIBuffer_Mesh_Instance;
NS_END

NS_BEGIN(Client)

class CEffect_Mesh_Instance : public Engine::CEffect_Element
{
public:
	typedef struct tagParticlePrototypeDesc : public CVIBuffer_Mesh_Instance::POINT_MESH_DESC
	{
		_float4		vColor;
		_uint		iTextureIdx;
		_uint		iMeshTypeIdx;
		_float2		iScrollSpeed;
		_uint		iMaskTextureIdx;
		_float		fMaskScrollSpeed;
		_uint		bIsScrollVertical;
		_uint		bIsScrollInverse;
		_uint		bIsTurbulence;					//노이즈텍스쳐
		_uint		iTurbulenceTextureIdx;			//랜덤 노이즈텍스쳐 인덱스
		_uint		bGravity = false;
		_uint		bIsFresnel = false;
		DISSOLVE_DATA sDissolveData;
	}PARTICLE_DESC;

private:
	CEffect_Mesh_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CEffect_Mesh_Instance(const CEffect_Mesh_Instance& Prototype);
	virtual ~CEffect_Mesh_Instance() = default;

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

private:
	CTexture*						m_pTextureCom = { nullptr };
	CTexture*						m_pMaskTextureCom = { nullptr };
	CTexture*						m_pDissolveTextureCom = { nullptr };
    CTexture* m_pNormalTextureCom = { nullptr };
	CVIBuffer_Mesh_Instance*		m_pVIBufferCom = { nullptr };

private :
	PARTICLE_DESC					m_sData;
	_float							m_fAccTime;
    _bool                           m_bIsNormal;

public:
	static CEffect_Mesh_Instance*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	virtual CEffect_Element*		Clone();
	virtual void					Free() override;

};

NS_END



