#pragma once
#include "Engine_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)

class CEffect_Element;

class ENGINE_DLL CPrefab abstract : public CGameObject
{
protected:
	CPrefab(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CPrefab(const CPrefab& Prototype);
	virtual ~CPrefab() = default;

public:
	virtual HRESULT					Initialize_Clone(void* pArg) override;
	virtual void					Priority_Update(_float fTimeDelta) PURE;
	virtual void					Update(_float fTimeDelta) PURE;
	virtual void					Late_Update(_float fTimeDelta) PURE;

	void							Add_Effect_Element(class CEffect_Element* newElement);

protected:
	vector<class CEffect_Element*>	m_Children;
	_float							m_fCurTime;

public:
	virtual CGameObject*			Clone(void* pArg) PURE;
	virtual void					Free() override; 
};

NS_END
