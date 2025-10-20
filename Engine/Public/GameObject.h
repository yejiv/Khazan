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

	void Set_IsActive(_bool isActive) { m_isActive = isActive; }
	_bool Get_IsActive() { return m_isDead; }

	// Team 관련
	_uint				Get_Team() { return m_iTeam; }

public:
	virtual void Collision_Enter(CGameObject* pOther, _uint	iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal) {};
	virtual void Collision_Stay(CGameObject* pOther, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal) {};

protected:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };
	class CGameInstance*		m_pGameInstance = { nullptr };
	class CTransform*			m_pTransformCom = { nullptr };

	map<const _wstring, class CComponent*>		m_Components;

	_bool						m_isPool = { false };
	_bool						m_isDead = { false };
	_bool						m_isActive = { true };

	// 추후에 파생 클래스 나눠지게 되면 옮기거나 다른 방법으로 바꿔보겠습니다.
	_uint						m_iTeam = {};

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