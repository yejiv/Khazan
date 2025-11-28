#pragma once
#include "Engine_Defines.h"

NS_BEGIN(Engine)

class CJolt_ContactListener : public ContactListener
{
public:
	explicit CJolt_ContactListener(class BodyInterface* pBodyInterface);
	virtual ~CJolt_ContactListener();

public:
	virtual		ValidateResult	OnContactValidate(const Body& BodyA, const Body& BodyB, RVec3Arg BaseOffset, const CollideShapeResult& CollisionResult) override;
	virtual		void			OnContactAdded(const Body& BodyA, const Body& BodyB, const ContactManifold& Manifold, ContactSettings& Settings) override;
	virtual		void			OnContactPersisted(const Body& BodyA, const Body& BodyB, const ContactManifold& Manifold, ContactSettings& Settings) override;
	virtual		void			OnContactRemoved(const SubShapeIDPair& SubShapePair) override;

private:
	class BodyInterface* m_pBodyInterface = { nullptr };
	class CGameInstance* m_pGameInstance = { nullptr };
};

NS_END