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
	virtual void					UpdateWorldMatrix(_fvector Quaternion, _gvector Position);

	void							Add_Effect_Element(class CEffect_Element* newElement);

	void							SetID(_uint ID) { m_iID = ID; }
    _uint							GetID() { return m_iID; }
    //void                            SetName(const _wstring& name) { m_sName = name; }
    //_wstring                        GetName() { return m_sName; }
    void                            SetPool(vector<CPrefab*>* address) { m_pPool = address;}
    void                            ReturnToPool() { m_pPool->push_back(this);}
	_bool							IsActive() {return m_bPlaying; }
	void							SetClose();
	void							SetClose_Force();


protected:
	vector<class CEffect_Element*>	m_Children;
	_float							m_fCurTime;
	_uint							m_iID;
    //_wstring                        m_sName;
	_bool							m_bPlaying;

	_float4x4						m_CombinedWorldMatrix = {};

    vector<CPrefab*>*               m_pPool = nullptr;

public:
	virtual CGameObject*			Clone(void* pArg) PURE;
	virtual void					Free() override; 
};

NS_END
