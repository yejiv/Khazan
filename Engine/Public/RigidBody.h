#pragma once
#include "Component.h"

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

#ifdef new
#pragma pop_macro("new") // DBG_NEW 복원
#endif

NS_BEGIN(Engine)

class ENGINE_DLL CRigidBody final : public CComponent
{
public:
	typedef struct tagRigidBody 
	{
		SHAPE		eShapeType = SHAPE::BOX;
		_float3		vPos;
		_float4		vQuat;
		EMotionType	eMotion = EMotionType::Static;
		_uint		iObjectLayer;

		_float		fFriction = 0.8f;
		_float		fRestitution = 0.0f;

		_bool		bStartActive = true;
		_bool		bIsTrigger = false;

		// Dynamic 전용 옵션
		_float          fMass = 1.0f; // 필요 시 사용
		EMotionQuality  eQuality = EMotionQuality::Discrete;
	}RIGIDBODY_DESC;

	typedef struct tagBoxShape : RIGIDBODY_DESC
	{
		_float3 vExtent = _float3(0.5f, 0.5f, 0.5f);
	}RIGID_BOXSHAPE_DESC;

	typedef struct tagSphereShape : RIGIDBODY_DESC
	{
		_float fRadius = 0.5f;
	}RIGID_SPHERESHAPE_DESC;

	typedef struct tagCapsuleShape : RIGIDBODY_DESC
	{
		_float				fHeight;
		_float				fRadius;
	}RIGID_CAPSULESHAPE_DESC;

	typedef struct tagMeshShape : RIGIDBODY_DESC
	{
		class CModel* pModel = { nullptr };
	}RIGID_MESHSHAPE_DESC;

	typedef struct tagConvexShape : RIGIDBODY_DESC
	{
		class CModel* pModel = { nullptr };
	}RIGID_CONVEXSHAPE_DESC;
private:
	CRigidBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRigidBody(const CRigidBody& Prototype);
	virtual ~CRigidBody() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize_Clone(void* pArg) override;

	void Build_Shape(RIGIDBODY_DESC* pDesc, RefConst<Shape>& pShape);

	void Update(_float fTimeDelta, const _matrix& matWorld);
	void Sync_Update(class CTransform* pTransform);

	void	Activate(_bool isActivate) { true == isActivate ? m_pBodyInterface->ActivateBody(m_BodyID) : m_pBodyInterface->DeactivateBody(m_BodyID); }
	void	OnGravity(_bool isGravity) { m_pBodyInterface->SetGravityFactor(m_BodyID, isGravity); }
	// 즉시 제어
	void Add_Force(const _float3& fForce);
	void Add_Torque(const _float3& vTorque);

	// 속성
	BodyID           Get_BodyID() const { return m_BodyID; }
	EMotionType      Get_Motion() const { return m_eMotion; }

public:
	//RefConst<Shape>  Build_Shape(const BODY_SHAPE_DESC& BodyDesc);
	//void             Apply_Material(Body& Body, const BODY_MATERIAL_DESC& MaterialDesc);
	//void             Fetch_To_Transform(class CTransform* pTransform); // Dynamic → Transform

private:
	void			Set_MeshShape(void* pArg);

private:
	Body*			m_pBody = { nullptr };
	BodyID			m_BodyID;
	BodyInterface*	m_pBodyInterface = { nullptr };

	// 보조 저장
	EMotionType				m_eMotion = EMotionType::Static;
	_uint					m_iObjectLayer = 0;

private:
	const JPH::Array<Vec3> ConvertToArrayVec3(CModel* pModel);
	const JPH::Array<Float3> ConvertToArrayFloat3(CModel* pModel, _uint iIndex);
	const JPH::Array<IndexedTriangle> ConvertToArrayTri(CModel* pModel, _uint iIndex);
	void Make_MeshShape(RIGID_MESHSHAPE_DESC* pDesc);

public:
	static CRigidBody* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END