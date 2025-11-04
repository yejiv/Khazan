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
#include <Jolt/Physics/Constraints/SwingTwistConstraint.h>
#include <Jolt/Physics/Body/BodyLockMulti.h>
#include <Jolt/Physics/Constraints/DistanceConstraint.h>
#include <Jolt/Physics/Collision/GroupFilterTable.h>   // ★
#include <Jolt/Physics/Collision/CollisionGroup.h>     // ★

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
        _float fLinearDamping = 0.20f;
        _float fAngularDamping = 0.40f;
        _float fFriction = 0.15f;
        _float fRestitution = 0.02f;

        _float fRootConeDeg = 35.f;
        _float fRootTwistDeg = 20.f;
        _float fMidConeDeg = 30.f;
        _float fMidTwistDeg = 15.f;

        _float fMassRoot = 0.08f;
        _float fMassTip = 0.02f;

        _int   iSolverIters = 10; // Jolt 전역
    };

    struct BCP_Chain
    {
        std::vector<JPH::BodyID>             vBodies;   // 세그먼트당 1개 (N-1)
        std::vector<JPH::TwoBodyConstraint*> vCons;     // 모든 연결/제한
        std::vector<int>                     vBoneIdx;  // [root, b1, ...] (N개)
        std::vector<uint32_t>                vSubGroup; // ★ 각 바디 SubGroupID
        _float                               fRadius = 0.015f;
    };

private:
    CBoneChainPhysic(JPH::PhysicsSystem* pPhys, JPH::BodyInterface* pBI);
    virtual ~CBoneChainPhysic();

public:
    HRESULT Initialize(class CModel* pModel, const BCP_BuildDesc& tBuild, const BCP_RuntimeParams& tParams,
        _uint iRootObjectLayer, _uint iBoneChainObjectLayer, class CTransform** pTransform);

    // 프레임 루틴
    void SyncRootProxy_PrePhysics(class CModel* pModel, _float fTimeDelta);          // ★ dt 반영
    void ApplyBodiesToBones_PostPhysics(class CModel* pModel, _float fTimeDelta);

    // 이벤트/제어
    void TeleportSnapToSkeleton(class CModel* pModel);
    void SetEnabled(_bool isEnable);

    // 프리셋 스위치
    void ApplyPreset(EBCP_Preset ePreset);

private:
    // 생성/파괴
    JPH::BodyID CreateRootKinematicProxy(class CModel* pModel, _int iHeadBone, _float fRadius, _float fHalfHeight);
    void        BuildChain(class CModel* pModel, const std::vector<int>& vBoneIdx, _float fRadius);
    void        DestroyAll();

private:
    // 내부 유틸
    static JPH::Vec3  ToJ(const _float3& v);
    static JPH::Quat  ToJQ(const _float4& q);
    static _float3    GetBoneWorldPos(CModel* pModel, _int iBoneIdx, _matrix OwnerMatrix);
    static _float4    GetBoneWorldRot(CModel* pModel, _int iBoneIdx, _matrix OwnerMatrix);
    void              SetChildBoneBySegment(CModel* pModel, _int iParentBone, _int iChildBone,
        const JPH::Vec3& vFrom, const JPH::Vec3& vTo);
    void              ConfigureByPreset(EBCP_Preset ePreset);

private:
    // 외부
    JPH::PhysicsSystem* m_pPhysics = nullptr;
    JPH::BodyInterface* m_pBI = nullptr;

    // 리소스
    JPH::BodyID         m_tRootProxy; // Kinematic (Head/Neck 등)
    std::vector<BCP_Chain> m_vChains;

    // 파라미터
    BCP_RuntimeParams   m_tParams;
    EBCP_Preset         m_ePreset = BCP_PRESET_Hair;

    // 루트 프록시 모양
    _float m_fRootProxyRadius = 0.02f;
    _float m_fRootProxyHalf = 0.03f;

    // 상태
    _bool  m_isEnabled = true;
    _int   m_iHeadBone = -1;

    _uint  m_iRootObjectLayer = {};
    _uint  m_iBoneChainObjectLayer = {};

    class CTransform* m_pOwnerTransform = { nullptr };

    // 루트 포즈/속도 추정
    JPH::Vec3  m_prevRootPos = JPH::Vec3::sZero();
    JPH::Quat  m_prevRootRot = JPH::Quat::sIdentity();
    _bool      m_hasPrevRoot = false;
    _float     m_lastDT = 1.f / 60.f;

    // ★ 충돌 그룹/필터
    JPH::Ref<JPH::GroupFilterTable> m_pGroupFilter; // 캐릭터 전용
    uint32_t                        m_GroupID = 0;
    uint32_t                        m_NextSubGroup = 0;


public:
    static CBoneChainPhysic* Create(JPH::PhysicsSystem* pPhys, JPH::BodyInterface* pBI, class CModel* pModel,
        const BCP_BuildDesc& tBuild, const BCP_RuntimeParams& tParams,
        _uint iRootObjectLayer, _uint iBoneChainObjectLayer, class CTransform** pTransform);
    virtual void Free() override;
};

NS_END
