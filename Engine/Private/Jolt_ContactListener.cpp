#include "Jolt_ContactListener.h"

#include "GameObject.h"
#include "GameInstance.h"

CJolt_ContactListener::CJolt_ContactListener(class BodyInterface* pBodyInterface)
	: m_pBodyInterface { pBodyInterface }
	, m_pGameInstance { CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

CJolt_ContactListener::~CJolt_ContactListener()
{
	Safe_Release(m_pGameInstance);
}

ValidateResult CJolt_ContactListener::OnContactValidate(const Body& inBody1, const Body& inBody2, RVec3Arg inBaseOffset, const CollideShapeResult& inCollisionResult)
{
    if (inBody1.GetObjectLayer() == inBody2.GetObjectLayer())
    {
        return ValidateResult::RejectContact;
    }

    COLLISION_DESC* pBody1Desc = reinterpret_cast<COLLISION_DESC*>(static_cast<std::uintptr_t>(inBody1.GetUserData()));
    COLLISION_DESC* pBody2Desc = reinterpret_cast<COLLISION_DESC*>(static_cast<std::uintptr_t>(inBody2.GetUserData()));

    if (pBody1Desc == nullptr ||
        pBody2Desc == nullptr ||
        pBody1Desc->pGameObject == nullptr || 
        pBody2Desc->pGameObject == nullptr)
    {
        return ValidateResult::RejectContact;
    }

    if (pBody1Desc->isForceVaildation || pBody2Desc->isForceVaildation)
    {
        return ValidateResult::AcceptAllContactsForThisBodyPair;
    }

    if (pBody1Desc->pGameObject->Get_IsGhost() || pBody2Desc->pGameObject->Get_IsGhost())
    {
        return ValidateResult::RejectContact;
    }

    
	
    return ValidateResult();
}

void CJolt_ContactListener::OnContactAdded(const Body& inBody1, const Body& inBody2, const ContactManifold& inManifold, ContactSettings& ioSettings)
{    
	COLLISION_DESC* pBody1Desc = reinterpret_cast<COLLISION_DESC*>(static_cast<std::uintptr_t>(inBody1.GetUserData()));
	COLLISION_DESC* pBody2Desc = reinterpret_cast<COLLISION_DESC*>(static_cast<std::uintptr_t>(inBody2.GetUserData()));

	if (pBody1Desc == nullptr || pBody2Desc == nullptr || pBody1Desc->pGameObject == nullptr || pBody2Desc->pGameObject == nullptr)
		return;

    OutputDebugStringA(("Layer1: " + to_string(pBody1Desc->iObjectLayer) + "Layer2: " + to_string(pBody2Desc->iObjectLayer) + "\n").c_str());
	if (!pBody1Desc->pGameObject->Get_IsDead() && !pBody2Desc->pGameObject->Get_IsDead())
	{
		pBody1Desc->pGameObject->Collision_Enter(pBody2Desc, pBody2Desc->iObjectLayer,
			_float3(inManifold.GetWorldSpaceContactPointOn1(0).GetX(), inManifold.GetWorldSpaceContactPointOn1(0).GetY(), inManifold.GetWorldSpaceContactPointOn1(0).GetZ()),
			_float3(inManifold.mWorldSpaceNormal.GetX(), inManifold.mWorldSpaceNormal.GetY(), inManifold.mWorldSpaceNormal.GetZ()),
            pBody1Desc
            );
		pBody2Desc->pGameObject->Collision_Enter(pBody1Desc, pBody1Desc->iObjectLayer,
			_float3(inManifold.GetWorldSpaceContactPointOn2(0).GetX(), inManifold.GetWorldSpaceContactPointOn2(0).GetY(), inManifold.GetWorldSpaceContactPointOn2(0).GetZ()),
			_float3(inManifold.mWorldSpaceNormal.GetX(), inManifold.mWorldSpaceNormal.GetY(), inManifold.mWorldSpaceNormal.GetZ()),
            pBody2Desc
            );
	}
		
}

void CJolt_ContactListener::OnContactPersisted(const Body& inBody1, const Body& inBody2, const ContactManifold& inManifold, ContactSettings& ioSettings)
{
	COLLISION_DESC* pBody1Desc = reinterpret_cast<COLLISION_DESC*>(static_cast<std::uintptr_t>(inBody1.GetUserData()));
	COLLISION_DESC* pBody2Desc = reinterpret_cast<COLLISION_DESC*>(static_cast<std::uintptr_t>(inBody2.GetUserData()));

	if (pBody1Desc == nullptr || pBody2Desc == nullptr || pBody1Desc->pGameObject == nullptr || pBody2Desc->pGameObject == nullptr)
		return;
	
	if (!pBody1Desc->pGameObject->Get_IsDead() && !pBody2Desc->pGameObject->Get_IsDead())
	{
		pBody1Desc->pGameObject->Collision_Stay(pBody2Desc, pBody2Desc->iObjectLayer,
			_float3(inManifold.GetWorldSpaceContactPointOn1(0).GetX(), inManifold.GetWorldSpaceContactPointOn1(0).GetY(), inManifold.GetWorldSpaceContactPointOn1(0).GetZ()),
			_float3(inManifold.mWorldSpaceNormal.GetX(), inManifold.mWorldSpaceNormal.GetY(), inManifold.mWorldSpaceNormal.GetZ()),
            pBody1Desc
        );
		pBody2Desc->pGameObject->Collision_Stay(pBody1Desc, pBody1Desc->iObjectLayer,
			_float3(inManifold.GetWorldSpaceContactPointOn2(0).GetX(), inManifold.GetWorldSpaceContactPointOn2(0).GetY(), inManifold.GetWorldSpaceContactPointOn2(0).GetZ()),
			_float3(inManifold.mWorldSpaceNormal.GetX(), inManifold.mWorldSpaceNormal.GetY(), inManifold.mWorldSpaceNormal.GetZ()),
            pBody2Desc
        );
	}

}

void CJolt_ContactListener::OnContactRemoved(const SubShapeIDPair& inSubShapePair)
{
	BodyID body1ID = inSubShapePair.GetBody1ID();
	BodyID body2ID = inSubShapePair.GetBody2ID();

	COLLISION_DESC* pBody1Desc = reinterpret_cast<COLLISION_DESC*>(static_cast<std::uintptr_t>(m_pGameInstance->Find_BodyDesc(body1ID)));
	COLLISION_DESC* pBody2Desc = reinterpret_cast<COLLISION_DESC*>(static_cast<std::uintptr_t>(m_pGameInstance->Find_BodyDesc(body2ID)));

	if(pBody1Desc == nullptr || pBody2Desc == nullptr || pBody1Desc->pGameObject == nullptr || pBody2Desc->pGameObject == nullptr)
		return;

	if (!pBody1Desc->pGameObject->Get_IsDead() && !pBody2Desc->pGameObject->Get_IsDead())
	{
		pBody1Desc->pGameObject->Collision_Exit(
            pBody2Desc, 
            pBody2Desc->iObjectLayer,
            pBody1Desc
        );
		pBody2Desc->pGameObject->Collision_Exit(
            pBody1Desc, 
            pBody1Desc->iObjectLayer,
            pBody2Desc
        );
	}
}
