#include "Jolt_CharacterContactListener.h"
#include "GameObject.h"
#include "GameInstance.h"

// -------------------- »ý¼ºÀÚ --------------------
CJolt_CharacterContactListener::CJolt_CharacterContactListener(BodyInterface* pBodyInterface)
    : m_pBodyInterface { pBodyInterface }
    , m_pGameInstance { CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

CJolt_CharacterContactListener::~CJolt_CharacterContactListener()
{
    Safe_Release(m_pGameInstance);
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

    ObjectLayer CharObj = m_pBodyInterface->GetObjectLayer(inCharacter->GetInnerBodyID());
    ObjectLayer BodyObj = m_pBodyInterface->GetObjectLayer(inBodyID2);

    if (pCharGameObject->Get_IsDead())
    {
        m_pGameInstance->Remove_CharacterVirtual(inCharacter->GetID());
    }

    if (pCharGameObject != nullptr && pBodyGameObject != nullptr && !pCharGameObject->Get_IsDead() && !pBodyGameObject->Get_IsDead())
    {
        pCharGameObject->Collision_Enter(pBodyGameObject, static_cast<_uint>(BodyObj), 
            _float3(inContactPosition.GetX(), inContactPosition.GetY(), inContactPosition.GetZ()),
            _float3(inContactNormal.GetX(), inContactNormal.GetY(), inContactNormal.GetZ()));
        pBodyGameObject->Collision_Enter(pCharGameObject, static_cast<_uint>(CharObj),
            _float3(inContactPosition.GetX(), inContactPosition.GetY(), inContactPosition.GetZ()),
            _float3(inContactNormal.GetX(), inContactNormal.GetY(), inContactNormal.GetZ()));
    }
}

void CJolt_CharacterContactListener::OnContactPersisted(const JPH::CharacterVirtual* inCharacter, const JPH::BodyID& inBodyID2, const JPH::SubShapeID& inSubShapeID2, JPH::RVec3Arg inContactPosition, JPH::Vec3Arg inContactNormal, JPH::CharacterContactSettings& ioSettings)
{

    CGameObject* pCharGameObject = reinterpret_cast<CGameObject*>(static_cast<std::uintptr_t>(inCharacter->GetUserData()));
    CGameObject* pBodyGameObject = reinterpret_cast<CGameObject*>(static_cast<std::uintptr_t>(m_pBodyInterface->GetUserData(inBodyID2)));


    ObjectLayer CharObj = m_pBodyInterface->GetObjectLayer(inCharacter->GetInnerBodyID());
    ObjectLayer BodyObj = m_pBodyInterface->GetObjectLayer(inBodyID2);

    if (pCharGameObject->Get_IsDead())
    {
        m_pGameInstance->Remove_CharacterVirtual(inCharacter->GetID());
    }

    if (pCharGameObject != nullptr && pBodyGameObject != nullptr && !pCharGameObject->Get_IsDead() && !pBodyGameObject->Get_IsDead())
    {
        pCharGameObject->Collision_Stay(pBodyGameObject, static_cast<_uint>(BodyObj),
            _float3(inContactPosition.GetX(), inContactPosition.GetY(), inContactPosition.GetZ()),
            _float3(inContactNormal.GetX(), inContactNormal.GetY(), inContactNormal.GetZ()));
        pBodyGameObject->Collision_Stay(pCharGameObject, static_cast<_uint>(CharObj), 
            _float3(inContactPosition.GetX(), inContactPosition.GetY(), inContactPosition.GetZ()),
            _float3(inContactNormal.GetX(), inContactNormal.GetY(), inContactNormal.GetZ()));
    }
        
}

void CJolt_CharacterContactListener::OnContactRemoved(const JPH::CharacterVirtual* inCharacter, const JPH::BodyID& inBodyID2, const JPH::SubShapeID& inSubShapeID2)
{
    CGameObject* pCharGameObject = reinterpret_cast<CGameObject*>(static_cast<std::uintptr_t>(inCharacter->GetUserData()));
    CGameObject* pBodyGameObject = reinterpret_cast<CGameObject*>(static_cast<std::uintptr_t>(m_pBodyInterface->GetUserData(inBodyID2)));

	ObjectLayer CharObj = m_pBodyInterface->GetObjectLayer(inCharacter->GetInnerBodyID());
	ObjectLayer BodyObj = m_pBodyInterface->GetObjectLayer(inBodyID2);

    if (pCharGameObject->Get_IsDead())
    {
        m_pGameInstance->Remove_CharacterVirtual(inCharacter->GetID());
    }

	if (pCharGameObject != nullptr && pBodyGameObject != nullptr && !pCharGameObject->Get_IsDead() && !pBodyGameObject->Get_IsDead())
	{
		pCharGameObject->Collision_Exit(pBodyGameObject, static_cast<_uint>(BodyObj));
		pBodyGameObject->Collision_Exit(pCharGameObject, static_cast<_uint>(CharObj));
	}
}

void CJolt_CharacterContactListener::OnCharacterContactAdded(const CharacterVirtual* inCharacter, const CharacterVirtual* inOtherCharacter, const SubShapeID& inSubShapeID2, RVec3Arg inContactPosition, Vec3Arg inContactNormal, CharacterContactSettings& ioSettings)
{
    CGameObject* pGameObject1 = reinterpret_cast<CGameObject*>(static_cast<std::uintptr_t>(inCharacter->GetUserData()));
    CGameObject* pGameObject2 = reinterpret_cast<CGameObject*>(static_cast<std::uintptr_t>(inOtherCharacter->GetUserData()));

    ObjectLayer CharObj1 = m_pBodyInterface->GetObjectLayer(inCharacter->GetInnerBodyID());
    ObjectLayer CharObj2 = m_pBodyInterface->GetObjectLayer(inOtherCharacter->GetInnerBodyID());

    if (pGameObject1->Get_IsDead())
    {
        m_pGameInstance->Remove_CharacterVirtual(inCharacter->GetID());
    }
    if (pGameObject2->Get_IsDead())
    {
        m_pGameInstance->Remove_CharacterVirtual(inOtherCharacter->GetID());
    }


    if (pGameObject1 != nullptr && pGameObject2 != nullptr && !pGameObject1->Get_IsDead() && !pGameObject2->Get_IsDead())
    {
        pGameObject1->Collision_Enter(pGameObject2, static_cast<_uint>(CharObj2),
            _float3(inContactPosition.GetX(), inContactPosition.GetY(), inContactPosition.GetZ()),
            _float3(inContactNormal.GetX(), inContactNormal.GetY(), inContactNormal.GetZ()));
        pGameObject2->Collision_Enter(pGameObject1, static_cast<_uint>(CharObj1),
            _float3(inContactPosition.GetX(), inContactPosition.GetY(), inContactPosition.GetZ()),
            _float3(inContactNormal.GetX(), inContactNormal.GetY(), inContactNormal.GetZ()));
    }
        

    ioSettings.mCanPushCharacter = true;
    
    return;
}

void CJolt_CharacterContactListener::OnCharacterContactPersisted(const CharacterVirtual* inCharacter, const CharacterVirtual* inOtherCharacter, const SubShapeID& inSubShapeID2, RVec3Arg inContactPosition, Vec3Arg inContactNormal, CharacterContactSettings& ioSettings)
{
    CGameObject* pGameObject1 = reinterpret_cast<CGameObject*>(static_cast<std::uintptr_t>(inCharacter->GetUserData()));
    CGameObject* pGameObject2 = reinterpret_cast<CGameObject*>(static_cast<std::uintptr_t>(inOtherCharacter->GetUserData()));

    ObjectLayer CharObj1 = m_pBodyInterface->GetObjectLayer(inCharacter->GetInnerBodyID());
    ObjectLayer CharObj2 = m_pBodyInterface->GetObjectLayer(inOtherCharacter->GetInnerBodyID());

    if (pGameObject1->Get_IsDead())
    {
        m_pGameInstance->Remove_CharacterVirtual(inCharacter->GetID());
    }
    if (pGameObject2->Get_IsDead())
    {
        m_pGameInstance->Remove_CharacterVirtual(inOtherCharacter->GetID());
    }

    if (pGameObject1 != nullptr && pGameObject2 != nullptr && !pGameObject1->Get_IsDead() && !pGameObject2->Get_IsDead())
    {
        pGameObject1->Collision_Stay(pGameObject2, static_cast<_uint>(CharObj2),
            _float3(inContactPosition.GetX(), inContactPosition.GetY(), inContactPosition.GetZ()),
            _float3(inContactNormal.GetX(), inContactNormal.GetY(), inContactNormal.GetZ()));
        pGameObject2->Collision_Stay(pGameObject1, static_cast<_uint>(CharObj1),
            _float3(inContactPosition.GetX(), inContactPosition.GetY(), inContactPosition.GetZ()),
            _float3(inContactNormal.GetX(), inContactNormal.GetY(), inContactNormal.GetZ()));
    }
    ioSettings.mCanPushCharacter = true;

    return;
}

void CJolt_CharacterContactListener::OnCharacterContactRemoved(const CharacterVirtual* inCharacter, const CharacterID& inOtherCharacterID, const SubShapeID& inSubShapeID2)
{
    CGameObject* pGameObject1 = reinterpret_cast<CGameObject*>(static_cast<std::uintptr_t>(inCharacter->GetUserData()));
    
	CharacterVirtual* pCharVir = m_pGameInstance->Find_CharacterVirtual(inOtherCharacterID);
    CGameObject* pGameObject2 = reinterpret_cast<CGameObject*>(static_cast<std::uintptr_t>(pCharVir->GetUserData()));

    ObjectLayer CharObj1 = m_pBodyInterface->GetObjectLayer(inCharacter->GetInnerBodyID());
    ObjectLayer CharObj2 = m_pBodyInterface->GetObjectLayer(pCharVir->GetInnerBodyID());

    if (pGameObject1->Get_IsDead())
    {
        m_pGameInstance->Remove_CharacterVirtual(inCharacter->GetID());
    }
    if (pGameObject2->Get_IsDead())
    {
        m_pGameInstance->Remove_CharacterVirtual(inOtherCharacterID);
    }

    if (pGameObject1 != nullptr && pGameObject2 != nullptr && !pGameObject1->Get_IsDead() && !pGameObject2->Get_IsDead())
    {
        pGameObject1->Collision_Exit(pGameObject2, static_cast<_uint>(CharObj2));
        pGameObject2->Collision_Exit(pGameObject1, static_cast<_uint>(CharObj1));
    }
    

    return;
}
