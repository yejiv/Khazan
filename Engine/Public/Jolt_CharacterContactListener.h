#pragma once
#include "Engine_Defines.h"

#ifdef new
#pragma push_macro("new")
#undef new
#endif

#include <Jolt/Physics/Character/CharacterVirtual.h>
#include <Jolt/Physics/Body/BodyID.h>

#ifdef new
#pragma pop_macro("new")
#endif

NS_BEGIN(Engine)

// JPH 네임스페이스를 자주 쓰면 using 해도 OK
// using namespace JPH;

class CJolt_CharacterContactListener final : public JPH::CharacterContactListener
{
public:
    explicit CJolt_CharacterContactListener(class BodyInterface* pBodyInterface);
    ~CJolt_CharacterContactListener() override;

    // ---- CharacterContactListener 인터페이스 ----
    // (1) 바디와 접촉 가능한지(필요하면 필터링)
    bool OnContactValidate(const JPH::CharacterVirtual* inCharacter,
        const JPH::BodyID& inBodyID2,
        const JPH::SubShapeID& inSubShapeID2) override;

    // (2) 다른 캐릭터와 접촉 가능한지
    bool OnCharacterContactValidate(const JPH::CharacterVirtual* inCharacter,
        const JPH::CharacterVirtual* inOtherCharacter,
        const JPH::SubShapeID& inSubShapeID2) override;

    // (3) 최초 접촉
    void OnContactAdded(const JPH::CharacterVirtual* inCharacter,
        const JPH::BodyID& inBodyID2,
        const JPH::SubShapeID& inSubShapeID2,
        JPH::RVec3Arg inContactPosition,
        JPH::Vec3Arg inContactNormal,
        JPH::CharacterContactSettings& ioSettings) override;

    // (4) 접촉 유지
    void OnContactPersisted(const JPH::CharacterVirtual* inCharacter,
        const JPH::BodyID& inBodyID2,
        const JPH::SubShapeID& inSubShapeID2,
        JPH::RVec3Arg inContactPosition,
        JPH::Vec3Arg inContactNormal,
        JPH::CharacterContactSettings& ioSettings) override;

    // (5) 접촉 종료
    void OnContactRemoved(const JPH::CharacterVirtual* inCharacter,
        const JPH::BodyID& inBodyID2,
        const JPH::SubShapeID& inSubShapeID2) override;

    // Called whenever the character collides with a virtual character.
    void			OnCharacterContactAdded(const CharacterVirtual* inCharacter, const CharacterVirtual* inOtherCharacter, const SubShapeID& inSubShapeID2, RVec3Arg inContactPosition, Vec3Arg inContactNormal, CharacterContactSettings& ioSettings) override;

    // Called whenever the character persists colliding with a virtual character.
    void			OnCharacterContactPersisted(const CharacterVirtual* inCharacter, const CharacterVirtual* inOtherCharacter, const SubShapeID& inSubShapeID2, RVec3Arg inContactPosition, Vec3Arg inContactNormal, CharacterContactSettings& ioSettings) override;

    // Called whenever the character loses contact with a virtual character.
    void			OnCharacterContactRemoved(const CharacterVirtual* inCharacter, const CharacterID& inOtherCharacterID, const SubShapeID& inSubShapeID2) override;



private:
    class BodyInterface* m_pBodyInterface = { nullptr };
	class CGameInstance* m_pGameInstance = { nullptr };
};

NS_END // Engine
