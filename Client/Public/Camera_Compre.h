#pragma once
#include "Client_Defines.h"
#include "Base.h"
#include "Camera.h"

NS_BEGIN(Engine)
class CCamera;
class CBody;
class CGameObject;
NS_END

NS_BEGIN(Client)
class CCamera_Compre final : public CCamera
{
public:
	typedef struct tagCameraCompreDesc : public CCamera::CAMERA_DESC
	{

	}CAMERA_COMPRE_DESC;

private:
	CCamera_Compre(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_Compre(const CCamera_Compre& Prototype);
	virtual ~CCamera_Compre() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Update_Free(_float fTimeDelta);
	void Update_Spring(_float fTimeDelta);

public:
	HRESULT Ready_Camera(void* pArg);
	HRESULT Ready_Body();

public:
	HRESULT Spring(_float fTimeDelta);
	HRESULT RayCast(_float fTimeDelta);
	HRESULT LockOn(_float fTimeDelta);
    void Update_BlendBack(_float fTimeDelta);
public:
	void LockOn_Check(_float fTimeDelta);

public:
	class CGameObject* Pick_ClosetTarget();

public:
	_vector Cal_CamPos(_float fTimeDelta, _vector& vTargetPos, _vector& vDir);

public:
    virtual void OnCameraAniEnd() override;

public:
    void Apply_MoveOrbitYaw(_float fTimeDelta, _vector vTargetPosWS);

public:
    void SyncOrbitFromCurrentPose();
    void Start_ForceOrbit(CAMERA_FORCE_DIR eForceDir);
    void Update_ForceOrbit(_float fTimeDelta);

public:
	CAMERA_COMPRE_DESC Get_Desc();

private:
	_float m_fYVel = { 0.f };
	_bool m_isInited = { false };
	_float m_fSmoothY = { 0.f };

	_float m_fYSmoothTime = { 0.25f }; // 감쇠 시간
	_float m_fDeadZone = { 0.1f }; // 미세 요철 무시
	_float m_fMaxRise = { 5.f }; // 초당 상승 한도
	_float m_fMaxFall = { 9.f }; // 초당 하강한도

	CBody* m_pBody = { nullptr };
	_bool m_isLockOn = { false };
	_float m_fLockOnDelay = {};
	vector<class CGameObject*> m_CollMonsters;
	class CGameObject* m_pLockMonster = { nullptr };
	_float4* m_pLockOnPos = {};
	class CTarget_LockOn* m_pLockOnUI = { nullptr };
	
	_float m_fTargetHalfFovDegrees = { 50.f };
    _float m_fTargetHalfFovCos = { 0.f };
    _float m_fTargetMaxDistance = { 20.f };


    // 특정위치에서 강제 이동시 보간
    _bool m_isBlendBack = { false };
    _float m_fBlendBackTime = { 0.f };
    _float m_fBlendBackDuration = { 0.7f };

    _vector m_vBlendStartPos = XMVectorZero();
    _vector m_vBlendStartRight = XMVectorZero();
    _vector m_vBlendStartUp = XMVectorZero();
    _vector m_vBlendStartLook = XMVectorZero();

    // 타겟 이동값에 따른 카메라 이동
    _bool   m_isHasPrevTargetPos = false;
    _float4 m_vPrevTargetPosWS = { 0.f, 0.f, 0.f, 1.f };

    float   m_fOrbitYawSpeed = 1.5f;   // side=1일 때 초당 회전량(라디안). 튜닝용.
    float   m_fMoveSpeedMin = 1.0f;   // 너무 느린 움직임은 무시

    // 카메라 뷰 강제 이동시 사용할 변수
    CAMERA_FORCE_DIR m_eForceOrbit = CAMERA_FORCE_DIR::NONE;
    _bool m_isForceOrbit = false;
    _float m_fForceOrbitTime = 0.f;
    _float m_fForceOrbitDuration = 0.35f;

    _float m_fForceStartYaw = 0.f;
    _float m_fForceTargetYaw = 0.f;
           
    _float m_fForceStartPitch = 0.f;
    _float m_fForceTargetPitch = 0.f;

public:
	void Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal) override;
	void Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal) override;
	void Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer) override;


public:
	static CCamera_Compre* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg)override;
	virtual void Free() override;

};

NS_END