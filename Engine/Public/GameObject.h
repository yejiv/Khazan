#pragma once

#include "Transform.h"

NS_BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CBase
{
public:
	typedef struct tagGameObject : public CTransform::TRANSFORM_DESC
	{

	}GAMEOBJECT_DESC;
protected:
	CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject(const CGameObject& Prototype);
	virtual ~CGameObject() = default;

public:
	class CComponent* Get_Component(const _wstring& strComponentTag);

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize_Clone(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();
	virtual HRESULT Render_Shadow() { return S_OK; }

public:
	void Set_IsPool(_bool isPool) { m_isPool = isPool; }
	_bool Get_IsPool() { return m_isPool; }

	void Set_IsDead(_bool isDead) { m_isDead = isDead; }
	_bool Get_IsDead() { return m_isDead; }

// 충돌 처리용
public:
	virtual void Collision_Enter(JPH::ObjectLayer Layer, CGameObject* pObject, JPH::ContactManifold ContactManifold) {};
	virtual void Collision_Stay(JPH::ObjectLayer Layer, CGameObject* pObject, JPH::ContactManifold ContactManifold) {};

protected:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };
	class CGameInstance*		m_pGameInstance = { nullptr };
	class CTransform*			m_pTransformCom = { nullptr };

	map<const _wstring, class CComponent*>		m_Components;

	_bool						m_isPool = { false };
	_bool						m_isDead = { false };

protected:
	HRESULT Add_Component(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, 
		const _wstring& strComponentTag, CComponent** ppOut, void* pArg = nullptr);

public:
	HRESULT Add_Component(const _wstring& strComponentTag, CComponent* pComponent);
	HRESULT Remove_Component(const _wstring& strComponentTag);

public:	
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;

};

NS_END