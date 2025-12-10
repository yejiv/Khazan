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
    void Set_ObjMatrix(const _float4x4* pObjMatrix) { m_pObjMatrix = pObjMatrix; }
    void Set_SocketMatrix(const _float4x4* pSocketMatrix) { m_pSocketMatrix = pSocketMatrix; }
public:
    void Switch_CameraMode(CAMERATYPE eType);

public:
    HRESULT Ready_Camera(void* pArg);
    HRESULT Ready_Body();

public:
    HRESULT Spring(_float fTimeDelta);
    HRESULT RayCast(_float fTimeDelta);
    HRESULT LockOn(_float fTimeDelta);
    void Update_BlendBack(_float fTimeDelta);
    void Update_InteractFocus(_float fTimeDelta);
    void Update_PoseBlend(_float fTimeDelta);

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
    void Update_Cinematic(_float fTimeDelta);

public:
    void Yetuga_Holding_Start();
    void Yetuga_Holding_End();
    void Update_Yetuga_Holding(_float fTimeDelta);

public:
    void Start_InteractFocus(CAMERA_FORCE_DIR eDir, _float fScreenX, _float fFrameDur, _bool isHold);
    void Exit_PostForceFrameRight(_bool isSmoothReturn = true, _float fReturnDur = 0.22f);

public:
    CAMERA_POSE CaptureCurrentPose() const;
    void ApplyPose(const CAMERA_POSE& tPose);

    void PushCurrentPose();
    void PushPose(const CAMERA_POSE& pose);
    _bool HasPose() const { return !m_PoseStack.empty(); }
    void ClearPoseStack() { m_PoseStack.clear(); }

    void StartPoseBlendTo(const CAMERA_POSE& targetPose, _float fDuration, _bool bPushCurrent);

    void ReturnToPreviousPose(_float fDuration);

    void Play_SubShotOnce(const CAMERA_POSE& subShotPose, _float fInDur, _float fOutDur);

public:
    CAMERA_COMPRE_DESC  Get_Desc();
    _bool               Get_IsLockOn() const { return m_isLockOn; }
    _float4* Get_LockOnPosition() { return m_pLockOnPos; }

public:
    void Set_NpcTalk(_bool isNpcTalk, _float3 vTargetPos = _float3(0.f, 0.f, 0.f), _float3 vLookAt = _float3(0.f, 0.f, 0.f));
private:
    CBody* m_pBody = { nullptr };

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

    const _float4x4* m_pObjMatrix = { nullptr };
    const _float4x4* m_pSocketMatrix = { nullptr };
    
    _bool m_isLockOn = { false };
    _float m_fLockOnDelay = {};
    vector<class CGameObject*> m_CollMonsters;
    class CGameObject* m_pLockMonster = { nullptr };
    _float4* m_pLockOnPos = {};
    _uint m_iLockOrder = {};
    _float m_fSmoothingVelocityYaw = {};
    _float m_fSmoothingVelocityPitch = {};
    class CTarget_LockOn* m_pLockOnUI = { nullptr };

    // LockOn 보정
    _float m_fTopClampNearDist = 2.5f;   // 이 이하에서 강하게 개입
    _float m_fTopClampFarDist = 6.0f;   // 이 이상이면 개입 해제
    _float m_fTopViewClampDeg = -2.0f;  // 근접 시 허용하는 최소(가장 내려다보는) pitch(도). -2~-5 추천
	
	_float m_fTargetHalfFovDegrees = { 50.f };
    _float m_fTargetHalfFovCos = { 0.f };
    _float m_fTargetMaxDistance = { 20.f };

    _float m_fLockPitchDownClampDeg = -35.f;
    _float m_fLockPitchUpClampDeg = 10.f;

    // 적용 거리
    _float m_fLockClampNearDist = 3.f;
    _float m_fLockClampFarDist = 8.f;


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

    _float   m_fOrbitYawSpeed = { 1.5f };   // side=1일 때 초당 회전량(라디안). 튜닝용.
    _float   m_fMoveSpeedMin = { 1.0f };   // 너무 느린 움직임은 무시

    // 카메라 뷰 강제 이동시 사용할 변수
    CAMERA_FORCE_DIR m_eForceOrbit = { CAMERA_FORCE_DIR::NONE };
    _bool m_isForceOrbit = { false };
    _float m_fForceOrbitTime = { 0.f };
    _float m_fForceOrbitDuration = { 0.35f };

    _float m_fForceStartYaw = { 0.f };
    _float m_fForceTargetYaw = { 0.f };
           
    _float m_fForceStartPitch = { 0.f };
    _float m_fForceTargetPitch = { 0.f };


    // 시네마틱으로 돌려서 사용할 경우
    _bool m_isCinematic = { false };


    // 예투가 잡기 카메라 모드 여부
    _bool    m_isYetuga_Holding = false;

    // 잡기 시작 시 고정할 카메라 위치
    _vector m_vYetugaHoldPos = XMVectorZero();

    // 잡기 시작 시 기준 축 (fallback 용)
    _vector m_vYetugaBaseRight = XMVectorZero();
    _vector m_vYetugaBaseUp = XMVectorZero();
    _vector m_vYetugaBaseLook = XMVectorZero();

    // 시선 추적용 yaw/pitch + 속도
    _float   m_fYetugaYaw = 0.f;
    _float   m_fYetugaPitch = 0.f;
    _float   m_fYetugaYawVel = 0.f;
    _float   m_fYetugaPitchVel = 0.f;

    // 피치 제한 (거의 수직까지 허용)
    _float   m_fYetugaPitchMin = XMConvertToRadians(-80.f);
    _float   m_fYetugaPitchMax = XMConvertToRadians(89.f);

    // 부드러움 정도 (클수록 더 부드럽게, 느리게)
    _float   m_fYetugaYawSmoothTime = 0.10f;
    _float   m_fYetugaPitchSmoothTime = 0.10f;

    // 콜리전 시작 호출
    _float m_fCollTime = {};
    _bool  m_isCollTime = { false };

    // 인터렉션 전용 
    _bool  m_isPostFramePending = false; // ForceOrbit 끝나면 프레이밍 시작할지
    _bool  m_isPostForceFrameRight = false; // 현재 프레이밍 중인지
    _bool  m_isPostFrameHold = false; // 목표에 도착해도 계속 고정 유지
    
    _float m_fPostFrameScreenXTarget = 0.75f; // 캐릭터 화면 X 위치(0~1) : 0.75 = 오른쪽 75%
    _float m_fPostFrameScreenXCur = 0.5f;  // 현재 X (보간 시작점: 중앙)
    _float m_fPostFrameScreenXVel = 0.f;   // SmoothDamp 속도
    
    _float m_fPostFrameDuration = 0.25f; // 오른쪽으로 미는 보간 시간
    _float m_fPostFrameEyeOffsetY = 1.5f;  // 캐릭터 눈높이
    _float m_fPostFrameMinDist = 0.5f;  // 너무 가까울 때 안정화용

    _float m_fStartTime = {};

    // NPC 대화전용
    _float  m_fNpcTalkBlendTime = 0.f;
    _float  m_fNpcTalkBlendDuration = 0.5f;

    _vector m_vNpcTalkStartPos;
    _vector m_vNpcTalkStartRight;
    _vector m_vNpcTalkStartUp;
    _vector m_vNpcTalkStartLook;

    _bool m_isNpcTalk = { false };
    _float3 m_vNpcCamTargetPos = _float3(0.f, 0.f, 0.f);
    _float3 m_vNpcCamLookAt = _float3(0.f, 0.f, 0.f);

    // 스택형 화면 전환
    vector<CAMERA_POSE> m_PoseStack;
    POSE_BLEND_STATE    m_tPoseBlend;

    mutex m_CollMonsterMutex;

public:
    virtual void Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc = nullptr) override;
    virtual void Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc = nullptr) override;
    virtual void Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, COLLISION_DESC* pMyDesc = nullptr) override;


public:
	static CCamera_Compre* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg)override;
	virtual void Free() override;

};

NS_END