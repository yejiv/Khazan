#include "Camera_Compre.h"
#include "GameInstance.h"
#include "ClientInstance.h"
#include "Target_LockOn.h"
#include "Monster.h"
static inline float Saturate(float x) { return Clamp(x, 0.f, 1.f); }


static inline float SmoothDampScalar(float current, float target, float& currentVel, float smoothTime, float dt)
{
    const float eps = 1e-4f;
    float omega = 2.0f / max(eps, smoothTime);
    float x = omega * dt;
    float expv = 1.0f / (1.0f + x + 0.48f * x * x + 0.235f * x * x * x);

    float change = current - target;
    float temp = (currentVel + omega * change) * dt;
    currentVel = (currentVel - omega * temp) * expv;
    float output = target + (change + temp) * expv;
    return output;
}

static inline float Clamp(float v, float lo, float hi) { return max(lo, min(v, hi)); }

static inline float WrapAngle(float a) {
    const float twoPi = XM_PI * 2.0f;
    while (a > XM_PI) a -= twoPi;
    while (a < -XM_PI) a += twoPi;
    return a;
}

static inline float DeltaAngle(float a, float b) {
    // shortest path from a to b in (-pi..pi]
    float d = WrapAngle(b - a);
    return d;
}

static inline float SmoothDampAngle(float current, float target, float& currentVel, float smoothTime, float dt)
{
    float delta = DeltaAngle(current, target);
    float out = SmoothDampScalar(0.f, delta, currentVel, smoothTime, dt);
    return WrapAngle(current + out);
}

static inline void BuildSafeBasis(_vector vLookIn, _vector& outRight, _vector& outUp, _vector& outLook)
{
    const _vector vWorldUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

    // Look 정규화 + 너무 짧으면 이전 값이나 fallback 사용해야 함
    _vector look = XMVector3Normalize(vLookIn);
    if (XMVectorGetX(XMVector3LengthSq(look)) < 1e-6f)
    {
        // 완전 비는 경우를 방지하기 위해 아주 기본 방향으로 fallback
        look = XMVectorSet(0.f, 0.f, 1.f, 0.f);
    }

    _vector right = XMVector3Cross(vWorldUp, look);
    float rightLenSq = XMVectorGetX(XMVector3LengthSq(right));

    if (rightLenSq < 1e-6f)
    {
        // look 이 거의 (0, ±1, 0) 이면, 임의의 축을 하나 선택해서 다시 만든다.
        // 예: x축 기반으로 새 basis 생성
        _vector arbitrary = XMVectorSet(1.f, 0.f, 0.f, 0.f);
        right = XMVector3Cross(arbitrary, look);
        rightLenSq = XMVectorGetX(XMVector3LengthSq(right));

        if (rightLenSq < 1e-6f)
        {
            // 그래도 0이면 마지막 fallback
            right = XMVectorSet(0.f, 0.f, 1.f, 0.f);
        }
    }

    right = XMVector3Normalize(right);
    _vector up = XMVector3Normalize(XMVector3Cross(look, right));

    outLook = look;
    outRight = right;
    outUp = up;
}

CCamera_Compre::CCamera_Compre(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CCamera{ pDevice, pContext }
{
}

CCamera_Compre::CCamera_Compre(const CCamera_Compre& Prototype)
    : CCamera{ Prototype }
{
}

HRESULT CCamera_Compre::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCamera_Compre::Initialize_Clone(void* pArg)
{
    m_strCameraTag = TEXT("Default");

    CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);
    
    CHECK_FAILED(Ready_Camera(pArg), E_FAIL);

    if(m_iCameraType == ENUM_CLASS(CAMERATYPE::PLAYER))
        CHECK_FAILED(Ready_Body(), E_FAIL);

    m_isActive = false;

    return S_OK;
}

void CCamera_Compre::Priority_Update(_float fTimeDelta)
{
    if (m_pGameInstance->Key_Down(DIK_NUMPAD5))
    {
        m_isAniFix = false;
    }

    if (m_fStartTime < 2.f)
    {
        m_fStartTime += fTimeDelta;
        m_pTransformCom->LookAt(XMLoadFloat4(&m_vAt));
    }
    if (!m_isCollTime)
    {
        m_fCollTime += fTimeDelta;
        if (m_fCollTime > 5.f)
        {
            m_isCollTime = true;
        }
    }

    if (!m_isActive)
        return;

    //======================================================================================================

    if (m_isAnimation)
    {
        __super::Play_Animation(fTimeDelta);
    }
    else if (m_isAniFix && !m_isAnimation)
    {

    }
    else
    {
        if (m_isForceOrbit)
            Update_ForceOrbit(fTimeDelta);
        else if (m_isPostForceFrameRight || m_isPostFrameHold) {
            Update_InteractFocus(fTimeDelta);
        }
        else if (m_isBlendBack)
        {
            Update_BlendBack(fTimeDelta);
        }
        else {
            if (m_iCameraType == ENUM_CLASS(CAMERATYPE::FREE))
                Update_Free(fTimeDelta);
            else if (m_iCameraType == ENUM_CLASS(CAMERATYPE::PLAYER))
            {
                if (m_isCinematic)
                    Update_Cinematic(fTimeDelta);
                else if (m_isYetuga_Holding)
                    Update_Yetuga_Holding(fTimeDelta);
                else if (m_isNpcTalk)
                    Update_NpcTalk(fTimeDelta);
                else
                    Update_Spring(fTimeDelta);
            }
                
        }     

    }

    {
        m_vShaking_BasePos = m_pTransformCom->Get_State(STATE::POSITION);
        m_vShaking_BaseRight = m_pTransformCom->Get_State(STATE::RIGHT);
        m_vShaking_BaseUp = m_pTransformCom->Get_State(STATE::UP);
        m_vShaking_BaseLook = m_pTransformCom->Get_State(STATE::LOOK);
        Shaking(fTimeDelta);
    }

    __super::Update_PipeLines(fTimeDelta);
}

void CCamera_Compre::Update(_float fTimeDelta)
{
    if (!m_isActive)
        return;

    if (m_iCameraType == ENUM_CLASS(CAMERATYPE::PLAYER))
    {
        if (!m_isPostFrameHold && !m_isPostForceFrameRight)
        {
            if (m_isAnimation || m_isAniFix || m_isBlendBack ||
                m_isForceOrbit || m_isYetuga_Holding ||
                m_isPostForceFrameRight || m_isPostFrameHold || m_isNpcTalk)
            {
                return;
            }

            m_pBody->Update(fTimeDelta, m_pTransformCom);
            m_pBody->Sync_Update(m_pTransformCom);
        }
        
    }
}

void CCamera_Compre::Late_Update(_float fTimeDelta)
{
    if (!m_isActive)
        return;
}

HRESULT CCamera_Compre::Render()
{
    if (!m_isActive)
        return  S_OK;

    return S_OK;
}

void CCamera_Compre::Update_Free(_float fTimeDelta)
{
    
    if (m_pGameInstance->Key_Pressing(DIK_UP, fTimeDelta, INPUT_TYPE::FORCE))
    {
        m_pTransformCom->Go_Straight(fTimeDelta * 1.5f);
    }
    if (m_pGameInstance->Key_Pressing(DIK_DOWN, fTimeDelta, INPUT_TYPE::FORCE))
    {
        m_pTransformCom->Go_Backward(fTimeDelta * 1.5);
    }
    if (m_pGameInstance->Key_Pressing(DIK_LEFT, fTimeDelta, INPUT_TYPE::FORCE))
    {
        m_pTransformCom->Go_Left(fTimeDelta * 1.5f);
    }
    if (m_pGameInstance->Key_Pressing(DIK_RIGHT, fTimeDelta, INPUT_TYPE::FORCE))
    {
        m_pTransformCom->Go_Right(fTimeDelta * 1.5f);
    }

    _int    iMouseMove = {};
    if (m_pGameInstance->Mouse_Pressing(MOUSEKEYSTATE::RB, INPUT_TYPE::FORCE))
    {
        if (iMouseMove = m_pGameInstance->Mouse_Move(MOUSEMOVESTATE::X, INPUT_TYPE::FORCE))
        {
            m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * iMouseMove * m_fMouseSensor);
        }

        if (iMouseMove = m_pGameInstance->Mouse_Move(MOUSEMOVESTATE::Y, INPUT_TYPE::FORCE))
        {
            m_pTransformCom->Turn(m_pTransformCom->Get_State(STATE::RIGHT), fTimeDelta * iMouseMove * m_fMouseSensor);
        }
    }
    
}

void CCamera_Compre::Update_Spring(_float fTimeDelta)
{
    LockOn_Check(fTimeDelta);

    if (m_isLockOn && m_pLockMonster)
        LockOn(fTimeDelta);

    Spring(fTimeDelta);
    RayCast(fTimeDelta);
}

void CCamera_Compre::Switch_CameraMode(CAMERATYPE eType)
{
    m_iCameraType = ENUM_CLASS(eType);
}

HRESULT CCamera_Compre::Ready_Camera(void* pArg)
{
    CAMERA_COMPRE_DESC* pDesc = static_cast<CAMERA_COMPRE_DESC*>(pArg);

    if (pDesc->iCameraType == ENUM_CLASS(CAMERATYPE::PLAYER))
    {
        m_pLockOnUI = static_cast<CTarget_LockOn*>(CClientInstance::GetInstance()->Get_RootUI(TEXT("LockOn")));
        Safe_AddRef(m_pLockOnUI);
    }

    return S_OK;
}


HRESULT CCamera_Compre::Ready_Body()
{
    CBody::BODY_SPHERESHAPE_DESC TriggerDesc{};
    TriggerDesc.fRadius = 20.f;
    TriggerDesc.bIsTrigger = true;
    TriggerDesc.bStartActive = true;
    TriggerDesc.eMotion = EMotionType::Kinematic;
    TriggerDesc.eQuality = EMotionQuality::LinearCast;
    TriggerDesc.eShapeType = SHAPE::SPHERE;
    TriggerDesc.fFriction = 0.8f;
    TriggerDesc.fMass = 0.0f;
    TriggerDesc.fRestitution = 0.0f;
    TriggerDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::CAMERA);
    TriggerDesc.fGravity = 0.f;

    XMStoreFloat3(&TriggerDesc.vPos, m_pTransformCom->Get_State(STATE::POSITION));
    XMStoreFloat4(&TriggerDesc.vQuat, m_pTransformCom->Get_Rotation_Quat());
    TriggerDesc.vShapeOffset = _float3(0.f, 0.f, 0.f);
    m_tCollisionDesc.pGameObject = this;
    //pCollDesc.pInfo = ?? // 작성하기
    TriggerDesc.pCollisionDesc = &m_tCollisionDesc;
    TriggerDesc.bStartActive = false;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Body"),
        TEXT("Com_Body"), reinterpret_cast<CComponent**>(&m_pBody), &TriggerDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CCamera_Compre::Spring(_float fTimeDelta)
{
    if (m_pObjMatrix == nullptr)
        return E_FAIL;

    _vector vTargetPos, vDir;

    _vector vCamPos = Cal_CamPos(fTimeDelta, vTargetPos, vDir);

    _vector vWorldUp, vLook, vRight, vUp;
    vWorldUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
    vLook = XMVector3Normalize(XMVectorSubtract(vTargetPos, vCamPos));
    BuildSafeBasis(vLook, vRight, vUp, vLook);
    //vRight = XMVector3Normalize(XMVector3Cross(vWorldUp, vLook));
    //vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));

    m_pTransformCom->Set_State(STATE::RIGHT, vRight);
    m_pTransformCom->Set_State(STATE::UP, vUp);
    m_pTransformCom->Set_State(STATE::LOOK, vLook);
    m_pTransformCom->Set_State(STATE::POSITION, vCamPos);

    return S_OK;
}

HRESULT CCamera_Compre::RayCast(_float fTimeDelta)
{
    _vector vPos = m_pTransformCom->Get_State(STATE::POSITION) - XMVectorSetW(XMVector3Normalize(m_pTransformCom->Get_State(STATE::LOOK)), 0.f);
    _vector vTargetPos = XMVectorSet(m_pObjMatrix->_41, m_pObjMatrix->_42 + 1.5f, m_pObjMatrix->_43, 1.f);

    _float fFraction;
    _float4 vPosition;

    if (m_pGameInstance->RayCast(
        _float3(vTargetPos.m128_f32[0], vTargetPos.m128_f32[1], vTargetPos.m128_f32[2]),
        _float3(vPos.m128_f32[0], vPos.m128_f32[1], vPos.m128_f32[2]),
        fFraction,
        vPosition
    ))
    {
        m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat4(&vPosition));
    }

    return S_OK;
}

HRESULT CCamera_Compre::LockOn(_float fTimeDelta)
{
    if (!m_pObjMatrix || !m_pLockMonster) return S_OK;

    const _float playerEyeOffsetY = 1.5f;

    const _vector playerWorldPosition = XMVectorSet(
        m_pObjMatrix->_41,
        m_pObjMatrix->_42 + playerEyeOffsetY,
        m_pObjMatrix->_43,
        1.f
    );

    _vector playerToTargetVector = XMVectorSubtract(XMLoadFloat4(m_pLockOnPos), playerWorldPosition);
    _float playerToTargetDistance = XMVectorGetX(XMVector3Length(playerToTargetVector));

    if (playerToTargetDistance < 1e-4f)
        return S_OK;

    _vector directionNormalized = XMVectorScale(playerToTargetVector, 1.0f / playerToTargetDistance);

    _float normalizedX = XMVectorGetX(directionNormalized);
    _float normalizedY = XMVectorGetY(directionNormalized) - 0.3f;
    // 근접일수록 내려다보기 바이어스(-0.3f)를 0으로 완화
    {
        const _float rawY = XMVectorGetY(directionNormalized);
        const _float yBiasFar = -0.3f;
        // k = 근접일수록 1, 멀수록 0
        const _float k = 1.f - Saturate((playerToTargetDistance - m_fTopClampNearDist) / (m_fTopClampFarDist - m_fTopClampNearDist));
        const _float yBias = Lerp(0.0f, yBiasFar, 1.f - k); // 근접(k=1)→0, 멀(k=0)→-0.3
        normalizedY = rawY + yBias;
    }

    _float normalizedZ = XMVectorGetZ(directionNormalized);
    _float targetYaw = atan2f(normalizedZ, normalizedX);
    _float targetPitch = asinf(Clamp(normalizedY, -1.f, 1.f));

    const _float topClampPitch = XMConvertToRadians(m_fTopViewClampDeg); // 예: -3도
    // k = 근접일수록 1, 멀수록 0
    const _float k = 1.f - Saturate((playerToTargetDistance - m_fTopClampNearDist) / (m_fTopClampFarDist - m_fTopClampNearDist));
    if (k > 0.f && targetPitch < topClampPitch) {
        // targetPitch가 너무 "내려가면"(음수 더 큼) 근접 비율(k)에 따라 topClampPitch 쪽으로 보간
        targetPitch = Lerp(targetPitch, topClampPitch, k);
    }

    targetPitch = Clamp(targetPitch, m_fPitchMin, m_fPitchMax);

    m_fYaw = SmoothDampAngle(
        m_fYaw,
        targetYaw,
        m_fSmoothingVelocityYaw,
        0.08f,
        fTimeDelta
    );

    m_fPitch = SmoothDampScalar(
        m_fPitch,
        targetPitch,
        m_fSmoothingVelocityPitch,
        0.08f,
        fTimeDelta
    );
    return S_OK;
}

void CCamera_Compre::Update_BlendBack(_float fTimeDelta)
{
    if (!m_isBlendBack)
        return;

    m_fBlendBackTime += fTimeDelta;
    float BlendTime = Clamp(m_fBlendBackTime / m_fBlendBackDuration, 0.f, 1.f);

    // 1) 현재 프레임 기준 "정상 플레이 카메라" 목표값 계산
    _vector vTargetPosWS, vTargetDir;
    _vector vSpringCamPos = Cal_CamPos(fTimeDelta, vTargetPosWS, vTargetDir);

    _vector vWorldUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
    _vector vLookTarget = XMVector3Normalize(XMVectorSubtract(vTargetPosWS, vSpringCamPos));
    _vector vRightTarget = XMVector3Normalize(XMVector3Cross(vWorldUp, vLookTarget));
    _vector vUpTarget = XMVector3Normalize(XMVector3Cross(vLookTarget, vRightTarget));

    // 2) 시작 포즈 → 타겟 포즈로 부드럽게 보간
    _vector vPos = XMVectorLerp(m_vBlendStartPos, vSpringCamPos, BlendTime);
    _vector vRight = XMVector3Normalize(XMVectorLerp(m_vBlendStartRight, vRightTarget, BlendTime));
    _vector vUp = XMVector3Normalize(XMVectorLerp(m_vBlendStartUp, vUpTarget, BlendTime));
    _vector vLook = XMVector3Normalize(XMVectorLerp(m_vBlendStartLook, vLookTarget, BlendTime));

    m_pTransformCom->Set_State(STATE::RIGHT, vRight);
    m_pTransformCom->Set_State(STATE::UP, vUp);
    m_pTransformCom->Set_State(STATE::LOOK, vLook);
    m_pTransformCom->Set_State(STATE::POSITION, vPos);

    // 3) 끝나면 정상 Spring 모드로 전환
    if (BlendTime >= 1.f)
    {
        m_isBlendBack = false;
    }

    return;
}

void CCamera_Compre::Update_InteractFocus(_float fTimeDelta)
{
    if (!m_isPostForceFrameRight || !m_pObjMatrix || !m_pTransformCom) return;
    if (fTimeDelta <= 0.f) return;

    const _vector camPos = m_pTransformCom->Get_State(STATE::POSITION);

    const _vector targetWS = XMVectorSet(
        m_pObjMatrix->_41,
        m_pObjMatrix->_42 + m_fPostFrameEyeOffsetY,
        m_pObjMatrix->_43, 1.f);

    _vector toTarget = XMVectorSubtract(targetWS, camPos);
    float dist = XMVectorGetX(XMVector3Length(toTarget));
    dist = max(dist, m_fPostFrameMinDist);

    // 수직 FOV → 수평 FOV
    const float aspect = (float)m_iWinSizeX / max(1.f, (float)m_iWinSizeY);
    const float vFov = m_fFovy; // 라디안
    const float hFov = 2.f * atanf(tanf(vFov * 0.5f) * aspect);

    // 화면 X(0.5→target) 보간: 부드럽게 오른쪽으로
    m_fPostFrameScreenXCur = SmoothDampScalar(
        m_fPostFrameScreenXCur,
        m_fPostFrameScreenXTarget,
        m_fPostFrameScreenXVel,
        m_fPostFrameDuration,
        fTimeDelta
    );

    const float dx = (m_fPostFrameScreenXCur - 0.5f); // 중심 대비
    const float offsetAng = hFov * dx;
    const float offsetLen = dist * tanf(offsetAng);

    const _vector worldUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
    _vector look = XMVector3Normalize(toTarget);
    _vector right = XMVector3Normalize(XMVector3Cross(worldUp, look));
    _vector up = XMVector3Normalize(XMVector3Cross(look, right));

    // ‘왼쪽’을 조준하면 캐릭터가 화면 ‘오른쪽’에 위치
    _vector aimWS = XMVectorMultiplyAdd(XMVectorReplicate(-offsetLen), right, targetWS);

    _vector newLook = XMVector3Normalize(XMVectorSubtract(aimWS, camPos));
    _vector newRight = XMVector3Normalize(XMVector3Cross(worldUp, newLook));
    _vector newUp = XMVector3Normalize(XMVector3Cross(newLook, newRight));

    m_pTransformCom->Set_State(STATE::POSITION, camPos); // 위치는 유지
    m_pTransformCom->Set_State(STATE::RIGHT, newRight);
    m_pTransformCom->Set_State(STATE::UP, newUp);
    m_pTransformCom->Set_State(STATE::LOOK, newLook);

    // 끝났는지 판정(충분히 근접 + 속도 거의 0)
    const bool doneX = (fabsf(m_fPostFrameScreenXCur - m_fPostFrameScreenXTarget) < 1e-3f) &&
        (fabsf(m_fPostFrameScreenXVel) < 1e-3f);
    if (doneX) {
        m_isPostForceFrameRight = false; // 프레이밍 종료 (원하면 유지해도 됨)
    }

}

void CCamera_Compre::Update_NpcTalk(_float fTimeDelta)
{
    if (!m_isNpcTalk || !m_pTransformCom)
        return;

    if (fTimeDelta <= 0.f)
        return;

    _vector vCamTargetPos = XMLoadFloat3(&m_vNpcCamTargetPos);
    _vector vLookDir = XMLoadFloat3(&m_vNpcCamLookAt);

    // 길이 체크 + fallback
    float lenSq = XMVectorGetX(XMVector3LengthSq(vLookDir));
    if (lenSq < 1e-6f)
    {
        vLookDir = XMVector3Normalize(m_pTransformCom->Get_State(STATE::LOOK));
        if (XMVectorGetX(XMVector3LengthSq(vLookDir)) < 1e-6f)
            vLookDir = XMVectorSet(0.f, 0.f, 1.f, 0.f);
    }
    else
    {
        vLookDir = XMVector3Normalize(vLookDir);
    }

    _vector vRightTarget, vUpTarget, vLookTarget;
    BuildSafeBasis(vLookDir, vRightTarget, vUpTarget, vLookTarget);

    // ===== 블렌드 비율(smoothstep) 계산 =====
    m_fNpcTalkBlendTime += fTimeDelta;

    _float t = 0.f;
    if (m_fNpcTalkBlendDuration <= 0.f)
        m_fNpcTalkBlendDuration = 0.001f; // 방어

    t = Clamp(m_fNpcTalkBlendTime / m_fNpcTalkBlendDuration, 0.f, 1.f);
    _float s = t * t * (3.f - 2.f * t); // smoothstep

    // ===== 포즈 보간 =====
    _vector vPos;
    _vector vRight;
    _vector vUp;
    _vector vLook;

    if (t < 1.f)
    {
        vPos = XMVectorLerp(m_vNpcTalkStartPos, vCamTargetPos, s);
        vRight = XMVector3Normalize(
            XMVectorLerp(m_vNpcTalkStartRight, vRightTarget, s));
        vUp = XMVector3Normalize(
            XMVectorLerp(m_vNpcTalkStartUp, vUpTarget, s));
        vLook = XMVector3Normalize(
            XMVectorLerp(m_vNpcTalkStartLook, vLookTarget, s));
    }
    else
    {
        vPos = vCamTargetPos;
        vRight = vRightTarget;
        vUp = vUpTarget;
        vLook = vLookTarget;
    }

    m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(vPos, 1.f));
    m_pTransformCom->Set_State(STATE::RIGHT, vRight);
    m_pTransformCom->Set_State(STATE::UP, vUp);
    m_pTransformCom->Set_State(STATE::LOOK, vLook);
}

void CCamera_Compre::LockOn_Check(_float fTimeDelta)
{
    m_fLockOnDelay += fTimeDelta;

    if (m_isLockOn)
    {
        if (m_pLockMonster == nullptr ||
            m_pLockMonster->Get_IsDead() == true ||
            m_pLockMonster->Get_IsActive() == false)
        {
            m_fLockOnDelay = 0.f;
            m_isLockOn = false;
            m_pLockMonster = nullptr;
            m_pLockOnUI->LockOff();
            m_iLockOrder = 0;
            m_fSmoothingVelocityYaw = 0.f;
            m_fSmoothingVelocityPitch = 0.f;
            return;
        }
        
        if (m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::WB))
        {
            m_fLockOnDelay = 0.f;
            m_isLockOn = false;
            m_pLockMonster = nullptr;
            m_pLockOnUI->LockOff();
            m_iLockOrder = 0;
            m_fSmoothingVelocityYaw = 0.f;
            m_fSmoothingVelocityPitch = 0.f;
            return;
        }

        if (m_pGameInstance->Mouse_Move(MOUSEMOVESTATE::WHEEL))
        {
            if (m_pGameInstance->Mouse_Move(MOUSEMOVESTATE::WHEEL) > 0)
            {
                m_iLockOrder--;
            }
            else {
                m_iLockOrder++;
            }
            m_pLockMonster = Pick_ClosetTarget();
            if (m_pLockMonster)
            {
                m_fSmoothingVelocityYaw = 0.f;
                m_fSmoothingVelocityPitch = 0.f;
                m_pLockOnUI->LockOff();
                m_pLockOnPos = dynamic_cast<CMonster*>(m_pLockMonster)->Get_LockOnPosition();
                m_pLockOnUI->LockOn(m_pLockOnPos);
            }
            else {
                m_fLockOnDelay = 0.f;
                m_isLockOn = false;
                m_pLockMonster = nullptr;
                m_pLockOnUI->LockOff();
                m_iLockOrder = 0;
                m_fSmoothingVelocityYaw = 0.f;
                m_fSmoothingVelocityPitch = 0.f;
            }
        }
    }
}

CGameObject* CCamera_Compre::Pick_ClosetTarget()
{
    if (!m_pTransformCom || m_CollMonsters.empty())
        return nullptr;

    if (m_iLockOrder < 0)
        m_iLockOrder = m_CollMonsters.size() - 1;

    if (m_iLockOrder >= m_CollMonsters.size())
        m_iLockOrder = 0;

    const _vector PlayerWorldPosition = XMVectorSet(
        m_pObjMatrix->_41,
        m_pObjMatrix->_42,
        m_pObjMatrix->_43,
        1.f
    );
    const _vector cameraLookDirection = XMVector3Normalize(
        m_pTransformCom->Get_State(STATE::LOOK)
    );

    vector<pair<_float, CGameObject*>> vCandidates;
    vCandidates.reserve(m_CollMonsters.size());

    for (CGameObject* pObj : m_CollMonsters)
    {
        if (!pObj || !pObj->Get_IsActive() || pObj->Get_IsDead())
            continue;

        CTransform* pTransform = dynamic_cast<CTransform*>(
            pObj->Get_Component(TEXT("Com_Transform"))
            );
        if (!pTransform)
            continue;

        const _matrix world = pTransform->Get_WorldMatrix();
        const _vector objectWorldPosition = XMVectorSet(
            world.r[3].m128_f32[0],
            world.r[3].m128_f32[1],
            world.r[3].m128_f32[2],
            1.f
        );

        const _vector toTargetVector = XMVectorSubtract(
            objectWorldPosition,
            PlayerWorldPosition
        );

        const float worldDistance = XMVectorGetX(XMVector3Length(toTargetVector));
        if (worldDistance > m_fTargetMaxDistance)
            continue;

        const _vector toTargetNormalized = XMVector3Normalize(toTargetVector);
        const float forwardCos = XMVectorGetX(
            XMVector3Dot(toTargetNormalized, cameraLookDirection)
        );
        if (forwardCos < m_fTargetHalfFovCos)
            continue;

        const float projectedDistance = XMVectorGetX(
            XMVector3Dot(toTargetVector, cameraLookDirection)
        );
        if (projectedDistance <= 0.0f)
            continue;

        vCandidates.emplace_back(projectedDistance, pObj);
    }

    if (vCandidates.empty())
        return nullptr;

    if (m_iLockOrder >= static_cast<int>(vCandidates.size()))
        return nullptr;

    nth_element(
        vCandidates.begin(),
        vCandidates.begin() + m_iLockOrder,
        vCandidates.end(),
        [](const auto& a, const auto& b)
        {
            return a.first < b.first; 
        }
    );

    if (m_pLockMonster)
    {
        if (vCandidates[m_iLockOrder].second == m_pLockMonster && vCandidates.size() >= 2)
        {
            if (m_iLockOrder == 0)
                m_iLockOrder++;

            if (vCandidates.size() <= m_iLockOrder)
                m_iLockOrder--;

            if (m_iLockOrder <= 0)
                m_iLockOrder = vCandidates.size() - 1;
            else {
                m_iLockOrder = 0;
            }


        }

        if (vCandidates.size() == 1)
            m_iLockOrder = 0;

    }

    return vCandidates[m_iLockOrder].second;
}

_vector CCamera_Compre::Cal_CamPos(_float fTimeDelta, _vector& vTargetPos, _vector& vDir)
{
    // 1) 타겟(플레이어) 위치
    vTargetPos = XMVectorSet(
        m_pObjMatrix->_41,
        m_pObjMatrix->_42 + 1.5f,
        m_pObjMatrix->_43,
        1.f
    );

    // 2) 마우스 회전 (락온 아닐 때)
    if (!m_isLockOn)
    {
        _int iMouseMoveX = m_pGameInstance->Mouse_Move(MOUSEMOVESTATE::X);
        _int iMouseMoveY = m_pGameInstance->Mouse_Move(MOUSEMOVESTATE::Y);

        m_fYaw = WrapAngle(m_fYaw - fTimeDelta * iMouseMoveX * m_fMouseSensor);
        m_fPitch = Clamp(
            m_fPitch - fTimeDelta * iMouseMoveY * m_fMouseSensor,
            m_fPitchMin,
            m_fPitchMax
        );
    }

    // 3) 이동값 기반 궤도 회전 (락온 아닐 때만)
    if (!m_isLockOn)
    {
        //Apply_MoveOrbitYaw(fTimeDelta, vTargetPos);
    }
    else
    {
        m_isHasPrevTargetPos = false;
    }

    // 4) 최종 방향 벡터 계산
    vDir = XMVectorSet(
        cosf(m_fPitch) * cosf(m_fYaw),
        sinf(m_fPitch),
        cosf(m_fPitch) * sinf(m_fYaw),
        0.f
    );
    vDir = XMVector3Normalize(vDir);

    // 5) 카메라 위치 = 타겟 - dir * radius
    _vector vCamPos = XMVectorMultiplyAdd(
        XMVectorReplicate(-m_fRadius),
        vDir,
        vTargetPos
    );

    return vCamPos;
}

void CCamera_Compre::OnCameraAniEnd()
{
    m_isBlendBack = true;
    m_fBlendBackTime = 0.f;

    m_vBlendStartPos = m_pTransformCom->Get_State(STATE::POSITION);
    m_vBlendStartRight = m_pTransformCom->Get_State(STATE::RIGHT);
    m_vBlendStartUp = m_pTransformCom->Get_State(STATE::UP);
    m_vBlendStartLook = m_pTransformCom->Get_State(STATE::LOOK);

    // Spring 카메라랑 이어질 때 튀지 않도록
    // 현재 Look 기준으로 yaw/pitch를 역산해서 세팅
    _vector vLookN = XMVector3Normalize(m_vBlendStartLook);
    float x = XMVectorGetX(vLookN);
    float y = XMVectorGetY(vLookN);
    float z = XMVectorGetZ(vLookN);

    m_fYaw = atan2f(z, x);
    m_fPitch = Clamp(asinf(Clamp(y, -1.f, 1.f)), m_fPitchMin, m_fPitchMax);
}

void CCamera_Compre::Apply_MoveOrbitYaw(_float fTimeDelta, _vector vTargetPosWS)
{
    // 이전 프레임 타겟 위치 없으면 초기화
    if (!m_isHasPrevTargetPos)
    {
        XMStoreFloat4(&m_vPrevTargetPosWS, vTargetPosWS);
        m_isHasPrevTargetPos = true;
        return;
    }

    _vector vPrev = XMLoadFloat4(&m_vPrevTargetPosWS);
    _vector vVel = XMVectorSubtract(vTargetPosWS, vPrev);
    XMStoreFloat4(&m_vPrevTargetPosWS, vTargetPosWS);

    float fDist = XMVectorGetX(XMVector3Length(vVel));
    if (fDist < 1e-5f || fTimeDelta <= 0.f)
        return;

    float fSpeed = fDist / fTimeDelta;
    if (fSpeed < m_fMoveSpeedMin)
        return; // 너무 느리면 노이즈로 보고 무시

    _vector vMoveDir = XMVector3Normalize(vVel);

    // 현재 카메라 기준 축 (이미 m_fYaw에 마우스 입력 반영된 상태라고 가정)
    _vector vWorldUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
    _vector vCamForward = XMVectorSet(cosf(m_fYaw), 0.f, sinf(m_fYaw), 0.f);
    _vector vCamRight = XMVector3Normalize(XMVector3Cross(vWorldUp, vCamForward));

    // 이동 방향의 "카메라 오른쪽 성분"
    float fSide = XMVectorGetX(XMVector3Dot(vMoveDir, vCamRight));
    // fSide > 0 : 카메라 기준 오른쪽으로 이동
    // fSide < 0 : 카메라 기준 왼쪽으로 이동

    // 여기서 핵심:
    // "캐릭터가 왼쪽으로 이동하면 카메라는 오른쪽으로 이동" → 반대 방향으로 궤도 회전
    // fSide < 0 (왼쪽)이면, 카메라는 +yaw 또는 -yaw 중 하나로 "오른쪽으로" 돌아야 함.
    // 축 헷갈릴 수 있으니, 일단 직관 기준 공식을 만들고, 반대로 돌면 부호만 바꾸면 된다.

    float fSideAbs = fabsf(fSide);
    if (fSideAbs < 0.05f)
        return; // 거의 정면/후면일 땐 회전 X

    // side 성분에 비례해서 회전 속도 생성
    // NOTE: 부호 중요!
    //  - 캐릭터가 카메라 기준 왼쪽(fSide < 0) → 카메라를 "오른쪽"으로 돌리고 싶으면:
    //    yawDelta = -fSide * ...  (fSide<0 → yawDelta>0)
    //  - 반대로 느껴지면 아래 한 줄의 부호를 반대로 바꾸면 된다.
    float fYawDelta = -fSide * m_fOrbitYawSpeed * fTimeDelta;

    m_fYaw = WrapAngle(m_fYaw + fYawDelta);
}

void CCamera_Compre::SyncOrbitFromCurrentPose()
{
    if (!m_pObjMatrix || !m_pTransformCom)
        return;

    const _float fEyeOffsetY = 1.5f;

    _vector vTargetPos = XMVectorSet(
        m_pObjMatrix->_41,
        m_pObjMatrix->_42 + fEyeOffsetY,
        m_pObjMatrix->_43,
        1.f
    );

    _vector vCamPos = m_pTransformCom->Get_State(STATE::POSITION);

    // 카메라가 "타겟을 바라보는" 방향: 타겟 - 카메라
    _vector vToTarget = XMVectorSubtract(vTargetPos, vCamPos);

    _float fRadius = XMVectorGetX(XMVector3Length(vToTarget));
    if (fRadius < 0.1f)
        fRadius = max(m_fRadius, 1.f);

    _vector vLook = XMVector3Normalize(vToTarget); // 이것이 카메라 Look
    _float fX = XMVectorGetX(vLook);
    _float fY = XMVectorGetY(vLook);
    _float fZ = XMVectorGetZ(vLook);

    _float yaw = atan2f(fZ, fX);
    _float pitch = asinf(Clamp(fY, -1.f, 1.f));

    m_fYaw = WrapAngle(yaw);
    m_fPitch = Clamp(pitch, m_fPitchMin, m_fPitchMax);
}

void CCamera_Compre::Start_ForceOrbit(CAMERA_FORCE_DIR eForceDir)
{
    if (eForceDir == CAMERA_FORCE_DIR::NONE || !m_pObjMatrix)
        return;

    // 지금 실제 시점 기준으로 동기화 (순간이동 방지 핵심)
    SyncOrbitFromCurrentPose();

    m_eForceOrbit = eForceDir;
    m_isForceOrbit = true;
    m_fForceOrbitTime = 0.f;

    m_fForceStartYaw = m_fYaw;
    m_fForceStartPitch = m_fPitch;

    // 캐릭터 정면 yaw
    _vector vCharLook = XMVector3Normalize(
        XMVectorSet(m_pObjMatrix->_31, 0.f, m_pObjMatrix->_33, 0.f));
    float fCharYaw = atan2f(
        XMVectorGetZ(vCharLook),
        XMVectorGetX(vCharLook));

    if (eForceDir == CAMERA_FORCE_DIR::BACK)
    {
        m_fForceTargetYaw = WrapAngle(fCharYaw);      // 뒤에서 보는 시점
        m_fForceTargetPitch = m_fForceStartPitch;   // 필요하면 그대로 두거나 살짝 조정
    }
    else // FRONT
    {
        m_fForceTargetYaw = WrapAngle(fCharYaw + XM_PI); // 앞에서 보는 시점

        // 여기: "플레이어 정면 보기 좋은" pitch로 유도
        float targetPitch = XMConvertToRadians(-5.f); // 살짝 내려다보는 각도, 튜닝 가능
        m_fForceTargetPitch = Clamp(targetPitch, m_fPitchMin, m_fPitchMax);
    }
}

void CCamera_Compre::Update_ForceOrbit(_float fTimeDelta)
{
    m_fForceOrbitTime += fTimeDelta;
    _float fRatio = Clamp(m_fForceOrbitTime / m_fForceOrbitDuration, 0.f, 1.f);

    _float fSmooth = fRatio * fRatio * (3.f - 2.f * fRatio); // smoothstep

    _float dyaw = DeltaAngle(m_fForceStartYaw, m_fForceTargetYaw);
    m_fYaw = WrapAngle(m_fForceStartYaw + dyaw * fSmooth);

    m_fPitch = m_fForceStartPitch +
        (m_fForceTargetPitch - m_fForceStartPitch) * fSmooth;
    m_fPitch = Clamp(m_fPitch, m_fPitchMin, m_fPitchMax);

    // yaw/pitch만 손대고, 위치 계산은 기존 Spring/Cal_CamPos에 맡김
    Spring(fTimeDelta);

    if (fRatio >= 1.f)
    {
        m_isForceOrbit = false;
        m_eForceOrbit = CAMERA_FORCE_DIR::NONE;

        //if (m_isPostFramePending)
        //{
        //    m_isPostFramePending = false; 
        //    m_isPostForceFrameRight = true; 
        //    m_fPostFrameScreenXCur = 0.5f;
        //    m_fPostFrameScreenXVel = 0.f;

        //}
    }
}

void CCamera_Compre::Update_Cinematic(_float fTimeDelta)
{

}

void CCamera_Compre::Yetuga_Holding_Start()
{
    if (!m_pTransformCom || !m_pObjMatrix)
        return;

    // 다른 카메라 연출은 끈다 (LockOn flag는 건들지 않음)
    m_isForceOrbit = false;
    m_isBlendBack = false;
    m_isCinematic = false;

    m_isYetuga_Holding = true;

    // 1) 현재 카메라 상태 고정
    m_vYetugaHoldPos = m_pTransformCom->Get_State(STATE::POSITION);
    m_vYetugaBaseRight = m_pTransformCom->Get_State(STATE::RIGHT);
    m_vYetugaBaseUp = m_pTransformCom->Get_State(STATE::UP);
    m_vYetugaBaseLook = XMVector3Normalize(m_pTransformCom->Get_State(STATE::LOOK));

    // 2) 현재 Look 기반 yaw/pitch 계산
    _float fX = XMVectorGetX(m_vYetugaBaseLook);
    _float fY = XMVectorGetY(m_vYetugaBaseLook);
    _float fZ = XMVectorGetZ(m_vYetugaBaseLook);

    m_fYetugaYaw = atan2f(fZ, fX);
    m_fYetugaPitch = Clamp(asinf(Clamp(fY, -1.f, 1.f)), m_fYetugaPitchMin, m_fYetugaPitchMax);

    m_fYetugaYawVel = 0.f;
    m_fYetugaPitchVel = 0.f;
}

void CCamera_Compre::Yetuga_Holding_End()
{
    if (!m_isYetuga_Holding)
        return;

    m_isYetuga_Holding = false;

    // 1) 블렌드백 시작 플래그
    m_isBlendBack = true;
    m_fBlendBackTime = 0.f;

    // 2) 시작 포즈: 지금 예투가 연출용 카메라 포즈
    m_vBlendStartPos = m_pTransformCom->Get_State(STATE::POSITION);
    m_vBlendStartRight = m_pTransformCom->Get_State(STATE::RIGHT);
    m_vBlendStartUp = m_pTransformCom->Get_State(STATE::UP);
    m_vBlendStartLook = m_pTransformCom->Get_State(STATE::LOOK);

    // 3) Spring 타겟과 자연스럽게 이어지도록
    //    현재 Look 기반으로 yaw/pitch를 세팅해 둔다.
    _vector vLookN = XMVector3Normalize(m_vBlendStartLook);
    _float fX = XMVectorGetX(vLookN);
    _float fY = XMVectorGetY(vLookN);
    _float fZ = XMVectorGetZ(vLookN);

    m_fYaw = atan2f(fZ, fX);
    m_fPitch = Clamp(asinf(Clamp(fY, -1.f, 1.f)), m_fPitchMin, m_fPitchMax);

}

void CCamera_Compre::Update_Yetuga_Holding(_float fTimeDelta)
{
    if (!m_isYetuga_Holding || !m_pObjMatrix || !m_pTransformCom)
        return;

    if (fTimeDelta <= 0.f)
        return;

    // 1) 고정 카메라 위치
    _vector vCamPos = m_vYetugaHoldPos;

    // 2) 타겟(플레이어) 위치 (연출용 높이 조절 가능)
    _vector vTargetPos = XMVectorSet(
        m_pObjMatrix->_41,
        m_pObjMatrix->_42 + 1.5f,
        m_pObjMatrix->_43,
        1.f
    );

    _vector vToTarget = XMVectorSubtract(vTargetPos, vCamPos);
    _float fLenSq = XMVectorGetX(XMVector3LengthSq(vToTarget));
    if (fLenSq < 1e-6f)
    {
        // 너무 가까우면 그냥 현재 자세 유지
        m_pTransformCom->Set_State(STATE::POSITION, vCamPos);
        m_pTransformCom->Set_State(STATE::RIGHT, m_vYetugaBaseRight);
        m_pTransformCom->Set_State(STATE::UP, m_vYetugaBaseUp);
        m_pTransformCom->Set_State(STATE::LOOK, m_vYetugaBaseLook);
        return;
    }

    vToTarget = XMVector3Normalize(vToTarget);

    _float fTargetX = XMVectorGetX(vToTarget);
    _float fTargetY = XMVectorGetY(vToTarget);
    _float fTargetZ = XMVectorGetZ(vToTarget);

    // 3) 목표 yaw/pitch
    float targetYaw = atan2f(fTargetZ, fTargetX);
    float targetPitch = asinf(Clamp(fTargetY, -1.f, 1.f));
    targetPitch = Clamp(targetPitch, m_fYetugaPitchMin, m_fYetugaPitchMax);

    // 4) 스무딩 (떨림 방지)
    m_fYetugaYaw = SmoothDampAngle(
        m_fYetugaYaw,
        targetYaw,
        m_fYetugaYawVel,
        m_fYetugaYawSmoothTime,
        fTimeDelta
    );

    m_fYetugaPitch = SmoothDampScalar(
        m_fYetugaPitch,
        targetPitch,
        m_fYetugaPitchVel,
        m_fYetugaPitchSmoothTime,
        fTimeDelta
    );

    // 5) 보간된 yaw/pitch → 최종 Look
    _vector vLook = XMVectorSet(
        cosf(m_fYetugaPitch) * cosf(m_fYetugaYaw),
        sinf(m_fYetugaPitch),
        cosf(m_fYetugaPitch) * sinf(m_fYetugaYaw),
        0.f
    );
    vLook = XMVector3Normalize(vLook);

    _vector vWorldUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
    _vector vRight = XMVector3Normalize(XMVector3Cross(vWorldUp, vLook));
    if (XMVectorGetX(XMVector3LengthSq(vRight)) < 1e-6f)
        vRight = m_vYetugaBaseRight; // 드물게 수직에 가까울 때 fallback
    _vector vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));

    // 6) 적용 (위치는 고정)
    m_pTransformCom->Set_State(STATE::POSITION, vCamPos);
    m_pTransformCom->Set_State(STATE::RIGHT, vRight);
    m_pTransformCom->Set_State(STATE::UP, vUp);
    m_pTransformCom->Set_State(STATE::LOOK, vLook);
}

void CCamera_Compre::Start_InteractFocus(CAMERA_FORCE_DIR eDir, _float fScreenX, _float fFrameDur, _bool isHold)
{
    m_fPostFrameScreenXTarget = Clamp(fScreenX, 0.f, 1.f);
    m_fPostFrameDuration = max(0.05f, fFrameDur);
    m_isPostFrameHold = isHold;

    // 1) ForceOrbit 실행
    //Start_ForceOrbit(eDir);

    //// 2) “이번 ForceOrbit이 끝나면 프레이밍 하겠다”는 의사 표시
    //m_isPostFramePending = true;

    //// 초기화
    //m_fPostFrameScreenXCur = 0.5f;
    //m_fPostFrameScreenXVel = 0.f;

    m_isPostFramePending = false;
    m_isPostForceFrameRight = true;
    m_fPostFrameScreenXCur = 0.5f;
    m_fPostFrameScreenXVel = 0.f;

    // (선택) 다른 모드 비활성화
    m_isLockOn = false;
    m_isBlendBack = false;
    m_isCinematic = false;
}

void CCamera_Compre::Exit_PostForceFrameRight(_bool isSmoothReturn, _float fReturnDur)
{
    // 1) 프레이밍/고정 상태가 아니면 할 일 없음
    if (!m_isPostForceFrameRight && !m_isPostFrameHold)
        return;

    if (!isSmoothReturn)
    {
        // 즉시 해제
        m_isPostForceFrameRight = false;
        m_isPostFrameHold = false;

        // 스냅 방지: 현 시점(yaw/pitch)을 스프링/자유 카메라 기준으로 동기화
        SyncOrbitFromCurrentPose();
        return;
    }

    // 2) 부드러운 복귀: target을 중앙(0.5)로 변경하여 SmoothDamp로 복귀
    m_fPostFrameScreenXTarget = 0.5f;
    m_fPostFrameDuration = (fReturnDur > 0.f) ? fReturnDur : m_fPostFrameDuration;

    // 보간 루틴이 계속 돌도록 유지
    m_isPostFrameHold = false;   // 고정 모드 해제
    m_isPostForceFrameRight = true;    // 보간 활성화 (이미 true여도 상관없음)
}

CCamera_Compre::CAMERA_COMPRE_DESC CCamera_Compre::Get_Desc()
{
    CAMERA_COMPRE_DESC tDesc{};

    tDesc.fFar = m_fFar;
    tDesc.fFovy = m_fFovy;
    tDesc.fMouseSensor = m_fMouseSensor;
    tDesc.fNear = m_fNear;
    tDesc.iCameraType = m_iCameraType;
    tDesc.strCameraTag = m_strCameraTag;
    XMStoreFloat4(&tDesc.vEye, m_pTransformCom->Get_State(STATE::POSITION));
    XMStoreFloat4(&tDesc.vAt, m_pTransformCom->Get_State(STATE::LOOK));

    return tDesc;
}

void CCamera_Compre::Set_NpcTalk(_bool isNpcTalk, _float3 vTargetPos, _float3 vLookAt)
{
    m_isNpcTalk = isNpcTalk;

    if (m_isNpcTalk)
    {
        m_vNpcCamTargetPos = vTargetPos;
        m_vNpcCamLookAt = vLookAt;

        m_fNpcTalkBlendTime = 0.f;

        m_vNpcTalkStartPos = m_pTransformCom->Get_State(STATE::POSITION);
        m_vNpcTalkStartRight = m_pTransformCom->Get_State(STATE::RIGHT);
        m_vNpcTalkStartUp = m_pTransformCom->Get_State(STATE::UP);
        m_vNpcTalkStartLook = m_pTransformCom->Get_State(STATE::LOOK);
    }
    else
    {
        m_isBlendBack = true;
        m_fBlendBackTime = 0.f;

        m_vBlendStartPos = m_pTransformCom->Get_State(STATE::POSITION);
        m_vBlendStartRight = m_pTransformCom->Get_State(STATE::RIGHT);
        m_vBlendStartUp = m_pTransformCom->Get_State(STATE::UP);
        m_vBlendStartLook = m_pTransformCom->Get_State(STATE::LOOK);

        _vector vLookN = XMVector3Normalize(m_vBlendStartLook);
        float x = XMVectorGetX(vLookN);
        float y = XMVectorGetY(vLookN);
        float z = XMVectorGetZ(vLookN);

        m_fYaw = atan2f(z, x);
        m_fPitch = Clamp(asinf(Clamp(y, -1.f, 1.f)), m_fPitchMin, m_fPitchMax);
    }
}

void CCamera_Compre::Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{
   if (iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::MONSTER))
    {
        if (!m_isCollTime)
            return;

        if (pDesc->pGameObject == nullptr)
            return;

        CGameObject* pObj = pDesc->pGameObject;
        CMonster* pMonster = dynamic_cast<CMonster*>(pDesc->pGameObject);
        if (pObj && find(m_CollMonsters.begin(), m_CollMonsters.end(), pObj) == m_CollMonsters.end())
            m_CollMonsters.push_back(pObj);
    }
}

void CCamera_Compre::Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{
    if (!m_isLockOn)
    {
        if (iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::MONSTER))
        {
            if (!m_isCollTime)
                return;

            if (m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::WB))
            {
                if (m_fLockOnDelay > 0.15f)
                {
                    
                    m_pLockMonster = Pick_ClosetTarget();
                    if (m_pLockMonster)
                    {
                        m_fSmoothingVelocityYaw = 0.f;
                        m_fSmoothingVelocityPitch = 0.f;
                        m_isLockOn = true;
                        m_pLockOnPos = dynamic_cast<CMonster*>(m_pLockMonster)->Get_LockOnPosition();
                        m_pLockOnUI->LockOn(m_pLockOnPos);
                    }
                }
            }
        }
    }
}

void CCamera_Compre::Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, COLLISION_DESC* pMyDesc)
{

   if (iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::MONSTER))
    {
       if (!m_isCollTime)
           return;

        CGameObject* pObj = pDesc->pGameObject;
        if (!pObj) return;
        auto it = std::remove(m_CollMonsters.begin(), m_CollMonsters.end(), pObj);
        if (it != m_CollMonsters.end()) m_CollMonsters.erase(it, m_CollMonsters.end());
    }
}

CCamera_Compre* CCamera_Compre::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CCamera_Compre* pInstance = new CCamera_Compre(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CCamera_Compre"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CCamera_Compre::Clone(void* pArg)
{
    CCamera_Compre* pInstance = new CCamera_Compre(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CCamera_Compre"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCamera_Compre::Free()
{
    Safe_Release(m_pLockOnUI);
    __super::Free();

    Safe_Release(m_pBody);

    m_pObjMatrix = nullptr;
    m_pSocketMatrix = nullptr;
}
