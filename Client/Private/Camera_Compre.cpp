#include "Camera_Compre.h"
#include "GameInstance.h"
#include "ClientInstance.h"
#include "Target_LockOn.h"
#include "Monster.h"

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
    if (!m_isActive)
        return;

    if (!m_isAnimation)
    {
        if (m_iCameraType == ENUM_CLASS(CAMERATYPE::FREE))
            Update_Free(fTimeDelta);
        else if (m_iCameraType == ENUM_CLASS(CAMERATYPE::PLAYER))
            Update_Spring(fTimeDelta);

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
        m_pBody->Update(fTimeDelta, m_pTransformCom);
        m_pBody->Sync_Update(m_pTransformCom);
    }
    __super::Play_Animation(fTimeDelta);
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
    
    if (m_pGameInstance->Key_Pressing(DIK_UP, fTimeDelta))
    {
        m_pTransformCom->Go_Straight(fTimeDelta);
    }
    if (m_pGameInstance->Key_Pressing(DIK_DOWN, fTimeDelta))
    {
        m_pTransformCom->Go_Backward(fTimeDelta);
    }
    if (m_pGameInstance->Key_Pressing(DIK_LEFT, fTimeDelta))
    {
        m_pTransformCom->Go_Left(fTimeDelta);
    }
    if (m_pGameInstance->Key_Pressing(DIK_RIGHT, fTimeDelta))
    {
        m_pTransformCom->Go_Right(fTimeDelta);
    }

    _int    iMouseMove = {};
    if (m_pGameInstance->Mouse_Pressing(MOUSEKEYSTATE::RB))
    {
        if (iMouseMove = m_pGameInstance->Mouse_Move(MOUSEMOVESTATE::X))
        {
            m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * iMouseMove * m_fMouseSensor);
        }

        if (iMouseMove = m_pGameInstance->Mouse_Move(MOUSEMOVESTATE::Y))
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
    TriggerDesc.fRadius = 40.f;
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

    // Y 스무딩
    /*_float fDesiredY = vCamPos.m128_f32[1];
    _float fCurrentY = m_pTransformCom->Get_State(STATE::POSITION).m128_f32[1];

    _float smoothedY = UpdateY_Stable(fCurrentY, fDesiredY, fTimeDelta);
    vCamPos = XMVectorSetY(vCamPos, smoothedY);*/

    //_float fAlphaTarget = 1.f - expf(-m_fFollowValue * fTimeDelta);
    //m_vLerpMove = XMVectorLerp(m_vLerpMove, vTargetPos, fAlphaTarget);

    _vector vWorldUp, vLook, vRight, vUp;
    vWorldUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
    vLook = XMVector3Normalize(XMVectorSubtract(vTargetPos, vCamPos));
    vRight = XMVector3Normalize(XMVector3Cross(vWorldUp, vLook));
    vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));

    m_pTransformCom->Set_State(STATE::RIGHT, vRight);
    m_pTransformCom->Set_State(STATE::UP, vUp);
    m_pTransformCom->Set_State(STATE::LOOK, vLook);
    m_pTransformCom->Set_State(STATE::POSITION, vCamPos);

    return S_OK;
}

HRESULT CCamera_Compre::RayCast(_float fTimeDelta)
{
    _vector vPos = m_pTransformCom->Get_State(STATE::POSITION) - XMVectorSetW(XMVector3Normalize(m_pTransformCom->Get_State(STATE::LOOK)), 0.f) * 1.5f;
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

    const float playerEyeOffsetY = 1.5f;

    const _vector playerWorldPosition = XMVectorSet(
        m_pObjMatrix->_41,
        m_pObjMatrix->_42 + playerEyeOffsetY,
        m_pObjMatrix->_43,
        1.f
    );

    _vector playerToTargetVector = XMVectorSubtract(XMLoadFloat4(m_pLockOnPos), playerWorldPosition);
    float playerToTargetDistance = XMVectorGetX(XMVector3Length(playerToTargetVector));

    if (playerToTargetDistance < 1e-4f)
        return S_OK;

    _vector directionNormalized = XMVectorScale(playerToTargetVector, 1.0f / playerToTargetDistance);

    float normalizedX = XMVectorGetX(directionNormalized);
    float normalizedY = XMVectorGetY(directionNormalized) - 0.3f;   
    float normalizedZ = XMVectorGetZ(directionNormalized);

    float targetYaw = atan2f(normalizedZ, normalizedX);
    float targetPitch = asinf(Clamp(normalizedY, -1.f, 1.f));

    targetPitch = Clamp(targetPitch, m_fPitchMin, m_fPitchMax);

    static float smoothingVelocityYaw = 0.f;
    static float smoothingVelocityPitch = 0.f;

    m_fYaw = SmoothDampAngle(
        m_fYaw,
        targetYaw,
        smoothingVelocityYaw,
        0.08f,
        fTimeDelta
    );

    m_fPitch = SmoothDampScalar(
        m_fPitch,
        targetPitch,
        smoothingVelocityPitch,
        0.08f,
        fTimeDelta
    );
    return S_OK;
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
        }
        
        if (m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::WB))
        {
            m_fLockOnDelay = 0.f;
            m_isLockOn = false;
            m_pLockMonster = nullptr;
            m_pLockOnUI->LockOff();
        }
    }
}

CGameObject* CCamera_Compre::Pick_ClosetTarget()
{
    if (!m_pTransformCom || m_CollMonsters.empty())
        return nullptr;

    const _vector cameraWorldPosition = m_pTransformCom->Get_State(STATE::POSITION);
    const _vector cameraLookDirection = XMVector3Normalize(m_pTransformCom->Get_State(STATE::LOOK));

    CGameObject* bestObject = nullptr;
    float bestProjectedDistance = FLT_MAX;

    for (CGameObject* pObj : m_CollMonsters)
    {
        if (!pObj) continue;

        CTransform* pTransform = dynamic_cast<CTransform*>(pObj->Get_Component(TEXT("Com_Transform")));
        const _matrix world = pTransform->Get_WorldMatrix();
        const _vector objectWorldPosition = XMVectorSet(world.r[3].m128_f32[0], world.r[3].m128_f32[1], world.r[3].m128_f32[2], 1.f);
        const _vector toTargetVector = XMVectorSubtract(objectWorldPosition, cameraWorldPosition);

        const float worldDistance = XMVectorGetX(XMVector3Length(toTargetVector));
        if (worldDistance > m_fTargetMaxDistance) continue;

        const _vector toTargetNormalized = XMVector3Normalize(toTargetVector);
        const float forwardCos = XMVectorGetX(XMVector3Dot(toTargetNormalized, cameraLookDirection));
        if (forwardCos < m_fTargetHalfFovCos) continue;

        const float projectedDistance = XMVectorGetX(XMVector3Dot(toTargetVector, cameraLookDirection));
        if (projectedDistance <= 0.0f) continue;

        if (projectedDistance < bestProjectedDistance)
        {
            bestProjectedDistance = projectedDistance;
            bestObject = pObj;
        }
    }

    return bestObject;
}

_vector CCamera_Compre::Cal_CamPos(_float fTimeDelta, _vector& vTargetPos, _vector& vDir)
{
    if (!m_isLockOn) {
        _int iMouseMoveX = m_pGameInstance->Mouse_Move(MOUSEMOVESTATE::X);
        _int iMouseMoveY = m_pGameInstance->Mouse_Move(MOUSEMOVESTATE::Y);

        m_fYaw = WrapAngle(m_fYaw - fTimeDelta * iMouseMoveX * m_fMouseSensor);
        m_fPitch = Clamp(m_fPitch - fTimeDelta * iMouseMoveY * m_fMouseSensor, m_fPitchMin, m_fPitchMax);
    }
    vTargetPos = XMVectorSet(m_pObjMatrix->_41, m_pObjMatrix->_42 + 1.5f, m_pObjMatrix->_43, 1.f);
    vDir = XMVectorSet(cosf(m_fPitch) * cosf(m_fYaw), sinf(m_fPitch), cosf(m_fPitch) * sinf(m_fYaw), 0.f);
    vDir = XMVector3Normalize(vDir);

    _vector vCamPos = XMVectorMultiplyAdd(XMVectorReplicate(-m_fRadius), vDir, vTargetPos);

    return vCamPos;
}

_float CCamera_Compre::UpdateY_Stable(_float fCurrentY, _float fDesiredY, _float fTimeDelta)
{
    if (!m_isInited) { m_fSmoothY = fCurrentY; m_fYVel = 0.f; m_isInited = true; }

    // 1) 데드존: 미세한 요철 변화 무시
    float delta = fDesiredY - m_fSmoothY;
    if (fabsf(delta) < m_fDeadZone)
        fDesiredY = m_fSmoothY;

    // 2) 상승/하강 속도 제한
    {
        float raw = fDesiredY - m_fSmoothY;
        float maxStep = (raw >= 0 ? m_fMaxRise : m_fMaxFall) * fTimeDelta;
        fDesiredY = m_fSmoothY + std::clamp(raw, -fabsf(maxStep), fabsf(maxStep));
    }

    // 3) 크리티컬 감쇠
    m_fSmoothY = SmoothDampScalar(m_fSmoothY, fDesiredY, m_fYVel, m_fYSmoothTime, fTimeDelta);

    return m_fSmoothY;
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

void CCamera_Compre::Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal)
{
    if (iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::MONSTER))
    {
        if (pDesc->pGameObject == nullptr)
            return;

        CGameObject* pObj = pDesc->pGameObject;
        if (pObj && std::find(m_CollMonsters.begin(), m_CollMonsters.end(), pObj) == m_CollMonsters.end())
            m_CollMonsters.push_back(pObj);
    }
}

void CCamera_Compre::Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal)
{
    if (!m_isLockOn)
    {
        if (iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::MONSTER))
        {
            if (m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::WB))
            {
                if (m_fLockOnDelay > 0.3f)
                {
                    
                    m_pLockMonster = Pick_ClosetTarget();
                    if (m_pLockMonster)
                    {
                        m_isLockOn = true;
                        m_pLockOnPos = dynamic_cast<CMonster*>(m_pLockMonster)->Get_LockOnPosition();
                        m_pLockOnUI->LockOn(m_pLockOnPos);
                    }
                }
            }
        }
    }
}

void CCamera_Compre::Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer)
{
    if (iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::MONSTER))
    {
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
    __super::Free();

    Safe_Release(m_pLockOnUI);
    Safe_Release(m_pBody);
}
