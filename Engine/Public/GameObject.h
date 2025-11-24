#pragma once

#include "Transform.h"

NS_BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CBase
{
public:
	typedef struct tagGameObject : public CTransform::TRANSFORM_DESC
	{
        _uint iLevelIndex = {};
	}GAMEOBJECT_DESC;

protected:
	CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject(const CGameObject& Prototype);
	virtual ~CGameObject() = default;

public:
	virtual class CComponent* Get_Component(const _wstring& strComponentTag);

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize_Clone(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();
	virtual HRESULT Deferred_Render(ID3D11DeviceContext* pDeferredContext);
	virtual HRESULT Render_Shadow() { return S_OK; }
	virtual HRESULT Render_Outline() { return S_OK; }
    virtual HRESULT Render_MotionVector() { return S_OK; }

public:
	void Set_IsPool(_bool isPool) { m_isPool = isPool; }
	_bool Get_IsPool() { return m_isPool; }

	void Set_IsDead(_bool isDead) { m_isDead = isDead; }

     _bool Get_IsDead() { return m_isDead; }

	void Set_IsActive(_bool isActive) { m_isActive = isActive; }
	_bool Get_IsActive() { return m_isActive; }

	void Set_IsGhost(_bool isGhost) { m_isGhost = isGhost; }
	_bool Get_IsGhost() { return m_isGhost; }

	void Set_IsDeferred(_bool isDeferred) { m_isDeferredContext = isDeferred; }
	_bool Get_IsDeferred() { return m_isDeferredContext; }

	// Team 관련
	_uint				Get_Team() { return m_iTeam; }

	void Set_Tag(_wstring strTag) { m_strTag = strTag; }
	_wstring Get_Tag() { return m_strTag; }

	void Set_Layer(_uint iLayer) { m_iLayer = iLayer; }
	_uint Get_Layer() { return m_iLayer; }

	void Set_Name(string strTag) { m_strName = strTag; }
    _bool Compare_Name(string strName) { return strName == m_strName; }
	string Get_Name() { return m_strName; }

    void Set_LevelIndex(_uint iLevelIndex) { m_iLevelIndex = iLevelIndex; }
    _uint Get_LevelIndex() { return m_iLevelIndex; }

    _vector     Get_Position();
    _vector     Get_Look();
    _vector     Get_Right();
    _vector     Get_Up();
    class CTransform*      Get_Transform();

public:
	virtual void Reset() {};

public:
	virtual void Collision_Enter(COLLISION_DESC* pDesc, _uint	iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal) {};
	virtual void Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal) {};
	virtual void Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer) {};

protected:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };
	class CGameInstance*		m_pGameInstance = { nullptr };
	class CTransform*			m_pTransformCom = { nullptr };

	map<const _wstring, class CComponent*>		m_Components;

	_bool						m_isPool = { false };
	_wstring					m_strTag;
	string						m_strName;
	_uint						m_iLayer = {};
    _uint                       m_iLevelIndex = {};

	_bool						m_isDead = { false };
	_bool						m_isActive = { true };
	_bool						m_isGhost = { false };
	_bool						m_isDeferredContext = { false };
	_bool						m_isPrototype = { true };

	// 추후에 파생 클래스 나눠지게 되면 옮기거나 다른 방법으로 바꿔보겠습니다.
	_uint						m_iTeam = {};
    _int                        m_iPrototypeIndex = { -1 };
	COLLISION_DESC				m_tCollisionDesc = {};

    _uint                       m_iMtrlFlags = { 0 };

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