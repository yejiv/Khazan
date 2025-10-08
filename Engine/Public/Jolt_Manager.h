#pragma once
#include "Base.h"


#ifdef new
#pragma push_macro("new")
#undef new
#endif

#include <Core/Factory.h>

#ifdef new
#pragma pop_macro("new") // DBG_NEW 복원
#endif

NS_BEGIN(Engine)

// -------------------------------
// ObjectLayer/BroadPhaseLayer 정의
// -------------------------------
namespace JoltLayers
{
    // 너의 용도에 맞게 늘려 써도 됨
    static constexpr ObjectLayer    NON_MOVING = 0;
    static constexpr ObjectLayer    MOVING = 1;
    static constexpr ObjectLayer    SENSOR = 2;
    static constexpr uint32         NUM_LAYERS = 3;
}

// 브로드페이즈 레이어 (간단 매핑: 0=정적, 1=동적)
namespace JoltBPLayers
{
    static constexpr BroadPhaseLayer NON_MOVING(0);
    static constexpr BroadPhaseLayer MOVING(1);
    static constexpr uint32          NUM_LAYERS = 2;
}

// BroadPhaseLayerInterface 구현
class CJolt_BPLayerIF final : public BroadPhaseLayerInterface
{
public:
    CJolt_BPLayerIF() = default;
    virtual uint32      GetNumBroadPhaseLayers() const override { return JoltBPLayers::NUM_LAYERS; }
    virtual BroadPhaseLayer GetBroadPhaseLayer(ObjectLayer inLayer) const override
    {
        switch (inLayer)
        {
        case JoltLayers::NON_MOVING: return JoltBPLayers::NON_MOVING;
        case JoltLayers::MOVING:     return JoltBPLayers::MOVING;
        case JoltLayers::SENSOR:     return JoltBPLayers::MOVING;
        default:                     return JoltBPLayers::MOVING;
        }
    }
#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
    virtual const char* GetBroadPhaseLayerName(BroadPhaseLayer inLayer) const override
    {
        switch ((uint32)inLayer.GetValue())
        {
        case 0: return "NON_MOVING";
        case 1: return "MOVING";
        default: return "UNKNOWN";
        }
    }
#endif
};

// ObjectLayer ↔ BroadPhaseLayer 충돌 필터
class CJolt_ObjectVsBPLayerFilter final : public ObjectVsBroadPhaseLayerFilter
{
public:
    virtual bool ShouldCollide(ObjectLayer inLayer1, BroadPhaseLayer inLayer2) const override
    {
        if (inLayer1 == JoltLayers::NON_MOVING) return inLayer2 == JoltBPLayers::MOVING;
        if (inLayer1 == JoltLayers::MOVING)     return true; // 동적은 전부 검사
        if (inLayer1 == JoltLayers::SENSOR)     return inLayer2 == JoltBPLayers::MOVING;
        return true;
    }
};

// ObjectLayer ↔ ObjectLayer 충돌 필터
class CJolt_ObjectLayerPairFilter final : public ObjectLayerPairFilter
{
public:
    virtual bool ShouldCollide(ObjectLayer inObject1, ObjectLayer inObject2) const override
    {
        if (inObject1 == JoltLayers::SENSOR || inObject2 == JoltLayers::SENSOR)
            return true; // 센서는 모두와 접촉 체크 (원하면 바꿔)
        if (inObject1 == JoltLayers::NON_MOVING && inObject2 == JoltLayers::NON_MOVING)
            return false; // 정적-정적 무시
        return true;
    }
};

// Contact Listener (옵션: 필요 시 접촉 이벤트 훅)
class CJolt_ContactListener final : public ContactListener
{
public:
    virtual ValidateResult    OnContactValidate(const Body& inBody1, const Body& inBody2, RVec3Arg inBaseOffset, const CollideShapeResult& inCollisionResult) override
    {
        // 기본: 모두 허용
        return ValidateResult::AcceptAllContactsForThisBodyPair;
    }
    virtual void OnContactAdded(const Body& inBody1, const Body& inBody2, const ContactManifold& inManifold, ContactSettings& ioSettings) override {}
    virtual void OnContactPersisted(const Body& inBody1, const Body& inBody2, const ContactManifold& inManifold, ContactSettings& ioSettings) override {}
    virtual void OnContactRemoved(const SubShapeIDPair& inSubShapePair) override {}
};

// -------------------------------
// Jolt Manager
// -------------------------------
class CJolt_Manager final : public CBase
{
private:
    CJolt_Manager();
    virtual ~CJolt_Manager() = default;

public:
    // 초기화/종료/스텝
    HRESULT Initialize(_uint iMaxBodies = 10240, _uint iNumBodyMutexes = 0, _uint iMaxBodyPairs = 65536, _uint iMaxContactConstraints = 65536, _uint iJobThreadCount = 0);
    void    Update(_float fDeltaTime);
    void    Clear(); // 모든 바디 제거

    // 편의 접근자
    PhysicsSystem& Get_PhysicsSystem() { return m_Physics; }
    BodyInterface& Get_BodyInterface() { return m_Physics.GetBodyInterface(); }

    // 디버그용 중력/스텝 고정값 설정
    void    Set_Gravity(const Vec3& vGravity) { m_Physics.SetGravity(vGravity); }
    Vec3    Get_Gravity() const { return m_Physics.GetGravity(); }

public:
    // 엔진 매니저 패턴
    static CJolt_Manager* Create();
    virtual void Free() override;

private:
    // 필수 구성요소
    PhysicsSystem           m_Physics;
    TempAllocatorImpl*      m_pTempAlloc = nullptr;
    JobSystemThreadPool*    m_pJobSystem = nullptr;

    // 레이어/필터/리스너
    CJolt_BPLayerIF             m_BPLayerIF;
    CJolt_ObjectVsBPLayerFilter m_ObjectVsBPLayerFilter;
    CJolt_ObjectLayerPairFilter m_ObjectPairFilter;
    CJolt_ContactListener       m_ContactListener;

private:
    // 생성 파라미터 보관(선택)
    _uint m_iMaxBodies = 0;
    _uint m_iNumBodyMutexes = 0;
    _uint m_iMaxBodyPairs = 0;
    _uint m_iMaxContactConstraints = 0;
    _uint m_iJobThreadCount = 0;
};

NS_END
