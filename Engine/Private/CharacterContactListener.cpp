#include "CharacterContactListener.h"

// -------------------- 생성자 --------------------
CCharacterContactListener::CCharacterContactListener(const CONFIG_DESC& cfg)
    : m_Cfg(cfg)
{
}

// -------------------- Validate 계열 --------------------
bool CCharacterContactListener::OnContactValidate(const CharacterVirtual*,
    const BodyID&,
    const SubShapeID&)
{
    // 기본은 모두 허용. 필요 시 레이어/태그 기반 필터링 추가
    return true;
}

bool CCharacterContactListener::OnCharacterContactValidate(const CharacterVirtual*,
    const CharacterVirtual*,
    const SubShapeID&)
{
    // 기본은 캐릭터-캐릭터 접촉 허용
    return true;
}

// -------------------- Added/Persisted/Removed --------------------
void CCharacterContactListener::OnContactAdded(const CharacterVirtual* inCharacter,
    const BodyID&,
    const SubShapeID&,
    RVec3Arg,
    Vec3Arg inContactNormal,
    CharacterContactSettings& ioSettings)
{
  
}

void CCharacterContactListener::OnContactPersisted(const CharacterVirtual* inCharacter,
    const BodyID&,
    const SubShapeID&,
    RVec3Arg,
    Vec3Arg inContactNormal,
    CharacterContactSettings& ioSettings)
{
   
}

void CCharacterContactListener::OnContactRemoved(const CharacterVirtual*,
    const BodyID&,
    const SubShapeID&)
{
    // 접촉 하나가 사라졌다는 뜻일 뿐, 아직 다른 접촉이 있을 수 있음.
    // 간단히는 캐시를 지우지 않거나, 프레임 끝에서 m_bHasGround를 재평가(권장).
    // 여기서는 즉시 플래그만 false로:
    m_bHasGround = false;
}
