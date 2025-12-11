#include "Body.h"
#include "GameInstance.h"
#include "Transform.h"
#include "Model.h"

static inline bool IsFiniteVec3(const JPH::Vec3& v) {
    return std::isfinite(v.GetX()) && std::isfinite(v.GetY()) && std::isfinite(v.GetZ());
}

CBody::CBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CRigidBody{ pDevice, pContext }
{
}

CBody::CBody(const CBody& Prototype)
    : CRigidBody{ Prototype }
{
}

HRESULT CBody::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBody::Initialize_Clone(void* pArg)
{
    if (pArg == nullptr)
        return E_FAIL;

    BODY_DESC* pDesc = static_cast<BODY_DESC*>(pArg);

    RefConst<Shape> BodyShape;

    Build_Shape(pDesc, BodyShape);

    m_eMotion = pDesc->eMotion;
    m_iObjectLayer = pDesc->iObjectLayer;
    pDesc->pCollisionDesc->iObjectLayer = m_iObjectLayer;
    BodyCreationSettings BCS(
        BodyShape,
        Vec3(pDesc->vPos.x, pDesc->vPos.y, pDesc->vPos.z),
        Quat(pDesc->vQuat.x, pDesc->vQuat.y, pDesc->vQuat.z, pDesc->vQuat.w),
        m_eMotion,
        ObjectLayer(m_iObjectLayer)
    );
    // Motion Quality
    BCS.mMotionQuality = pDesc->eQuality;

    // Trigger(센서) 처리
    BCS.mIsSensor = pDesc->bIsTrigger;
    BCS.mUserData = static_cast<uint64>(reinterpret_cast<uintptr_t>(pDesc->pCollisionDesc));        
    // Dynamic 질량/관성(필요 시 주석 해제하여 특정 질량/관성 지정)
    if (m_eMotion == EMotionType::Dynamic)
    {
        // 예시) 질량만 오버라이드
        BCS.mAngularDamping = pDesc->fAngularDamping;
        BCS.mLinearDamping = pDesc->fLinearDamping;
        BCS.mOverrideMassProperties = EOverrideMassProperties::CalculateInertia;
        BCS.mMassPropertiesOverride.mMass = pDesc->fMass;
        BCS.mAllowSleeping = true;
    }

    BCS.mGravityFactor = pDesc->fGravity;
    if (pDesc->eMotion == EMotionType::Static)
    {
        BCS.mEnhancedInternalEdgeRemoval = true;
    }
    
    if (pDesc->eShapeType != SHAPE::MESH)
    {
        m_pBody = m_pGameInstance->CreateAndAdd_Body(BCS, &m_pBodyInterface);
        m_pBody->SetCollideKinematicVsNonDynamic(pDesc->isCollideKinematicVsNonDynamic);
        //BCS.
        m_BodyID = m_pBody->GetID();
    }


    // 머티리얼 반영
    

    return S_OK;
}

void CBody::Update(_float fTimeDelta, class CTransform* pTransform)
{
    if (!m_pBodyInterface->IsAdded(m_BodyID))
        return;

    if (!m_pBodyInterface->IsActive(m_BodyID))
        return;


    if (m_pBody->GetMotionType() == EMotionType::Kinematic)
    {
        _vector vScale{}, vRotation{}, vTranslation{};

        if (!XMMatrixDecompose(&vScale, &vRotation, &vTranslation, pTransform->Get_WorldMatrix()))
        {
            MSG_BOX(TEXT("Decompose Failed in CBody::Update"));
        }

        m_pBodyInterface->MoveKinematic(m_BodyID, LoadVec3(vTranslation), LoadQuat(vRotation), fTimeDelta);

    }

    Vec3 vPos;
    Quat qRotation;
    m_pBodyInterface->GetPositionAndRotation(m_BodyID, vPos, qRotation);

    _vector vQuaternion = XMVectorSet(qRotation.GetX(), qRotation.GetY(), qRotation.GetZ(), qRotation.GetW());
    pTransform->Set_Quaternion(vQuaternion);
    pTransform->Set_State(STATE::POSITION, XMVectorSet(vPos.GetX(), vPos.GetY(), vPos.GetZ(), 1.f));
    
}

void CBody::Sync_Update(CTransform* pTransform)
{
    if (!m_pBodyInterface->IsAdded(m_BodyID))
        return;

    if (!m_pBodyInterface->IsActive(m_BodyID))
        return;

    if (m_pBody->GetMotionType() == EMotionType::Kinematic)
        Set_PosRot(pTransform->Get_State(STATE::POSITION), pTransform->Get_Rotation_Quat());
}

void CBody::Update(_float fTimeDelta, _matrix WorldMatirx, _vector& outQuatRotation, _vector& outPosition)
{
    if (!m_pBodyInterface->IsAdded(m_BodyID))
        return;

    if (!m_pBodyInterface->IsActive(m_BodyID))
        m_pBodyInterface->ActivateBody(m_BodyID);


    if (m_pBody->GetMotionType() == EMotionType::Kinematic)
    {
        _vector vScale{}, vRotation{}, vTranslation{};

        XMMatrixDecompose(&vScale, &vRotation, &vTranslation, WorldMatirx);

        m_pBodyInterface->MoveKinematic(m_BodyID, LoadVec3(vTranslation), LoadQuat(vRotation), fTimeDelta);

    }

    Vec3 vPos;
    Quat qRotation;
    m_pBodyInterface->GetPositionAndRotation(m_BodyID, vPos, qRotation);

    _vector vQuaternion = XMVectorSet(qRotation.GetX(), qRotation.GetY(), qRotation.GetZ(), qRotation.GetW());

    outQuatRotation = vQuaternion;
    outPosition = XMVectorSet(vPos.GetX(), vPos.GetY(), vPos.GetZ(), 1.f);
}

void CBody::Sync_Update(_matrix WorldMatirx)
{
    if (!m_pBodyInterface->IsAdded(m_BodyID))
        return;

    _vector vScale, vRotation, vTranslation;

    XMMatrixDecompose(&vScale, &vRotation, &vTranslation, WorldMatirx);

    /*vRotation = XMVector3Normalize(vRotation);*/

    if (m_pBody->GetMotionType() == EMotionType::Kinematic)
        Set_PosRot(vTranslation, vRotation);
}

void CBody::MoveKinematic(_float fTimeDelta, CTransform* pTransform)
{
    if (!m_pBodyInterface->IsAdded(m_BodyID))
        return;

    if (!m_pBodyInterface->IsActive(m_BodyID))
    {
        m_pBodyInterface->ActivateBody(m_BodyID);
    }

    if (m_pBody->GetMotionType() == EMotionType::Kinematic)
    {
        _vector vScale{}, vRotation{}, vTranslation{};

        if (!XMMatrixDecompose(&vScale, &vRotation, &vTranslation, pTransform->Get_WorldMatrix()))
            return;

        m_pBodyInterface->MoveKinematic(
            m_BodyID,
            LoadVec3(vTranslation),
            LoadQuat(vRotation),
            fTimeDelta
        );

        Vec3 a = m_pBody->GetLinearVelocity();
        int b = 1;
        return;
    }
}

void CBody::MoveKinematic(_float fTimeDelta, _matrix WorldMatirx)
{
    if (!m_pBodyInterface->IsAdded(m_BodyID))
        return;

    if (!m_pBodyInterface->IsActive(m_BodyID))
    {
        m_pBodyInterface->ActivateBody(m_BodyID);
    }

    if (m_pBody->GetMotionType() == EMotionType::Kinematic)
    {
        // Kinematic은 Transform -> Body만
        _vector vScale{}, vRotation{}, vTranslation{};

        if (!XMMatrixDecompose(&vScale, &vRotation, &vTranslation, WorldMatirx))
            return;

        m_pBodyInterface->MoveKinematic(
            m_BodyID,
            LoadVec3(vTranslation),
            LoadQuat(vRotation),
            fTimeDelta
        );
        return;
    }
}

void CBody::Set_PosRot(_vector vPos, _vector vRot)
{
    m_pBodyInterface->SetPositionAndRotation(m_BodyID, LoadVec3(vPos), LoadQuat(vRot), EActivation::Activate);
}

void CBody::Set_Pos(_vector vPos)
{
    m_pBodyInterface->SetPosition(m_BodyID, LoadVec3(vPos), EActivation::Activate);
}

_vector CBody::Get_Pos()
{
    Vec3 vPostion = m_pBodyInterface->GetPosition(m_BodyID);

    return XMVectorSet(vPostion.GetX(), vPostion.GetY(), vPostion.GetZ(), 1.f);
}

_vector CBody::Get_Rot()
{
    Quat vRotation = m_pBodyInterface->GetRotation(m_BodyID);

    return XMVectorSet(vRotation.GetX(), vRotation.GetY(), vRotation.GetZ(), vRotation.GetW());
}

_float3 CBody::Get_Velocity()
{
    Vec3 vVelocity = m_pBody->GetLinearVelocity();
    return _float3(vVelocity.GetX(), vVelocity.GetY(), vVelocity.GetZ());
}

void CBody::Add_Force(_float fMass)
{
    if (m_BodyID.IsInvalid()) return;
    m_pBodyInterface->AddForce(m_BodyID, m_pBody->GetLinearVelocity() * fMass);
}

void CBody::Add_Torque(_float fMass)
{
    if (m_BodyID.IsInvalid()) return;
    m_pBodyInterface->AddTorque(m_BodyID, m_pBody->GetLinearVelocity() * fMass);
}

void CBody::Add_Impulse(_float fMass)
{
    if (m_BodyID.IsInvalid()) return;
    m_pBodyInterface->AddImpulse(m_BodyID, m_pBody->GetLinearVelocity() * fMass);
}

void CBody::Add_ImpulseDir(_float3 vImpulse)
{
    if (m_BodyID.IsInvalid()) return;
    m_pBodyInterface->AddImpulse(m_BodyID, LoadVec3(vImpulse));
}

void CBody::Add_AngularImpulseDir(_float3 vAngularImpulse)
{
    if (m_BodyID.IsInvalid()) return;
    m_pBodyInterface->AddImpulse(m_BodyID, LoadVec3(vAngularImpulse));
}

void CBody::ApplyExplosion(
    const _float3& vExplosionPos,
    _float fBaseImpulse,
    _float fBaseTorque)
{
    if (m_BodyID.IsInvalid())
        return;

    // 1) 현재 파편 위치
    Vec3 chunkPos = m_pBodyInterface->GetPosition(m_BodyID);

    // 2) 폭심지: 파편 높이에 맞춰서 수평 기준만 사용
    Vec3 explosionPos = LoadVec3(vExplosionPos);
    explosionPos.SetY(chunkPos.GetY());

    // 3) 기본 방향 (수평 XZ)
    Vec3 toChunk = chunkPos - explosionPos;
    toChunk.SetY(0.0f);

    float distSq = toChunk.LengthSq();
    if (distSq < 1e-6f)
    {
        // 완전 겹치면 대충 +X 방향
        toChunk = Vec3(1, 0, 0);
        distSq = 1.0f;
    }
    float dist = sqrt(distSq);
    Vec3 mainDir = toChunk / dist; // 정규화 (기본 폭발 방향)

    // ============================
    // 4) Y축 기준으로 랜덤 회전 (yaw)
    //    → 같은 "옆 방향"이지만 각 파편마다 각도만 조금씩 다르게
    // ============================
    const float maxAngleDeg = 35.0f; // 최대 편차 각도 (원하면 20~45 사이에서 조절)
    float angleDeg = m_pGameInstance->Rand(-maxAngleDeg, maxAngleDeg);
    float angleRad = angleDeg * (XM_PI / 180.0f);

    float c = cosf(angleRad);
    float s = sinf(angleRad);

    float x = mainDir.GetX();
    float z = mainDir.GetZ();

    // Y축 회전 (yaw 회전)
    Vec3 dir(
        x * c - z * s,   // X'
        0.0f,            // 수평만 유지
        x * s + z * c    // Z'
    );
    dir = dir.Normalized();

    // ============================
    // 5) 힘 크기 랜덤 + (원하면) 살짝 거리 감쇠
    // ============================
    float randStrength = m_pGameInstance->Rand(0.6f, 1.4f);   // 파편마다 속도 차이
    float distanceFalloff = 1.0f;                             // 일단 감쇠 제거해서 확실히 보이게
    // 원하면:
    // distanceFalloff = 1.0f / max(dist * 0.5f, 1.0f);

    Vec3 impulse = dir * fBaseImpulse * randStrength * distanceFalloff;

    // ★ 위로(Lift) 없음 → "옆으로 펑"이 기본
    // 살짝 위로 띄우고 싶으면 이 정도만:
    // impulse += Vec3(0, 1, 0) * (fBaseImpulse * 0.05f);

    // ============================
    // 6) 랜덤 회전 (토크)
    // ============================
    Vec3 randomAxis(
        m_pGameInstance->Rand(-1.f, 1.f),
        m_pGameInstance->Rand(-1.f, 1.f),
        m_pGameInstance->Rand(-1.f, 1.f)
    );
    if (randomAxis.LengthSq() < 1e-4f)
        randomAxis = Vec3(0, 1, 0);
    randomAxis = randomAxis.Normalized();

    Vec3 spinAxis = randomAxis.Cross(dir);
    if (spinAxis.LengthSq() < 1e-4f)
        spinAxis = randomAxis;
    spinAxis = spinAxis.Normalized();

    float torqueStrength = fBaseTorque * m_pGameInstance->Rand(0.5f, 1.5f);
    Vec3 angularImpulse = spinAxis * torqueStrength;

    // 7) 적용
    m_pBodyInterface->AddImpulse(m_BodyID, impulse);
    m_pBodyInterface->AddAngularImpulse(m_BodyID, angularImpulse);
}

void CBody::Set_Velocity(const _float3& vVelocity)
{
    m_pBody->SetLinearVelocity(LoadVec3(vVelocity));
}

void CBody::Collision_Active(_bool isActive)
{
    if (!m_pBodyInterface)
        return;
    if (isActive)
    {
        if (!m_pBodyInterface->IsAdded(m_BodyID))
        {
            m_pBodyInterface->AddBody(m_BodyID, EActivation::Activate);
        }
    }
    else
    {
        if (m_pBodyInterface->IsAdded(m_BodyID))
        {
            m_pBodyInterface->RemoveBody(m_BodyID);
        }
    }
}

_bool CBody::Get_Collision_Active()
{
    return m_pBodyInterface->IsAdded(m_BodyID);
}

void CBody::Build_Shape(BODY_DESC* pDesc, RefConst<Shape>& pShape)
{
    switch (pDesc->eShapeType)
    {
    case SHAPE::BOX:
    {
        BODY_BOXSHAPE_DESC* pBoxDesc = static_cast<BODY_BOXSHAPE_DESC*>(pDesc);
        pShape = new BoxShape(Vec3(pBoxDesc->vExtent.x, pBoxDesc->vExtent.y, pBoxDesc->vExtent.z));
        pShape = new RotatedTranslatedShape(LoadVec3(pBoxDesc->vShapeOffset), LoadQuat(pBoxDesc->vShapeRotation), pShape);
        break;
    }
    case SHAPE::SPHERE:
    {
        BODY_SPHERESHAPE_DESC* pSphereDesc = static_cast<BODY_SPHERESHAPE_DESC*>(pDesc);
        pShape = new SphereShape(pSphereDesc->fRadius);
        pShape = new RotatedTranslatedShape(LoadVec3(pSphereDesc->vShapeOffset), LoadQuat(pSphereDesc->vShapeRotation), pShape);
        break;
    }
    case SHAPE::CAPSULE:
    {
        BODY_CAPSULESHAPE_DESC* pCapsuleDesc = static_cast<BODY_CAPSULESHAPE_DESC*>(pDesc);
        pShape = new CapsuleShape(pCapsuleDesc->fHeight * 0.5f, pCapsuleDesc->fRadius);
        pShape = new RotatedTranslatedShape(LoadVec3(pCapsuleDesc->vShapeOffset), LoadQuat(pCapsuleDesc->vShapeRotation), pShape);
        break;
    }

    case SHAPE::MESH:
    {
        BODY_MESHSHAPE_DESC* pMeshDesc = static_cast<BODY_MESHSHAPE_DESC*>(pDesc);
        Make_MeshShape(pMeshDesc);
        break;
    }
    case SHAPE::CONVEX:
    {
        /*BODY_CONVEXSHAPE_DESC* pConvexHullDesc = static_cast<BODY_CONVEXSHAPE_DESC*>(pDesc);
        if (pConvexHullDesc->pModel == nullptr)
            return;
        Ref<ConvexHullShapeSettings> ConvexHullSetting = new ConvexHullShapeSettings(ConvertToArrayVec3(pConvexHullDesc->pModel));
        pShape = ConvexHullSetting->Create().Get();*/
        BODY_CONVEXSHAPE_DESC* pConvexDesc = static_cast<BODY_CONVEXSHAPE_DESC*>(pDesc);
        if (pConvexDesc->pModel == nullptr)
            return;

        // 1) 모델 스페이스에서만 스케일 (보통 1,1,1)
        Vec3 vScaleModel = Vec3::sReplicate(1.0f);

        Vec3 center;
        float normalizeScale = 1.0f;

        Array<Vec3> points = ConvertToHullPoints(
            pConvexDesc->pModel,
            0,
            vScaleModel,
            center,
            normalizeScale
        );

        if (points.size() < 4)
            return;

        // 2) Hull 생성 (항상 "적당한 크기" 범위 내)
        Ref<ConvexHullShapeSettings> convexSettings = new ConvexHullShapeSettings(points);

        // (옵션) 허용 오차 살짝 풀기
         convexSettings->mHullTolerance = 0.5f;

        ShapeSettings::ShapeResult result = convexSettings->Create();
        if (result.HasError())
        {
            // 디버그 로그
            // OutputDebugStringA("Convex hull failed even after normalize...\n");
            return;
        }

        RefConst<Shape> hullShape = result.Get();

        // 3) Transform 스케일 + 정규화 보정까지 합친 실제 스케일
        Vec3 vScaleWorld(
            pConvexDesc->pTransform->Get_Scaled().x,
            pConvexDesc->pTransform->Get_Scaled().y,
            pConvexDesc->pTransform->Get_Scaled().z
        );

        // normalizeScale 로 줄여서 만들었으니까, 실제로는 역수로 다시 키워야 함
        Vec3 finalScale = vScaleWorld / normalizeScale; // (1 / normalizeScale) * worldScale

        RefConst<Shape> scaledHull = hullShape;
        if (finalScale != Vec3::sReplicate(1.0f))
            scaledHull = new ScaledShape(hullShape, finalScale);

        // 4) center 도 정규화 전에 기준이었으니까 다시 되돌려야 함
        Vec3 shapeOffset =
            LoadVec3(pConvexDesc->vShapeOffset)
            + center * vScaleWorld; // center는 원래 모델 스페이스 기준이라 worldScale만 곱하면 됨

        Quat shapeRot = LoadQuat(pConvexDesc->vShapeRotation);

        pShape = new RotatedTranslatedShape(
            shapeOffset,
            shapeRot,
            scaledHull
        );
        break;
    }
    case SHAPE::CYLINDER:
    {
        BODY_CYLINDERSHAPE_DESC* pCylinderDesc = static_cast<BODY_CYLINDERSHAPE_DESC*>(pDesc);
        pShape = new CylinderShape(pCylinderDesc->fHeight * 0.5f, pCylinderDesc->fRadius);
        pShape = new RotatedTranslatedShape(LoadVec3(pCylinderDesc->vShapeOffset), LoadQuat(pCylinderDesc->vShapeRotation), pShape);
        break;
    }
    default:
        break;
    }
}

const JPH::Array<Vec3> CBody::ConvertToArrayVec3(CModel* pModel)
{
    JPH::Array<Vec3> Vertices;

    vector<_float3> ModelVertices = pModel->Get_VerticesPos(0);

    for (size_t i = 0; i < ModelVertices.size(); ++i)
        Vertices.push_back(LoadVec3(ModelVertices[i]));

    return Vertices;
}

const JPH::Array<Vec3> CBody::ConvertToArrayVec3(CModel* pModel, _uint iMeshIndex, const Vec3& vScale)
{
    JPH::Array<Vec3> Vertices;

    vector<_float3> ModelVertices = pModel->Get_VerticesPos(iMeshIndex);

    Vertices.reserve(ModelVertices.size());

    for (size_t i = 0; i < ModelVertices.size(); ++i)
    {
        _float3 v = ModelVertices[i];

        // Transform 스케일 적용 (ConvexHull은 ScaledShape 써도 되지만
        // 여기서 그냥 정점에 직접 반영하는 방식)
        v.x *= vScale.GetX();
        v.y *= vScale.GetY();
        v.z *= vScale.GetZ();

        Vertices.push_back(LoadVec3(v));
    }

    return Vertices;
}

JPH::Array<Vec3> CBody::ConvertToHullPoints(
    CModel* pModel,
    _uint iMeshIndex,
    const Vec3& vScaleModel, // 이제 Transform 스케일 말고 "모델 고정 스케일"만
    Vec3& outCenter,
    _float& outNormalizeScale) // 정규화 스케일도 같이 반환
{
    JPH::Array<Vec3> points;

    vector<_float3> modelVerts = pModel->Get_VerticesPos(iMeshIndex);
    if (modelVerts.empty())
    {
        outCenter = Vec3::sZero();
        outNormalizeScale = 1.0f;
        return points;
    }

    points.reserve(modelVerts.size());

    // 1) 스케일 적용 + AABB 계산
    Vec3 minV = Vec3(FLT_MAX, FLT_MAX, FLT_MAX);
    Vec3 maxV = Vec3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

    outCenter = Vec3::sZero();
    for (auto& v : modelVerts)
    {
        Vec3 p(
            v.x * vScaleModel.GetX(),
            v.y * vScaleModel.GetY(),
            v.z * vScaleModel.GetZ()
        );

        if (!IsFiniteVec3(p))
            continue;

        outCenter += p;

        minV = Vec3::sMin(minV, p);
        maxV = Vec3::sMax(maxV, p);

        points.push_back(p);
    }

    if (points.empty())
    {
        outCenter = Vec3::sZero();
        outNormalizeScale = 1.0f;
        return points;
    }

    outCenter /= (float)points.size();

    // 2) 크기 측정
    Vec3 extents = maxV - minV;
    float maxExtent = max(extents.GetX(), max(extents.GetY(), extents.GetZ()));

    // 3) 정규화 스케일 계산 (Hull이 너무 큰 공간에서 안 돌아가게)
    //   - 예: targetSize = 10.0f (대략 10m 정도 공간에 들어오게)
    const float targetSize = 10.0f;
    outNormalizeScale = 1.0f;
    if (maxExtent > targetSize)
        outNormalizeScale = targetSize / maxExtent; // ex) 1000 -> 0.01 로 줄임

    // 4) center 기준/정규화/중복점 제거
    const float weldEps = 0.0001f;
    JPH::Array<Vec3> cleaned;
    cleaned.reserve(points.size());

    for (auto& p : points)
    {
        // 중심 기준 이동
        p -= outCenter;
        // 정규화 스케일 적용
        p *= outNormalizeScale;

        bool duplicate = false;
        for (auto& c : cleaned)
        {
            if ((p - c).LengthSq() < weldEps * weldEps)
            {
                duplicate = true;
                break;
            }
        }

        if (!duplicate)
            cleaned.push_back(p);
    }

    return cleaned;
}

const JPH::Array<Float3> CBody::ConvertToArrayFloat3(CModel* pModel, _uint iIndex)
{
    JPH::Array<Float3> Vertices;

    vector<_float3> ModelVertices = pModel->Get_VerticesPos(iIndex);

    for (size_t i = 0; i < ModelVertices.size(); ++i)
        Vertices.push_back(Float3(ModelVertices[i].x, ModelVertices[i].y, ModelVertices[i].z));

    return Vertices;
}

const JPH::Array<IndexedTriangle> CBody::ConvertToArrayTri(CModel* pModel, _uint iIndex)
{
    JPH::Array<IndexedTriangle> Indices;

    vector<_uint> ModelIndices = pModel->Get_Indices(iIndex);

    for (size_t i = 0; i < ModelIndices.size(); i += 3)
        Indices.push_back(IndexedTriangle(ModelIndices[i], ModelIndices[i + 1], ModelIndices[i + 2]));

    return Indices;
}

void CBody::Make_MeshShape(BODY_MESHSHAPE_DESC* pDesc)
{
    _uint iNumMesh = pDesc->pModel->Get_NumMeshes();

    //for (_uint i = 0; i < iNumMesh; ++i)
    //{
    //    RefConst<Shape> BodyShape;

    //    MeshShapeSettings MeshSetting{};
    //    MeshSetting = MeshShapeSettings(ConvertToArrayFloat3(pDesc->pModel, i), ConvertToArrayTri(pDesc->pModel, i));
    //    BodyShape = MeshSetting.Create().Get();

    //    BodyCreationSettings bodySetting(
    //        BodyShape,
    //        Vec3(pDesc->vPos.x, pDesc->vPos.y, pDesc->vPos.z),
    //        Quat(pDesc->vQuat.x, pDesc->vQuat.y, pDesc->vQuat.z, pDesc->vQuat.w),
    //        pDesc->eMotion,
    //        ObjectLayer(pDesc->iObjectLayer)
    //    );

    //    bodySetting.mOverrideMassProperties = EOverrideMassProperties::MassAndInertiaProvided;
    //    MassProperties mp;
    //    mp.mMass = 50.0f; // 예: 50kg, 상황에 맞게 조정

    //    bodySetting.mMassPropertiesOverride = mp;

    //    m_pBody = m_pGameInstance->CreateAndAdd_Body(bodySetting, &m_pBodyInterface);
    //    m_BodyID = m_pBody->GetID();
    //}

    iNumMesh = pDesc->pModel->Get_NumMeshes();

    Vec3 vScale = Vec3(pDesc->pTransform->Get_Scaled().x, pDesc->pTransform->Get_Scaled().y, pDesc->pTransform->Get_Scaled().z); // Transform의 스케일

    for (_uint i = 0; i < iNumMesh; ++i)
    {
        // 1) 원본 메쉬로 Shape 생성
        MeshShapeSettings meshSettings{
            ConvertToArrayFloat3(pDesc->pModel, i),
            ConvertToArrayTri(pDesc->pModel, i)
        };
        RefConst<Shape> baseShape = meshSettings.Create().Get();

        // 2) 스케일 적용 (ScaledShape)
        RefConst<Shape> scaledShape = new ScaledShape(baseShape, vScale);

        // (선택) 서브메쉬에 로컬 오프셋/회전이 있다면 RotatedTranslatedShape로 한 번 더 감쌈
        // RMat44 local = ...; // 모델 내부에서 i번째 메쉬의 로컬 변환이 있다면
        // scaledShape = new RotatedTranslatedShape(local.GetTranslation(), Quat::sFromMat44(local.GetRotation()), scaledShape);

        BodyCreationSettings bodySetting(
            scaledShape,
            Vec3(pDesc->vPos.x, pDesc->vPos.y, pDesc->vPos.z),
            Quat(pDesc->vQuat.x, pDesc->vQuat.y, pDesc->vQuat.z, pDesc->vQuat.w),
            pDesc->eMotion,
            ObjectLayer(pDesc->iObjectLayer)
        );

        // Dynamic으로 삼각형 메쉬를 쓰는 건 비추입니다. (가능하더라도 안정성이 떨어짐)
        // 꼭 써야 한다면 질량/관성 재정의
        bodySetting.mOverrideMassProperties = EOverrideMassProperties::MassAndInertiaProvided;
        MassProperties mp;
        mp.mMass = 50.0f;
        // 관성 텐서는 상황에 맞게 설정해야 합니다. (삼각형 메쉬는 실제 부피가 없어 비현실적일 수 있음)
        bodySetting.mMassPropertiesOverride = mp;
        bodySetting.mUserData = static_cast<uint64>(reinterpret_cast<uintptr_t>(pDesc->pCollisionDesc));

        Body* body = m_pGameInstance->CreateAndAdd_Body(bodySetting, &m_pBodyInterface);
        if (body)
            m_MeshBodyIDs.push_back(body->GetID());
    }
}

CBody* CBody::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CBody* pInstance = new CBody(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CBody"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CBody::Clone(void* pArg)
{
    CBody* pInstance = new CBody(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CBody"));
        Safe_Release(pInstance);
    }

    return pInstance;
}


void CBody::Free()
{
    if (!m_BodyID.IsInvalid() && m_pBodyInterface)
    {
        if (!m_BodyID.IsInvalid())
        {
            m_pGameInstance->Destroy_Body(m_BodyID);
            m_BodyID = BodyID();
            m_pBody = nullptr;
        }

        for (BodyID id : m_MeshBodyIDs)
        {
            if (!id.IsInvalid())
                m_pGameInstance->Destroy_Body(id);
        }
        m_MeshBodyIDs.clear();
    }
    __super::Free();

}
