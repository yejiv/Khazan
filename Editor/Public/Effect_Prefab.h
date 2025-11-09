#pragma once
#include "Editor_Defines.h"
#include "GameObject.h"

NS_BEGIN(Editor)

class CEffect_Element;
class CMeshTrail;
class CLineTrail;
class CScreenTrail;

class CEffect_Prefab : public CGameObject
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
	virtual HRESULT					Initialize_Prototype() override;
	virtual HRESULT					Initialize_Clone(void* pArg) override;
	virtual void					Priority_Update(_float fTimeDelta) override;
	virtual void					Update(_float fTimeDelta) override;
	virtual void					Late_Update(_float fTimeDelta) override;
	virtual HRESULT					Render() override;

	/*[Edit]*/

	void							Add_Effect_Element(_uint EffectType, void *pArg);
	void							Add_TimeTrack(EFFECT_EVENT TrackData);

	void							Edit_Element(_uint ChildIdx);
	void							Edit_TimeTrack(_uint ChildIdx);
	void							RevertChanges(_uint ChildIdx);
	void							RemoveEffect(_uint ChildIdx);
	_uint							Get_ChildrenSize() { return (_uint)m_Children.size(); }
	_float							Get_Time() { return m_fCurTime; }
	_float							Get_MaxTrack();
	void							Setting_Loop();
	void							ResetChildren();
	void							Save(const char* filename);
	void							Load(const char* filename);

private:
	vector<class CEffect_Element*>	m_Children;
	vector<EFFECT_EVENT>			m_eEventTracks;
	vector<bool>					m_bEventTriggered;

	_float							m_fCurTime;
	_bool							m_bPlaying;
	_bool							m_IsLoop;

	/*[Edit]*/
	EFFECT_EVENT					m_sEditingData;
	_int							m_PrevTrackIdx;
	_int							m_TrackIdx;
	_bool							m_Gravity;

private:
	class CMeshTrail*				m_pMeshTrail;
	class CLineTrail*				m_pLineTrail;
	class CScreenTrail*				m_pScreenTrail;
	 
public:
	static CEffect_Prefab*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*			Clone(void* pArg);
	virtual void					Free() override; 
};

NS_END
