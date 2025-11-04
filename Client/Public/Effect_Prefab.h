#pragma once
#include "Client_Defines.h"
#include "Prefab.h"

NS_BEGIN(Client)

class CEffect_Element;

class CEffect_Prefab : public CPrefab
{
public :
	enum class EffectType { POINT_INSTANCE, MESH_INSTANCE, SPRITE };
	enum class EffectEventType {	ACTIVATE,
									ANIMATE_SPREAD, ANIMATE_ROTATE, ANIMATE_TWINLKE, ANIMATE_LINEAR_MOVE,
									SCROLL_SPEAD,
									DISSOLVE, }; 
public:
	typedef struct tagEffectEvent
	{
		EffectEventType eEventType;
		_uint			iElementIdx;

		_float			fStartTime;
		_float			fDuration;

		_float2			fScaleSpeed;
		_float2			fSpreadSpeed;
		_float2			fUpwardSpeed;
		_float2			fRotationSpeed;
		_float2			fScrollSpeed;

		_float3			fPivot;
		_uint			bGravity;
	}EFFECT_EVENT;

private:
	CEffect_Prefab(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CEffect_Prefab(const CEffect_Prefab& Prototype);
	virtual ~CEffect_Prefab() = default;

public:
	virtual HRESULT					Initialize_Prototype(const char* filename);
	virtual HRESULT					Initialize_Clone(void* pArg) override;
	virtual void					Priority_Update(_float fTimeDelta) override;
	virtual void					Update(_float fTimeDelta) override;
	virtual void					Late_Update(_float fTimeDelta) override;
	virtual HRESULT					Render() override;

	void							Add_TimeTrack(EFFECT_EVENT TrackData);
	void							Load(const char* filename);

	void							ResetChildren();

private:
	vector<EFFECT_EVENT>			m_eEventTracks;
	vector<bool>					m_bEventTriggered;

	_bool							m_bPlaying;
	_bool							m_IsLoop;
	 
public:
	static CEffect_Prefab*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const char* filename);
	virtual CGameObject*			Clone(void* pArg);
	virtual void					Free() override; 
};

NS_END
