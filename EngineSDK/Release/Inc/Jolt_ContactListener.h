#pragma once
#include "Engine_Defines.h"

NS_BEGIN(Engine)

class CJolt_ContactListener : public ContactListener
{
public:
	explicit CJolt_ContactListener();
	virtual ~CJolt_ContactListener();

public:
	virtual		ValidateResult	OnContactValidate(const Body& BodyA, const Body& BodyB, RVec3Arg BaseOffset, const CollideShapeResult& CollisionResult) override;
	virtual		void				OnContactAdded(const Body& BodyA, const Body& BodyB, const ContactManifold& Manifold, ContactSettings& Settings) override;
	virtual		void				OnContactPersisted(const Body& BodyA, const Body& BodyB, const ContactManifold& Manifold, ContactSettings& Settings) override;
	virtual		void				OnContactRemoved(const SubShapeIDPair& SubShapePair) override;
};

NS_END