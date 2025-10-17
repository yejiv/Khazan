#pragma once
#include "Editor_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CShader;
NS_END

NS_BEGIN(Editor)

class CEffect_Element abstract: public CGameObject
{
protected:
	typedef struct tagRunningTrackData
	{
		_float	fCurTime = 0.f;
		_float	fDurTime;
		
		_uint	EventType;

	}TRACK_DATA;
protected:
	CEffect_Element(ID3D11Device* pDevice,ID3D11DeviceContext* pDeviceContext);
	CEffect_Element(const CEffect_Element& Prototype);
	virtual ~CEffect_Element() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize_Clone(void* pArg) override;
	virtual void			Priority_Update(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			Late_Update(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

public: 
	virtual void			SetSpreadData(void* pArg)	PURE;
	virtual void			SetRotateData(void* pArg)	PURE;
	virtual void			SetTwinkleData(void* pArg)	PURE;
	virtual void			SetUpwardData(void* pArg)	PURE;
	virtual void			SetScrollData(void* pArg)	PURE;
	void					SetData(_uint eventType, _float fDurTime);
	void					Active();
	_bool					IsActive() { return (m_TimeTracks.size() > 0) ? true : false; }
	void					SetParentsMatrix(const _float4x4* pMatrix) { m_pParentMatrix = pMatrix; }

	/* [Edit] */
	virtual void			Save_Data(ofstream& os)		PURE;
	virtual void			Edit_Element()				PURE;
	virtual void			RevertChanges()				PURE;
	virtual void			Reset();

protected:
	virtual HRESULT			Ready_Component();
	virtual HRESULT			Bind_ShaderResources();
	virtual void			Apply(void* pArg)			PURE;

protected:
	CShader*				m_pShaderCom = {nullptr};

protected:
	const _float4x4*		m_pParentMatrix = {nullptr};
	_float4x4				m_CombinedWorldMatrix = {};
	_uint					m_iEffect_Type;
	_float2					m_fScrollSpeed;

	list<TRACK_DATA>		m_TimeTracks;

public:
	virtual CGameObject*	Clone(void* pArg) PURE;
	virtual void			Free() override;

};

NS_END




