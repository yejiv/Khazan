// =============================================================
// BoneTreePBD.cpp
// =============================================================
#include "BoneTreePBD.h"

#include <queue>
#include <algorithm>
#include <cmath>

// ====== 내부 유틸(파일 한정) ==================================
namespace
{
    inline float Saturate(float x) { return max(0.0f, min(1.0f, x)); }
    inline float Lerp(float a, float b, float t) { return a + (b - a) * t; }
    inline float XM_GetX(XMVECTOR v) { return XMVectorGetX(v); }
    inline float XM_Length(XMVECTOR v) { return XM_GetX(XMVector3Length(v)); }

    inline XMVECTOR ClampLength(XMVECTOR v, float maxLen)
    {
        XMVECTOR len = XMVector3Length(v);
        XMVECTOR mask = XMVectorGreater(len, XMVectorReplicate(maxLen));
        XMVECTOR scale = XMVectorDivide(XMVectorReplicate(maxLen), len);
        return XMVectorSelect(v, XMVectorMultiply(v, scale), mask);
    }

    static inline void DecomposeOwnerW(CTransform* xf, XMVECTOR& outPos, XMVECTOR& outQ)
    {
        _matrix W = xf ? xf->Get_WorldMatrix() : XMMatrixIdentity();
        XMVECTOR s, q, t;
        XMMatrixDecompose(&s, &q, &t, W);
        outPos = t;
        outQ = q;
    }
}

NS_BEGIN(Engine)

// --------------------------------------------------------------
// ctor / dtor
// --------------------------------------------------------------
CBoneTreePBD::CBoneTreePBD(CModel* pModel, CTransform* pXform)
    : m_pModel{ pModel }, m_pOwnerTransform{ pXform }
{
}

CBoneTreePBD* CBoneTreePBD::Create(CModel* pModel, CTransform* pOwnerXform)
{
    return new CBoneTreePBD(pModel, pOwnerXform);
}

void CBoneTreePBD::Free()
{
    __super::Free();
    m_vNodes.clear();
    m_vTrees.clear();
    m_vSpheres.clear();
    m_vRootCache.clear();
}

// --------------------------------------------------------------
// Public API
// --------------------------------------------------------------
void CBoneTreePBD::ClearColliders() { m_vSpheres.clear(); }
void CBoneTreePBD::AddSphereCollider(XMVECTOR centerWS, float r) { m_vSpheres.push_back({ centerWS, r }); }

// 루트만 주면 하위 서브트리를 자동 확장(자식 나열 API 필요)
HRESULT CBoneTreePBD::BuildForestFromRoots(const vector<int>& vRootBones)
{
    m_vTrees.clear();
    m_vNodes.clear();
    m_vRootCache.clear();

    // CModel에 다음 API가 필요합니다:
    // vector<int> CModel::Get_ChildrenIndices(int parentBoneIdx);
    // 없다면, 초기화 시 parent 배열을 받아서 선형 스캔으로 만들어도 충분함.

    for (int rootBone : vRootBones)
    {
        TREE T;
        T.iRootBone = rootBone;

        auto AddNode = [&](int iBone, uint16_t parent, uint16_t depth, float pathLen)->uint16_t
            {
                NODE N;
                N.iBoneIdx = iBone;
                N.uParent = parent;
                N.uDepth = depth;
                N.fPathLen = pathLen;
                N.fInvMass = (depth == 0) ? 0.0f : 1.0f; // 루트 고정
                N.vCurWS = GetBoneWorldPos(iBone);
                N.vPrevWS = N.vCurWS;

                uint16_t idx = (uint16_t)m_vNodes.size();
                m_vNodes.push_back(N);
                T.vNodeIndex.push_back(idx);
                return idx;
            };

        // BFS
        queue<pair<int, uint16_t>> Q; // (boneIdx, nodeIndex)
        uint16_t rootNode = AddNode(rootBone, 0xFFFF, 0, 0.0f);
        Q.push({ rootBone, rootNode });

        float segSum = 0.f; int segCnt = 0;

        while (!Q.empty())
        {
            auto [parentBone, parentNode] = Q.front(); Q.pop();
            XMVECTOR pP = GetBoneWorldPos(parentBone);

            vector<int> children = m_pModel->Get_ChildIndices(parentBone); // 구현 필요
            for (int childBone : children)
            {
                XMVECTOR pC = GetBoneWorldPos(childBone);
                float rest = XM_Length(XMVectorSubtract(pC, pP));
                rest = max(1e-3f, rest);

                uint16_t childNode = AddNode(childBone,
                    parentNode,
                    m_vNodes[parentNode].uDepth + 1,
                    m_vNodes[parentNode].fPathLen + rest);

                T.vEdges.push_back({ parentNode, childNode, rest });

                segSum += rest; segCnt++;
                Q.push({ childBone, childNode });
            }
        }

        T.fAvgSegLen = (segCnt > 0) ? (segSum / segCnt) : 0.02f;

        m_vTrees.push_back(move(T));
        m_vRootCache.push_back(RootCache{});
    }

    TeleportSnapToSkeleton();
    return S_OK;
}

void CBoneTreePBD::TeleportSnapToSkeleton()
{
    for (TREE& T : m_vTrees)
    {
        for (uint16_t n : T.vNodeIndex)
        {
            NODE& N = m_vNodes[n];
            XMVECTOR p = GetBoneWorldPos(N.iBoneIdx);
            N.vPrevWS = N.vCurWS = p;
        }
    }
    for (auto& rc : m_vRootCache) { rc.hasPrev = false; rc.pPrev = XMVectorZero(); }
    m_hasPrevOwner = false;
    m_prevOwnerPos = XMVectorZero();
    m_prevOwnerRotQ = XMQuaternionIdentity();
}

void CBoneTreePBD::PreUpdate(float dt, XMVECTOR vWindWS)
{
    if (!m_bEnabled) return;
    dt = max(1e-4f, std::min(1.f / 10.f, dt));

    // 3-1) Owner 현재 트랜스폼 분해
    XMVECTOR ownerPosNow, ownerRotQNow;
    DecomposeOwnerW(m_pOwnerTransform, ownerPosNow, ownerRotQNow);

    // 3-2) ΔT, ΔR 계산
    XMVECTOR dT = XMVectorZero();
    XMVECTOR dR = XMQuaternionIdentity();
    if (m_hasPrevOwner)
    {
        dT = XMVectorSubtract(ownerPosNow, m_prevOwnerPos);
        // ΔR = Q_now * Q_prev^{-1}
        dR = XMQuaternionMultiply(ownerRotQNow, XMQuaternionInverse(m_prevOwnerRotQ));
        dR = XMQuaternionNormalize(dR);
    }

    // 3-3) 이월 강도(가감 가능)
    XMVECTOR dT_gain = XMVectorScale(dT, m_AdvecPosGain);
    XMVECTOR dR_gain = XMQuaternionSlerp(XMQuaternionIdentity(), dR, m_AdvecRotGain);

    // 3-4) 회전 Pivot 선택: 머리 루트(혹은 각 트리 루트) 월드 좌표
    auto GetTreePivot = [&](const TREE& T)->XMVECTOR {
        if (T.vNodeIndex.empty()) return ownerPosNow;
        const NODE& rootN = m_vNodes[T.vNodeIndex.front()];
        return rootN.vCurWS; // 현재 루트 월드 위치
        };

    // 3-5) 모든 노드에 ΔT/ΔR 이월 (강체처럼 함께 이동/회전)
    if (m_hasPrevOwner)
    {
        for (TREE& T : m_vTrees)
        {
            XMVECTOR pivot = GetTreePivot(T);
            for (uint16_t ni : T.vNodeIndex)
            {
                NODE& N = m_vNodes[ni];

                // cur/prev 둘 다 같은 만큼 이동
                N.vCurWS = XMVectorAdd(N.vCurWS, dT_gain);
                N.vPrevWS = XMVectorAdd(N.vPrevWS, dT_gain);

                // pivot 기준 회전 적용: p' = pivot + ΔR * (p - pivot)
                auto rotate_about = [&](XMVECTOR p) {
                    XMVECTOR off = XMVectorSubtract(p, pivot);
                    XMVECTOR offR = XMVector3Rotate(off, dR_gain);
                    return XMVectorAdd(pivot, offR);
                    };
                N.vCurWS = rotate_about(N.vCurWS);
                N.vPrevWS = rotate_about(N.vPrevWS);
            }
        }
    }

    // 3-6) 캐시 갱신
    m_prevOwnerPos = ownerPosNow;
    m_prevOwnerRotQ = ownerRotQNow;
    m_hasPrevOwner = true;

    dt = max(1e-4f, min(1.f / 10.f, dt)); // 0.0001 ~ 0.1 안전범위

    // 공통 가속도
    XMVECTOR vGravity = XMVectorSet(0, m_Params.fGravity, 0, 0);

    for (size_t ti = 0; ti < m_vTrees.size(); ++ti)
    {
        TREE& T = m_vTrees[ti];
        RootCache& RC = m_vRootCache[ti];

        if (T.vNodeIndex.empty()) continue;
        uint16_t iRoot = T.vNodeIndex.front();
        NODE& R = m_vNodes[iRoot];

        // 루트 현재 위치/속도
        XMVECTOR pNow = GetBoneWorldPos(R.iBoneIdx);
        XMVECTOR rootDelta = XMVectorZero();
        if (RC.hasPrev)
            rootDelta = XMVectorSubtract(pNow, RC.pPrev);
        XMVECTOR vLin = XMVectorZero();

        R.vCurWS = pNow;
        R.vPrevWS = pNow;

        if (RC.hasPrev)
        {
            for (uint16_t n : T.vNodeIndex)
            {
                if (n == iRoot) continue;
                NODE& N = m_vNodes[n];
                N.vCurWS = XMVectorAdd(N.vCurWS, rootDelta);
                N.vPrevWS = XMVectorAdd(N.vPrevWS, rootDelta);
            }
        }

        if (RC.hasPrev) vLin = XMVectorDivide(XMVectorSubtract(pNow, RC.pPrev), XMVectorReplicate(dt));
        RC.pPrev = pNow; RC.hasPrev = true;

        float speed = XM_Length(vLin);
        float t = Saturate(speed / 0.5f);

        float dampingUsed = Lerp(m_Params.fDampStill, m_Params.fDampMove, t);
        float gravityScale = Lerp(0.5f, 1.0f, t); // 정지 시 덜 처지도록

        XMVECTOR aTerm = XMVectorAdd(XMVectorScale(vGravity, gravityScale * dt * dt),
            XMVectorScale(vWindWS, m_Params.fWindScale * dt * dt));
        XMVECTOR deltaRoot = XMVectorScale(vLin, dt * 0.35f);

        // 루트 고정
        R.vCurWS = R.vPrevWS = pNow;

        // 자식 예측
        for (uint16_t n : T.vNodeIndex)
        {
            if (n == iRoot) continue;
            NODE& N = m_vNodes[n];

            XMVECTOR v = XMVectorSubtract(N.vCurWS, N.vPrevWS);
            v = ClampLength(v, m_Params.fMaxSegSpeed * dt);

            N.vPrevWS = N.vCurWS;
            N.vCurWS = XMVectorAdd(N.vCurWS,
                XMVectorAdd(XMVectorScale(v, dampingUsed),
                    XMVectorAdd(aTerm, deltaRoot)));
        }

        // 저속 안정화(옵션)
        if (speed < 0.02f)
        {
            for (uint16_t n : T.vNodeIndex)
            {
                if (n == iRoot) continue;
                NODE& N = m_vNodes[n];
                XMVECTOR v = XMVectorSubtract(N.vCurWS, N.vPrevWS);
                if (XM_Length(v) < m_Params.fSleepLin * dt)
                    N.vCurWS = XMVectorLerp(N.vCurWS, N.vPrevWS, 0.2f);
            }
        }
    }
}

void CBoneTreePBD::Simulate(float /*dt*/)
{
    if (!m_bEnabled) return;

    const int iters = max(1, m_Params.iIterations);
    for (int it = 0; it < iters; ++it)
    {
        for (const TREE& T : m_vTrees)
        {
            if (T.vNodeIndex.empty()) continue;
            uint16_t rootNode = T.vNodeIndex.front();
            const NODE& Root = m_vNodes[rootNode];

            // 거리 제약(부모-자식)
            for (const EDGE& e : T.vEdges)
                ProjectDistance(m_vNodes[e.uParent], m_vNodes[e.uChild], e.fRestLen, m_Params.fDistanceStiffness);

            // 콘/테더/충돌
            for (const EDGE& e : T.vEdges)
            {
                NODE& P = m_vNodes[e.uParent];
                NODE& C = m_vNodes[e.uChild];

                float coneDeg = (C.uDepth == 1 ? m_Params.fRootConeDeg : m_Params.fMidConeDeg);
                XMVECTOR qParentWS = GetBoneWorldRotQ(P.iBoneIdx);

                ProjectCone(P, qParentWS, C, coneDeg);

                float tetherMax = C.fPathLen * m_Params.fTetherScale;
                ProjectTether(Root.vCurWS, C, tetherMax);

                ResolveSphereCollisions(C);
            }
        }
    }
}

void CBoneTreePBD::PostApply()
{
    if (!m_bEnabled) return;

    for (const TREE& T : m_vTrees)
    {
        for (const EDGE& e : T.vEdges)
        {
            const XMVECTOR a = m_vNodes[e.uParent].vCurWS;
            const XMVECTOR b = m_vNodes[e.uChild].vCurWS;
            ApplyBoneLook(m_vNodes[e.uParent].iBoneIdx, m_vNodes[e.uChild].iBoneIdx, a, b);
        }
    }
    m_pModel->Update_BoneCombinedMatrices();
}

// --------------------------------------------------------------
// Constraints
// --------------------------------------------------------------
void CBoneTreePBD::ProjectDistance(NODE& P, NODE& C, float restLen, float stiffness)
{
    XMVECTOR d = XMVectorSubtract(C.vCurWS, P.vCurWS);
    float    l = XM_GetX(XMVector3Length(d));
    if (l < 1e-8f) return;

    float wP = P.fInvMass, wC = C.fInvMass;
    float wSum = wP + wC;
    if (wSum <= 0.0f) return;

    float diff = (l - restLen) / l;
    float k = stiffness;

    XMVECTOR corr = XMVectorScale(d, diff * k);

    if (wP > 0.0f) P.vCurWS = XMVectorAdd(P.vCurWS, XMVectorScale(corr, -wP / wSum));
    if (wC > 0.0f) C.vCurWS = XMVectorAdd(C.vCurWS, XMVectorScale(corr, wC / wSum));
}

void CBoneTreePBD::ProjectCone(const NODE& P, XMVECTOR qParentWS, NODE& C, float coneDeg)
{
    // parent 기준으로 child-offset 방향이 cone 각을 넘지 않도록
    XMVECTOR zLocal = XMVectorSet(0, 0, 1, 0);
    XMVECTOR zAxisW = XMVector3Rotate(zLocal, qParentWS); // parent의 "forward"
    XMVECTOR dir = XMVector3Normalize(XMVectorSubtract(C.vCurWS, P.vCurWS));

    float cosMax = cosf(coneDeg * 3.14159265f / 180.0f);
    float dot = XM_GetX(XMVector3Dot(zAxisW, dir));
    if (dot >= cosMax) return; // 안 넘음

    // dir를 cone 경계에 투영
    XMVECTOR vPerp = XMVectorSubtract(dir, XMVectorScale(zAxisW, dot));
    float lenPerp = XM_GetX(XMVector3Length(vPerp));
    if (lenPerp < 1e-6f)
    {
        // 거의 역방향 등 -> 간단히 보간
        XMVECTOR boundary = XMVector3Normalize(XMVectorLerp(zAxisW, XMVectorNegate(zAxisW), 0.999f));
        float dist = XM_GetX(XMVector3Length(XMVectorSubtract(C.vCurWS, P.vCurWS)));
        C.vCurWS = XMVectorAdd(P.vCurWS, XMVectorScale(boundary, dist));
        return;
    }
    vPerp = XMVectorScale(vPerp, 1.0f / lenPerp);

    float sinMax = sqrtf(max(0.f, 1.f - cosMax * cosMax));
    XMVECTOR boundary = XMVectorAdd(XMVectorScale(zAxisW, cosMax), XMVectorScale(vPerp, sinMax));
    boundary = XMVector3Normalize(boundary);

    float dist = XM_GetX(XMVector3Length(XMVectorSubtract(C.vCurWS, P.vCurWS)));
    C.vCurWS = XMVectorAdd(P.vCurWS, XMVectorScale(boundary, dist));
}

void CBoneTreePBD::ProjectTether(XMVECTOR rootPos, NODE& C, float tetherMax)
{
    XMVECTOR v = XMVectorSubtract(C.vCurWS, rootPos);
    float l = XM_GetX(XMVector3Length(v));
    if (l <= tetherMax) return;

    XMVECTOR dir = XMVectorScale(v, 1.0f / max(l, 1e-6f));
    C.vCurWS = XMVectorAdd(rootPos, XMVectorScale(dir, tetherMax));
}

void CBoneTreePBD::ResolveSphereCollisions(NODE& N)
{
    if (N.fInvMass <= 0.0f) return;
    for (const auto& S : m_vSpheres)
    {
        XMVECTOR d = XMVectorSubtract(N.vCurWS, S.vCenterWS);
        float l = XM_GetX(XMVector3Length(d));
        float pen = S.fRadius - l;
        if (pen > 0.0f)
        {
            XMVECTOR n = (l > 1e-6f) ? XMVectorScale(d, 1.0f / l) : XMVectorSet(0, 1, 0, 0);
            N.vCurWS = XMVectorAdd(N.vCurWS, XMVectorScale(n, pen));
        }
    }
}

// --------------------------------------------------------------
// Helpers
// --------------------------------------------------------------
XMVECTOR CBoneTreePBD::GetBoneWorldPos(int iBone) const
{
    const _float4x4* pM = m_pModel->Get_BoneMatrix(iBone);
    _matrix m = XMLoadFloat4x4(pM);
    _matrix ownerW = m_pOwnerTransform ? m_pOwnerTransform->Get_WorldMatrix() : XMMatrixIdentity();
    _matrix W = XMMatrixMultiply(ownerW, m);
    return W.r[3]; // xyz
}

XMVECTOR CBoneTreePBD::GetBoneWorldRotQ(int iBone) const
{
    const _float4x4* pM = m_pModel->Get_BoneMatrix(iBone);
    _matrix m = XMLoadFloat4x4(pM);
    _matrix ownerW = m_pOwnerTransform ? m_pOwnerTransform->Get_WorldMatrix() : XMMatrixIdentity();
    _matrix W = XMMatrixMultiply(ownerW, m);

    XMVECTOR s, q, t;
    XMMatrixDecompose(&s, &q, &t, W);
    return q;
}

void CBoneTreePBD::ApplyBoneLook(int iParentBone, int iChildBone, XMVECTOR aWS, XMVECTOR bWS)
{
    // parent 월드 회전 / 위치
    const _float4x4* pParentM = m_pModel->Get_BoneMatrix(iParentBone);
    _matrix ownerW = m_pOwnerTransform ? m_pOwnerTransform->Get_WorldMatrix() : XMMatrixIdentity();
    _matrix parentW = XMMatrixMultiply(ownerW, XMLoadFloat4x4(pParentM));
    XMVECTOR sP, qParentW, tP;
    XMMatrixDecompose(&sP, &qParentW, &tP, parentW);

    // child bind/local 회전
    const _float4x4* pChildLocal = m_pModel->Get_LocalBoneMatrix(iChildBone);
    XMVECTOR sC, qChildBindLocal, tC;
    XMMatrixDecompose(&sC, &qChildBindLocal, &tC, XMLoadFloat4x4(pChildLocal));

    // parent→child 방향
    XMVECTOR dirW = XMVector3Normalize(XMVectorSubtract(bWS, aWS));
    XMVECTOR zBind = XMVectorSet(0, 0, 1, 0);
    XMVECTOR parentZ = XMVector3Rotate(zBind, qParentW);

    XMVECTOR axis = XMVector3Cross(parentZ, dirW);
    float dot = XM_GetX(XMVector3Dot(parentZ, dirW));
    dot = max(-1.f, min(1.f, dot));
    float angle = acosf(dot);
    if (fabsf(angle) < 1e-6f) return;

    axis = XMVector3Normalize(axis);
    XMVECTOR qFromTo = XMQuaternionRotationAxis(axis, angle);

    XMVECTOR qChildWorld = XMQuaternionMultiply(qFromTo, XMQuaternionMultiply(qParentW, qChildBindLocal));
    XMVECTOR qChildLocal = XMQuaternionMultiply(XMQuaternionConjugate(qParentW), qChildWorld);

    _float4 outLocal;
    XMStoreFloat4(&outLocal, qChildLocal);
    m_pModel->Set_BoneLocalRotation(iChildBone, XMLoadFloat4(&outLocal));
}

NS_END
