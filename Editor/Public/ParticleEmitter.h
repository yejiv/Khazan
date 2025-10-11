#pragma once
#include "GameObject.h"
#include "Editor_Defines.h"

NS_BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Point_Instance;
NS_END

NS_BEGIN(Editor)

class CParticleEmitter final : public CGameObject
{
private:
	CParticleEmitter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CParticleEmitter(const CParticleEmitter& Prototype);
	~CParticleEmitter() = default;

public:
	virtual HRESULT						Initialize_Prototype() override;
	virtual HRESULT						Initialize_Clone(void* pArg) override;
	virtual void						Priority_Update(_float fTimeDelta) override;
	virtual void						Update(_float fTimeDelta) override;
	virtual void						Late_Update(_float fTimeDelta) override;
	virtual HRESULT						Render() override;

public:
	_wstring							Get_Name() { return m_strName; }

public:
	void								Recreate_Particle();

private:
	CShader*							m_pShaderCom = { nullptr };
	CTexture*							m_pTextureCom = { nullptr };
	CVIBuffer_Point_Instance*			m_pVIBufferCom = { nullptr };

	_wstring							m_strName = {};

private:
	HRESULT								Ready_Components();
	HRESULT								Bind_ShaderResources();

public:
	static CParticleEmitter*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*				Clone(void* pArg) override;
	virtual void						Free() override;
};

NS_END