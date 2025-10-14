#include "CharacterContactListener.h"

// 최초/유지 접촉 시 둘 다 같은 로직 사용
static inline void ApplyWalkableOnly(const CharacterVirtual* character,
    Vec3Arg contactNormal,
    float floor_dot,
    CharacterContactSettings& ioSettings)
{
    const Vec3 up = character->GetUp();

    // n·up 이 충분히 크면(= 경사 허용 범위 이내) -> '지면처럼' 그대로 둠
    // 작으면(가파른 경사/벽/천장) -> 캐릭터에 영향 최소화
    if (contactNormal.Dot(up) < floor_dot)
    {
        // 밀림/임펄스 영향 제거 → 사실상 '지면 아님'으로 취급
        ioSettings.mCanPushCharacter = false;
        ioSettings.mCanReceiveImpulses = false;
        // 추가 제어 필요 없으면 여기서 끝. (슬라이딩/비접지 처리는 CharacterVirtual 내부가 담당)
    }
}

CCharacterContactListener::CCharacterContactListener(const Config& cfg)
    : m_Cfg {cfg}
{
}

void CCharacterContactListener::OnContactAdded(const JPH::CharacterVirtual* inCharacter, const JPH::BodyID& inBodyID2, const JPH::SubShapeID& inSubShapeID2, JPH::RVec3Arg inContactPosition, JPH::Vec3Arg inContactNormal, JPH::CharacterContactSettings& ioSettings)
{
    ApplyWalkableOnly(inCharacter, inContactNormal, m_Cfg.floor_dot, ioSettings);
}

void CCharacterContactListener::OnContactPersisted(const JPH::CharacterVirtual* inCharacter, const JPH::BodyID& inBodyID2, const JPH::SubShapeID& inSubShapeID2, JPH::RVec3Arg inContactPosition, JPH::Vec3Arg inContactNormal, JPH::CharacterContactSettings& ioSettings)
{
    ApplyWalkableOnly(inCharacter, inContactNormal, m_Cfg.floor_dot, ioSettings);
}

