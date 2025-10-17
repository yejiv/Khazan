#include "Jolt_CharacterContactListener.h"
#include "GameObject.h"

// -------------------- »ý¼ºÀÚ --------------------
CJolt_CharacterContactListener::CJolt_CharacterContactListener(BodyInterface* pBodyInterface)
    : m_pBodyInterface { pBodyInterface }
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
    CGameObject* pCharGameObject = reinterpret_cast<CGameObject*>(static_cast<std::uintptr_t>(inCharacter->GetUserData()));
    CGameObject* pBodyGameObject = reinterpret_cast<CGameObject*>(static_cast<std::uintptr_t>(m_pBodyInterface->GetUserData(inBodyID2)));

    if (pCharGameObject != nullptr && pBodyGameObject != nullptr)
    {
        pCharGameObject->Collision_Enter(pBodyGameObject, JOLT_COLLSION_TYPE::BODY);
        pBodyGameObject->Collision_Enter(pCharGameObject, JOLT_COLLSION_TYPE::CHARVIR);
    }
}

void CJolt_CharacterContactListener::OnContactPersisted(const JPH::CharacterVirtual* inCharacter, const JPH::BodyID& inBodyID2, const JPH::SubShapeID& inSubShapeID2, JPH::RVec3Arg inContactPosition, JPH::Vec3Arg inContactNormal, JPH::CharacterContactSettings& ioSettings)
{

    CGameObject* pCharGameObject = reinterpret_cast<CGameObject*>(static_cast<std::uintptr_t>(inCharacter->GetUserData()));
    CGameObject* pBodyGameObject = reinterpret_cast<CGameObject*>(static_cast<std::uintptr_t>(m_pBodyInterface->GetUserData(inBodyID2)));

    if (pCharGameObject != nullptr && pBodyGameObject != nullptr)
    {
        pCharGameObject->Collision_Stay(pBodyGameObject, JOLT_COLLSION_TYPE::BODY);
        pBodyGameObject->Collision_Stay(pCharGameObject, JOLT_COLLSION_TYPE::CHARVIR);
    }
        
}

void CJolt_CharacterContactListener::OnContactRemoved(const JPH::CharacterVirtual* inCharacter, const JPH::BodyID& inBodyID2, const JPH::SubShapeID& inSubShapeID2)
{
}

void CJolt_CharacterContactListener::OnCharacterContactAdded(const CharacterVirtual* inCharacter, const CharacterVirtual* inOtherCharacter, const SubShapeID& inSubShapeID2, RVec3Arg inContactPosition, Vec3Arg inContactNormal, CharacterContactSettings& ioSettings)
{
    CGameObject* pGameObject1 = reinterpret_cast<CGameObject*>(static_cast<std::uintptr_t>(inCharacter->GetUserData()));
    CGameObject* pGameObject2 = reinterpret_cast<CGameObject*>(static_cast<std::uintptr_t>(inOtherCharacter->GetUserData()));

    if (pGameObject1 != nullptr && pGameObject2 != nullptr)
    {
        pGameObject1->Collision_Enter(pGameObject2, JOLT_COLLSION_TYPE::CHARVIR);
        pGameObject2->Collision_Enter(pGameObject1, JOLT_COLLSION_TYPE::CHARVIR);
    }
        

    ioSettings.mCanPushCharacter = true;
    
    return;
}

void CJolt_CharacterContactListener::OnCharacterContactPersisted(const CharacterVirtual* inCharacter, const CharacterVirtual* inOtherCharacter, const SubShapeID& inSubShapeID2, RVec3Arg inContactPosition, Vec3Arg inContactNormal, CharacterContactSettings& ioSettings)
{
    CGameObject* pGameObject1 = reinterpret_cast<CGameObject*>(static_cast<std::uintptr_t>(inCharacter->GetUserData()));
    CGameObject* pGameObject2 = reinterpret_cast<CGameObject*>(static_cast<std::uintptr_t>(inOtherCharacter->GetUserData()));


    if (pGameObject1 != nullptr && pGameObject2 != nullptr)
    {
        pGameObject1->Collision_Stay(pGameObject2, JOLT_COLLSION_TYPE::CHARVIR);
        pGameObject2->Collision_Stay(pGameObject1, JOLT_COLLSION_TYPE::CHARVIR);
    }
    ioSettings.mCanPushCharacter = true;

    return;
}

void CJolt_CharacterContactListener::OnCharacterContactRemoved(const CharacterVirtual* inCharacter, const CharacterID& inOtherCharacterID, const SubShapeID& inSubShapeID2)
{
    return;
}
