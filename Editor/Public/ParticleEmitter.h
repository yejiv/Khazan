#pragma once
#include "GameObject.h"
#include "Editor_Defines.h"
#include "VIBuffer_Point_Instance.h"

NS_BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Point_Instance;
NS_END

NS_BEGIN(Editor)

class CParticleEmitter final : public CGameObject
{
public:
	typedef struct tagParticleEmitterDesc
	{
		_wstring strName{};
	}PARTICLE_EMITTER_DESC;

private:
	CParticleEmitter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CParticleEmitter(const CParticleEmitter& Prototype);
	~CParticleEmitter() = default;

public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize_Clone(void* pArg) override;
	virtual void				Priority_Update(_float fTimeDelta) override;
	virtual void				Update(_float fTimeDelta) override;
	virtual void				Late_Update(_float fTimeDelta) override;
	virtual HRESULT				Render() override;

public:
	_wstring					Get_Name() { return m_strName; }
	void						Set_Name(_wstring strName) { m_strName = strName; }

	_bool						Get_Spread() { return m_isSpread; }
	void						Set_Spread(_bool isEnable) { m_isSpread = isEnable; }

	_bool						Get_Drop() { return m_isDrop; }
	void						Set_Drop(_bool isEnable) { m_isDrop = isEnable; }

	CVIBuffer_Point_Instance::POINT_INSTANCE_DESC Get_ParticleInfo() { return m_ParticleInfo; }

public:
	HRESULT						Recreate_Particle(CVIBuffer_Point_Instance::POINT_INSTANCE_DESC PointDesc);

private:
	CShader*					m_pShaderCom = { nullptr };
	CTexture*					m_pTextureCom = { nullptr };
	CVIBuffer_Point_Instance*	m_pVIBufferCom = { nullptr };

	_wstring					m_strName = {};
	
	_bool						m_isSpread = {};
	_bool						m_isDrop = {};
	_bool						m_isGravity = {};

	CVIBuffer_Point_Instance::POINT_INSTANCE_DESC	m_ParticleInfo = {};

private:
	HRESULT								Ready_Components();
	HRESULT								Bind_ShaderResources();

public:
	static CParticleEmitter*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*				Clone(void* pArg) override;
	virtual void						Free() override;
};

NS_END