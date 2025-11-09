#pragma once
#include "Engine_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)

class CShader;

class ENGINE_DLL CEffect_Element abstract: public CGameObject
{
protected:
	typedef struct tagRunningTrackData
	{
		_float	fCurTime = 0.f;
		_float	fDurTime;
		_uint	EventType;

	}TRACK_DATA;

	typedef struct tagDissolveData
	{
		_uint		bIsDissolve;
		_float		fDissolveEdgeWidth;
		_float4		fDissolveEdgeColor;
		_uint		iDissolveTextureIdx;
	}DISSOLVE_DATA;

protected:
	CEffect_Element(ID3D11Device* pDevice,ID3D11DeviceContext* pDeviceContext);
	CEffect_Element(const CEffect_Element& Prototype);
	virtual ~CEffect_Element() = default;

public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize_Clone(void* pArg) override;
	virtual void				Priority_Update(_float fTimeDelta) override;
	virtual void				Update(_float fTimeDelta) override;
	virtual void				Late_Update(_float fTimeDelta) override;
	virtual HRESULT				Render() override;

public: 
	virtual void				Active();
	virtual void				SetSpreadData(void* pArg);
	virtual void				SetRotateData(void* pArg);
	virtual void				SetTwinkleData(void* pArg);
	virtual void				SetUpwardData(void* pArg);
	virtual void				SetScrollData(void* pArg);
	virtual void				SetLoopOff();
	virtual void				Reset();
	void						SetData(_uint eventType, _float fDurTime);
	_bool						IsActive() { return m_bRunning; }
	void						SetParentsMatrix(const _float4x4* pMatrix) { m_pParentMatrix = pMatrix; }


protected:
	virtual HRESULT				Ready_Component();
	virtual HRESULT				Bind_ShaderResources();
	virtual void				Apply(void* pArg);

protected:
	CShader*					m_pShaderCom = {nullptr};

protected:
	const _float4x4*			m_pParentMatrix = {nullptr};
	_float4x4					m_CombinedWorldMatrix = {};
	_uint						m_iEffect_Type;
	_float2						m_fScrollSpeed;
	_bool						m_bRunning;

	list<TRACK_DATA>			m_TimeTracks;

public:
	virtual CEffect_Element*	Clone() PURE;
	virtual CGameObject*		Clone(void* pArg);	//안 쓸거
	virtual void				Free() override;

};

NS_END




