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
	virtual void					ResetChildren() PURE;
	virtual void					UpdatePosition(_fvector Pos);

	void							Add_Effect_Element(class CEffect_Element* newElement);

	void							SetID(_uint ID) { m_iID = ID; }
	_uint							GetID() { return m_iID; }
	_bool							IsActive() {return m_bPlaying; }
	void							SetClose();


protected:
	vector<class CEffect_Element*>	m_Children;
	_float							m_fCurTime;
	_uint							m_iID;
	_bool							m_bPlaying;

public:
	virtual CGameObject*			Clone(void* pArg) PURE;
	virtual void					Free() override; 
};

NS_END
