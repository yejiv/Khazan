#pragma once
#include "Editor_Defines.h"
#include "Effect_Element.h"
#include "VIBuffer_Mesh_Instance.h"

NS_BEGIN(Engine)
class CTexture;
class CVIBuffer_Mesh_Instance;
NS_END

NS_BEGIN(Editor)

class CEffect_Mesh_Instance : public CEffect_Element
{
public:
	typedef struct tagParticlePrototypeDesc : public CVIBuffer_Mesh_Instance::POINT_MESH_DESC
	{
		_float4		vColor;
		_uint		iTextureIdx;
		_uint		iMeshTypeIdx;
		_float2		iScrollSpeed;
		_uint		iMaskTextureIdx;
		_uint		bScrollDir;
		_uint		bGravity = false;
	}PARTICLE_DESC;

private:
	CEffect_Mesh_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CEffect_Mesh_Instance(const CEffect_Mesh_Instance& Prototype);
	virtual ~CEffect_Mesh_Instance() = default;

public:
	virtual HRESULT					Initialize_Clone(void* pArg) override;
	virtual void					Priority_Update(_float fTimeDelta) override;
	virtual void					Update(_float fTimeDelta) override;
	virtual void					Late_Update(_float fTimeDelta) override;
	virtual HRESULT					Render() override;

public:
	virtual void					Save_Data(ofstream& os);
	virtual void					Edit_Element() override;
	virtual void					RevertChanges() override;
	virtual void					Reset() override;

	virtual void					SetSpreadData(void* pArg) override;
	virtual void					SetRotateData(void* pArg) override;
	virtual void					SetTwinkleData(void* pArg) override;
	virtual void					SetUpwardData(void* pArg) override;
	virtual void					SetScrollData(void* pArg) override;

private:
	HRESULT							Ready_Component() override;
	HRESULT							Bind_ShaderResources() override;
	virtual void					Apply(void* pArg) override;

private:
	CTexture*						m_pTextureCom = { nullptr };
	CTexture*						m_pMaskTextureCom = { nullptr };
	CVIBuffer_Mesh_Instance*		m_pVIBufferCom = { nullptr };

private :
	PARTICLE_DESC					m_sData;
	PARTICLE_DESC					m_sEditingData;
	_float							m_fCurTime;

public:
	static CEffect_Mesh_Instance*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	virtual CGameObject*			Clone(void* pArg);
	virtual void					Free() override;

};

NS_END



