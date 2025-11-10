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
	void Set_ObjMatrix(const _float4x4* pObjMatrix) { m_pObjMatrix = pObjMatrix; }
	void Set_SocketMatrix(const _float4x4* pSocketMatrix) { m_pSocketMatrix = pSocketMatrix; }

public:
	void Update_PipeLines(_float fTimeDelta);

	// 애니메이션
public:
	void Set_Animation(_wstring strAnimationTag);
	void Play_Animation(_float fTimeDelta);
    virtual void OnCameraAniEnd() {};

	// 툴 관련
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
	HRESULT Load_Animation(map<_wstring, vector<CAMERA_KEYFRAME>> Animations);

// 쉐이킹
public:
	void Shaking_Start(_float fPower, _float fDuration);
	void Shaking(_float fTimeDelta);
	_bool Shaking_Active() const { return m_fShaking_Time < m_fShaking_Duration && m_fShaking_Power > 0.f; }

// Fov 관련
public:
	void    Set_BaseFOV(_float fDegree) { m_fBaseFOV = fDegree; }
	_float	Get_CurrentFOV() const { return m_fFovy; }

	void	Push_FOVModifier(const FOVModifier& tMod);
	void	Kill_FOVModifier(const _wstring& strID);
	void	Update_FOVChannel(_float fTimeDelta);

private:
	_int	FindModIndexByID(const _wstring& strID) const;



protected:
	// 기본 투입값들
	_wstring			m_strCameraTag = {};
	_uint				m_iCameraType = {};

	_float				m_fMouseSensor = {};

	_float4				m_vEye = {};
	_float4				m_vAt = {};
	_float				m_fAspect = {};
	_float				m_fNear{}, m_fFar{};

	_float				m_fYaw = 0.f;
	_float				m_fPitch = 0.6f;
	_float				m_fRadius = 4.f;

	_float				m_fPitchMin = -1.2f;
	_float				m_fPitchMax = 0.7f;
	_float				m_fRadiusMin = 2.f;
	_float				m_fRadiusMax = 12.f;
	_float				m_fSkin = 0.02f;

	_float				m_fFollowValue = 2.f;
	_vector				m_vLerpMove = { 0.f, 0.f, 0.f, 1.f };

	const _float4x4*	m_pObjMatrix = { nullptr };
	const _float4x4*	m_pSocketMatrix = { nullptr };

	map<_wstring, vector<CAMERA_KEYFRAME>> m_Animations;
	map<_wstring, vector<CAMERA_EVENT_DATA>> m_Events;


	// 쉐이킹 관련
	_float m_fShaking_Time = { 0.f }; // 계산용 시간
	_float m_fShaking_Duration = { 0.f }; // 지속시간
	_float m_fShaking_Power = { 0.f }; // 세기
	_uint m_fShaking_Seed = 1337u; // 매 실행마다 위상 랜덤화용
	_float3 m_vShaking_Amplitude = { 0.1f, 0.08f, 0.09f }; // 진폭
	_float3 m_vShaking_Phase = { 0.f, 0.f, 0.f }; // 위상
	_float m_fShaking_Freq = { 0.f }; // 주파수
	_vector m_vShaking_BasePos;
	_vector m_vShaking_BaseRight;
	_vector m_vShaking_BaseUp;
	_vector m_vShaking_BaseLook;

	// Animation 관련
	_bool							m_isAnimation = {false};
	_bool							m_isLoop = { false };
	vector<CAMERA_KEYFRAME>*		m_pCurrentAnimation = { nullptr };
	vector<CAMERA_KEYFRAME>			m_RuntimeKeys;
	_vector							m_vEntryPos = {};
	_vector							m_vEntryLook = {};
	_uint							m_iAnimationIndex = {};
	_float							m_fCurrentTrackPosition = {};
	_float							m_fAnimationRatio = {};
	CATMULLROM						m_tPosCatmullrom{};
	_vector							m_vOldLook = {};
	_int    m_iSeg = { 0 };
	_float  m_fSegTime = { 0.f };
	_float  m_fPosSmooth = { 8.f };
	_float  m_fRotSmooth = { 8.f };
	_vector m_vCurPos = XMVectorSet(0, 0, 0, 1);
	_vector m_vCurQ = XMQuaternionIdentity();
	_bool m_isCurPos = { false };
	_bool m_isStarted = { true };

	// Fov 관련
	_float				m_fFovy = {};
	_float				m_fBaseFOV = XMConvertToRadians(60.f);   // 기본 FOV
	vector<FOVModifier> m_vFOVMods;
	_float				m_fFOVSmooth = 20.f;   // 클수록 빠르게 따라감
	_float				m_fFOVMin = XMConvertToRadians(30.f);
	_float				m_fFOVMax = XMConvertToRadians(110.f);
	
public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

NS_END