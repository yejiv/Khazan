#pragma once

#include "Component.h"
#ifdef new
#pragma push_macro("new")
#undef new
#endif

#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Character/CharacterVirtual.h>

#ifdef new
#pragma pop_macro("new") // DBG_NEW 복원
#endif

NS_BEGIN(Engine)

class ENGINE_DLL CCharacterVirtual final : public CComponent
{
public:
	enum class WORLDUP { X, Y, Z, END};
public:
	typedef struct tagCharacterVirtual
	{
		SHAPE			eShapeType = SHAPE::BOX;
		_float3			vPos; // 초기 위치
		_float4			vQuat; // 초기 회전
		WORLDUP			eUp = WORLDUP::Y; // 월드 Up
		_float			fMaxSlopeAngle = DegreesToRadians(45.0f); // 오를 수 있는 경사
		_float			fPadding = 0.02f; // 벽/바닥과 여유 거리
		_float			fPenetrationRecoverySpeed = 1.0f; // 겹침 복구 속도
		_float			fPredictiveContactDistance = 0.1f; // 에측 접촉(미리 감지)
		EBackFaceMode	eBackFaceMode = EBackFaceMode::CollideWithBackFaces; // 양면 메쉬 대응

		_float			fMinTimeRemaining = 1e-3f; //서브스텝 통합 중 잔여 시간 최소치.
		_float			fCollisionTolerance = 0.01f; //충돌 허용 오차
		_uint			fMaxNumHits = 5.f; // 한 프레임 업데이트 동안 저장할 최대 충돌 히트 개수 제한.
		_float			fHitReductionCosMaxAngle = DegreesToRadians(15.0f); // 히트 축약 기준 각도(코사인 값)
		_float3			vShapeOffset;
		_bool			bEnhancedInternalEdgeRemoval = true;
		_uint			iMaxCollisionIterations = 10;
		_uint			iMaxConstraintIterations = 10;

		_float			fMass = 70.f;
		_float			fMaxStrength = 30.f;

		Plane			fSupportingVolume = Plane(Vec3::sAxisY(), 0.02f);


		_uint			iObjectLayer;

	}CHARACTERVIRTUAL_DESC;

	typedef struct tagBoxShape : CHARACTERVIRTUAL_DESC
	{
		_float3 vExtent = _float3(0.5f, 0.5f, 0.5f);
	}CV_BOXSHAPE_DESC;

	typedef struct tagSphereShape : CHARACTERVIRTUAL_DESC
	{
		_float fRadius = 0.5f;
	}CV_SPHERESHAPE_DESC;

	typedef struct tagCapsuleShape : CHARACTERVIRTUAL_DESC
	{
		_float				fHeight;
		_float				fRadius;
	}CV_CAPSULESHAPE_DESC;

	typedef struct tagMeshShape : CHARACTERVIRTUAL_DESC
	{
		class CModel* pModel = { nullptr };
	}CV_MESHSHAPE_DESC;

	typedef struct tagConvexShape : CHARACTERVIRTUAL_DESC
	{
		class CModel* pModel = { nullptr };
	}CV_CONVEXSHAPE_DESC;
private:
	CCharacterVirtual(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCharacterVirtual(const CCharacterVirtual& Prototype);
	virtual ~CCharacterVirtual() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize_Clone(void* pArg) override;
	void Update(_float fTimeDelta, class CTransform* pTransform);

//private:
//	void    Update_Kinematic(_float fTimeDelta, class CPhysicsBody* pBody, class CTransform* pTransform);

private:
	JPH::CharacterVirtual* m_pCharacterVir = { nullptr };

	JPH::BodyID m_BodyId;
	JPH::BodyInterface* m_pBodyInterface = { nullptr };
	JPH::BodyFilter* m_pBodyFilter = { nullptr };
	JPH::ShapeFilter* m_pShapeFilter = { nullptr };
	class CharacterContactListener* m_pContactListener = { nullptr };

	JPH::Vec3	m_vVelocity = {};
	JPH::Vec3	m_vUp = {};
	JPH::Vec3	m_vGravity = {};

	_uint		m_iNumObjectLayer = {};

public:
	static CCharacterVirtual* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END