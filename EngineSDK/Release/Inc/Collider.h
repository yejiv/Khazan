#pragma once

#include "Component.h"

#include "Bounding_AABB.h"
#include "Bounding_OBB.h"
#include "Bounding_Sphere.h"

#ifdef new
#pragma push_macro("new")
#undef new
#endif

#include <Jolt/Physics/PhysicsScene.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/CylinderShape.h>
#include <Jolt/Physics/Collision/Shape/RotatedTranslatedShape.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/StaticCompoundShape.h>
#include <Jolt/Physics/Collision/Shape/MeshShape.h>

#ifdef new
#pragma pop_macro("new") // DBG_NEW 복원
#endif

NS_BEGIN(Engine)

class ENGINE_DLL CCollider final : public CComponent
{
public:
	typedef struct tagColliderDesc {
		_float fStandingHeight, fStandingRadius;
		_float fCrouchingHeight, fCrouchingRadius;
		_float fInnerShapeFraction; // 0~1
		_bool  bStartCrouch = false;
		_bool  bUseInner = false;
		COLLIDER_SHAPE eShape = COLLIDER_SHAPE::CAPSULE;
		_uint  uObjectLayer = 1; // (예) MOVING
	}COLLIDER_DESC;
private:
	CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCollider(const CCollider& Prototype);
	virtual ~CCollider() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize_Clone(void* pArg) override;
	void Update(_fmatrix WorldMatrix);

public:
	_bool Intersect(CCollider* pTarget);

#ifdef _DEBUG
	virtual HRESULT Render() override;

#endif

public:
	void     BuildShapes();
	void     SelectActiveShape();
	void     ApplyActiveShapeToBody(); // SetShape

public:
	void     SetCrouch(bool v);
	void     UseInner(bool v);
	BodyID   GetBodyID() const { return m_BodyID; }


private:
	class CGameInstance* m_pGameInstance = { nullptr };
	COLLIDER_SHAPE m_eCollider_Shape;

	// Active 상태
	bool     m_bCrouching = false;
	bool     m_bUseInner = false;

	// 파라미터
	float    m_fStandingHeight{}, m_fStandingRadius{};
	float    m_fCrouchingHeight{}, m_fCrouchingRadius{};
	float    m_fInnerShapeFraction{};

	// Jolt
	ShapeRefC m_ShStanding, m_ShCrouching, m_ShInnerStanding, m_ShInnerCrouching, m_ShActive;
	BodyID    m_BodyID = {};
	RMat44    m_World = RMat44::sIdentity();

	// 상태
	_bool     m_isColl = false;



public:
	static CCollider* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, COLLIDER eType);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END