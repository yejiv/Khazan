#pragma once
#include "GameObject.h"
#include "Editor_Defines.h"
#include "ParticleEmitter.h"

NS_BEGIN(Editor)

class CParticleSystem final : public CGameObject
{
public:
	typedef struct tagParticleSystemDesc
	{
		_wstring strName{};
	}PARTICLE_SYSTEM_DESC;

private:
	CParticleSystem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CParticleSystem(const CParticleSystem& Prototype);
	~CParticleSystem() = default;

public:
	virtual HRESULT						Initialize_Prototype() override;
	virtual HRESULT						Initialize_Clone(void* pArg) override;
	virtual void						Priority_Update(_float fTimeDelta) override;
	virtual void						Update(_float fTimeDelta) override;
	virtual void						Late_Update(_float fTimeDelta) override;
	virtual HRESULT						Render() override;

public:
	_wstring							Get_Name() { return m_strName; }
	_uint								Get_NumEmitters() { return m_iNumEmitters; }
	CParticleEmitter*					Get_Emitter(_uint iIndex);

public:
	HRESULT								Add_Emitter(CParticleEmitter::PARTICLE_EMITTER_DESC Desc);
	HRESULT								Remove_Emitter(_uint iIndex);
	void								Recreate_Emitter();

private:
	vector<CParticleEmitter*>			m_Emitters;
	_uint								m_iNumEmitters = {};

	_wstring							m_strName = {};

public:
	static CParticleSystem*				Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*				Clone(void* pArg) override;
	virtual void						Free() override;
};

NS_END