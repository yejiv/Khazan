#include "Jolt_CharacterContactListener.h"
#include "GameObject.h"

// -------------------- »ý¼ºÀÚ --------------------
CJolt_CharacterContactListener::CJolt_CharacterContactListener(const CONFIG_DESC& cfg)
    : m_Cfg(cfg)
{
}

bool CJolt_CharacterContactListener::OnContactValidate(const JPH::CharacterVirtual* inCharacter, const JPH::BodyID& inBodyID2, const JPH::SubShapeID& inSubShapeID2)
{
    return true;
}

bool CJolt_CharacterContactListener::OnCharacterContactValidate(const JPH::CharacterVirtual* inCharacter, const JPH::CharacterVirtual* inOtherCharacter, const JPH::SubShapeID& inSubShapeID2)
{
    return true;
}

void CJolt_CharacterContactListener::OnContactAdded(const JPH::CharacterVirtual* inCharacter, const JPH::BodyID& inBodyID2, const JPH::SubShapeID& inSubShapeID2, JPH::RVec3Arg inContactPosition, JPH::Vec3Arg inContactNormal, JPH::CharacterContactSettings& ioSettings)
{
    CGameObject* pGameObject1 = reinterpret_cast<CGameObject*>(static_cast<std::uintptr_t>(inCharacter->GetUserData()));
}

void CJolt_CharacterContactListener::OnContactPersisted(const JPH::CharacterVirtual* inCharacter, const JPH::BodyID& inBodyID2, const JPH::SubShapeID& inSubShapeID2, JPH::RVec3Arg inContactPosition, JPH::Vec3Arg inContactNormal, JPH::CharacterContactSettings& ioSettings)
{
    CGameObject* pGameObject1 = reinterpret_cast<CGameObject*>(static_cast<std::uintptr_t>(inCharacter->GetUserData()));
}

void CJolt_CharacterContactListener::OnContactRemoved(const JPH::CharacterVirtual* inCharacter, const JPH::BodyID& inBodyID2, const JPH::SubShapeID& inSubShapeID2)
{
    CGameObject* pGameObject1 = reinterpret_cast<CGameObject*>(static_cast<std::uintptr_t>(inCharacter->GetUserData()));
}

void CJolt_CharacterContactListener::OnCharacterContactAdded(const CharacterVirtual* inCharacter, const CharacterVirtual* inOtherCharacter, const SubShapeID& inSubShapeID2, RVec3Arg inContactPosition, Vec3Arg inContactNormal, CharacterContactSettings& ioSettings)
{
    CGameObject* pGameObject1 = reinterpret_cast<CGameObject*>(static_cast<std::uintptr_t>(inCharacter->GetUserData()));
    CGameObject* pGameObject2 = reinterpret_cast<CGameObject*>(static_cast<std::uintptr_t>(inOtherCharacter->GetUserData()));

    pGameObject1->Collision_Enter(pGameObject2);
    pGameObject2->Collision_Enter(pGameObject1);

    ioSettings.mCanPushCharacter = true;
    
    return;
}

void CJolt_CharacterContactListener::OnCharacterContactPersisted(const CharacterVirtual* inCharacter, const CharacterVirtual* inOtherCharacter, const SubShapeID& inSubShapeID2, RVec3Arg inContactPosition, Vec3Arg inContactNormal, CharacterContactSettings& ioSettings)
{
    CGameObject* pGameObject1 = reinterpret_cast<CGameObject*>(static_cast<std::uintptr_t>(inCharacter->GetUserData()));
    CGameObject* pGameObject2 = reinterpret_cast<CGameObject*>(static_cast<std::uintptr_t>(inOtherCharacter->GetUserData()));

    pGameObject1->Collision_Stay(pGameObject2);
    pGameObject2->Collision_Stay(pGameObject1);

    ioSettings.mCanPushCharacter = true;

    return;
}

void CJolt_CharacterContactListener::OnCharacterContactRemoved(const CharacterVirtual* inCharacter, const CharacterID& inOtherCharacterID, const SubShapeID& inSubShapeID2)
{
    return;
}
