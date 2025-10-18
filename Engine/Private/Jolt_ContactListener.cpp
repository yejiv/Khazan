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
	/*CGameObject* pGameObject1 = reinterpret_cast<CGameObject*>(inBody1.GetUserData());
	CGameObject* pGameObject2 = reinterpret_cast<CGameObject*>(inBody2.GetUserData());

	ObjectLayer Obj1 = inBody1.GetObjectLayer();
	ObjectLayer Obj2 = inBody2.GetObjectLayer();

	if (nullptr != pGameObject1 && nullptr != pGameObject2)
	{
		pGameObject1->Collision_Enter(pGameObject2, Obj2);
		pGameObject2->Collision_Enter(pGameObject1, Obj1);
	}*/
		
}

void CJolt_ContactListener::OnContactPersisted(const Body& inBody1, const Body& inBody2, const ContactManifold& inManifold, ContactSettings& ioSettings)
{
	/*CGameObject* pGameObject1 = reinterpret_cast<CGameObject*>(inBody1.GetUserData());
	CGameObject* pGameObject2 = reinterpret_cast<CGameObject*>(inBody2.GetUserData());

	ObjectLayer Obj1 = inBody1.GetObjectLayer();
	ObjectLayer Obj2 = inBody2.GetObjectLayer();

	if (nullptr != pGameObject1 && nullptr != pGameObject2)
	{
		pGameObject1->Collision_Stay(pGameObject2, Obj2);
		pGameObject2->Collision_Stay(pGameObject1, Obj1);
	}*/
		
}

void CJolt_ContactListener::OnContactRemoved(const SubShapeIDPair& inSubShapePair)
{

}
