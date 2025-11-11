#pragma once
#include "RigidBody.h"

#ifdef new
#pragma push_macro("new")
#undef new
#endif

#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Body/BodyID.h>
#include <Jolt/Physics/Body/BodyInterface.h>

#include <Jolt/Physics/PhysicsScene.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/CylinderShape.h>
#include <Jolt/Physics/Collision/Shape/RotatedTranslatedShape.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/StaticCompoundShape.h>
#include <Jolt/Physics/Collision/Shape/MeshShape.h>
#include <Jolt/Physics/Collision/Shape/ConvexHullShape.h>
#include <Jolt/Physics/Collision/Shape/ScaledShape.h>

#ifdef new
#pragma pop_macro("new") // DBG_NEW 복원
#endif

NS_BEGIN(Engine)

class ENGINE_DLL CBody final : public CRigidBody
{
public:
	typedef struct tagBody
	{
		SHAPE		eShapeType = SHAPE::BOX;
		_float3		vPos;
		_float4		vQuat;
		EMotionType	eMotion = EMotionType::Static;
		_uint		iObjectLayer;

		_float		fFriction = 0.2f;
		_float		fRestitution = 0.0f;

		_bool		bStartActive = true;
		_bool		bIsTrigger = false;

		_float3		vShapeOffset = { 0.f, 0.f, 0.f };
		_float4		vShapeRotation = { 0.f, 0.f, 0.f, 1.f };
		// Dynamic 전용 옵션
		_float          fMass = 1.0f; // 필요 시 사용
		EMotionQuality  eQuality = EMotionQuality::Discrete;
		_float		fAngularDamping = 0.05f;

		_float		fGravity = 1.f;

        _bool       isCollideKinematicVsNonDynamic = false;

		//class CGameObject* pGameObject = nullptr;
		COLLISION_DESC* pCollisionDesc = nullptr;
	}BODY_DESC;

	typedef struct tagBoxShape : BODY_DESC
	{
		_float3 vExtent = _float3(0.5f, 0.5f, 0.5f);
	}BODY_BOXSHAPE_DESC;

	typedef struct tagSphereShape : BODY_DESC
	{
		_float fRadius = 0.5f;
	}BODY_SPHERESHAPE_DESC;

	typedef struct tagCapsuleShape : BODY_DESC
	{
		_float				fHeight;
		_float				fRadius;
	}BODY_CAPSULESHAPE_DESC;

	typedef struct tagMeshShape : BODY_DESC
	{
		class CModel* pModel = { nullptr };
		class CTransform* pTransform = { nullptr };
	}BODY_MESHSHAPE_DESC;

	typedef struct tagConvexShape : BODY_DESC
	{
		class CModel* pModel = { nullptr };
	}BODY_CONVEXSHAPE_DESC;
private:
	CBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBody(const CBody& Prototype);
	virtual ~CBody() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize_Clone(void* pArg) override;

	void Build_Shape(BODY_DESC* pDesc, RefConst<Shape>& pShape);

	virtual void Update(_float fTimeDelta, class CTransform* pTransform) override;
	virtual void Sync_Update(class CTransform* pTransform) override;

	virtual void Update(_float fTimeDelta, _matrix WorldMatirx, _vector& outQuatRotation, _vector& outPosition);
	virtual void Sync_Update(_matrix WorldMatirx);

public:
	void	Activate(_bool isActivate) { true == isActivate ? m_pBodyInterface->ActivateBody(m_BodyID) : m_pBodyInterface->DeactivateBody(m_BodyID); }
	void	OnGravity(_bool isGravity) { m_pBodyInterface->SetGravityFactor(m_BodyID, isGravity); }

public:
	void	Add_Force(_float fMass);
	void	Add_Torque(_float fMass);
	void	Add_Impulse(_float fMass);

public:
	void	Set_Velocity(const _float3& vVelocity);

	void	Collision_Active(_bool isActive);

public:
	virtual void	Set_PosRot(_vector vPos, _vector vRot);
	BodyID           Get_BodyID() const { return m_BodyID; }
	EMotionType      Get_Motion() const { return m_eMotion; }


private:
	Body* m_pBody = { nullptr };
	BodyID			m_BodyID;
	BodyInterface* m_pBodyInterface = { nullptr };

	// 보조 저장
	EMotionType				m_eMotion = EMotionType::Static;
	_uint					m_iObjectLayer = 0;

private:
	const JPH::Array<Vec3> ConvertToArrayVec3(CModel* pModel);
	const JPH::Array<Float3> ConvertToArrayFloat3(CModel* pModel, _uint iIndex);
	const JPH::Array<IndexedTriangle> ConvertToArrayTri(CModel* pModel, _uint iIndex);
	void Make_MeshShape(BODY_MESHSHAPE_DESC* pDesc);

public:
	static CBody* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END