#include "Jolt_ContactListener.h"

#include "GameObject.h"

CJolt_ContactListener::CJolt_ContactListener(class BodyInterface* pBodyInterface)
	: m_pBodyInterface { pBodyInterface }
{
}

CJolt_ContactListener::~CJolt_ContactListener()
{
}

ValidateResult CJolt_ContactListener::OnContactValidate(const Body& inBody1, const Body& inBody2, RVec3Arg inBaseOffset, const CollideShapeResult& inCollisionResult)
{
	return ValidateResult();
}

void CJolt_ContactListener::OnContactAdded(const Body& inBody1, const Body& inBody2, const ContactManifold& inManifold, ContactSettings& ioSettings)
{
	
	COLLISION_DESC* pBody1Desc = reinterpret_cast<COLLISION_DESC*>(static_cast<std::uintptr_t>(inBody1.GetUserData()));
	COLLISION_DESC* pBody2Desc = reinterpret_cast<COLLISION_DESC*>(static_cast<std::uintptr_t>(inBody2.GetUserData()));

	if (pBody1Desc == nullptr || pBody2Desc == nullptr || pBody1Desc->pGameObject == nullptr || pBody2Desc->pGameObject == nullptr)
		return;

	ObjectLayer pBody1ObjectLayer = inBody1.GetObjectLayer();
	ObjectLayer pBody2ObjcetLayer = inBody2.GetObjectLayer();

	if (!pBody1Desc->pGameObject->Get_IsDead() && !pBody2Desc->pGameObject->Get_IsDead())
	{
		pBody1Desc->pGameObject->Collision_Enter(pBody2Desc, static_cast<_uint>(pBody2ObjcetLayer),
			_float3(inManifold.GetWorldSpaceContactPointOn1(0).GetX(), inManifold.GetWorldSpaceContactPointOn1(0).GetY(), inManifold.GetWorldSpaceContactPointOn1(0).GetZ()),
			_float3(inManifold.mWorldSpaceNormal.GetX(), inManifold.mWorldSpaceNormal.GetY(), inManifold.mWorldSpaceNormal.GetZ()));
		pBody2Desc->pGameObject->Collision_Enter(pBody1Desc, static_cast<_uint>(pBody1ObjectLayer),
			_float3(inManifold.GetWorldSpaceContactPointOn2(0).GetX(), inManifold.GetWorldSpaceContactPointOn2(0).GetY(), inManifold.GetWorldSpaceContactPointOn2(0).GetZ()),
			_float3(inManifold.mWorldSpaceNormal.GetX(), inManifold.mWorldSpaceNormal.GetY(), inManifold.mWorldSpaceNormal.GetZ()));
	}
		
}

void CJolt_ContactListener::OnContactPersisted(const Body& inBody1, const Body& inBody2, const ContactManifold& inManifold, ContactSettings& ioSettings)
{

	COLLISION_DESC* pBody1Desc = reinterpret_cast<COLLISION_DESC*>(static_cast<std::uintptr_t>(inBody1.GetUserData()));
	COLLISION_DESC* pBody2Desc = reinterpret_cast<COLLISION_DESC*>(static_cast<std::uintptr_t>(inBody2.GetUserData()));

	if (pBody1Desc == nullptr || pBody2Desc == nullptr || pBody1Desc->pGameObject == nullptr || pBody2Desc->pGameObject == nullptr)
		return;

	ObjectLayer pBody1ObjectLayer = inBody1.GetObjectLayer();
	ObjectLayer pBody2ObjcetLayer = inBody2.GetObjectLayer();
	
	if (!pBody1Desc->pGameObject->Get_IsDead() && !pBody2Desc->pGameObject->Get_IsDead())
	{
		pBody1Desc->pGameObject->Collision_Stay(pBody2Desc, static_cast<_uint>(pBody2ObjcetLayer),
			_float3(inManifold.GetWorldSpaceContactPointOn1(0).GetX(), inManifold.GetWorldSpaceContactPointOn1(0).GetY(), inManifold.GetWorldSpaceContactPointOn1(0).GetZ()),
			_float3(inManifold.mWorldSpaceNormal.GetX(), inManifold.mWorldSpaceNormal.GetY(), inManifold.mWorldSpaceNormal.GetZ()));
		pBody2Desc->pGameObject->Collision_Stay(pBody1Desc, static_cast<_uint>(pBody1ObjectLayer),
			_float3(inManifold.GetWorldSpaceContactPointOn2(0).GetX(), inManifold.GetWorldSpaceContactPointOn2(0).GetY(), inManifold.GetWorldSpaceContactPointOn2(0).GetZ()),
			_float3(inManifold.mWorldSpaceNormal.GetX(), inManifold.mWorldSpaceNormal.GetY(), inManifold.mWorldSpaceNormal.GetZ()));
	}

}

void CJolt_ContactListener::OnContactRemoved(const SubShapeIDPair& inSubShapePair)
{
	//COLLISION_DESC* pBody1Desc = reinterpret_cast<COLLISION_DESC*>(static_cast<std::uintptr_t>(m_pBodyInterface->GetUserData(inSubShapePair.GetBody1ID())));
	//COLLISION_DESC* pBody2Desc = reinterpret_cast<COLLISION_DESC*>(static_cast<std::uintptr_t>(m_pBodyInterface->GetUserData(inSubShapePair.GetBody2ID())));

	//if(pBody1Desc == nullptr || pBody2Desc == nullptr || pBody1Desc->pGameObject == nullptr || pBody2Desc->pGameObject == nullptr)
	//	return;

	//if (!pBody1Desc->pGameObject->Get_IsDead() && !pBody2Desc->pGameObject->Get_IsDead())
	//{
	//	pBody1Desc->pGameObject->Collision_Exit(pBody2Desc, static_cast<_uint>(m_pBodyInterface->GetObjectLayer(inSubShapePair.GetBody2ID())));
	//	pBody2Desc->pGameObject->Collision_Exit(pBody1Desc, static_cast<_uint>(m_pBodyInterface->GetObjectLayer(inSubShapePair.GetBody1ID())));
	//}
}
