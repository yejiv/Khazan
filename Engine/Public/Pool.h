#pragma once

#include "GameObject.h"

NS_BEGIN(Engine)

class ENGINE_DLL CPool abstract : public CGameObject
{
public:
	typedef struct tagPool : public CGameObject::GAMEOBJECT_DESC
	{

	}POOL_DESC;
protected:
	CPool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPool(const CPool& Prototype);
	virtual ~CPool() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize_Clone(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();
	virtual HRESULT Render_Shadow() { return S_OK; }

public:
	void Set_Tag(_wstring strTag) { m_strTag = strTag; }
	_wstring Get_Tag() { return m_strTag; }

public:
	virtual void Reset() = 0;

protected:
	_wstring	m_strTag;
public:
	virtual CPool* Clone(void* pArg) = 0;
	virtual void Free() override;

};

NS_END