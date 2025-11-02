#pragma once
#include "Base.h"

#ifdef new
#pragma push_macro("new")
#undef new
#endif

#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Body/BodyID.h>
#include <Jolt/Physics/Body/BodyInterface.h>

#include <Jolt/Physics/PhysicsScene.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>

#include <Jolt/Physics/Constraints/Constraint.h>
#include <Jolt/Physics/Constraints/PointConstraint.h>
#include <Jolt/Physics/Constraints/SwingTwistConstraint.h>
#include <Jolt/Physics/Body/BodyLockMulti.h>
#include "Jolt/Physics/Constraints/DistanceConstraint.h"

#ifdef new
#pragma pop_macro("new") // DBG_NEW 복원
#endif

NS_BEGIN(Engine)

class ENGINE_DLL CBoneChainPhysic : public CBase
{
public:
    enum EBCP_Preset : _uint
    {
        BCP_PRESET_Hair = 0,
        BCP_PRESET_ClothStrip, // 치맛단/망토 등 세로 스트립
        BCP_PRESET_Custom
    };

    struct BCP_BuildDesc
    {
        std::vector<std::vector<int>> vBoneChains; // 각 체인: [root, b1, b2, ...]
        EBCP_Preset ePreset = BCP_PRESET_Hair;
        _float      fCapsuleRadius = 0.015f;
        _bool       isTipCCD = true;
        _bool       isSelfCollide = false; // 체인끼리 충돌
    };

    struct BCP_RuntimeParams
    {
        // 공통 감쇠/마찰
        _float fLinearDamping = 0.20f;
        _float fAngularDamping = 0.40f;
        _float fFriction = 0.15f;
        _float fRestitution = 0.02f;

        // 조인트 제한(기본값: 헤어 느낌)
        _float fRootConeDeg = 35.f;
        _float fRootTwistDeg = 20.f;
        _float fMidConeDeg = 30.f;
        _float fMidTwistDeg = 15.f;

        // 질량(루트→팁 선형 보간)
        _float fMassRoot = 0.08f;
        _float fMassTip = 0.02f;

        // 솔버
        _int   iSolverIters = 10; // Jolt 설정에서 전역으로 다룸
    };

    struct BCP_Chain
    {
        vector<JPH::BodyID>              vBodies; // 세그먼트당 1개 (N-1)
        vector<JPH::TwoBodyConstraint*>  vCons;   // 모든 연결/제한
        vector<int>                      vBoneIdx; // [root, b1, ...] (N개)
        _float                                fRadius = 0.015f;
    };

private:
    CBoneChainPhysic(JPH::PhysicsSystem* pPhys, JPH::BodyInterface* pBI);
    virtual ~CBoneChainPhysic();

public:
    HRESULT Initialize(class CModel* pModel, const BCP_BuildDesc& tBuild, const BCP_RuntimeParams& tParams, _uint iRootObjectLayer, _uint iBoneChainObjectLayer);

    // 프레임 루틴
    void SyncRootProxy_PrePhysics(class CModel* pModel);
    void ApplyBodiesToBones_PostPhysics(class CModel* pModel);

    // 이벤트/제어
    void TeleportSnapToSkeleton(class CModel* pModel); // 텔레포트 / 컷씬 대응
    void SetEnabled(_bool isEnable);

    // 프리셋 스위치(런타임 튜닝)
    void ApplyPreset(EBCP_Preset ePreset);

private:
    // 생성/파괴
    BodyID CreateRootKinematicProxy(class CModel* pModel, _int iHeadBone, _float fRadius, _float fHalfHeight);
    void BuildChain(class CModel* pModel, const vector<int>& vBoneIdx, _float fRadius);
    void DestroyAll();

private:
    // 내부 유틸
    static JPH::Vec3  ToJ(const _float3& v);
    static JPH::Quat  ToJQ(const _float4& q);
    static _float3    GetBoneWorldPos(CModel* pModel, _int iBoneIdx);
    static _float4    GetBoneWorldRot(CModel* pModel, _int iBoneIdx); // (선택) 필요 시
    void              SetChildBoneBySegment(CModel* pModel, _int iParentBone, _int iChildBone,
        const JPH::Vec3& vFrom, const JPH::Vec3& vTo);

    void              SetupConstraint_ChainSegment(JPH::BodyID a, JPH::BodyID b,
        const JPH::Vec3& pivotWorld,
        _bool isRootLink);

    void              ConfigureByPreset(EBCP_Preset ePreset);


private:
    // 외부
    JPH::PhysicsSystem* m_pPhysics = nullptr;
    JPH::BodyInterface* m_pBI = nullptr;

    // 리소스
    JPH::BodyID                m_tRootProxy; // Kinematic (Head/Neck 등)
    vector<BCP_Chain>     m_vChains;

    // 파라미터
    BCP_RuntimeParams          m_tParams;
    EBCP_Preset                m_ePreset = BCP_PRESET_Hair;

    // 루트 프록시 모양
    _float m_fRootProxyRadius = 0.02f;
    _float m_fRootProxyHalf = 0.03f;

    // 상태
    _bool  m_isEnabled = true;

    _int   m_iHeadBone = -1;

    _uint m_iRootObjectLayer = {};
    _uint m_iBoneChainObjectLayer = {};

public:
    static CBoneChainPhysic* Create(JPH::PhysicsSystem* pPhys, JPH::BodyInterface* pBI, class CModel* pModel, const BCP_BuildDesc& tBuild, const BCP_RuntimeParams& tParams, _uint iRootObjectLayer, _uint iBoneChainObjectLayer);
    virtual void Free() override;
};

NS_END