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

#ifdef new
#pragma pop_macro("new") // DBG_NEW 복원
#endif

NS_BEGIN(Engine)

class ENGINE_DLL CCharacterVirtual final : public CRigidBody
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
		_float			fMaxSlopeAngle = 50.0f; // 오를 수 있는 경사
		_float			fPadding = 0.02f; // 벽/바닥과 여유 거리
		_float			fPenetrationRecoverySpeed = 1.0f; // 겹침 복구 속도
		_float			fPredictiveContactDistance = 0.1f; // 에측 접촉(미리 감지)
		EBackFaceMode	eBackFaceMode = EBackFaceMode::IgnoreBackFaces; // 양면 메쉬 대응

		_float			fMinTimeRemaining = 1e-3f; //서브스텝 통합 중 잔여 시간 최소치.
		_float			fCollisionTolerance = 0.01f; //충돌 허용 오차
		_uint			fMaxNumHits = 5; // 한 프레임 업데이트 동안 저장할 최대 충돌 히트 개수 제한.
		_float			fHitReductionCosMaxAngle = 15.f; // 히트 축약 기준 각도(코사인 값)
		_float3			vShapeOffset;
		_bool			bEnhancedInternalEdgeRemoval = true;
		_uint			iMaxCollisionIterations = 10;
		_uint			iMaxConstraintIterations = 10;

		_float			fMass = 70.f;
		_float			fMaxStrength = 30.f;

		Plane			fSupportingVolume = Plane(Vec3::sAxisY(), -0.02f);

		// 바닥으로 ‘내려 붙잡기’ 벡터 (월드기준 하향)
		_float3			vStickToFloorStepDown = _float3(0.0f, -0.3f, 0.0f);   // 최대 0.5m까지 아래로 붙잡기
		// 계단 ‘올라가기’ 허용 벡터 (월드기준 상향)
		_float3			vWalkStairsStepUp = _float3(0.0f, 0.2f, 0.0f);   // 0.3m까지 허용
		// 앞으로 얼마나 전진하고 ‘계단/턱’을 시험할지
		_float			fWalkStairsMinStepForward = 0.02f;                      // 최소 전진량
		_float			fWalkStairsStepForwardTest = 0.12f;                      // 테스트 전진량
		// 전방 벡터와 지면 법선(수평면 사영)의 허용 각 (코사인 값)
		_float			fWalkStairsCosAngleForwardContact = 75.0f;
		// 추가로 더 내려 붙잡고 싶을 때 사용 (없으면 Zero)
		_float3			vWalkStairsStepDownExtra = _float3(0.0f, 0.0f, 0.0f);

		_uint			iObjectLayer;


		class CGameObject* pGameObject = nullptr;
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

private:
	CCharacterVirtual(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCharacterVirtual(const CCharacterVirtual& Prototype);
	virtual ~CCharacterVirtual() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;
	virtual void Sync_Update(class CTransform* pTransform) override;
	virtual void Update(_float fTimeDelta, class CTransform* pTransform, _vector vGravity = XMVectorSet(0.f, g_fGravity, 0.f, 0.f));

public:
	virtual void	Set_PosRot(_vector vPos, _vector vRot);
	virtual void    Set_Position(_vector vPos);
	virtual void	Set_Velocity(_vector vVelocity);
	virtual void	Set_Rotation(_vector vRotation);
	virtual void	Set_Gravity(_float fGravity);

private:
	JPH::CharacterVirtual* m_pCharVir = { nullptr };

	JPH::BodyID m_BodyId;
	JPH::BodyInterface* m_pBodyInterface = { nullptr };
	JPH::BodyFilter* m_pBodyFilter = { nullptr };
	JPH::ShapeFilter* m_pShapeFilter = { nullptr };

	JPH::Vec3	m_vVelocity = {};
	JPH::Vec3	m_vUp = {};
	JPH::Vec3	m_vGravity = {};
	CharacterVirtual::ExtendedUpdateSettings m_tEXUpdateSetting{};

	_uint		m_iNumObjectLayer = {};


public:
	static CCharacterVirtual* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END