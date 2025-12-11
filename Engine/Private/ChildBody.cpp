#include "ChildBody.h"
#include "GameInstance.h"
#include "Transform.h"
#include "Model.h"
#include "Bone.h"

static inline bool IsFiniteVec3(const JPH::Vec3& v) {
    return std::isfinite(v.GetX()) && std::isfinite(v.GetY()) && std::isfinite(v.GetZ());
}

CChildBody::CChildBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice { pDevice }
    , m_pContext { pContext }
    , m_pGameInstance { CGameInstance::GetInstance() }
{

    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
    Safe_AddRef(m_pGameInstance);
}

HRESULT CChildBody::Initialize(CHILD_BODY_DESC* pDesc)
{
    m_pModel = pDesc->pModel;
    m_iBoneIndex = pDesc->iBoneIndex;
    m_pBone = m_pModel->Find_Bone(pDesc->iBoneIndex);
    m_strBoneName = m_pBone->Get_Name();
    m_pOwnerTransform = pDesc->pOwnerTransform;
    m_iObjectLayer = pDesc->iObjectLayer;
    m_pParentBody = pDesc->pParentBody;
    m_iDepth = pDesc->iDepth;

    m_fGravity = pDesc->fGravity;
    m_fAngularDamping = pDesc->fAngularDamping;
    m_fLinearDamping = pDesc->fLinearDamping;
    m_fMass = pDesc->fMass;

    m_fMinDistance = pDesc->fMinDistance;
    m_fMaxDistance = pDesc->fMaxDistance;
    m_fSpringFrequency = pDesc->fSpringFrequency;
    m_fSpringDamping = pDesc->fSpringDamping;

    m_pRootBody = pDesc->pRootBody;

    m_eClothType = pDesc->eType;

    if (FAILED(Ready_Body(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Child(pDesc)))
        return E_FAIL;

    _matrix restLocal = m_pBone->Get_TransformationMatrix();
    _vector vScale, vQuat, vTrans;
    XMMatrixDecompose(&vScale, &vQuat, &vTrans, restLocal);

    XMStoreFloat3(&m_vRestLocalPos, vTrans);
    XMStoreFloat4(&m_vRestLocalRot, vQuat);
    

    return S_OK;
}

void CChildBody::Priority_Update(_float fTimeDelta)
{
    _matrix NonScaleBoneMatrix = m_pBone->Get_CombinedTransformationMatrix();
    for (_uint i = 0; i < 3; i++)
        NonScaleBoneMatrix.r[i] =
        XMVector3Normalize(NonScaleBoneMatrix.r[i]); // 0~2번 행만 노멀라이즈, 3번은 그대로

    XMStoreFloat4x4(&m_CombindMatrix,
        NonScaleBoneMatrix * m_pOwnerTransform->Get_WorldMatrix());

    for (auto Child : m_ChildBodys)
    {
        Child->Priority_Update(fTimeDelta);
    }
}

void CChildBody::Update(_float fTimeDelta, _bool isFrozen)
{
    m_fFeelerTime += fTimeDelta;

    if (isFrozen)
    {
        ClampToCharacter();
        return;
    }

    Apply_RootInertia(fTimeDelta);
    Type_Update(fTimeDelta);

    Limit_Velocity();
    ClampToCharacter();
}

void CChildBody::Late_Update(_float fAlpha, _bool isFrozen)
{
    float baseAlpha = 1.f;

    if (m_eClothType == CLOTHTYPE::SKIRT)
        baseAlpha = 0.5f;
    else if (m_eClothType == CLOTHTYPE::CAPE)
        baseAlpha = 1.0f;

    ApplyToBones(fAlpha * baseAlpha, isFrozen);

    for (auto Child : m_ChildBodys)
    {
        Child->Late_Update(fAlpha, isFrozen);
    }
}

void CChildBody::ApplyToBones(_float fAlpha, _bool isFrozen)
{
    if (!m_pBody || !m_pOwnerTransform)
        return;

    if (isFrozen)
        fAlpha = 0.f;

    const _int parentIndex = m_pBone->Get_ParentBoneIndex();
    if (parentIndex < 0)
        return;

    CBone* pParentBone = m_pModel->Find_Bone(parentIndex);
    if (!pParentBone)
        return;

    // 부모 뼈 월드
    _matrix parentCombined = pParentBone->Get_CombinedTransformationMatrix();
    _matrix parentWorld = parentCombined * m_pOwnerTransform->Get_WorldMatrix();
    _matrix invParentWorld = XMMatrixInverse(nullptr, parentWorld);

    // 물리 바디 월드 위치
    RVec3  bodyPosR = m_pBodyInterface->GetCenterOfMassPosition(m_BodyID);
    _vector bodyWorldPos = XMVectorSet(
        (float)bodyPosR.GetX(),
        (float)bodyPosR.GetY(),
        (float)bodyPosR.GetZ(),
        1.f
    );

    // 부모 기준 로컬 위치로 변환
    _vector physLocalPos = XMVector3TransformCoord(bodyWorldPos, invParentWorld);

    // 기준 로컬 포즈 (rest pose)
    _vector tRest = XMLoadFloat3(&m_vRestLocalPos);
    _vector rRest = XMLoadFloat4(&m_vRestLocalRot);

    _vector delta = physLocalPos - tRest;

    // ========= 위/아래, 수평 분리 =========
    XMVECTOR up = XMVectorSet(0.f, 1.f, 0.f, 0.f);
    XMVECTOR vertical = XMVector3Dot(delta, up) * up; // Y
    XMVECTOR horizontal = delta - vertical;            // XZ

    float vLen = XMVectorGetX(XMVector3Dot(vertical, up)); // +면 위, -면 아래

    float maxHoriz;
    float verticalUpScale;
    float baseSagPerDepth;

    if (m_eClothType == CLOTHTYPE::SKIRT)
    {
        maxHoriz = 0.35f;  // 치마는 많이 안 퍼지게
        verticalUpScale = 0.3f;   // 위로는 30%만
        baseSagPerDepth = 0.03f;  // depth당 3cm 처짐
    }
    if (m_eClothType == CLOTHTYPE::FEELER)
    {
        baseSagPerDepth = 0.01f;  // 거의 안 처짐
        verticalUpScale = 0.7f;
        maxHoriz = 0.25f;    // 옆으로도 많이 안 퍼지게
    }
    else // CAPE
    {
        maxHoriz = 0.6f;   // 망토는 조금 더 많이 퍼져도 됨
        verticalUpScale = 0.7f;   // 위로도 꽤 움직이게
        baseSagPerDepth = 0.02f;  // 너무 축 늘어지지 않게
    }

    // 위로 튀는 건 scale 줄여서 눌러줌
    XMVECTOR verticalClamped;
    if (vLen > 0.f)
        verticalClamped = vertical * verticalUpScale;
    else
        verticalClamped = vertical;

    // 수평 퍼짐 길이 제한
    float horizLen = XMVectorGetX(XMVector3Length(horizontal));
    if (horizLen > maxHoriz && horizLen > 1e-6f)
        horizontal = horizontal * (maxHoriz / horizLen);

    delta = horizontal + verticalClamped;

    // depth에 따라 영향도 조절 (점점 더 많이 흔들리게)
    float depth = (float)m_iDepth;
    float depthFactor = 0.2f + 0.6f * (1.0f - expf(-0.5f * depth)); // 0.2~0.8
    depthFactor = clamp(depthFactor, 0.2f, 0.8f);

    float finalAlpha = fAlpha * depthFactor;

    // depth에 따른 기본 처짐
    float sag = baseSagPerDepth * (float)m_iDepth;
    XMVECTOR sagOffset = XMVectorSet(0.f, -sag, 0.f, 0.f);

    XMVECTOR finalPos = tRest + delta * finalAlpha + sagOffset;

    // scale은 1 유지, 회전은 rest 기준
    XMVECTOR sRest = XMVectorSet(1.f, 1.f, 1.f, 0.f);
    XMMATRIX finalLocalM = XMMatrixAffineTransformation(
        sRest,
        XMVectorZero(),
        rRest,
        finalPos
    );

    m_pBone->Set_TransformationMatrix(finalLocalM);
    m_pBone->Update_CombinedTransformationMatrix(pParentBone);
}

void CChildBody::Type_Update(_float fTimeDelta)
{
    if (m_eClothType == CLOTHTYPE::CAPE)
    {
        Cape_Update(fTimeDelta);
    }
    else if (m_eClothType == CLOTHTYPE::FEELER)
    {
        Feeler_Update(fTimeDelta);
    }
}

void CChildBody::Cape_Update(_float fTimeDelta)
{
    if (!m_pBody || !m_pParentBody || !m_pRootBody || !m_pOwnerTransform)
        return;

    // === 캐릭터 선속도 ===
    Vec3 charVel = m_pRootBody->GetLinearVelocity();
    Vec3 horizVel(charVel.GetX(), 0.f, charVel.GetZ());
    float speed = horizVel.Length();

    // 캐릭터 기준 방향 벡터
    Vec3 charForward = LoadVec3(m_pOwnerTransform->Get_State(STATE::LOOK));
    charForward.SetY(0.f);
    charForward = charForward.Normalized();

    Vec3 charRight = LoadVec3(m_pOwnerTransform->Get_State(STATE::RIGHT));
    charRight.SetY(0.f);
    charRight = charRight.Normalized();

    // depth에 따라 영향도 조금 조절 (꼬리일수록 더 많이 펄럭)
    float depth = (float)m_iDepth;

    // 0 뼈 = 0.3, 깊어질수록 0.7까지 점점 증가 (그리고 절대 1 안 넘어가게)
    float depthFactor = 0.3f + 0.4f * (1.0f - expf(-0.5f * depth));
    depthFactor = clamp(depthFactor, 0.3f, 0.7f);

    // 누적 force
    Vec3 totalForce = Vec3::sZero();

    // === 1) 전진 속도 → 뒤로 젖혀지는 "바람" 효과 ===
    if (speed > 0.1f)
    {
        horizVel /= speed;
        float dirDot = horizVel.Dot(charForward);   // 1 = 정면 전진, 0 = 옆, 음수 = 후진
        dirDot = clamp(dirDot, 0.f, 1.f);           // 전진일 때만 사용

        float maxConsiderSpeed = 8.0f;
        float sp01 = min(speed / maxConsiderSpeed, 1.f);

        float windStrength = 8.0f; // 기본 강도
        float finalScale = windStrength * dirDot * sp01 * depthFactor;

        if (finalScale > 0.0f)
        {
            Vec3 windDir = -charForward; // 전진할수록 뒤로 젖혀짐
            totalForce += windDir * finalScale;
        }
    }

    // === 2) RootBody yaw 회전 속도 → 옆으로 휘날리는 효과 ===
    float yawVel = m_pRootBody->GetAngularVelocity().GetY();
    if (fabsf(yawVel) > 0.1f)
    {
        float maxYaw = 5.0f;
        float yaw01 = min(fabsf(yawVel) / maxYaw, 1.f);

        float k_yaw = 5.0f * depthFactor; // 회전 강도 (꼬리쪽이 더 크게 반응)

        // 회전 방향에 따라 반대쪽으로 "쓸린다"는 느낌
        Vec3 sideDir = (yawVel > 0.f) ? -charRight : charRight;
        Vec3 yawForce = sideDir * (k_yaw * yaw01);

        totalForce += yawForce;
    }

    // === 3) Force 적용 ===
    if (!totalForce.IsNearZero())
    {
        // 너무 과한 force 방지용 클램프
        float maxForce = 80.0f;
        float fLen = totalForce.Length();
        if (fLen > maxForce)
            totalForce *= (maxForce / fLen);

        m_pBodyInterface->AddForce(m_BodyID, totalForce);
    }

    // === 4) 망토 속도 상한 ===
    Vec3 vel = m_pBodyInterface->GetLinearVelocity(m_BodyID);
    //float maxSpeed = 4.0f + 1.0f * (float)m_iDepth; // 깊을수록 조금 더 허용
    float maxSpeed = 3.0f;


    float len = vel.Length();

    if (len > maxSpeed && len > 1e-4f)
    {
        vel *= (maxSpeed / len);
        m_pBodyInterface->SetLinearVelocity(m_BodyID, vel);
    }
}

void CChildBody::Feeler_Update(_float fTimeDelta)
{
    if (!m_pBody || !m_pRootBody || !m_pOwnerTransform || fTimeDelta <= 0.f)
        return;

    // ===== 1) Root 속도 기반 관성 =====
    Vec3 rootVel = m_pRootBody->GetLinearVelocity();
    float rootSpeed = rootVel.Length();

    // 촉수가 느낄 수 있는 최대 속도 제한
    const float maxClothSpeed = 10.0f;
    if (rootSpeed > maxClothSpeed && rootSpeed > 1e-4f)
    {
        rootVel *= (maxClothSpeed / rootSpeed);
        rootSpeed = maxClothSpeed;
    }

    // 캐릭터 기준 방향 벡터
    Vec3 forward = LoadVec3(m_pOwnerTransform->Get_State(STATE::LOOK));
    forward.SetY(0.f); forward = forward.Normalized();

    Vec3 right = LoadVec3(m_pOwnerTransform->Get_State(STATE::RIGHT));
    right.SetY(0.f); right = right.Normalized();

    Vec3 up(0.f, 1.f, 0.f);

    // 속도 성분 분해
    float vF = rootVel.Dot(forward);
    float vR = rootVel.Dot(right);
    float vU = rootVel.Dot(up);

    float depth = (float)m_iDepth;
    float depthFactor = 0.3f + 0.4f * (1.0f - expf(-0.4f * depth)); // 0.3~0.7 근처
    depthFactor = clamp(depthFactor, 0.3f, 0.7f);

    float kInertia = 2.0f * depthFactor;  // 기존 3.0f → 2.0f 정도로 전반적 감소

    Vec3 inertiaForce =
        (-vF * kInertia) * forward +   // 앞으로 달리면 뒤로 끌림
        (-vR * kInertia * 0.5f) * right +
        (-vU * kInertia * 0.3f) * up;

    // ===== 2) 살짝 wiggle 노이즈 =====
    float time = m_fFeelerTime; // 엔진 쪽에서 쓰는 전역 시간 함수 아무거나

    float freq = 5.0f + depth * 0.8f;
    float amp = 0.2f + depth * 0.03f;  // 기존 0.4f + depth * 0.05f 에서 절반 정도로

    // rest 방향 기준으로 수직인 방향 하나 잡아서 wiggle
    Vec3 restLocal = Vec3(m_vRestLocalPos.x, m_vRestLocalPos.y, m_vRestLocalPos.z);
    restLocal = restLocal.Normalized();

    Vec3 axis = restLocal.Cross(Vec3::sAxisY());
    if (axis.LengthSq() < 1e-4f)
        axis = restLocal.Cross(Vec3::sAxisX());
    axis = axis.Normalized();

    float phase = depth * 0.7f;
    float s = sinf(time * freq + phase);

    Vec3 wiggleForce = axis * (amp * s);

    // ===== 3) force 합산 및 속도 상한 =====
    Vec3 totalForce = inertiaForce + wiggleForce;

    // 너무 과한 힘 방지
    float maxForce = 80.f;
    float fLen = totalForce.Length();
    if (fLen > maxForce)
        totalForce *= (maxForce / fLen);

    m_pBodyInterface->AddForce(m_BodyID, totalForce);

    // 속도 상한 (끝자락 늘어나는 것 방지)
    Vec3 vel = m_pBodyInterface->GetLinearVelocity(m_BodyID);
    //float maxSpeed = 2.5f + 0.3f * min(depth, 4.0f); // 2.5 ~ 3.7
    float maxSpeed = 3.0f;
    float len = vel.Length();
    if (len > maxSpeed && len > 1e-4f)
    {
        vel *= (maxSpeed / len);
        m_pBodyInterface->SetLinearVelocity(m_BodyID, vel);
    }
}

void CChildBody::Apply_RootInertia(_float fTimeDelta)
{
    if (!m_pBody || !m_pRootBody || fTimeDelta <= 0.f)
        return;

    Vec3 vRootVel = m_pRootBody->GetLinearVelocity();

    if (!m_isPrevRootVel)
    {
        m_vPrevRootVel = vRootVel;
        m_isPrevRootVel = true;
        return;
    }

    // a = (v - v_prev) / dt
    Vec3 rootAcc = (vRootVel - m_vPrevRootVel) / fTimeDelta;
    m_vPrevRootVel = vRootVel;

    // 캐릭터 기준 좌표로 분해 (전/후, 좌/우, 상/하)
    Vec3 forward = LoadVec3(m_pOwnerTransform->Get_State(STATE::LOOK));
    forward.SetY(0.f); forward = forward.Normalized();

    Vec3 right = LoadVec3(m_pOwnerTransform->Get_State(STATE::RIGHT));
    right.SetY(0.f); right = right.Normalized();

    Vec3 up(0.f, 1.f, 0.f);

    float accForward = rootAcc.Dot(forward); // +면 앞으로 가속, -면 뒤로 가속
    float accRight = rootAcc.Dot(right);
    float accUp = rootAcc.Dot(up);

    // 뎁스에 따라 조금 강도 다르게 (꼬리쪽이 더 크게 출렁)
    float depthFactor = 0.5f + 0.1f * (float)m_iDepth;
    if (depthFactor > 1.f) depthFactor = 1.f;

    // 계수 값은 나중에 데이터로 빼고, 우선은 감으로 튜닝
    float k_forward = 0.8f * depthFactor;
    float k_side = 0.4f * depthFactor;
    float k_up = 0.3f * depthFactor;

    // 관성은 항상 "가속 방향의 반대"로 느껴지니까 부호를 반대로
    Vec3 inertialForce =
        (-accForward * k_forward) * forward +
        (-accRight * k_side) * right +
        (-accUp * k_up) * up;

    // 너무 미친 값 나오지 않게 한 번 클램프
    float maxForce = 50.0f;
    float fLen = inertialForce.Length();
    if (fLen > maxForce)
        inertialForce *= (maxForce / fLen);

    m_pBodyInterface->AddForce(m_BodyID, inertialForce);
}

void CChildBody::ClampToCharacter()
{
    if (!m_pBody || !m_pOwnerTransform)
        return;

    const _int parentIndex = m_pBone->Get_ParentBoneIndex();
    if (parentIndex < 0)
        return;

    CBone* pParentBone = m_pModel->Find_Bone(parentIndex);
    if (!pParentBone)
        return;

    _matrix parentCombined = pParentBone->Get_CombinedTransformationMatrix();
    _matrix parentWorld = parentCombined * m_pOwnerTransform->Get_WorldMatrix();

    XMVECTOR restLocalPos = XMLoadFloat3(&m_vRestLocalPos);
    XMVECTOR restWorldPosV = XMVector3TransformCoord(restLocalPos, parentWorld);

    RVec3 restWorldPos(
        XMVectorGetX(restWorldPosV),
        XMVectorGetY(restWorldPosV),
        XMVectorGetZ(restWorldPosV)
    );

    RVec3 cur = m_pBodyInterface->GetCenterOfMassPosition(m_BodyID);
    Vec3  diff = Vec3(cur - restWorldPos);
    float len = diff.Length();

    // --- bone 길이 기반 하드 반경 ---
    float restLen = XMVectorGetX(XMVector3Length(restLocalPos));
    // 한 segment 는 자기 rest 길이의 0.6배까지만 벗어날 수 있음 (수치는 취향대로)
    float hardRadius = restLen * 0.6f;
    if (hardRadius < 0.02f) hardRadius = 0.02f; // 너무 작은 뼈 보호용

    if (len > hardRadius && len > 1e-4f)
    {
        diff *= (hardRadius / len);
        RVec3 clampedPos = restWorldPos + diff;

        Quat rot = m_pBody->GetRotation();
        m_pBodyInterface->SetPositionAndRotation(
            m_BodyID,
            clampedPos,
            rot,
            EActivation::Activate
        );

        // 폭주한 애는 속도도 확 줄이기
        Vec3 vel = m_pBodyInterface->GetLinearVelocity(m_BodyID);
        vel *= 0.1f;
        m_pBodyInterface->SetLinearVelocity(m_BodyID, vel);
    }
}


void CChildBody::HardSnapToAnimationRecursive()
{
    if (!m_pBody || !m_pOwnerTransform)
        return;

    const _int parentIndex = m_pBone->Get_ParentBoneIndex();
    if (parentIndex < 0)
        return;

    CBone* pParentBone = m_pModel->Find_Bone(parentIndex);
    if (!pParentBone)
        return;

    _matrix parentCombined = pParentBone->Get_CombinedTransformationMatrix();
    _matrix parentWorld = parentCombined * m_pOwnerTransform->Get_WorldMatrix();

    XMVECTOR tRest = XMLoadFloat3(&m_vRestLocalPos);
    XMVECTOR rRest = XMLoadFloat4(&m_vRestLocalRot);

    XMMATRIX worldM = XMMatrixAffineTransformation(
        XMVectorSet(1.f, 1.f, 1.f, 0.f),
        XMVectorZero(),
        rRest,
        tRest
    );

    worldM = worldM * parentWorld;

    // 월드 행렬 → pos / rot
    _vector vS, vR, vT;
    XMMatrixDecompose(&vS, &vR, &vT, worldM);

    RVec3 pos(
        XMVectorGetX(vT),
        XMVectorGetY(vT),
        XMVectorGetZ(vT)
    );
    Quat rot(
        XMVectorGetX(vR),
        XMVectorGetY(vR),
        XMVectorGetZ(vR),
        XMVectorGetW(vR)
    );

    m_pBodyInterface->SetPositionAndRotation(m_BodyID, pos, rot, EActivation::DontActivate);
    m_pBodyInterface->SetLinearVelocity(m_BodyID, Vec3::sZero());
    m_pBodyInterface->SetAngularVelocity(m_BodyID, Vec3::sZero());

    // 자식들도 재귀적으로
    for (auto Child : m_ChildBodys)
        Child->HardSnapToAnimationRecursive();
}


HRESULT CChildBody::Ready_Child(CHILD_BODY_DESC* pDesc)
{
    vector<_int> ChildBoneIdx = m_pBone->Get_ChildBones();
    for (_int i = 0; i < ChildBoneIdx.size(); i++)
    {
        CHILD_BODY_DESC desc;
        desc.pModel = m_pModel;
        desc.iBoneIndex = ChildBoneIdx[i];
        desc.pOwnerTransform = m_pOwnerTransform;
        desc.iObjectLayer = m_iObjectLayer;
        desc.pParentBody = m_pBody;
        desc.iDepth = m_iDepth + 1;

        desc.fGravity = m_fGravity;
        desc.fAngularDamping = m_fAngularDamping;
        desc.fLinearDamping = m_fLinearDamping;
        desc.fMass = m_fMass;

        desc.fMinDistance = m_fMinDistance;
        desc.fMaxDistance = m_fMaxDistance;
        desc.fSpringFrequency = m_fSpringFrequency;
        desc.fSpringDamping = m_fSpringDamping;

        desc.eType = m_eClothType;

        desc.pRootBody = m_pRootBody;
        desc.pCollisionDesc = pDesc->pCollisionDesc;
        m_ChildBodys.push_back(CChildBody::Create(m_pDevice, m_pContext, &desc));
    }
    return S_OK;
}

HRESULT CChildBody::Ready_Body(CHILD_BODY_DESC* pDesc)
{
    RefConst<Shape> pShape;
    pShape = new SphereShape(0.05f);
    pShape = new RotatedTranslatedShape(Vec3(0.f, 0.f, 0.f), Quat::sIdentity(), pShape);

    _matrix NonScaleBoneMatrix = m_pBone->Get_CombinedTransformationMatrix();
    for (_uint i = 0; i < 3; i++)
        NonScaleBoneMatrix.r[i] =
        XMVector3Normalize(NonScaleBoneMatrix.r[i]);

    _matrix CombindedMatirx = NonScaleBoneMatrix * m_pOwnerTransform->Get_WorldMatrix();

    _vector vScale, vTrans, vQuat;

    XMMatrixDecompose(&vScale, &vQuat, &vTrans, CombindedMatirx);

    BodyCreationSettings BCS(
        pShape,
        Vec3(vTrans.m128_f32[0], vTrans.m128_f32[1], vTrans.m128_f32[2]),
        Quat(vQuat.m128_f32[0], vQuat.m128_f32[1], vQuat.m128_f32[2], vQuat.m128_f32[3]),
        m_eMotion,
        ObjectLayer(m_iObjectLayer)
    );

    BCS.mMotionQuality = EMotionQuality::LinearCast;
    BCS.mIsSensor = false;
    BCS.mGravityFactor = m_fGravity;      // 완전 0 말고 살짝
    BCS.mAngularDamping = m_fAngularDamping;     // 회전 감쇠 너무 크지 않게
    BCS.mLinearDamping = m_fLinearDamping;     // 직선 감쇠도
    BCS.mOverrideMassProperties = EOverrideMassProperties::CalculateInertia;
    BCS.mMassPropertiesOverride.mMass = m_fMass; // 너무 무겁지 않게
    BCS.mUserData = static_cast<uint64>(reinterpret_cast<uintptr_t>(pDesc->pCollisionDesc));
    m_pBody = m_pGameInstance->CreateAndAdd_Body(BCS, &m_pBodyInterface);
    m_BodyID = m_pBody->GetID();


    // 제약 설정
    if (!m_pParentBody->GetID().IsInvalid())
    {
        // 부모/자식 COM 월드 위치     
        RVec3 parentPos = m_pParentBody->GetCenterOfMassPosition();
        RVec3 childPos = m_pBody->GetCenterOfMassPosition();

        DistanceConstraintSettings settings;
        settings.mSpace = EConstraintSpace::WorldSpace;
        settings.mPoint1 = parentPos;
        settings.mPoint2 = childPos;

        // rest 거리
        _float restLen = (_float)(childPos - parentPos).Length();
        // depth에 따라 슬랙 조절
        float depth = (float)m_iDepth;

        // 루트 근처(0~2뎁스)는 조금 여유, 끝으로 갈수록 1.0에 가까워지도록 lerp
        float t = min(depth / 5.0f, 1.0f); // depth 5 이상이면 t=1

        float minBase = 0.98f;
        float maxBase = 1.02f;
        float minTip = 0.995f;
        float maxTip = 1.005f;

        float localMin = Lerp(minBase, minTip, t);
        float localMax = Lerp(maxBase, maxTip, t);

        settings.mMinDistance = restLen * localMin;
        settings.mMaxDistance = restLen * localMax;

        // 부드러운 스프링
        settings.mLimitsSpringSettings.mFrequency = m_fSpringFrequency; // 2Hz 정도 (너무 크면 탱탱볼처럼 튐)
        settings.mLimitsSpringSettings.mDamping = m_fSpringDamping; // 약간 감쇠

        m_pConstraint = static_cast<DistanceConstraint*>(settings.Create(*m_pParentBody, *m_pBody));

        // PhysicsSystem에 등록 (엔진 래퍼에 맞게)
        m_pGameInstance->Add_Constraint(m_pConstraint);
    }

    return S_OK;
}

void CChildBody::Limit_Velocity()
{
    if (!m_pBodyInterface || m_BodyID.IsInvalid())
        return;

    Vec3 vel = m_pBodyInterface->GetLinearVelocity(m_BodyID);

    // depth에 따라 조금씩 허용 (꼬리일수록 살짝 더 빠르게)
    float depth = (float)m_iDepth;
    //float maxSpeed = 2.5f + 0.3f * min(depth, 4.0f); // 2.5 ~ 3.7
    float maxSpeed = 3.0f;

    float len = vel.Length();
    if (len > maxSpeed && len > 1e-4f)
    {
        vel *= (maxSpeed / len);
        m_pBodyInterface->SetLinearVelocity(m_BodyID, vel);
    }
}

CChildBody* CChildBody::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CHILD_BODY_DESC* pDesc)
{
    CChildBody* pInstance = new CChildBody(pDevice, pContext);

    if (FAILED(pInstance->Initialize(pDesc)))
    {
        MSG_BOX(TEXT("Failed to Created : CChildBody"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CChildBody::Free()
{

    if (m_pConstraint != nullptr)
    {
        /*m_pGameInstance->Remove_Constraint(m_pConstraint);
        m_pConstraint = nullptr;*/
    }

    if (!m_BodyID.IsInvalid() && m_pBodyInterface)
    {
        if (!m_BodyID.IsInvalid())
        {
            m_pGameInstance->Destroy_Body(m_BodyID);
            m_BodyID = BodyID();
            m_pBody = nullptr;
        }

    }

    for (auto pChild : m_ChildBodys)
        Safe_Release(pChild);
    m_ChildBodys.clear();

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
    Safe_Release(m_pGameInstance);

    __super::Free();

}
