#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
NS_END


NS_BEGIN(Client)

class CProjectile abstract : public CGameObject
{
public:
	typedef struct tagProjectileDesc : public CGameObject::GAMEOBJECT_DESC
	{
		_float3 vSpawnPosition = {};
		_float3 vDirection{ 0.f, 0.f, 1.f };
		_float fLifeTime = {};
		_float fDamage = {};

	}PROJECTILE_DESC;

protected:
	CProjectile(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CProjectile(const CProjectile& Prototype);
	virtual ~CProjectile() = default;

public:
	void					Set_SpanwPoint(_float3 vSpanwPoint) { m_vSpawnPoint = vSpanwPoint; }
	void					Set_SpawnDir(_float3 vSpawnDir) { m_vSpawnDir = vSpawnDir; };
	void					Set_Visible(_bool isVisible) { m_isVisible = isVisible; }

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize_Clone(void* pArg) override;
	virtual void			Priority_Update(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			Late_Update(_float fTimeDelta) override;
	virtual HRESULT			Render() override;
	virtual HRESULT			Render_Shadow() { return S_OK; }


protected:
	CModel*					m_pModelCom = { nullptr };
	CShader*				m_pShaderCom = { nullptr };

protected:
	_float					m_fLifeTime = {};
	_float					m_fCurrentTime = {};
	_float					m_fDamage = {};
	_float3					m_vSpawnPoint = {};
	_float3					m_vSpawnDir = {};

	_bool					m_isVisible = { false };

	_float4x4				m_OriginMatrix = {};

public:
	virtual CGameObject*	Clone(void* pArg) = 0;
	virtual void			Free() override;
};

NS_END