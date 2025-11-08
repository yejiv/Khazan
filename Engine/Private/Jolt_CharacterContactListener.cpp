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
    if (m_pBodyInterface->GetMotionType(inBodyID2) == EMotionType::Static)
        return true;

    return false;
}

bool CJolt_CharacterContactListener::OnCharacterContactValidate(const JPH::CharacterVirtual* inCharacter, const JPH::CharacterVirtual* inOtherCharacter, const JPH::SubShapeID& inSubShapeID2)
{
    return true;
}

void CJolt_CharacterContactListener::OnContactAdded(const JPH::CharacterVirtual* inCharacter, const JPH::BodyID& inBodyID2, const JPH::SubShapeID& inSubShapeID2, JPH::RVec3Arg inContactPosition, JPH::Vec3Arg inContactNormal, JPH::CharacterContactSettings& ioSettings)
{
    if (m_pBodyInterface->GetMotionType(inBodyID2) == EMotionType::Static)
        ioSettings.mCanPushCharacter = true;
    

  /*  COLLISION_DESC* pCharDesc = reinterpret_cast<COLLISION_DESC*>(static_cast<std::uintptr_t>(inCharacter->GetUserData()));
    COLLISION_DESC* pBodyDesc = reinterpret_cast<COLLISION_DESC*>(static_cast<std::uintptr_t>(m_pBodyInterface->GetUserData(inBodyID2)));

    ObjectLayer CharObj = m_pBodyInterface->GetObjectLayer(inCharacter->GetInnerBodyID());
    ObjectLayer BodyObj = m_pBodyInterface->GetObjectLayer(inBodyID2);

    if (pCharDesc->pGameObject->Get_IsDead())
    {
        m_pGameInstance->Remove_CharacterVirtual(inCharacter->GetID());
    }

    if (pCharDesc == nullptr || pBodyDesc == nullptr || pCharDesc->pGameObject == nullptr || pBodyDesc->pGameObject == nullptr)
        return;

    if (pCharDesc->pGameObject != nullptr && pBodyDesc->pGameObject != nullptr && 
        !pCharDesc->pGameObject->Get_IsDead() && !pBodyDesc->pGameObject->Get_IsDead())
    {
        pCharDesc->pGameObject->Collision_Enter(pBodyDesc, static_cast<_uint>(BodyObj),
            _float3(inContactPosition.GetX(), inContactPosition.GetY(), inContactPosition.GetZ()),
            _float3(inContactNormal.GetX(), inContactNormal.GetY(), inContactNormal.GetZ()));
        pBodyDesc->pGameObject->Collision_Enter(pCharDesc, static_cast<_uint>(CharObj),
            _float3(inContactPosition.GetX(), inContactPosition.GetY(), inContactPosition.GetZ()),
            _float3(inContactNormal.GetX(), inContactNormal.GetY(), inContactNormal.GetZ()));
    }*/
}

void CJolt_CharacterContactListener::OnContactPersisted(const JPH::CharacterVirtual* inCharacter, const JPH::BodyID& inBodyID2, const JPH::SubShapeID& inSubShapeID2, JPH::RVec3Arg inContactPosition, JPH::Vec3Arg inContactNormal, JPH::CharacterContactSettings& ioSettings)
{
    if (m_pBodyInterface->GetMotionType(inBodyID2) == EMotionType::Static)
        ioSettings.mCanPushCharacter = true;
   /* COLLISION_DESC* pCharDesc = reinterpret_cast<COLLISION_DESC*>(static_cast<std::uintptr_t>(inCharacter->GetUserData()));
    COLLISION_DESC* pBodyDesc = reinterpret_cast<COLLISION_DESC*>(static_cast<std::uintptr_t>(m_pBodyInterface->GetUserData(inBodyID2)));

    ObjectLayer CharObj = m_pBodyInterface->GetObjectLayer(inCharacter->GetInnerBodyID());
    ObjectLayer BodyObj = m_pBodyInterface->GetObjectLayer(inBodyID2);

    if (pCharDesc->pGameObject->Get_IsDead())
    {
        m_pGameInstance->Remove_CharacterVirtual(inCharacter->GetID());
    }

    if (pCharDesc == nullptr || pBodyDesc == nullptr || pCharDesc->pGameObject == nullptr || pBodyDesc->pGameObject == nullptr)
        return;

    if (pCharDesc->pGameObject != nullptr && pBodyDesc->pGameObject != nullptr && 
        !pCharDesc->pGameObject->Get_IsDead() && !pBodyDesc->pGameObject->Get_IsDead())
    {
        pCharDesc->pGameObject->Collision_Stay(pBodyDesc, static_cast<_uint>(BodyObj),
            _float3(inContactPosition.GetX(), inContactPosition.GetY(), inContactPosition.GetZ()),
            _float3(inContactNormal.GetX(), inContactNormal.GetY(), inContactNormal.GetZ()));
        pBodyDesc->pGameObject->Collision_Stay(pCharDesc, static_cast<_uint>(CharObj),
            _float3(inContactPosition.GetX(), inContactPosition.GetY(), inContactPosition.GetZ()),
            _float3(inContactNormal.GetX(), inContactNormal.GetY(), inContactNormal.GetZ()));
    }*/
        
}

void CJolt_CharacterContactListener::OnContactRemoved(const JPH::CharacterVirtual* inCharacter, const JPH::BodyID& inBodyID2, const JPH::SubShapeID& inSubShapeID2)
{
   /* COLLISION_DESC* pCharDesc = reinterpret_cast<COLLISION_DESC*>(static_cast<std::uintptr_t>(inCharacter->GetUserData()));
    COLLISION_DESC* pBodyDesc = reinterpret_cast<COLLISION_DESC*>(static_cast<std::uintptr_t>(m_pBodyInterface->GetUserData(inBodyID2)));

	ObjectLayer CharObj = m_pBodyInterface->GetObjectLayer(inCharacter->GetInnerBodyID());
	ObjectLayer BodyObj = m_pBodyInterface->GetObjectLayer(inBodyID2);

    if (pCharDesc->pGameObject->Get_IsDead())
    {
        m_pGameInstance->Remove_CharacterVirtual(inCharacter->GetID());
    }

    if (pCharDesc == nullptr || pBodyDesc == nullptr || pCharDesc->pGameObject == nullptr || pBodyDesc->pGameObject == nullptr)
        return;

	if (pCharDesc->pGameObject != nullptr && pBodyDesc->pGameObject != nullptr && 
        !pCharDesc->pGameObject->Get_IsDead() && !pBodyDesc->pGameObject->Get_IsDead())
	{
        pCharDesc->pGameObject->Collision_Exit(pBodyDesc, static_cast<_uint>(BodyObj));
        pBodyDesc->pGameObject->Collision_Exit(pCharDesc, static_cast<_uint>(CharObj));
	}*/
}

void CJolt_CharacterContactListener::OnCharacterContactAdded(const CharacterVirtual* inCharacter, const CharacterVirtual* inOtherCharacter, const SubShapeID& inSubShapeID2, RVec3Arg inContactPosition, Vec3Arg inContactNormal, CharacterContactSettings& ioSettings)
{
    /*COLLISION_DESC* pCharDesc1 = reinterpret_cast<COLLISION_DESC*>(static_cast<std::uintptr_t>(inCharacter->GetUserData()));
    COLLISION_DESC* pCharDesc2 = reinterpret_cast<COLLISION_DESC*>(static_cast<std::uintptr_t>(inOtherCharacter->GetUserData()));

    ObjectLayer CharObj1 = m_pBodyInterface->GetObjectLayer(inCharacter->GetInnerBodyID());
    ObjectLayer CharObj2 = m_pBodyInterface->GetObjectLayer(inOtherCharacter->GetInnerBodyID());

    if (pCharDesc1->pGameObject->Get_IsDead())
    {
        m_pGameInstance->Remove_CharacterVirtual(inCharacter->GetID());
    }
    if (pCharDesc2->pGameObject->Get_IsDead())
    {
        m_pGameInstance->Remove_CharacterVirtual(inOtherCharacter->GetID());
    }

    if (pCharDesc1 == nullptr || pCharDesc2 == nullptr || pCharDesc1->pGameObject == nullptr || pCharDesc2->pGameObject == nullptr)
        return;

    if (pCharDesc1->pGameObject != nullptr && pCharDesc2->pGameObject != nullptr &&
        !pCharDesc1->pGameObject->Get_IsDead() && !pCharDesc2->pGameObject->Get_IsDead())
    {
        pCharDesc1->pGameObject->Collision_Enter(pCharDesc2, static_cast<_uint>(CharObj2),
            _float3(inContactPosition.GetX(), inContactPosition.GetY(), inContactPosition.GetZ()),
            _float3(inContactNormal.GetX(), inContactNormal.GetY(), inContactNormal.GetZ()));
        pCharDesc2->pGameObject->Collision_Enter(pCharDesc1, static_cast<_uint>(CharObj1),
            _float3(inContactPosition.GetX(), inContactPosition.GetY(), inContactPosition.GetZ()),
            _float3(inContactNormal.GetX(), inContactNormal.GetY(), inContactNormal.GetZ()));
    }*/
    //if (inCharacter->GetMaxStrength() > inOtherCharacter->GetMaxStrength())
    //    ioSettings.mCanPushCharacter = false;
    //else 
    //    ioSettings.mCanPushCharacter = true;
    //

    COLLISION_DESC* pCharDesc1 = reinterpret_cast<COLLISION_DESC*>(static_cast<std::uintptr_t>(inCharacter->GetUserData()));
    COLLISION_DESC* pCharDesc2 = reinterpret_cast<COLLISION_DESC*>(static_cast<std::uintptr_t>(inOtherCharacter->GetUserData()));

    if (pCharDesc1->pGameObject->Get_IsGhost() || pCharDesc2->pGameObject->Get_IsGhost())
    {
        ioSettings.mCanPushCharacter = false;
        return;
    }

    ioSettings.mCanPushCharacter = true;

    return;
}

void CJolt_CharacterContactListener::OnCharacterContactPersisted(const CharacterVirtual* inCharacter, const CharacterVirtual* inOtherCharacter, const SubShapeID& inSubShapeID2, RVec3Arg inContactPosition, Vec3Arg inContactNormal, CharacterContactSettings& ioSettings)
{
    /*COLLISION_DESC* pCharDesc1 = reinterpret_cast<COLLISION_DESC*>(static_cast<std::uintptr_t>(inCharacter->GetUserData()));
    COLLISION_DESC* pCharDesc2 = reinterpret_cast<COLLISION_DESC*>(static_cast<std::uintptr_t>(inOtherCharacter->GetUserData()));

    ObjectLayer CharObj1 = m_pBodyInterface->GetObjectLayer(inCharacter->GetInnerBodyID());
    ObjectLayer CharObj2 = m_pBodyInterface->GetObjectLayer(inOtherCharacter->GetInnerBodyID());

    if (pCharDesc1->pGameObject->Get_IsDead())
    {
        m_pGameInstance->Remove_CharacterVirtual(inCharacter->GetID());
    }
    if (pCharDesc2->pGameObject->Get_IsDead())
    {
        m_pGameInstance->Remove_CharacterVirtual(inOtherCharacter->GetID());
    }

    if (pCharDesc1 == nullptr || pCharDesc2 == nullptr || pCharDesc1->pGameObject == nullptr || pCharDesc2->pGameObject == nullptr)
        return;

    if (pCharDesc1->pGameObject != nullptr && pCharDesc2->pGameObject != nullptr &&
        !pCharDesc1->pGameObject->Get_IsDead() && !pCharDesc2->pGameObject->Get_IsDead())
    {
        pCharDesc1->pGameObject->Collision_Stay(pCharDesc2, static_cast<_uint>(CharObj2),
            _float3(inContactPosition.GetX(), inContactPosition.GetY(), inContactPosition.GetZ()),
            _float3(inContactNormal.GetX(), inContactNormal.GetY(), inContactNormal.GetZ()));
        pCharDesc2->pGameObject->Collision_Stay(pCharDesc1, static_cast<_uint>(CharObj1),
            _float3(inContactPosition.GetX(), inContactPosition.GetY(), inContactPosition.GetZ()),
            _float3(inContactNormal.GetX(), inContactNormal.GetY(), inContactNormal.GetZ()));
    }*/

    /*if (inCharacter->GetMaxStrength() > inOtherCharacter->GetMaxStrength())
        ioSettings.mCanPushCharacter = false;
    else
        ioSettings.mCanPushCharacter = true;*/

    COLLISION_DESC* pCharDesc1 = reinterpret_cast<COLLISION_DESC*>(static_cast<std::uintptr_t>(inCharacter->GetUserData()));
    COLLISION_DESC* pCharDesc2 = reinterpret_cast<COLLISION_DESC*>(static_cast<std::uintptr_t>(inOtherCharacter->GetUserData()));

    if (pCharDesc1->pGameObject->Get_IsGhost() || pCharDesc2->pGameObject->Get_IsGhost())
    {
        ioSettings.mCanPushCharacter = false;
        return;
    }


    ioSettings.mCanPushCharacter = true;

    return;
}

void CJolt_CharacterContactListener::OnCharacterContactRemoved(const CharacterVirtual* inCharacter, const CharacterID& inOtherCharacterID, const SubShapeID& inSubShapeID2)
{
    /*COLLISION_DESC* pCharDesc1 = reinterpret_cast<COLLISION_DESC*>(static_cast<std::uintptr_t>(inCharacter->GetUserData()));
    
	CharacterVirtual* pCharVir = m_pGameInstance->Find_CharacterVirtual(inOtherCharacterID);
    COLLISION_DESC* pCharDesc2 = reinterpret_cast<COLLISION_DESC*>(static_cast<std::uintptr_t>(pCharVir->GetUserData()));

    ObjectLayer CharObj1 = m_pBodyInterface->GetObjectLayer(inCharacter->GetInnerBodyID());
    ObjectLayer CharObj2 = m_pBodyInterface->GetObjectLayer(pCharVir->GetInnerBodyID());

    if (pCharDesc1->pGameObject->Get_IsDead())
    {
        m_pGameInstance->Remove_CharacterVirtual(inCharacter->GetID());
    }
    if (pCharDesc2->pGameObject->Get_IsDead())
    {
        m_pGameInstance->Remove_CharacterVirtual(inOtherCharacterID);
    }
    if (pCharDesc1 == nullptr || pCharDesc2 == nullptr || pCharDesc1->pGameObject == nullptr || pCharDesc2->pGameObject == nullptr)
        return;

    if (pCharDesc1->pGameObject != nullptr && pCharDesc2->pGameObject != nullptr && !pCharDesc1->pGameObject->Get_IsDead() && !pCharDesc2->pGameObject->Get_IsDead())
    {
        pCharDesc1->pGameObject->Collision_Exit(pCharDesc2, static_cast<_uint>(CharObj2));
        pCharDesc2->pGameObject->Collision_Exit(pCharDesc1, static_cast<_uint>(CharObj1));
    }*/
    
    return;
}
