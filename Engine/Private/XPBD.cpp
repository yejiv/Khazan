#include "XPBD.h"
#include "Model.h"
#include "Transform.h"

inline _float  ClampF(_float v, _float a, _float b) { return max(a, min(b, v)); }
inline _float  LerpF(_float a, _float b, _float t) { return a + (b - a) * t; }
inline _float  XM_F(XMVECTOR v) { return XMVectorGetX(v); }
inline _float  Len3(_vector v) { return XM_F(XMVector3Length(v)); }

inline _vector ClampLength(_vector v, _float maxLen)
{
    _vector len = XMVector3Length(v);
    _vector mask = XMVectorGreater(len, XMVectorReplicate(maxLen));
    _vector scl = XMVectorDivide(XMVectorReplicate(maxLen), len);
    return XMVectorSelect(v, XMVectorMultiply(v, scl), mask);
}

inline void DecomposeWorld(CTransform* xf, _vector& outPos, _vector& outQ)
{
    _matrix W = xf ? xf->Get_WorldMatrix() : XMMatrixIdentity();
    _vector s, q, t; XMMatrixDecompose(&s, &q, &t, W);
    outPos = t; outQ = q;
}

CXPBD::CXPBD(CModel* pModel, CTransform* pTransform, const XPBD_Params& tParams)
	: m_pModel{ pModel }, m_pOwnerTransform{ pTransform }, m_tParams{ tParams }
{
}

HRESULT CXPBD::Initialize(const vector<vector<_int>>& vStrands)
{
    CHECK_FAILED(BuildFromStrands(vStrands), E_FAIL);

    TeleportSnapToSkeleton();

    // ↓ 중력 기준 릴랙스: 스트레치/테더만으로 아래로 "쭉" 늘어뜨리기
    {
        const int relaxIters = 80;     // 필요하면 120~200
        const float dt = 1.f / 120.f;  // 작은 dt로 천천히
        const _vector gWS = XMVectorSet(0, m_tParams.fGravityY, 0, 0);

        for (int it = 0; it < relaxIters; ++it) {
            // 간단한 Verlet 예측(속도=0 가정)
            for (auto& S : m_vStrands) {
                for (uint16 k = 1; k <= S.uMaxDepth; ++k) {
                    auto& P = m_vParticles[S.uRootPi + k];
                    _vector acc = gWS; // 공기저항 X
                    P.vPrev = P.vCur;
                    P.vCur = XMVectorAdd(P.vCur, XMVectorScale(acc, dt * dt));
                }
            }
            SolveStretch(dt);
            ApplyTetherSoft();
            // (충돌 넣고 싶으면 ResolveSphereCollisions() 추가)
        }
    }

	return S_OK;
}

void CXPBD::PreUpdate(_float fTimeDelta, _vector vWindWS)
{
    if (m_vParticles.empty()) return;
    fTimeDelta = ClampF(fTimeDelta, 1e-4f, 1.f / 10.f);

    // 1) 오너 변위/회전
    _vector ownerPosNow, ownerQNow;
    DecomposeWorld(m_pOwnerTransform, ownerPosNow, ownerQNow);

    _vector dT = XMVectorZero();
    _vector dR = XMQuaternionIdentity();
    if (m_isPrevOwner) {
        dT = XMVectorSubtract(ownerPosNow, m_vPrevOwnerPos);
        dR = XMQuaternionMultiply(ownerQNow, XMQuaternionInverse(m_vPrevOwnerQ));
        dR = XMQuaternionNormalize(dR);
    }
    // (주의) 이전 오너 상태는 함수 끝에서 갱신할 거라 여기서 바로 덮어쓰지 않아도 됨

    // 2) 깊이 가중 이월(원래 코드 그대로)
    auto DepthBlend = [&](const STRAND& S, uint16 depth)->_float {
        if (S.uMaxDepth == 0) return 0.f;
        return ClampF((float)depth / (float)S.uMaxDepth, 0.f, 1.f);
        };
    if (m_isPrevOwner) {
        for (size_t si = 0; si < m_vStrands.size(); ++si) {
            const STRAND& S = m_vStrands[si];
            if (S.uMaxDepth == 0) continue;

            const uint16 rootPi = S.uRootPi;
            const _vector pivot = m_vParticles[rootPi].vCur;

            for (uint16 k = 0; k <= S.uMaxDepth; ++k) {
                uint16 pi = S.uRootPi + k;
                _float u = DepthBlend(S, k);

                _float posGain = LerpF(m_tParams.fAdvectPosRoot, m_tParams.fAdvectPosTip, u);
                _float rotGain = LerpF(m_tParams.fAdvectRotRoot, m_tParams.fAdvectRotTip, u);

                _vector dTg = XMVectorScale(dT, posGain);
                _vector dRg = XMQuaternionSlerp(XMQuaternionIdentity(), dR, rotGain);

                auto advect = [&](_vector p)->_vector {
                    p = XMVectorAdd(p, dTg);
                    _vector off = XMVectorSubtract(p, pivot);
                    _vector offR = XMVector3Rotate(off, dRg);
                    return XMVectorAdd(pivot, offR);
                    };

                m_vParticles[pi].vCur = advect(m_vParticles[pi].vCur);
                m_vParticles[pi].vPrev = advect(m_vParticles[pi].vPrev);
            }
        }
    }

    // 3) 루트 스냅 + 예측 & 이동 속도 측정
    const _vector gWS = XMVectorSet(0, m_tParams.fGravityY, 0, 0);
    const _float  maxSpeed = max(0.1f, m_tParams.fMaxSpeed);

    // 이동 속도 집계용
    _float speedSum = 0.f;
    _float speedMax = 0.f;
    int    speedCnt = 0;

    for (size_t si = 0; si < m_vStrands.size(); ++si)
    {
        const STRAND& S = m_vStrands[si];
        RootCache& RC = m_vRootCache[si];
        const uint16 rootPi = S.uRootPi;
        PARTICLE& R = m_vParticles[rootPi];

        _vector rootNow = GetBoneWorldPos(R.iBone);
        _vector vLin = XMVectorZero();
        if (RC.hasPrev)
            vLin = XMVectorDivide(XMVectorSubtract(rootNow, RC.pPrev),
                XMVectorReplicate(fTimeDelta));

        R.vCur = R.vPrev = rootNow;

        // 이동 속도 집계
        _float rootSpeed = Len3(vLin);
        speedSum += rootSpeed;
        if (rootSpeed > speedMax) speedMax = rootSpeed;
        ++speedCnt;

        // 감쇠 계수
        _float tMove = ClampF(rootSpeed / 0.5f, 0.f, 1.f);
        _float damp = LerpF(m_tParams.fDampingStill, m_tParams.fDampingMove, tMove);

        // 자식 예측
        for (uint16 k = 1; k <= S.uMaxDepth; ++k)
        {
            PARTICLE& P = m_vParticles[S.uRootPi + k];

            // 기존 속도
            _vector vel = XMVectorDivide(
                XMVectorSubtract(P.vCur, P.vPrev),
                XMVectorReplicate(fTimeDelta)
            );

            // (A) 루트 병진 성분 제거: vel_rel = vel - vLin
            _vector vel_rel = XMVectorSubtract(vel, vLin);

            // (B) 공기저항은 상대풍 기준(vel_rel - vWindWS)
            _vector relV = XMVectorSubtract(vel_rel, vWindWS);

            _vector drag = XMVectorAdd(
                XMVectorScale(relV, -m_tParams.fAirLinear),
                XMVectorScale(ClampLength(relV, 1e9f), -m_tParams.fAirQuadratic * Len3(relV))
            );

            _vector acc = XMVectorAdd(gWS, drag);

            // (C) 감쇠/적분도 상대속도 기준으로
            vel_rel = XMVectorScale(vel_rel, damp);
            vel_rel = XMVectorAdd(vel_rel, XMVectorScale(acc, fTimeDelta));
            vel_rel = ClampLength(vel_rel, maxSpeed);

            // (D) 적분 시 루트 성분 복원: (vel_rel + vLin)
            _vector vel_new = XMVectorAdd(vel_rel, vLin);

            P.vPrev = P.vCur;
            P.vCur = XMVectorAdd(P.vCur, XMVectorScale(vel_new, fTimeDelta));
        }

        RC.pPrev = rootNow;
        RC.hasPrev = true;
    }

    // 전역 이동률(m/s → 0~1 비율) 계산 & 캐시
    m_fGlobalRootSpeed = (speedCnt > 0) ? (speedSum / speedCnt) : 0.f;  // 평균속도
    m_fLastMoveRatio = ClampF(m_fGlobalRootSpeed / 0.5f, 0.f, 1.f);

    // 4) 오너 상태 캐시
    m_vPrevOwnerPos = ownerPosNow;
    m_vPrevOwnerQ = ownerQNow;
    m_isPrevOwner = true;
   
}

void CXPBD::Solve(_float fTimeDelta)
{
    if (m_vParticles.empty()) return;

    int sub = max(1, m_tParams.iSubsteps);
    int it = max(1, m_tParams.iSolverIterations);

    float h = fTimeDelta / sub;
    for (int s = 0; s < sub; ++s)
    {
        // ===== [PATCH #2: SagBias는 서브스텝당 1회] =====
        ApplySagBias(m_fLastMoveRatio);

        for (int k = 0; k < it; ++k)
        {
            // 여기서는 더 이상 ApplySagBias 호출하지 않음
            SolveStretch(h);
            SolveBend(h);
            // 필요하면 SolveShear(h);
            ApplyTetherSoft();
            ResolveSphereCollisions();
        }
    }
}

void CXPBD::PostApply(_float fTImeDelta)
{
    if (!m_pModel) return;

    // 한 프레임 동안 같은 자식본을 두 번 이상 안 건드리기
    std::unordered_set<int> written;
    written.reserve(m_vEdges.size() * 2);

    for (const auto& E : m_vEdges)
    {
        const auto& Pi = m_vParticles[E.i];
        const auto& Pj = m_vParticles[E.j];

        if (Pi.iBone < 0 || Pj.iBone < 0) continue;

        // 루트(깊이 0) 또는 루트 바로 아래(깊이 1)는 스킵해서 상위 본 안정
        if (Pi.uDepth < 1) continue;

        if (written.insert(Pj.iBone).second) // 처음 쓰는 자식본만 회전 적용
            ApplyBoneLook(Pi.iBone, Pj.iBone, Pi.vCur, Pj.vCur);
    }

    m_pModel->Update_BoneCombinedMatrices();
}

void CXPBD::TeleportSnapToSkeleton()
{
    for (auto& P : m_vParticles)
    {
        _vector p = GetBoneWorldPos(P.iBone);
        P.vPrev = P.vCur = p;
    }
    for (auto& RC : m_vRootCache) { RC.hasPrev = false; RC.pPrev = XMVectorZero(); }
    m_isPrevOwner = false;
    m_vPrevOwnerPos = XMVectorZero();
    m_vPrevOwnerQ = XMQuaternionIdentity();
}

void CXPBD::ClearColliders()
{
    m_vSpheres.clear();
}

void CXPBD::AddSphereCollider(_vector vCenterWS, _float fRadius)
{
    m_vSpheres.push_back({ vCenterWS, fRadius });
}

HRESULT CXPBD::BuildFromStrands(const vector<vector<_int>>& strands)
{
    Clear();

    if (!m_pModel)
        return E_FAIL;

    // 각 스트랜드 : [root, b1, b2, ...]
    for (const auto& chain : strands)
    {
        if (chain.size() < 2)
            continue;

        STRAND tStrand{};
        tStrand.uRootPi = (uint16)m_vParticles.size();
        tStrand.uMaxDepth = (uint16)(chain.size() - 1);

        //파티클 생성
        _float fPathLenAcc = 0.f;
        for (uint16 i = 0; i < (uint16)chain.size(); ++i)
        {
            const _int iBone = chain[i];
            _vector vPos = GetBoneWorldPos(iBone);

            PARTICLE tParticle{};
            tParticle.vCur = tParticle.vPrev = vPos;
            tParticle.fInvMass = (i == 0) ? 0.f : 1.f;
            tParticle.uDepth = i;
            tParticle.iBone = iBone;
            tParticle.fPathLen = fPathLenAcc;

            m_vParticles.push_back(tParticle);

            if (i > 0)
            {
                _vector vPrevPos = GetBoneWorldPos(chain[i - 1]);
                fPathLenAcc += max(0.001f, Len3(XMVectorSubtract(vPos, vPrevPos)));
            }
        }

        // Stretch (연속 이웃)
        for (uint16 i = 0; i + 1 < (uint16)chain.size(); ++i)
        {
            uint16 uA = tStrand.uRootPi + i;
            uint16 uB = tStrand.uRootPi + i + 1;
            _float L0 = max(0.001f, Len3(XMVectorSubtract(m_vParticles[uB].vCur, m_vParticles[uA].vCur)));
            m_vEdges.push_back(EDGE_C{ uA, uB, L0, 0.f });
        }


        // Bend (거리-벤드, i-1 <-> i+1)
        for (uint16 k = 1; k + 1 < (uint16)chain.size(); ++k)
        {
            uint16 i0 = tStrand.uRootPi + (k - 1);
            uint16 i2 = tStrand.uRootPi + (k + 1);
            _float  L0 = max(0.001f, Len3(XMVectorSubtract(m_vParticles[i2].vCur, m_vParticles[i0].vCur)));
            m_vBends.push_back(BEND_C{ i0, i2, L0, 0.f });
        }

        // Angle-bend (i0 - i1 - i2)
        for (uint16 k = 1; k + 1 < (uint16)chain.size(); ++k)
        {
            uint16 i0 = tStrand.uRootPi + (k - 1);
            uint16 i1 = tStrand.uRootPi + k;
            uint16 i2 = tStrand.uRootPi + (k + 1);

            _vector e0 = XMVector3Normalize(XMVectorSubtract(m_vParticles[i0].vCur, m_vParticles[i1].vCur));
            _vector e1 = XMVector3Normalize(XMVectorSubtract(m_vParticles[i2].vCur, m_vParticles[i1].vCur));
            float theta0 = acosf(ClampF(XM_F(XMVector3Dot(e0, e1)), -1.f, 1.f));

            m_vAngleBends.push_back({ i0, i1, i2, theta0, 0.f });
        }

        m_vStrands.push_back(tStrand);
        m_vRootCache.push_back(RootCache{ XMVectorZero(), false });
    }


	return S_OK;
}

HRESULT CXPBD::BuildForestFromRoots(const vector<_int>& roots)
{
    if (!m_pModel)
        return E_FAIL;



	return S_OK;
}

_vector CXPBD::GetBoneWorldPos(_int iBone) const
{
    const _float4x4* pM = m_pModel->Get_BoneMatrix(iBone);
    if (!pM) return XMVectorZero();

    _matrix m = XMLoadFloat4x4(pM);
    _matrix ownerW = m_pOwnerTransform ? m_pOwnerTransform->Get_WorldMatrix() : XMMatrixIdentity();
    _matrix W = XMMatrixMultiply(ownerW, m);
    return W.r[3];
}

_vector CXPBD::GetBoneWorldRotQ(_int iBone) const
{
    const _float4x4* pM = m_pModel->Get_BoneMatrix(iBone);
    if (!pM) return XMQuaternionIdentity();

    _matrix m = XMLoadFloat4x4(pM);
    _matrix ownerW = m_pOwnerTransform ? m_pOwnerTransform->Get_WorldMatrix() : XMMatrixIdentity();
    _matrix W = XMMatrixMultiply(ownerW, m);

    _vector s, q, t;
    XMMatrixDecompose(&s, &q, &t, W);
    return q;
}

void CXPBD::ApplyBoneLook(_int iParentBone, _int iChildBone, _vector aWS, _vector bWS)
{
    if (iParentBone < 0 || iChildBone < 0) return;

    const _float4x4* pParentM = m_pModel->Get_BoneMatrix(iParentBone);
    const _float4x4* pChildLocal = m_pModel->Get_LocalBoneMatrix(iChildBone);
    if (!pParentM || !pChildLocal) return;

    _matrix ownerW = m_pOwnerTransform ? m_pOwnerTransform->Get_WorldMatrix() : XMMatrixIdentity();
    _matrix parentW = XMMatrixMultiply(ownerW, XMLoadFloat4x4(pParentM));

    _vector sP, qParentW, tP;
    XMMatrixDecompose(&sP, &qParentW, &tP, parentW);

    // 바인드 로컬 회전 분해
    _vector sC, qChildBindLocal, tC;
    XMMatrixDecompose(&sC, &qChildBindLocal, &tC, XMLoadFloat4x4(pChildLocal));

    // 새 방향(Z축) 계산
    _vector delta = XMVectorSubtract(bWS, aWS);
    float len2 = XMVectorGetX(XMVector3LengthSq(delta));
    if (len2 < 1e-10f) return;
    _vector zNew = XMVector3Normalize(delta);

    // 바인드 로컬의 Up(보통 Y)을 부모월드로 보냄 → 참조 업
    _vector bindUpLocal = XMVectorSet(0, 1, 0, 0);
    _vector refUpWorld = XMVector3Rotate(bindUpLocal, XMQuaternionMultiply(qParentW, qChildBindLocal));

    // refUpWorld를 zNew 평면에 투영해 Y축 구성(= 롤 고정)
    _vector yTemp = XMVectorSubtract(refUpWorld, XMVectorScale(zNew, XMVectorGetX(XMVector3Dot(refUpWorld, zNew))));
    float yLen2 = XMVectorGetX(XMVector3LengthSq(yTemp));
    if (yLen2 < 1e-12f)
    {
        // 참조 업이 방향과 평행이면 임의의 업 선택
        yTemp = XMVectorSubtract(XMVectorSet(0, 1, 0, 0), XMVectorScale(zNew, XMVectorGetX(XMVector3Dot(XMVectorSet(0, 1, 0, 0), zNew))));
        if (XMVectorGetX(XMVector3LengthSq(yTemp)) < 1e-12f)
            yTemp = XMVectorSet(1, 0, 0, 0);
    }
    _vector yNew = XMVector3Normalize(yTemp);
    _vector xNew = XMVector3Normalize(XMVector3Cross(yNew, zNew));

    // 월드 회전 행렬 만들기 (x,y,z를 열벡터로)
    _matrix childWorldR =
        _matrix(
            xNew.m128_f32[0], xNew.m128_f32[1], xNew.m128_f32[2], 0,
            yNew.m128_f32[0], yNew.m128_f32[1], yNew.m128_f32[2], 0,
            zNew.m128_f32[0], zNew.m128_f32[1], zNew.m128_f32[2], 0,
            0, 0, 0, 1);

    _vector qChildWorld = XMQuaternionRotationMatrix(childWorldR);

    // 로컬로 변환: qLocal = parent^{-1} * qWorld
    _vector qChildLocal = XMQuaternionMultiply(XMQuaternionConjugate(qParentW), qChildWorld);

    _float4 out; XMStoreFloat4(&out, qChildLocal);
    m_pModel->Set_BoneLocalRotation(iChildBone, XMLoadFloat4(&out));
}

void CXPBD::ApplySagBias(float moveRatio /*0~1: 느림~빠름*/)
{
    const float w = LerpF(m_tParams.fSagBiasStill, m_tParams.fSagBiasMove, moveRatio);
    if (w <= 0.f) return;

    // 월드 중력 단위벡터
    _vector g = XMVector3Normalize(XMVectorSet(0, m_tParams.fGravityY, 0, 0));

    for (const STRAND& S : m_vStrands)
    {
        const uint16 rootPi = S.uRootPi;
        const _vector root = m_vParticles[rootPi].vCur;

        for (uint16 k = 1; k <= S.uMaxDepth; ++k)
        {
            PARTICLE& P = m_vParticles[S.uRootPi + k];

            // root->P 벡터를 g로 투영한 길이를 현재 pathLen에 맞추는 방향으로만 살짝 섞기
            _vector r2p = XMVectorSubtract(P.vCur, root);
            float   proj = XMVectorGetX(XMVector3Dot(r2p, g)); // g방향 성분(부호 있음)
            float   want = -P.fPathLen; // 아래 방향(중력과 같은 방향)이 +라면 부호 맞춰줘도 됨
            float   delta = want - proj;

            // g축 방향으로만 보정
            _vector target = XMVectorAdd(P.vCur, XMVectorScale(g, delta));
            P.vCur = XMVectorLerp(P.vCur, target, w);
        }
    }
}

void CXPBD::XPBD_Distance(_vector& xi, _float wi, _vector& xj, _float wj, _float L0, _float compliance, _float dt, _float& lambda_io)
{
    _vector d = XMVectorSubtract(xj, xi);
    _float  l = Len3(d);
    if (l < 1e-7f) return;

    _vector n = XMVectorScale(d, 1.0f / l);
    _float  C = l - L0;

    _float wsum = wi + wj;
    if (wsum <= 0.f) { lambda_io = 0.f; return; }

    _float alpha = compliance / (dt * dt);
    _float dlambda = -C / (wsum + alpha);

    lambda_io += dlambda;

    _vector corr = XMVectorScale(n, dlambda);
    if (wi > 0.f) xi = XMVectorSubtract(xi, XMVectorScale(corr, wi));
    if (wj > 0.f) xj = XMVectorAdd(xj, XMVectorScale(corr, wj));
}

void CXPBD::SolveStretch(_float dt)
{
    const _float comp = max(0.f, m_tParams.fStretchCompliance);
    for (auto& E : m_vEdges)
    {
        PARTICLE& Pi = m_vParticles[E.i];
        PARTICLE& Pj = m_vParticles[E.j];
        XPBD_Distance(Pi.vCur, Pi.fInvMass, Pj.vCur, Pj.fInvMass, E.L0, comp, dt, E.lambda);
    }
}

void CXPBD::SolveBend(_float dt)
{
    const _float comp = max(0.f, m_tParams.fBendCompliance);
    if (comp <= 0.f) return;

    for (auto& B : m_vBends)
    {
        PARTICLE& P0 = m_vParticles[B.i0];
        PARTICLE& P2 = m_vParticles[B.i2];
        XPBD_Distance(P0.vCur, P0.fInvMass, P2.vCur, P2.fInvMass, B.L0, comp, dt, B.lambda);
    }
}

void CXPBD::SolveShear(_float dt)
{
    const _float comp = max(0.f, m_tParams.fShearCompliance);
    if (comp <= 0.f || m_vShears.empty()) return;

    for (auto& S : m_vShears)
    {
        PARTICLE& Pa = m_vParticles[S.a];
        PARTICLE& Pb = m_vParticles[S.b];
        XPBD_Distance(Pa.vCur, Pa.fInvMass, Pb.vCur, Pb.fInvMass, S.L0, comp, dt, S.lambda);
    }
}

void CXPBD::SolveAngleBend(float dt)
{
    const float comp = max(0.f, m_tParams.fAngleBendCompliance); 
    if (comp <= 0.f) return;

    for (auto& C : m_vAngleBends)
    {
        auto& P0 = m_vParticles[C.i0];
        auto& P1 = m_vParticles[C.i1];
        auto& P2 = m_vParticles[C.i2];

        _vector e0 = XMVector3Normalize(XMVectorSubtract(P0.vCur, P1.vCur));
        _vector e1 = XMVector3Normalize(XMVectorSubtract(P2.vCur, P1.vCur));
        float cosT = ClampF(XM_F(XMVector3Dot(e0, e1)), -1.f, 1.f);
        float theta = acosf(cosT);
        float Cval = theta - C.theta0;
        if (fabsf(Cval) < 1e-4f) continue;

        _vector n = XMVector3Normalize(XMVector3Cross(e0, e1));  
        _vector t0 = XMVector3Normalize(XMVector3Cross(n, e0));
        _vector t1 = XMVector3Normalize(XMVector3Cross(n, e1));

        float w0 = P0.fInvMass, w1 = P1.fInvMass, w2 = P2.fInvMass;
        float denom = w0 + w1 + w2;
        if (denom <= 0.f) { C.lambda = 0.f; continue; }

        float alpha = comp / (dt * dt);
        float dlambda = -Cval / (denom + alpha);
        C.lambda += dlambda;

        if (w0 > 0) P0.vCur = XMVectorAdd(P0.vCur, XMVectorScale(t0, dlambda * w0));
        if (w2 > 0) P2.vCur = XMVectorAdd(P2.vCur, XMVectorScale(t1, -dlambda * w2));
        if (w1 > 0) {
            _vector corr1 = XMVectorScale(XMVectorAdd(t1, XMVectorNegate(t0)), dlambda * w1 * 0.5f);
            P1.vCur = XMVectorAdd(P1.vCur, corr1);
        }
    }
}

void CXPBD::ApplyTetherSoft()
{
    // 루트~파티클 누적 길이 * Scale를 넘어가면 부드럽게 당김
    const _float scl = max(1.0f, m_tParams.fTetherScale);
    const _float soft = ClampF(m_tParams.fTetherSoftness, 0.f, 1.f);

    for (const STRAND& S : m_vStrands)
    {
        const uint16 rootPi = S.uRootPi;
        const _vector root = m_vParticles[rootPi].vCur;

        for (uint16 k = 1; k <= S.uMaxDepth; ++k)
        {
            PARTICLE& P = m_vParticles[S.uRootPi + k];
            _float maxL = max(0.001f, P.fPathLen * scl);

            _vector v = XMVectorSubtract(P.vCur, root);
            _float  l = Len3(v);
            if (l <= maxL) continue;

            _vector dir = XMVectorScale(v, 1.0f / l);
            _vector target = XMVectorAdd(root, XMVectorScale(dir, maxL));

            // 소프트 보정
            P.vCur = XMVectorLerp(P.vCur, target, soft);
        }
    }
}

void CXPBD::ResolveSphereCollisions()
{
    if (m_vSpheres.empty()) return;

    for (auto& P : m_vParticles)
    {
        if (P.fInvMass <= 0.f) continue;
        for (const auto& S : m_vSpheres)
        {
            _vector d = XMVectorSubtract(P.vCur, S.vCenterWS);
            _float  l = Len3(d);
            _float  pen = S.fRadius - l;
            if (pen > 0.f)
            {
                _vector n = (l > 1e-6f) ? XMVectorScale(d, 1.f / l) : XMVectorSet(0, 1, 0, 0);
                P.vCur = XMVectorAdd(P.vCur, XMVectorScale(n, pen));
            }
        }
    }
}

void CXPBD::Clear()
{
    m_vParticles.clear();
    m_vEdges.clear();
    m_vBends.clear();
    m_vShears.clear();
    m_vStrands.clear();
    m_vRootCache.clear();
    m_vSpheres.clear();
}

CXPBD* CXPBD::Create(CModel* pModel, CTransform* pOwnerTransform, const vector<vector<int>>& vStrands, const XPBD_Params& tParams)
{
    CXPBD* pInstance = new CXPBD(pModel, pOwnerTransform, tParams);
    if (FAILED(pInstance->Initialize(vStrands)))
    {
        MSG_BOX(TEXT("Failed to Created : CXPBD"));
        Safe_Release(pInstance);
    }
    return pInstance;
}


void CXPBD::Free()
{
    __super::Free();
    Clear();
}
