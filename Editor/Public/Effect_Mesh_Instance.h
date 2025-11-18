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
		_float		fMaskScrollSpeed;
		_uint		bIsScrollVertical;				//마스크 스크롤 방향 (상하 <-> 좌우)
		_uint		bIsScrollInverse;				//왼->오, 위-> 아래가 기본인데 이거 체크되어있으면 반대로!
		_uint		bIsTurbulence;					//노이즈텍스쳐
		_uint		iTurbulenceTextureIdx;				//랜덤 노이즈텍스쳐 인덱스
		_uint		bGravity = false;
		_uint		bIsFresnel = false;
		DISSOLVE_DATA sDissolveData;
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
	CTexture*						m_pDissolveTextureCom = { nullptr };
	CTexture*						m_pNormalTextureCom = { nullptr };
	CVIBuffer_Mesh_Instance*		m_pVIBufferCom = { nullptr };

private :
	PARTICLE_DESC					m_sData;
	PARTICLE_DESC					m_sEditingData;
	/*Editing data*/
	_bool							m_bIsMaskScrolling;
	_float							m_fAccTime;
    _bool                           m_bIsNormal; 

public:
	static CEffect_Mesh_Instance*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	virtual CGameObject*			Clone(void* pArg);
	virtual void					Free() override;

};

NS_END



