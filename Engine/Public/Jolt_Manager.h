#pragma once
#include "Base.h"

#ifdef _DEBUG
#include "PhysicsDebugRenderer.h"
#endif

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
        case JoltLayers::MAP: return JoltBPLayers::NON_MOVING;
        case JoltLayers::DYNAMIC:     return JoltBPLayers::MOVING;
        case JoltLayers::TRIGGER:     return JoltBPLayers::MOVING;
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
class CJolt_ObjectVsBPLayerFilter final : public ObjectVsBroadPhaseLayerFilter {
public:
    bool ShouldCollide(ObjectLayer ol, BroadPhaseLayer bpl) const override {
        switch (ol) {
        case JoltLayers::MAP:       return bpl == JoltBPLayers::MOVING; // 정적은 동적쪽만
        case JoltLayers::DYNAMIC:   return true;                        // 동적은 모두
        case JoltLayers::CHARACTER: return true;                        // 캐릭터도 모두
        case JoltLayers::TRIGGER:   return bpl == JoltBPLayers::MOVING; // 트리거는 움직이는 쪽만
        default:                    return true;
        }
    }
};

// ObjectLayer ↔ ObjectLayer 충돌 필터
class CJolt_ObjectLayerPairFilter final : public ObjectLayerPairFilter
{
public:
    bool ShouldCollide(ObjectLayer inObject1, ObjectLayer inObject2) const override
    {
        auto allow = [](ObjectLayer X, ObjectLayer Y) {
            if (X == JoltLayers::MAP && Y == JoltLayers::MAP)        return false; // 정적-정적 X
            if (X == JoltLayers::TRIGGER && Y == JoltLayers::TRIGGER)    return false; // 트리거-트리거 X
            if (X == JoltLayers::CHARACTER && Y == JoltLayers::CHARACTER)  return false; // 캐릭터-캐릭터 X (원하면 true)
            // 기본 허용 쌍 (필요한 조합만 true로)
            if (X == JoltLayers::MAP && Y == JoltLayers::DYNAMIC)    return true;
            if (X == JoltLayers::MAP && Y == JoltLayers::CHARACTER)  return true;
            if (X == JoltLayers::DYNAMIC && Y == JoltLayers::DYNAMIC)    return true;
            if (X == JoltLayers::DYNAMIC && Y == JoltLayers::CHARACTER)  return true;
            if (X == JoltLayers::TRIGGER && Y == JoltLayers::DYNAMIC)    return true;
            if (X == JoltLayers::TRIGGER && Y == JoltLayers::CHARACTER)  return true;
            if (X == JoltLayers::TRIGGER && Y == JoltLayers::MAP)        return true; // 맵 트리거 감지도 원하면
            return false;
            };
        return allow(inObject1, inObject2) || allow(inObject2, inObject1);
    }
};

// Contact Listener (옵션: 필요 시 접촉 이벤트 훅)
class CJolt_ContactListener final : public ContactListener
{
public:
    ValidateResult OnContactValidate(const Body& A, const Body& B, RVec3Arg, const CollideShapeResult&) override {
        return ValidateResult::AcceptAllContactsForThisBodyPair;
    }
    void OnContactAdded(const Body& A, const Body& B, const ContactManifold& M, ContactSettings&) override {
        // 예시) 트리거 Enter 감지
        // if (A.IsSensor() || B.IsSensor()) { ... }
        // 게임오브젝트 포인터는 Body::GetUserData()에 넣어두고 꺼내 쓰기
    }
    void OnContactPersisted(const Body&, const Body&, const ContactManifold&, ContactSettings&) override {}
    void OnContactRemoved(const SubShapeIDPair&) override {}
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

private:
    // 필수 구성요소
    PhysicsSystem           m_Physics;
    TempAllocatorImpl* m_pTempAlloc = nullptr;
    JobSystemThreadPool* m_pJobSystem = nullptr;

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

#ifdef _DEBUG

public:
    void Debug_Render();

private:
    //CPhysicsDebugRenderer m_DebugRenderer;
#endif

public:
    // 엔진 매니저 패턴
    static CJolt_Manager* Create();
    virtual void Free() override;
};

NS_END
