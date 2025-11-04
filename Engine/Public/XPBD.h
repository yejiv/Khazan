#pragma once

#include <vector>
#include "Base.h"

NS_BEGIN(Engine)

class ENGINE_DLL CXPBD : public CBase
{
private:
    CXPBD(class CModel* pModel, class CTransform* pTransform, const XPBD_Params& tParams);
    virtual ~CXPBD() = default;

public:
    // 단일 목적: Strands(본 인덱스 체인들의 목록)로 초기화
    HRESULT Initialize(const vector<vector<_int>>& vStrands);

    // 루프
    void PreUpdate(_float fTimeDelta, _vector vWindWS);
    void Solve(_float fTimeDelta);
    void PostApply(_float fTimeDelta);

    // 유틸
    void TeleportSnapToSkeleton();

    // 콜라이더
    void ClearColliders();
    void AddSphereCollider(_vector vCenterWS, _float fRadius);

    // 파라미터
    void SetParams(const XPBD_Params& tParams) { m_tParams = tParams; }

private:
    // ===== 내부 자료구조 =====
    struct PARTICLE
    {
        _vector  vCur = XMVectorZero();
        _vector  vPrev = XMVectorZero();
        _float   fInvMass = 1.f;
        uint16  uDepth = 0;
        _int     iBone = -1;
        _float   fPathLen = 0.f;     // 루트부터 누적 길이
    };

    // Stretch: i <-> j
    struct EDGE_C
    {
        uint16 i = 0, j = 0;
        _float  L0 = 0.02f;
        _float  lambda = 0.f;
    };

    // Bend(거리-벤드): (k-1) <-> (k+1)
    struct BEND_C
    {
        uint16 i0 = 0, i2 = 0;
        _float  L0 = 0.03f;
        _float  lambda = 0.f;
    };

    // Shear(선택): 2열 카드 가로/대각
    struct SHEAR_C
    {
        uint16 a = 0, b = 0;
        _float  L0 = 0.02f;
        _float  lambda = 0.f;
    };

    struct STRAND
    {
        uint16 uRootPi = 0;    // 이 스트랜드의 루트 파티클 인덱스
        uint16 uMaxDepth = 0;   // 파티클 개수 - 1
    };

    struct RootCache
    {
        _vector pPrev = XMVectorZero();
        _bool   hasPrev = false;
    };

    struct ANGLE_BEND_C {
        uint16 i0, i1, i2;  // (i0 - i1 - i2) 가운데 i1이 관절
        _float  theta0 = 0.f; // 초기 각
        _float  lambda = 0.f;
    };

private:
    // ===== 빌드 헬퍼(Initialize 내부 전용) =====
    HRESULT BuildFromStrands(const vector<vector<_int>>& strands);
    HRESULT BuildForestFromRoots(const vector<_int>& roots); 

    // ===== 헬퍼 =====
    _vector GetBoneWorldPos(_int iBone) const;
    _vector GetBoneWorldRotQ(_int iBone) const;
    void    ApplyBoneLook(_int iParentBone, _int iChildBone, _vector aWS, _vector bWS);

    void ApplySagBias(float moveRatio);

    // ===== XPBD 솔버 =====
    static void XPBD_Distance(_vector& xi, _float wi,
        _vector& xj, _float wj,
        _float L0, _float compliance, _float dt,
        _float& lambda_io);

    void SolveStretch(_float dt);
    void SolveBend(_float dt);
    void SolveShear(_float dt);
    void SolveAngleBend(float dt);
    void ApplyTetherSoft();
    void ResolveSphereCollisions();

    void Clear();

private:
    // 외부 레퍼런스
    class CModel* m_pModel = nullptr;
    class CTransform* m_pOwnerTransform = nullptr;

    // 파라미터
    XPBD_Params m_tParams = {};

    // 데이터
    vector<PARTICLE>    m_vParticles;
    vector<EDGE_C>      m_vEdges;
    vector<BEND_C>      m_vBends;
    vector<SHEAR_C>     m_vShears;
    vector<STRAND>      m_vStrands;
    vector<RootCache>   m_vRootCache;
    vector<XPBD_Sphere> m_vSpheres;
    vector<ANGLE_BEND_C> m_vAngleBends;

    // 오너 관성 이월
    _vector m_vPrevOwnerPos = XMVectorZero();
    _vector m_vPrevOwnerQ = XMQuaternionIdentity();
    _bool   m_isPrevOwner = false;

    _float m_fLastMoveRatio = 0.f;     // 0~1 (정지~빠른 이동)
    _float m_fGlobalRootSpeed = 0.f;   // 모든 스트랜드 루트 평균 속도(m/s)
public:
    static CXPBD* Create(class CModel* pModel,
        class CTransform* pOwnerTransform,
        const vector<vector<_int>>& vStrands,
        const XPBD_Params& tParams);

    virtual void Free() override;
};

NS_END
