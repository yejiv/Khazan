#include "BoneChainPhysic.h"
#include "Model.h"

inline JPH::TwoBodyConstraint* CreateConstraintLocked(
    JPH::PhysicsSystem* pPhys,
    const JPH::TwoBodyConstraintSettings& settings,
    JPH::BodyID a,
    JPH::BodyID b)
{
    // GetBodyLockInterface()는 const ref를 돌려줌
    // MultiWrite는 non-const ref 요구 → const_cast로 맞춰줌
    const JPH::BodyLockInterface& bli_const = pPhys->GetBodyLockInterface();
    JPH::BodyLockInterface& bli = const_cast<JPH::BodyLockInterface&>(bli_const);

    JPH::BodyID ids[2] = { a, b };
    JPH::BodyLockMultiWrite lock(bli, ids, 2);

    JPH::Body* pA = lock.GetBody(0);
    JPH::Body* pB = lock.GetBody(1);
    if (!pA || !pB) return nullptr;

    return settings.Create(*pA, *pB);
}

CBoneChainPhysic::CBoneChainPhysic(JPH::PhysicsSystem* pPhys, JPH::BodyInterface* pBI)
    :m_pPhysics{ pPhys }
    , m_pBI { pBI }
    , m_tRootProxy { BodyID() }

{
}

CBoneChainPhysic::~CBoneChainPhysic()
{
}

HRESULT CBoneChainPhysic::Initialize(CModel* pModel, const BCP_BuildDesc& tBuild, const BCP_RuntimeParams& tParams, _uint iRootObjectLayer, _uint iBoneChainObjectLayer)
{
    DestroyAll();

    m_tParams = tParams;
    m_ePreset = tBuild.ePreset;
    ConfigureByPreset(m_ePreset);

    m_iRootObjectLayer = iRootObjectLayer;
    m_iBoneChainObjectLayer = iBoneChainObjectLayer;

    // 1. 루트 프록시(키네마틱) - 보통 Head/Neck 본
    const _int iHeadBone = pModel->Get_BoneIndex("Hair_BoneRoot");
    m_iHeadBone = iHeadBone;
    m_tRootProxy = CreateRootKinematicProxy(pModel, iHeadBone, m_fRootProxyRadius, m_fRootProxyHalf);

    // 2. 체인
    for (const auto& vChain : tBuild.vBoneChains)
        BuildChain(pModel, vChain, tBuild.fCapsuleRadius);

    return S_OK;
}

void CBoneChainPhysic::SyncRootProxy_PrePhysics(CModel* pModel)
{
    if (!m_isEnabled || m_tRootProxy.IsInvalid() || m_iHeadBone < 0)
        return;

    const _float3 vHead = GetBoneWorldPos(pModel, m_iHeadBone);
    m_pBI->SetPositionAndRotation(m_tRootProxy, ToJ(vHead), JPH::Quat::sIdentity(), JPH::EActivation::DontActivate);
    m_pBI->SetLinearAndAngularVelocity(m_tRootProxy, JPH::Vec3::sZero(), JPH::Vec3::sZero());
}

void CBoneChainPhysic::ApplyBodiesToBones_PostPhysics(CModel* pModel)
{
    if (!m_isEnabled) return;

    for (auto& Chain : m_vChains)
    {
        const int N = (int)Chain.vBoneIdx.size();
        if (N < 2) continue;

        std::vector<Vec3> vPos(N);
        // root(본) 위치
        vPos[0] = ToJ(GetBoneWorldPos(pModel, Chain.vBoneIdx[0]));

        // 각 세그먼트 바디의 중심(= 부모-자식 중간)에 저장해둔 위치
        for (int i = 0; i < N - 1; ++i)
        {
            const RMat44 m = m_pBI->GetCenterOfMassTransform(Chain.vBodies[i]);
            vPos[i + 1] = m.GetTranslation();
        }

        // 부모→자식 방향으로 로컬 회전 갱신
        for (int i = 0; i < N - 1; ++i)
        {
            const int parentBone = Chain.vBoneIdx[i];
            const int childBone = Chain.vBoneIdx[i + 1];
            SetChildBoneBySegment(pModel, parentBone, childBone, vPos[i], vPos[i + 1]);
        }
    }

    // 모든 본 로컬 갱신 후, 한 번만 컴바인드 업데이트
    pModel->Update_BoneCombinedMatrices();
}

void CBoneChainPhysic::TeleportSnapToSkeleton(CModel* pModel)
{
    for (auto& c : m_vChains)
    {
        const _int N = (_int)c.vBoneIdx.size();
        for (int i = 0; i < N - 1; ++i)
        {
            const Vec3 a = ToJ(GetBoneWorldPos(pModel, c.vBoneIdx[i]));
            const Vec3 b = ToJ(GetBoneWorldPos(pModel, c.vBoneIdx[i + 1]));
            const Vec3 mid = (a + b) * 0.5f;
            const Vec3 z = (b - a).NormalizedOr(Vec3::sAxisZ());
            const Quat q = Quat::sFromTo(Vec3::sAxisZ(), z);

            m_pBI->SetPositionAndRotation(c.vBodies[i], mid, q, EActivation::DontActivate);
            m_pBI->SetLinearAndAngularVelocity(c.vBodies[i], Vec3::sZero(), Vec3::sZero());
        }
    }
    SyncRootProxy_PrePhysics(pModel);
}

void CBoneChainPhysic::SetEnabled(_bool isEnable)
{
    m_isEnabled = isEnable;
}

void CBoneChainPhysic::ApplyPreset(EBCP_Preset ePreset)
{
    m_ePreset = ePreset;
    ConfigureByPreset(ePreset);
}

BodyID CBoneChainPhysic::CreateRootKinematicProxy(CModel* pModel, _int iHeadBone, _float fRadius, _float fHalfHeight)
{
    const _float3 vHead = GetBoneWorldPos(pModel, iHeadBone);
    auto* pShape = new JPH::CapsuleShape(fHalfHeight, fRadius);
    JPH::BodyCreationSettings cs(
        pShape, ToJ(vHead), JPH::Quat::sIdentity(),
        JPH::EMotionType::Kinematic, (JPH::ObjectLayer)m_iRootObjectLayer);
    cs.mAllowSleeping = false;
    cs.mFriction = m_tParams.fFriction;
    cs.mRestitution = m_tParams.fRestitution;
    return m_pBI->CreateAndAddBody(cs, JPH::EActivation::DontActivate);
}

void CBoneChainPhysic::BuildChain(CModel* pModel, const vector<int>& vBoneIdx, _float fRadius)
{
    if (vBoneIdx.size() < 2) return;

    BCP_Chain tChain;
    tChain.vBoneIdx = vBoneIdx;
    tChain.fRadius = fRadius;

    const _int N = (_int)vBoneIdx.size();
    std::vector<Vec3> vWorldPos(N);
    for (int i = 0; i < N; ++i) vWorldPos[i] = ToJ(GetBoneWorldPos(pModel, vBoneIdx[i]));

    // 1) 세그먼트 바디 생성: [root-b1], [b1-b2], ...
    tChain.vBodies.resize(N - 1);
    for (int i = 0; i < N - 1; ++i)
    {
        Vec3 a = vWorldPos[i], b = vWorldPos[i + 1];
        Vec3 mid = (a + b) * 0.5f;
        Vec3 dir = (b - a); float len = dir.Length();
        Vec3 z = len > 1e-6f ? dir / len : Vec3::sAxisZ();
        Quat q = Quat::sFromTo(Vec3::sAxisZ(), z);

        float halfHeight = 0.5f * len;
        auto* pShape = new CapsuleShape(halfHeight, fRadius);

        BodyCreationSettings cs(pShape, mid, q, EMotionType::Dynamic, (ObjectLayer)m_iBoneChainObjectLayer);
        cs.mLinearDamping = m_tParams.fLinearDamping;
        cs.mAngularDamping = m_tParams.fAngularDamping;
        cs.mFriction = m_tParams.fFriction;
        cs.mRestitution = m_tParams.fRestitution;
        cs.mAllowSleeping = false;
        cs.mMotionQuality = (i == N - 2 ? EMotionQuality::LinearCast : EMotionQuality::Discrete);
        cs.mOverrideMassProperties = EOverrideMassProperties::CalculateInertia;

        const _float t = (N - 2 > 0) ? (_float)i / (_float)(N - 2) : 0.f;
        const _float fMass = m_tParams.fMassRoot + (m_tParams.fMassTip - m_tParams.fMassRoot) * t;
        cs.mMassPropertiesOverride.mMass = max(0.001f, fMass);

        BodyID id = m_pBI->CreateAndAddBody(cs, EActivation::Activate);
        tChain.vBodies[i] = id;
    }

    // 2) 루트 연결(프록시 ↔ 첫 세그먼트)
    {
        {
        // 위치 정렬(포인트)
            PointConstraintSettings pcs;
            pcs.mPoint1 = vWorldPos[0];
            pcs.mPoint2 = vWorldPos[0];

            if (auto* pc = CreateConstraintLocked(m_pPhysics, pcs, m_tRootProxy, tChain.vBodies[0]))
            {
                m_pPhysics->AddConstraint(pc);
                tChain.vCons.push_back(pc);
            }
        }

        // 스윙/트위스트 제한
        SwingTwistConstraintSettings sts;
        sts.mPosition1 = vWorldPos[0];
        sts.mTwistAxis1 = Vec3::sAxisZ();
        sts.mPlaneAxis1 = Vec3::sAxisX();
        sts.mPosition2 = vWorldPos[1];
        sts.mTwistAxis2 = Vec3::sAxisZ();
        sts.mPlaneAxis2 = Vec3::sAxisX();
        sts.mNormalHalfConeAngle = DegreesToRadians(m_tParams.fRootConeDeg);
        sts.mTwistMinAngle = DegreesToRadians(-m_tParams.fRootTwistDeg);
        sts.mTwistMaxAngle = DegreesToRadians(m_tParams.fRootTwistDeg);
        if (auto* sc = CreateConstraintLocked(m_pPhysics, sts, m_tRootProxy, tChain.vBodies[0]))
        {
            m_pPhysics->AddConstraint(sc);
            tChain.vCons.push_back(sc);
        }
    }

    // 3) 인접 세그먼트 연결
    for (int i = 1; i < N - 1; ++i)
    {
        // (a) 길이 보존
        {
            JPH::DistanceConstraintSettings dcs;
            dcs.mPoint1 = vWorldPos[i];
            dcs.mPoint2 = vWorldPos[i];
            const float segLen = (vWorldPos[i + 1] - vWorldPos[i]).Length();
            dcs.mMinDistance = segLen;
            dcs.mMaxDistance = segLen;

            if (auto* dc = CreateConstraintLocked(m_pPhysics, dcs, tChain.vBodies[i - 1], tChain.vBodies[i]))
            {
                m_pPhysics->AddConstraint(dc);
                tChain.vCons.push_back(dc);
            }
        }

        // (b) 각 제한 (그대로)
        {
            JPH::SwingTwistConstraintSettings sts;
            sts.mPosition1 = vWorldPos[i];
            sts.mTwistAxis1 = JPH::Vec3::sAxisZ();
            sts.mPlaneAxis1 = JPH::Vec3::sAxisX();
            sts.mPosition2 = vWorldPos[i + 1];
            sts.mTwistAxis2 = JPH::Vec3::sAxisZ();
            sts.mPlaneAxis2 = JPH::Vec3::sAxisX();
            sts.mNormalHalfConeAngle = JPH::DegreesToRadians(m_tParams.fMidConeDeg);
            sts.mTwistMinAngle = JPH::DegreesToRadians(-m_tParams.fMidTwistDeg);
            sts.mTwistMaxAngle = JPH::DegreesToRadians(m_tParams.fMidTwistDeg);
            if (auto* sc = CreateConstraintLocked(m_pPhysics, sts, tChain.vBodies[i - 1], tChain.vBodies[i]))
            {
                m_pPhysics->AddConstraint(sc);
                tChain.vCons.push_back(sc);
            }
        }
    }

    m_vChains.emplace_back(std::move(tChain));
}

void CBoneChainPhysic::DestroyAll()
{
    if (!m_pBI || !m_pPhysics) return;
    for (auto& c : m_vChains)
    {
        for (auto* pC : c.vCons) { m_pPhysics->RemoveConstraint(pC); delete pC; }
        for (auto id : c.vBodies) { m_pBI->RemoveBody(id); m_pBI->DestroyBody(id); }
    }
    m_vChains.clear();

    if (!m_tRootProxy.IsInvalid()) {
        m_pBI->RemoveBody(m_tRootProxy);
        m_pBI->DestroyBody(m_tRootProxy);
        m_tRootProxy = BodyID();
    }
}

JPH::Vec3 CBoneChainPhysic::ToJ(const _float3& v)
{
    return Vec3(v.x, v.y, v.z);
}

JPH::Quat CBoneChainPhysic::ToJQ(const _float4& q)
{
    return Quat(q.x, q.y, q.z, q.w);
}

_float3 CBoneChainPhysic::GetBoneWorldPos(CModel* pModel, _int iBoneIdx)
{
    _float4x4* pMatrix = pModel->Get_BoneMatrix(iBoneIdx);

    if (pMatrix == nullptr)
        return _float3();

    return _float3(pMatrix->_41, pMatrix->_42, pMatrix->_43);
}
_float4 CBoneChainPhysic::GetBoneWorldRot(CModel* pModel, _int iBoneIdx)
{
    _float4x4* pMatrix = pModel->Get_BoneMatrix(iBoneIdx);

    if (pMatrix == nullptr)
        return _float4(0.f, 1.f, 0.f, 0.f);

    _matrix Matrix = XMLoadFloat4x4(pMatrix);
    

    _vector vScale, vQuat, vTranslation;

    XMMatrixDecompose(&vScale, &vQuat, &vTranslation, Matrix);

    _float4 vRotation;
    XMStoreFloat4(&vRotation, vQuat);

    return vRotation;
}

void CBoneChainPhysic::SetChildBoneBySegment(CModel* pModel, _int iParentBone, _int iChildBone, const JPH::Vec3& vFrom, const JPH::Vec3& vTo)
{
    Vec3 dir = vTo - vFrom;
    if (dir.LengthSq() < 1e-12f) return;
    dir = dir.Normalized();

    // 부모 월드 쿼터니언
    _float4x4* pParentW = pModel->Get_BoneMatrix(iParentBone);
    if (!pParentW) return;

    _matrix mParentW = XMLoadFloat4x4(pParentW);
    _vector sP, qParentW, tP;
    XMMatrixDecompose(&sP, &qParentW, &tP, mParentW);
    Quat parentW(XMVectorGetX(qParentW), XMVectorGetY(qParentW), XMVectorGetZ(qParentW), XMVectorGetW(qParentW));

    // 자식 바인드 로컬(현재 로컬로 대체)
    _float4x4* pChildLocal = pModel->Get_LocalBoneMatrix(iChildBone);
    if (!pChildLocal) return;

    _matrix mChildLocal = XMLoadFloat4x4(pChildLocal);
    _vector sC, qChildBindLocal, tC;
    XMMatrixDecompose(&sC, &qChildBindLocal, &tC, mChildLocal);
    Quat childBindLocal(
        XMVectorGetX(qChildBindLocal),
        XMVectorGetY(qChildBindLocal),
        XMVectorGetZ(qChildBindLocal),
        XMVectorGetW(qChildBindLocal));

    // 부모-기준축(Z) → dir 로의 최소 회전
    const Vec3 bindAxisLocal = Vec3::sAxisZ();
    const Vec3 parentAxisWorld = parentW * bindAxisLocal;
    const Quat qWorldFromTo = Quat::sFromTo(parentAxisWorld, dir);

    // 자식 월드 회전 = 회전 보정 * 부모월드 * (자식 바인드 로컬)
    const Quat qChildWorld = qWorldFromTo * parentW * childBindLocal;

    // 로컬로 역산: local = inv(parentW) * world
    const Quat qChildLocal = parentW.Conjugated() * qChildWorld;

    // 모델 API 호출 (로컬 회전 세팅)
    _vector qLocal = XMVectorSet(qChildLocal.GetX(), qChildLocal.GetY(), qChildLocal.GetZ(), qChildLocal.GetW());
    pModel->Set_BoneLocalRotation(iChildBone, qLocal);
}

void CBoneChainPhysic::SetupConstraint_ChainSegment(JPH::BodyID a, JPH::BodyID b, const JPH::Vec3& pivotWorld, _bool isRootLink)
{
   
}

void CBoneChainPhysic::ConfigureByPreset(EBCP_Preset ePreset)
{
    switch (ePreset)
    {
    case BCP_PRESET_Hair:
        m_tParams.fLinearDamping = 0.20f;
        m_tParams.fAngularDamping = 0.40f;
        m_tParams.fFriction = 0.10f;
        m_tParams.fRestitution = 0.02f;
        m_tParams.fRootConeDeg = 35.f;
        m_tParams.fRootTwistDeg = 20.f;
        m_tParams.fMidConeDeg = 30.f;
        m_tParams.fMidTwistDeg = 15.f;
        m_tParams.fMassRoot = 0.08f;
        m_tParams.fMassTip = 0.02f;
        break;

    case BCP_PRESET_ClothStrip:
        // 옷자락 느낌: 콘을 좀 더 넓히고 선형 감쇠↑
        m_tParams.fLinearDamping = 0.25f;
        m_tParams.fAngularDamping = 0.45f;
        m_tParams.fFriction = 0.20f;
        m_tParams.fRestitution = 0.03f;
        m_tParams.fRootConeDeg = 45.f;
        m_tParams.fRootTwistDeg = 25.f;
        m_tParams.fMidConeDeg = 35.f;
        m_tParams.fMidTwistDeg = 20.f;
        m_tParams.fMassRoot = 0.10f;
        m_tParams.fMassTip = 0.03f;
        break;

    default:
        break;
    }
}

CBoneChainPhysic* CBoneChainPhysic::Create(JPH::PhysicsSystem* pPhys, JPH::BodyInterface* pBI, class CModel* pModel, const BCP_BuildDesc& tBuild, const BCP_RuntimeParams& tParams, _uint iRootObjectLayer, _uint iBoneChainObjectLayer)
{
    CBoneChainPhysic* pInstance = new CBoneChainPhysic(pPhys, pBI);

    if (FAILED(pInstance->Initialize(pModel, tBuild, tParams, iRootObjectLayer, iBoneChainObjectLayer)))
    {
        MSG_BOX(TEXT("Failed to Created : CModel"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBoneChainPhysic::Free()
{
    __super::Free();
    DestroyAll();
}
