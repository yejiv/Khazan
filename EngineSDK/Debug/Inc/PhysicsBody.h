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

#ifdef new
#pragma pop_macro("new") // DBG_NEW 복원
#endif

NS_BEGIN(Engine)

class ENGINE_DLL CPhysicsBody final : public CComponent
{
public:
	typedef struct tagShapeDesc {
		SHAPE eType = SHAPE::BOX;
		_float3 vHalfExtents{ 0.5f,0.5f,0.5f };
		_float  fRadius = 0.5f;
		_float  fHalfHeight = 0.9f; // Capsule half height
	}BODY_SHAPE_DESC;
	typedef struct tagMaterialDesc {
		_float fFriction = 0.8f;
		_float fRestitution = 0.0f;
	}BODY_MATERIAL_DESC;
	typedef struct tagBodyDesc {
		BODY_SHAPE_DESC				tShape;
		BODY_MATERIAL_DESC			tMat;
		class CTransform*			pTransform;
		EMotionType					eMotion = EMotionType::Static;
		_uint						iObjectLayer = 0;
		_bool						bStartActive = true;
		_bool						bIsTrigger = false;

		// Dynamic 전용 옵션
		_float          fMass = 1.0f; // 필요 시 사용
		EMotionQuality  eQuality = EMotionQuality::Discrete;
	}BODY_DESC;
private:
	CPhysicsBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPhysicsBody(const CPhysicsBody& Prototype);
	virtual ~CPhysicsBody() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize_Clone(void* pArg) override;
	// 프레임 훅
	void Update(_float fTimeDelta, class CTransform* pTransform);   // Dynamic fetch
	// Kinematic 포즈 반영
	void Sync_Kinematic(const _float4x4& matWorld);

	// 즉시 제어
	void Set_Position(const _float3& vPos);
	void Set_Rotation(const _float4& qRot);
	void Set_LinearVelocity(const _float3& vVelocity);
	void Add_Force(const _float3& fForce);
	void Add_Torque(const _float3& vTorque);

	// 속성
	BodyID           Get_BodyID() const { return m_BodyID; }
	EMotionType      Get_Motion() const { return m_eMotion; }
	const _float4x4& Get_WorldMatrix() const { return m_matWorld; }

public:
	RefConst<Shape>  Build_Shape(const BODY_SHAPE_DESC& BodyDesc);
	void             Apply_Material(Body& Body, const BODY_MATERIAL_DESC& MaterialDesc);
	void             Fetch_To_Transform(class CTransform* pTransform); // Dynamic → Transform


private:
	class CGameInstance* m_pGameInstance = { nullptr };
	PhysicsSystem*		m_pPhysics = { nullptr };
	BodyID				m_BodyID;
	_float4x4			m_matWorld{};

	// 보조 저장
	EMotionType				m_eMotion = EMotionType::Static;
	BODY_MATERIAL_DESC		m_tMat{};
	_uint					m_iLayer = 0;

public:
	static CPhysicsBody* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END