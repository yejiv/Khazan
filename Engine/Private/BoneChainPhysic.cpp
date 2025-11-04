#include "BoneChainPhysic.h"
#include "Model.h"
#include "Transform.h"
#include <algorithm> // std::clamp, std::max
#include <atomic>    // GroupID 발급

using namespace DirectX;
using namespace JPH;

static inline TwoBodyConstraint* CreateConstraintLocked(
    PhysicsSystem* pPhys,
    const TwoBodyConstraintSettings& settings,
    BodyID a,
    BodyID b)
{
    const BodyLockInterface& bli_const = pPhys->GetBodyLockInterface();
    BodyLockInterface& bli = const_cast<BodyLockInterface&>(bli_const);

    BodyID ids[2] = { a, b };
    BodyLockMultiWrite lock(bli, ids, 2);

    Body* pA = lock.GetBody(0);
    Body* pB = lock.GetBody(1);
    if (!pA || !pB) return nullptr;

    return settings.Create(*pA, *pB);
}

static inline Vec3 WorldToBodyLocalPoint(const BodyInterface* bi, BodyID id, const Vec3& p_world)
{
    RMat44 com = bi->GetCenterOfMassTransform(id);
    RMat44 inv = com.Inversed();
    return inv * p_world;
}
static inline Vec3 WorldToBodyLocalDir(const BodyInterface* bi, BodyID id, const Vec3& v_world)
{
    RMat44 com = bi->GetCenterOfMassTransform(id);
    Mat44 rot = com.GetRotation().Inversed();
    return rot * v_world;
}

CBoneChainPhysic::CBoneChainPhysic(PhysicsSystem* pPhys, BodyInterface* pBI)
    : m_pPhysics{ pPhys }
    , m_pBI{ pBI }
    , m_tRootProxy{ BodyID() }
{
}

CBoneChainPhysic::~CBoneChainPhysic() {}

HRESULT CBoneChainPhysic::Initialize(CModel* pModel, const BCP_BuildDesc& tBuild, const BCP_RuntimeParams& tParams,
    _uint iRootObjectLayer, _uint iBoneChainObjectLayer, CTransform** pTransform)
{
    DestroyAll();
    m_pOwnerTransform = *pTransform;
    m_tParams = tParams;
    m_ePreset = tBuild.ePreset;
    ConfigureByPreset(m_ePreset);

    m_iRootObjectLayer = iRootObjectLayer;
    m_iBoneChainObjectLayer = iBoneChainObjectLayer;


    uint32_t total_subgroups = 1; // root proxy
    for (const auto& chain : tBuild.vBoneChains)
        if (chain.size() >= 2) total_subgroups += static_cast<uint32_t>(chain.size() - 1);

    m_pGroupFilter = new GroupFilterTable(total_subgroups);
    static std::atomic<uint32_t> sNextGroupID{ 1 };
    m_GroupID = sNextGroupID++;
    m_NextSubGroup = 0; // 0 = root

    // 1. 루트 프록시(키네마틱)
    const _int iHeadBone = pModel->Get_BoneIndex("Hair_BoneRoot");
    m_iHeadBone = iHeadBone;
    m_tRootProxy = CreateRootKinematicProxy(pModel, iHeadBone, m_fRootProxyRadius, m_fRootProxyHalf);

    // 2. 체인
    for (const auto& vChain : tBuild.vBoneChains)
        BuildChain(pModel, vChain, tBuild.fCapsuleRadius);

    for (auto& c : m_vChains)
    {
        if (!c.vSubGroup.empty())
            m_pGroupFilter->DisableCollision(0, c.vSubGroup.front());
        for (size_t i = 1; i < c.vSubGroup.size(); ++i)
            m_pGroupFilter->DisableCollision(c.vSubGroup[i - 1], c.vSubGroup[i]);
    }

    TeleportSnapToSkeleton(pModel);
    return S_OK;
}

void CBoneChainPhysic::SyncRootProxy_PrePhysics(CModel* pModel, _float fTimeDelta)
{
    if (!m_isEnabled || m_tRootProxy.IsInvalid() || m_iHeadBone < 0)
        return;

    m_lastDT = (fTimeDelta > 1e-6f ? fTimeDelta : m_lastDT);

    const _matrix ownerW = m_pOwnerTransform->Get_WorldMatrix();
    const _float3 headPos = GetBoneWorldPos(pModel, m_iHeadBone, ownerW);
    const _float4 headRot = GetBoneWorldRot(pModel, m_iHeadBone, ownerW);

    Vec3 p = ToJ(headPos);
    Quat q = ToJQ(headRot);

    // 1) 루트 위치/회전 동기화 (회전 Identity 금지)
    m_pBI->SetPositionAndRotation(m_tRootProxy, p, q, EActivation::DontActivate);

    // 2) 루트 속도 추정
    Vec3 linV = Vec3::sZero(), angV = Vec3::sZero();
    if (m_hasPrevRoot && m_lastDT > 0.f)
    {
        linV = (p - m_prevRootPos) / m_lastDT;

        Quat dq = q * m_prevRootRot.Conjugated();
        Vec3 axis; float angle;
        dq.GetAxisAngle(axis, angle);
        if (angle > JPH_PI) angle -= JPH_PI * 2.0f;
        angV = axis * (angle / m_lastDT);
    }
    m_hasPrevRoot = true;
    m_prevRootPos = p;
    m_prevRootRot = q;

    // 3) 루트에 속도 반영(연속화)
    m_pBI->SetLinearAndAngularVelocity(m_tRootProxy, linV, angV);

    // 4) 캐릭터 속도 기반 감쇠/중력 스케일
    const float fCharSpeed = linV.Length();
    const float k = std::clamp(fCharSpeed / 0.5f, 0.f, 1.f); // 0~0.5 m/s → 0~1

    auto myLerp = [](float a, float b, float t) { return a + (b - a) * t; };
    const float linDamp = myLerp(0.4f, 0.2f, k); // 정지↑ 이동↓
    const float angDamp = myLerp(1.0f, 0.8f, k);
    const float gravitySc = myLerp(0.4f, 1.0f, k);

    for (auto& Chain : m_vChains)
    {
        for (auto id : Chain.vBodies)
        {
            // 필요 시 감쇠도 프레임별 조정
            // m_pBI->SetLinearDamping(id, linDamp);
            // m_pBI->SetAngularDamping(id, angDamp);
            m_pBI->SetGravityFactor(id, gravitySc);
        }
    }

    // 5) 정지면 하드 안정화(옵션)
    const float sleepLin = 0.02f; // m/s
    const float sleepAng = 0.10f; // rad/s
    if (fCharSpeed < 0.02f && linV.Length() < sleepLin && angV.Length() < sleepAng)
    {
        for (auto& Chain : m_vChains)
            for (auto id : Chain.vBodies)
            {
                m_pBI->SetLinearAndAngularVelocity(id, Vec3::sZero(), Vec3::sZero());
                m_pBI->DeactivateBody(id);
            }
    }
}

void CBoneChainPhysic::ApplyBodiesToBones_PostPhysics(CModel* pModel, _float /*fTimeDelta*/)
{
    if (!m_isEnabled) return;

    for (auto& Chain : m_vChains)
    {
        const int N = (int)Chain.vBoneIdx.size();
        if (N < 2) continue;

        std::vector<Vec3> vPos(N);
        vPos[0] = ToJ(GetBoneWorldPos(pModel, Chain.vBoneIdx[0], m_pOwnerTransform->Get_WorldMatrix()));

        for (int i = 0; i < N - 1; ++i)
        {
            const RMat44 m = m_pBI->GetCenterOfMassTransform(Chain.vBodies[i]);
            vPos[i + 1] = m.GetTranslation();
        }

        for (int i = 0; i < N - 1; ++i)
        {
            const int parentBone = Chain.vBoneIdx[i];
            const int childBone = Chain.vBoneIdx[i + 1];
            SetChildBoneBySegment(pModel, parentBone, childBone, vPos[i], vPos[i + 1]);
        }
    }

    pModel->Update_BoneCombinedMatrices();
}

void CBoneChainPhysic::TeleportSnapToSkeleton(CModel* pModel)
{
    for (auto& c : m_vChains)
    {
        const _int N = (_int)c.vBoneIdx.size();
        for (int i = 0; i < N - 1; ++i)
        {
            const Vec3 a = ToJ(GetBoneWorldPos(pModel, c.vBoneIdx[i], m_pOwnerTransform->Get_WorldMatrix()));
            const Vec3 b = ToJ(GetBoneWorldPos(pModel, c.vBoneIdx[i + 1], m_pOwnerTransform->Get_WorldMatrix()));
            const Vec3 mid = (a + b) * 0.5f;
            const Vec3 z = (b - a).NormalizedOr(Vec3::sAxisZ());
            const Quat q = Quat::sFromTo(Vec3::sAxisZ(), z);

            m_pBI->SetPositionAndRotation(c.vBodies[i], mid, q, EActivation::DontActivate);
            m_pBI->SetLinearAndAngularVelocity(c.vBodies[i], Vec3::sZero(), Vec3::sZero());
        }
    }
    SyncRootProxy_PrePhysics(pModel, 1.f / 60.f);
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
    const _float3 vHead = GetBoneWorldPos(pModel, iHeadBone, m_pOwnerTransform->Get_WorldMatrix());
    auto* pShape = new CapsuleShape(fHalfHeight, fRadius);

    BodyCreationSettings cs(
        pShape, ToJ(vHead), Quat::sIdentity(),
        EMotionType::Kinematic, (ObjectLayer)m_iRootObjectLayer);
    cs.mAllowSleeping = false;
    cs.mFriction = m_tParams.fFriction;
    cs.mRestitution = m_tParams.fRestitution;

    // ★ 루트 SubGroup = 0
    const uint32_t sg_root = m_NextSubGroup++; // 0
    cs.mCollisionGroup = CollisionGroup(m_pGroupFilter, m_GroupID, sg_root);

    return m_pBI->CreateAndAddBody(cs, EActivation::DontActivate);
}

void CBoneChainPhysic::BuildChain(CModel* pModel, const std::vector<int>& vBoneIdx, _float fRadius)
{
    if (vBoneIdx.size() < 2) return;

    BCP_Chain tChain;
    tChain.vBoneIdx = vBoneIdx;
    tChain.fRadius = max(fRadius, 1e-4f);

    const int N = (int)vBoneIdx.size();

    // 월드 좌표 모으기
    std::vector<Vec3> vWorldPos(N);
    _matrix ownerW = m_pOwnerTransform ? m_pOwnerTransform->Get_WorldMatrix() : XMMatrixIdentity();
    for (int i = 0; i < N; ++i)
        vWorldPos[i] = ToJ(GetBoneWorldPos(pModel, vBoneIdx[i], ownerW));

    auto isZero = [](const Vec3& v) { return v.GetX() == 0 && v.GetY() == 0 && v.GetZ() == 0; };
    bool all_zero = true;
    for (int i = 0; i < N; ++i) if (!isZero(vWorldPos[i])) { all_zero = false; break; }
    if (all_zero) return;

    // 최소 길이 보장
    constexpr float kMinLen = 1e-3f;  // 1mm
    constexpr float kMinHalf = 5e-3f;  // 5mm

    tChain.vBodies.resize(N - 1);
    tChain.vSubGroup.resize(N - 1);

    for (int i = 0; i < N - 1; ++i)
    {
        Vec3 a = vWorldPos[i];
        Vec3 b = vWorldPos[i + 1];

        Vec3  dir = b - a;
        float len = dir.Length();
        if (len < kMinLen)
        {
            float guess = 0.02f;
            if (const _float4x4* pLoc = pModel->Get_LocalBoneMatrix(vBoneIdx[i + 1])) {
                _vector s, q, t; XMMatrixDecompose(&s, &q, &t, XMLoadFloat4x4(pLoc));
                guess = max(guess, XMVectorGetX(XMVector3Length(t)));
            }
            b = a + Vec3(0, guess, 0);
            dir = b - a; len = dir.Length();
        }

        Vec3  z = dir / len;
        Quat  q = Quat::sFromTo(Vec3::sAxisZ(), z);
        Vec3  mid = (a + b) * 0.5f;
        float halfHeight = max(kMinHalf, 0.5f * len);

        auto* pShape = new CapsuleShape(halfHeight, tChain.fRadius);

        BodyCreationSettings cs(pShape, mid, q, EMotionType::Dynamic, (ObjectLayer)m_iBoneChainObjectLayer);
        cs.mMotionQuality = (i >= N - 3 ? EMotionQuality::LinearCast : EMotionQuality::Discrete);
        cs.mOverrideMassProperties = EOverrideMassProperties::CalculateInertia;

        const float t = (N - 2 > 0) ? float(i) / float(N - 2) : 0.f;
        const float fMass = m_tParams.fMassRoot + (m_tParams.fMassTip - m_tParams.fMassRoot) * t;
        cs.mMassPropertiesOverride.mMass = max(0.001f, fMass);

        cs.mAllowSleeping = true;
        cs.mLinearDamping = max(0.3f, m_tParams.fLinearDamping);
        cs.mAngularDamping = max(0.8f, m_tParams.fAngularDamping);
        cs.mRestitution = 0.0f;
        cs.mFriction = 0.05f;
        cs.mGravityFactor = 1.0f;

 
        const uint32_t sg = m_NextSubGroup++;
        tChain.vSubGroup[i] = sg;
        cs.mCollisionGroup = CollisionGroup(m_pGroupFilter, m_GroupID, sg);

        BodyID id = m_pBI->CreateAndAddBody(cs, EActivation::Activate);
        tChain.vBodies[i] = id;
    }

    // 루트(프록시) ↔ 첫 세그먼트 : SwingTwist (동일 앵커)
    {
        const BodyID a = m_tRootProxy;
        const BodyID b = tChain.vBodies[0];

        SwingTwistConstraintSettings sts;
        const Vec3 zW = Vec3::sAxisZ();
        const Vec3 xW = Vec3::sAxisX();

        sts.mPosition1 = WorldToBodyLocalPoint(m_pBI, a, vWorldPos[0]);
        sts.mTwistAxis1 = WorldToBodyLocalDir(m_pBI, a, zW);
        sts.mPlaneAxis1 = WorldToBodyLocalDir(m_pBI, a, xW);

        sts.mPosition2 = WorldToBodyLocalPoint(m_pBI, b, vWorldPos[0]); // 동일 앵커
        sts.mTwistAxis2 = WorldToBodyLocalDir(m_pBI, b, zW);
        sts.mPlaneAxis2 = WorldToBodyLocalDir(m_pBI, b, xW);

        sts.mNormalHalfConeAngle = DegreesToRadians(m_tParams.fRootConeDeg);
        sts.mTwistMinAngle = DegreesToRadians(-m_tParams.fRootTwistDeg);
        sts.mTwistMaxAngle = DegreesToRadians(m_tParams.fRootTwistDeg);

        if (auto* sc = CreateConstraintLocked(m_pPhysics, sts, a, b)) {
            m_pPhysics->AddConstraint(sc);
            tChain.vCons.push_back(sc);
        }
    }

    // 인접 세그먼트 : SwingTwist(동일 앵커)  / Distance 제거
    for (int i = 1; i < N - 1; ++i)
    {
        const BodyID a = tChain.vBodies[i - 1];
        const BodyID b = tChain.vBodies[i];

        SwingTwistConstraintSettings sts;
        const Vec3 zW = Vec3::sAxisZ();
        const Vec3 xW = Vec3::sAxisX();

        sts.mPosition1 = WorldToBodyLocalPoint(m_pBI, a, vWorldPos[i]);
        sts.mTwistAxis1 = WorldToBodyLocalDir(m_pBI, a, zW);
        sts.mPlaneAxis1 = WorldToBodyLocalDir(m_pBI, a, xW);

        sts.mPosition2 = WorldToBodyLocalPoint(m_pBI, b, vWorldPos[i]); // 동일 앵커
        sts.mTwistAxis2 = WorldToBodyLocalDir(m_pBI, b, zW);
        sts.mPlaneAxis2 = WorldToBodyLocalDir(m_pBI, b, xW);

        sts.mNormalHalfConeAngle = DegreesToRadians(m_tParams.fMidConeDeg);
        sts.mTwistMinAngle = DegreesToRadians(-m_tParams.fMidTwistDeg);
        sts.mTwistMaxAngle = DegreesToRadians(m_tParams.fMidTwistDeg);

        if (auto* sc = CreateConstraintLocked(m_pPhysics, sts, a, b)) {
            m_pPhysics->AddConstraint(sc);
            tChain.vCons.push_back(sc);
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

    m_pGroupFilter = nullptr;
    m_GroupID = 0;
    m_NextSubGroup = 0;

    m_hasPrevRoot = false;
}

Vec3 CBoneChainPhysic::ToJ(const _float3& v) { return Vec3(v.x, v.y, v.z); }
Quat CBoneChainPhysic::ToJQ(const _float4& q) { return Quat(q.x, q.y, q.z, q.w); }

_float3 CBoneChainPhysic::GetBoneWorldPos(CModel* pModel, _int iBoneIdx, _matrix OwnerMatrix)
{
    _float4x4* pMatrix = pModel->Get_BoneMatrix(iBoneIdx);
    if (pMatrix == nullptr) return _float3();

    _matrix M = XMLoadFloat4x4(pMatrix);
    _matrix W = OwnerMatrix * M;
    return _float3(W.r[3].m128_f32[0], W.r[3].m128_f32[1], W.r[3].m128_f32[2]);
}

_float4 CBoneChainPhysic::GetBoneWorldRot(CModel* pModel, _int iBoneIdx, _matrix OwnerMatrix)
{
    _float4x4* pMatrix = pModel->Get_BoneMatrix(iBoneIdx);
    if (pMatrix == nullptr) return _float4(0.f, 1.f, 0.f, 0.f);

    _matrix M = XMLoadFloat4x4(pMatrix);
    _matrix W = OwnerMatrix * M;

    _vector s, q, t;
    XMMatrixDecompose(&s, &q, &t, W);

    _float4 rot;
    XMStoreFloat4(&rot, q);
    return rot;
}

void CBoneChainPhysic::SetChildBoneBySegment(CModel* pModel, _int iParentBone, _int iChildBone,
    const Vec3& vFrom, const Vec3& vTo)
{
    Vec3 dir = vTo - vFrom;
    if (dir.LengthSq() < 1e-12f) return;
    dir = dir.Normalized();

    const _float4x4* pParentCombined = pModel->Get_BoneMatrix(iParentBone);
    if (!pParentCombined) return;

    _matrix ownerW = m_pOwnerTransform ? m_pOwnerTransform->Get_WorldMatrix() : XMMatrixIdentity();
    _matrix parentW = ownerW * XMLoadFloat4x4(pParentCombined);

    _vector sP, qParentW, tP;
    XMMatrixDecompose(&sP, &qParentW, &tP, parentW);
    Quat parentWQ(XMVectorGetX(qParentW), XMVectorGetY(qParentW), XMVectorGetZ(qParentW), XMVectorGetW(qParentW));

    const _float4x4* pChildLocal = pModel->Get_LocalBoneMatrix(iChildBone);
    if (!pChildLocal) return;

    _vector sC, qChildBindLocal, tC;
    XMMatrixDecompose(&sC, &qChildBindLocal, &tC, XMLoadFloat4x4(pChildLocal));
    Quat childBindLocal(XMVectorGetX(qChildBindLocal), XMVectorGetY(qChildBindLocal),
        XMVectorGetZ(qChildBindLocal), XMVectorGetW(qChildBindLocal));

    const Vec3 bindAxisLocal = Vec3::sAxisZ();
    const Vec3 parentAxisWorld = parentWQ * bindAxisLocal;
    const Quat qWorldFromTo = Quat::sFromTo(parentAxisWorld, dir);

    const Quat qChildWorld = qWorldFromTo * parentWQ * childBindLocal;
    const Quat qChildLocal = parentWQ.Conjugated() * qChildWorld;

    _float4 outLocal(qChildLocal.GetX(), qChildLocal.GetY(), qChildLocal.GetZ(), qChildLocal.GetW());
    pModel->Set_BoneLocalRotation(iChildBone, XMLoadFloat4(&outLocal));
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

CBoneChainPhysic* CBoneChainPhysic::Create(PhysicsSystem* pPhys, BodyInterface* pBI, CModel* pModel,
    const BCP_BuildDesc& tBuild, const BCP_RuntimeParams& tParams,
    _uint iRootObjectLayer, _uint iBoneChainObjectLayer, CTransform** pTransform)
{
    CBoneChainPhysic* pInstance = new CBoneChainPhysic(pPhys, pBI);
    if (FAILED(pInstance->Initialize(pModel, tBuild, tParams, iRootObjectLayer, iBoneChainObjectLayer, pTransform)))
    {
        MSG_BOX(TEXT("Failed to Created : CBoneChainPhysic"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CBoneChainPhysic::Free()
{
    __super::Free();
    DestroyAll();
}