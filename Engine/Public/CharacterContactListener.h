#pragma once

#include "Engine_Defines.h"

#ifdef new
#pragma push_macro("new")
#undef new
#endif

#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Body/BodyLockInterface.h>
#include <Jolt/Physics/Character/CharacterVirtual.h>

#ifdef new
#pragma pop_macro("new")
#endif

NS_BEGIN(Engine)

class ENGINE_DLL CCharacterContactListener final : public CharacterContactListener
{
public:
    typedef struct Config
    {
        // n·up >= floor_dot 이면 '설 수 있음'으로 취급 (예: 50도 → cos 50°)
        float floor_dot = JPH::Cos(JPH::DegreesToRadians(45.0f));
    }CONFIG_DESC;

public:
    explicit CCharacterContactListener(const Config& cfg);

    // 나머지는 전부 기본 허용. 실제 ‘지면 여부’는 Add/Persist에서 법선으로만 간단히 체크
    bool OnContactValidate(const JPH::CharacterVirtual*, const JPH::BodyID&, const JPH::SubShapeID&) override { return true; }
    bool OnCharacterContactValidate(const JPH::CharacterVirtual*, const JPH::CharacterVirtual*, const JPH::SubShapeID&) override { return true; }

    // 최초/유지 접촉 시, 경사만 체크해서 ‘지면처럼’ 다루도록 설정
    void OnContactAdded(const JPH::CharacterVirtual* inCharacter,
        const JPH::BodyID& inBodyID2,
        const JPH::SubShapeID& inSubShapeID2,
        JPH::RVec3Arg inContactPosition,
        JPH::Vec3Arg inContactNormal,
        JPH::CharacterContactSettings& ioSettings) override;

    void OnContactPersisted(const JPH::CharacterVirtual* inCharacter,
        const JPH::BodyID& inBodyID2,
        const JPH::SubShapeID& inSubShapeID2,
        JPH::RVec3Arg inContactPosition,
        JPH::Vec3Arg inContactNormal,
        JPH::CharacterContactSettings& ioSettings) override;

    // 제거 콜백은 사용 안 함
    void OnContactRemoved(const JPH::CharacterVirtual*, const JPH::BodyID&, const JPH::SubShapeID&) override {}

private:
    CONFIG_DESC m_Cfg;
};

NS_END // Engine
