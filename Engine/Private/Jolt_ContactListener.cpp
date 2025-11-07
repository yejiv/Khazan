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

	/*COLLISION_DESC* pBody1Desc = reinterpret_cast<COLLISION_DESC*>(static_cast<std::uintptr_t>(inBody1.GetUserData()));
	COLLISION_DESC* pBody2Desc = reinterpret_cast<COLLISION_DESC*>(static_cast<std::uintptr_t>(inBody2.GetUserData()));


	CGameObject* pGameObject1 = reinterpret_cast<CGameObject*>(inBody1.GetUserData());
	CGameObject* pGameObject2 = reinterpret_cast<CGameObject*>(inBody2.GetUserData());

	ObjectLayer Obj1 = inBody1.GetObjectLayer();
	ObjectLayer Obj2 = inBody2.GetObjectLayer();

	Mat44 body1WT = inBody1.GetWorldTransform();
	Mat44 body2WT = inBody2.GetWorldTransform();


	Vec3 worldPos1 = body1WT * inManifold.mRelativeContactPointsOn1[0];
	Vec3 vNormal = inManifold.mWorldSpaceNormal;
	
	_float3 vContactPos = _float3(worldPos1.GetX(), worldPos1.GetY(), worldPos1.GetZ());
	_float3 vContactNor = _float3(vNormal.GetX(), vNormal.GetY(), vNormal.GetZ());


	if (nullptr != pGameObject1 && nullptr != pGameObject2)
	{
		pGameObject1->Collision_Enter(pBody1Desc,Obj2,vContactPos, vContactNor);
		pGameObject2->Collision_Enter(pBody2Desc, Obj1, vContactPos, vContactNor);
	}*/
		
}

void CJolt_ContactListener::OnContactPersisted(const Body& inBody1, const Body& inBody2, const ContactManifold& inManifold, ContactSettings& ioSettings)
{
	/*COLLISION_DESC* pBody1Desc = reinterpret_cast<COLLISION_DESC*>(static_cast<std::uintptr_t>(inBody1.GetUserData()));
	COLLISION_DESC* pBody2Desc = reinterpret_cast<COLLISION_DESC*>(static_cast<std::uintptr_t>(inBody2.GetUserData()));


	CGameObject* pGameObject1 = reinterpret_cast<CGameObject*>(inBody1.GetUserData());
	CGameObject* pGameObject2 = reinterpret_cast<CGameObject*>(inBody2.GetUserData());

	ObjectLayer Obj1 = inBody1.GetObjectLayer();
	ObjectLayer Obj2 = inBody2.GetObjectLayer();

	Mat44 body1WT = inBody1.GetWorldTransform();
	Mat44 body2WT = inBody2.GetWorldTransform();


	Vec3 worldPos1 = body1WT * inManifold.mRelativeContactPointsOn1[0];
	Vec3 vNormal = inManifold.mWorldSpaceNormal;

	_float3 vContactPos = _float3(worldPos1.GetX(), worldPos1.GetY(), worldPos1.GetZ());
	_float3 vContactNor = _float3(vNormal.GetX(), vNormal.GetY(), vNormal.GetZ());


	if (nullptr != pGameObject1 && nullptr != pGameObject2)
	{
		pGameObject1->Collision_Stay(pBody1Desc, Obj2, vContactPos, vContactNor);
		pGameObject2->Collision_Stay(pBody2Desc, Obj1, vContactPos, vContactNor);
	}*/
		
}

void CJolt_ContactListener::OnContactRemoved(const SubShapeIDPair& inSubShapePair)
{
	
}
