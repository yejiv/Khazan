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
	//CGameObject* pSrc = reinterpret_cast<CGameObject*>(inBody1.GetUserData());
	//CGameObject* pDst = reinterpret_cast<CGameObject*>(inBody2.GetUserData());

	//if (nullptr != pSrc)
	//	pSrc->Collision_Enter(pDst);
	//if (nullptr != pDst)
	//	pDst->Collision_Enter(pDst);
}

void CJolt_ContactListener::OnContactPersisted(const Body& inBody1, const Body& inBody2, const ContactManifold& inManifold, ContactSettings& ioSettings)
{
	//CGameObject* pSrc = reinterpret_cast<CGameObject*>(inBody1.GetUserData());
	//CGameObject* pDst = reinterpret_cast<CGameObject*>(inBody2.GetUserData());

	//if (nullptr != pSrc)
	//	pSrc->Collision_Stay(pDst);
	//if (nullptr != pDst)
	//	pDst->Collision_Stay(pSrc);
}

void CJolt_ContactListener::OnContactRemoved(const SubShapeIDPair& inSubShapePair)
{

}
