#pragma once

#include "GameObject.h"

NS_BEGIN(Engine)

class ENGINE_DLL CCamera abstract : public CGameObject
{
public:
	typedef struct tagCameraDesc : public CGameObject::GAMEOBJECT_DESC
	{
		_float4		vEye, vAt;
		_float		fFovy, fNear, fFar;

		_float		fMouseSensor = {};

		_wstring	strCameraTag = {};
		_uint		iCameraType = {};
	}CAMERA_DESC;
protected:
	CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera(const CCamera& Prototype);
	virtual ~CCamera() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize_Clone(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

public:
	void Set_Animation(_wstring strAnimationTag);
	void Play_Animation(_float fTimeDelta);

public:
	void Create_Animation(_wstring strAnimationTag);
	void Create_Event(_wstring strAnimationTag);
	void Create_Animation_Item(_wstring strAnimationTag);
	void Create_Event_Item(_wstring strAnimationTag);
	void Set_Animation_Item(_wstring strAnimationTag, CAMERA_KEYFRAME tAnimation, _uint iIndex);
	void Set_Event_Item(_wstring strAnimationTag, CAMERA_EVENT_DATA tEvent, _uint iIndex);
	void Add_Animation(_wstring strAnimationTag, CAMERA_KEYFRAME tAnimation);
	void Add_Event(_wstring strAnimationTag, CAMERA_EVENT_DATA tEvent);

public:
	HRESULT Remove_Animation(_wstring strAnimationTag, _uint iIndex);
	HRESULT Remove_AllAnimation(_wstring strAnimationTag);
	HRESULT Remove_Event(_wstring strAnimationTag, _uint iIndex);
	HRESULT Remove_AllEvent(_wstring strAnimationTag);

public:
	_uint Get_CameraType() { return m_iCameraType; }
	_wstring Get_CameraTag() { return m_strCameraTag; }

	CCamera::CAMERA_DESC Get_CameraDesc();
	map<_wstring, vector<CAMERA_KEYFRAME>>* Get_AllAnimations() { return &m_Animations; }
	map<_wstring, vector<CAMERA_EVENT_DATA>>* Get_AllEvents() { return &m_Events; }

	vector<CAMERA_KEYFRAME>* Get_Animations(_wstring strAnimationTag);
	vector<CAMERA_EVENT_DATA>* Get_Events(_wstring strAnimationTag);

public:
	HRESULT Set_DefaultData(CAMERA_DESC tDesc);
	HRESULT Load(map<_wstring, vector<CAMERA_KEYFRAME>> Animations, map<_wstring, vector<CAMERA_EVENT_DATA>> Events);

public:
	void Set_ObjMatrix(const _float4x4* pObjMatrix) { m_pObjMatrix = pObjMatrix; }
	void Set_SocketMatrix(const _float4x4* pSocketMatrix) { m_pSocketMatrix = pSocketMatrix; }

public:
	void Update_PipeLines();

protected:
	_wstring			m_strCameraTag = {};
	_uint				m_iCameraType = {};

	_float				m_fMouseSensor = {};

	_float4				m_vEye = {};
	_float4				m_vAt = {};
	_float				m_fFovy = {};
	_float				m_fAspect = {};
	_float				m_fNear{}, m_fFar{};

	_float				m_fYaw = 0.f;
	_float				m_fPitch = 0.6f;
	_float				m_fRadius = 5.f;

	_float				m_fPitchMin = -1.2f;
	_float				m_fPitchMax = 0.7f;
	_float				m_fRadiusMin = 2.f;
	_float				m_fRadiusMax = 12.f;
	_float				m_fSkin = 0.02f;

	_float				m_fFollowValue = 4.f;
	_vector				m_vLerpMove = { 0.f, 0.f, 0.f, 1.f };

	const _float4x4*	m_pObjMatrix = { nullptr };
	const _float4x4*	m_pSocketMatrix = { nullptr };

	map<_wstring, vector<CAMERA_KEYFRAME>> m_Animations;
	map<_wstring, vector<CAMERA_EVENT_DATA>> m_Events;

	_bool							m_isAnimation = {false};
	vector<CAMERA_KEYFRAME>*		m_pCurrentAnimation = { nullptr };
	_uint							m_iAnimationIndex = {};
	_float							m_fCurrentTrackPosition = {};
	_float							m_fAnimationRatio = {};
	CATMULLROM						m_tPosCatmullrom{};
	_vector							m_vOldLook = {};
	
public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

NS_END