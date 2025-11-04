// =============================================================
// BoneTreePBD.h
// Forest(다중 루트) + 임의 깊이 본 트리에 대한 PBD 시뮬레이터
// Jo + ChatGPT
// =============================================================
#pragma once

#include <vector>
#include <cstdint>
#include <DirectXMath.h>
using namespace DirectX;

#include "Base.h"
#include "Model.h"
#include "Transform.h"

NS_BEGIN(Engine)

// -------------------------------------------------------------
// 공개 파라미터
// -------------------------------------------------------------
struct BTPBD_Params
{
    // Damping / Gravity / Wind
    float fDampStill = 0.98f;  // 정지에 가까울 때 적용(Verlet velocity 계수)
    float fDampMove = 0.92f;  // 이동 중 적용
    float fGravity = -9.8f;  // Y축 중력
    float fWindScale = 0.0f;   // 바람 힘 스케일

    // Stiffness / Iterations
    float fDistanceStiffness = 1.0f; // 거리 제약 강성(1.0이면 바로 붙임)
    int   iIterations = 2;    // PBD 반복 횟수

    // Angles
    float fRootConeDeg = 35.f; // 루트 바로 아래
    float fMidConeDeg = 30.f; // 그 아래들

    // Tether
    float fTetherScale = 1.25f; // 루트~노드 누적 길이 * 스케일 (최대 허용)

    // Speeds
    float fMaxSegSpeed = 6.0f;   // m/s
    float fSleepLin = 0.02f;  // 정지 안정화 선형 임계
};

// 간단한 구충돌 프록시
struct BTPBD_Sphere
{
    XMVECTOR vCenterWS;  // xyz
    float    fRadius;
};

// -------------------------------------------------------------
// BoneTreePBD (헤어/옷자락 PBD 본 시뮬레이터)
// -------------------------------------------------------------
class ENGINE_DLL CBoneTreePBD : public CBase
{
public:
    static CBoneTreePBD* Create(class CModel* pModel, class CTransform* pOwnerXform);
    virtual void Free() override;

    // 숲 구성(루트 본 인덱스 목록을 주면, 각 루트의 하위 서브트리를 자동 확장)
    HRESULT BuildForestFromRoots(const std::vector<int>& vRootBones);

    // 프레임 루프
    void PreUpdate(float dt, XMVECTOR vWindWS);  // 예측(관성/중력/바람), 루트 관성이월
    void Simulate(float dt);                     // 거리/콘/테더/충돌 제약 반복
    void PostApply();                            // 부모→자식 방향으로 자식본 로컬 회전 세팅

    // 이벤트/제어
    void TeleportSnapToSkeleton();               // 컷씬/로드 시 강제 스냅
    void SetEnabled(bool b) { m_bEnabled = b; }
    void SetParams(const BTPBD_Params& p) { m_Params = p; }

    // 충돌체 (머리/어깨/등 등)
    void ClearColliders();
    void AddSphereCollider(XMVECTOR centerWS, float r);
    // (움직이는 콜라이더라면 매 프레임 centerWS를 다시 Set하는 식으로 사용)

private:
    CBoneTreePBD(class CModel* pModel, class CTransform* pXform);

    // ===== 내부 데이터 =====
    struct NODE
    {
        XMVECTOR vCurWS = XMVectorZero();
        XMVECTOR vPrevWS = XMVectorZero();
        float    fInvMass = 1.0f;     // 루트=0
        uint16_t uParent = 0xFFFF;   // 없으면 0xFFFF
        uint16_t uDepth = 0;        // 루트=0
        float    fPathLen = 0.0f;     // 루트로부터 누적 rest 길이
        int      iBoneIdx = -1;       // CModel 본 인덱스
    };

    struct EDGE
    {
        uint16_t uParent = 0;
        uint16_t uChild = 0;
        float    fRestLen = 0.02f;    // 목표 길이
    };

    struct TREE
    {
        int                   iRootBone = -1;      // 루트 본 인덱스
        std::vector<uint16_t> vNodeIndex;          // 이 트리에 속한 노드들의 전역 인덱스
        std::vector<EDGE>     vEdges;              // (parent, child)
        float                 fAvgSegLen = 0.02f;  // 평균 세그 길이(참고)
    };

    struct RootCache { XMVECTOR pPrev = XMVectorZero(); bool hasPrev = false; };

private:
    // ===== 제약(프로젝터) =====
    void ProjectDistance(NODE& P, NODE& C, float restLen, float stiffness);
    void ProjectCone(const NODE& P, XMVECTOR qParentWS, NODE& C, float coneDeg);
    void ProjectTether(XMVECTOR rootPos, NODE& C, float tetherMax);
    void ResolveSphereCollisions(NODE& N);

    // ===== 헬퍼 =====
    XMVECTOR GetBoneWorldPos(int iBone) const;
    XMVECTOR GetBoneWorldRotQ(int iBone) const;
    void     ApplyBoneLook(int iParentBone, int iChildBone, XMVECTOR aWS, XMVECTOR bWS);

private:
    // 외부 레퍼런스
    class CModel* m_pModel = nullptr;
    class CTransform* m_pOwnerTransform = nullptr;

    // 그래프(숲)
    std::vector<NODE> m_vNodes;
    std::vector<TREE> m_vTrees;

    // 충돌체
    std::vector<BTPBD_Sphere> m_vSpheres;

    // 루트 속도 캐시(트리와 동일한 인덱스)
    std::vector<RootCache> m_vRootCache;

    // 파라미터/상태
    BTPBD_Params m_Params{};
    bool         m_bEnabled = true;

    XMVECTOR m_prevOwnerPos = XMVectorZero();
    XMVECTOR m_prevOwnerRotQ = XMQuaternionIdentity();
    bool     m_hasPrevOwner = false;

    float m_AdvecPosGain = 1.0f; // 1.0 = 100% 이월
    float m_AdvecRotGain = 0.7f; // 0.7 = 70% 회전 이월
};

NS_END
