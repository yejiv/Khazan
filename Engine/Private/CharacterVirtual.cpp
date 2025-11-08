#include "CharacterVirtual.h"
#include "GameInstance.h"

inline _float Smoothstep(_float t) { return t * t * (3.f - 2.f * t); }

CCharacterVirtual::CCharacterVirtual(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CRigidBody { pDevice, pContext }
{

}

CCharacterVirtual::CCharacterVirtual(const CCharacterVirtual& Prototype)
	: CRigidBody{ Prototype }
{

}

HRESULT CCharacterVirtual::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCharacterVirtual::Initialize_Clone(void* pArg)
{
	CHARACTERVIRTUAL_DESC* pDesc = static_cast<CHARACTERVIRTUAL_DESC*>(pArg);
	
	m_iNumObjectLayer = pDesc->iObjectLayer;

	CharacterVirtualSettings SettingDesc{};

	//SettingDesc.mID = CharacterID::CharacterID();
	SettingDesc.mBackFaceMode = pDesc->eBackFaceMode;
	SettingDesc.mPredictiveContactDistance = pDesc->fPredictiveContactDistance;
	SettingDesc.mMaxCollisionIterations = pDesc->iMaxCollisionIterations;
	SettingDesc.mMaxConstraintIterations = pDesc->iMaxConstraintIterations;
	SettingDesc.mMinTimeRemaining = pDesc->fMinTimeRemaining;
	SettingDesc.mCollisionTolerance = pDesc->fCollisionTolerance;
	SettingDesc.mCharacterPadding = pDesc->fPadding;
	SettingDesc.mMaxNumHits = pDesc->fMaxNumHits;
	SettingDesc.mHitReductionCosMaxAngle = pDesc->fHitReductionCosMaxAngle;
	SettingDesc.mPenetrationRecoverySpeed = pDesc->fPenetrationRecoverySpeed;
	SettingDesc.mEnhancedInternalEdgeRemoval = pDesc->bEnhancedInternalEdgeRemoval;
	SettingDesc.mShapeOffset = LoadVec3(pDesc->vShapeOffset);
	SettingDesc.mSupportingVolume = pDesc->fSupportingVolume;
	SettingDesc.mMaxSlopeAngle = DegreesToRadians(pDesc->fMaxSlopeAngle);
	SettingDesc.mMaxStrength = pDesc->fMaxStrength;
	switch (pDesc->eShapeType)
	{
	case SHAPE::BOX:
	{
		CV_BOXSHAPE_DESC* pBoxDesc = static_cast<CV_BOXSHAPE_DESC*>(pDesc);
		SettingDesc.mShape = new BoxShape(Vec3(pBoxDesc->vExtent.x, pBoxDesc->vExtent.y, pBoxDesc->vExtent.z));
		SettingDesc.mInnerBodyShape = new BoxShape(Vec3(pBoxDesc->vExtent.x, pBoxDesc->vExtent.y, pBoxDesc->vExtent.z));
		break;
	}
	case SHAPE::SPHERE:
	{
		CV_SPHERESHAPE_DESC* pSphereDesc = static_cast<CV_SPHERESHAPE_DESC*>(pDesc);
		SettingDesc.mShape = new SphereShape(pSphereDesc->fRadius);
		SettingDesc.mInnerBodyShape = new SphereShape(pSphereDesc->fRadius);
		break;
	}
	case SHAPE::CAPSULE:
	{
		CV_CAPSULESHAPE_DESC* pCapsuleDesc = static_cast<CV_CAPSULESHAPE_DESC*>(pDesc);
		SettingDesc.mShape = new CapsuleShape(pCapsuleDesc->fHeight * 0.5f, pCapsuleDesc->fRadius);
		SettingDesc.mInnerBodyShape = new CapsuleShape(pCapsuleDesc->fHeight * 0.5f, pCapsuleDesc->fRadius);
		break;
	}
	}

	if (pDesc->eUp == WORLDUP::X)
		SettingDesc.mUp = Vec3::sAxisX();
	else if (pDesc->eUp == WORLDUP::Y)
		SettingDesc.mUp = Vec3::sAxisY();
	else if (pDesc->eUp == WORLDUP::Z)
		SettingDesc.mUp = Vec3::sAxisZ();
	SettingDesc.mInnerBodyLayer = m_iNumObjectLayer;

	m_pCharVir = m_pGameInstance->CreateCharacterVirtual(&SettingDesc, RVec3Arg(LoadVec3(pDesc->vPos)), QuatArg(LoadQuat(pDesc->vQuat)), 0, &m_pBodyInterface);
	m_BodyId = m_pCharVir->GetInnerBodyID();
	pDesc->pCollisionDesc->iObjectLayer = m_iNumObjectLayer;
	m_pCharVir->SetUserData(static_cast<uint64>(reinterpret_cast<uintptr_t>(pDesc->pCollisionDesc)));
	
	if (!m_BodyId.IsInvalid())
	{
		m_pBodyInterface->SetObjectLayer(m_BodyId, m_iNumObjectLayer);
		m_pBodyInterface->SetIsSensor(m_BodyId, false);
		m_pBodyInterface->SetUserData(m_BodyId, static_cast<uint64>(reinterpret_cast<uintptr_t>(pDesc->pCollisionDesc)));
		m_pGameInstance->Push_BodyDesc(m_BodyId, static_cast<uint64>(reinterpret_cast<uintptr_t>(pDesc->pCollisionDesc)));
	}
	

	//m_vVelocity = Vec3::sZero();
	m_vUp = Vec3::sAxisY();
	m_vGravity = Vec3(0, -9.81f, 0);

	m_pBodyFilter = new BodyFilter();
	m_pShapeFilter = new ShapeFilter();

	m_tEXUpdateSetting.mStickToFloorStepDown = LoadVec3(pDesc->vStickToFloorStepDown);
	m_tEXUpdateSetting.mWalkStairsStepUp = LoadVec3(pDesc->vWalkStairsStepUp);
	m_tEXUpdateSetting.mWalkStairsMinStepForward = pDesc->fWalkStairsMinStepForward;
	m_tEXUpdateSetting.mWalkStairsStepForwardTest = pDesc->fWalkStairsStepForwardTest;
	m_tEXUpdateSetting.mWalkStairsCosAngleForwardContact = pDesc->fWalkStairsCosAngleForwardContact;
	m_tEXUpdateSetting.mWalkStairsStepDownExtra = LoadVec3(pDesc->vWalkStairsStepDownExtra);

	return S_OK;
}
void CCharacterVirtual::Sync_Update(CTransform* pTransform)
{
	Set_PosRot(pTransform->Get_State(STATE::POSITION), pTransform->Get_Rotation_Quat());
}

void CCharacterVirtual::Update(_float fTimeDelta, CTransform* pTransform, _vector vGravity)
{
	m_fAcc += fTimeDelta;

	while (m_fAcc >= fFixedDt)
	{
		if (!m_pCharVir) return;

		m_vGravity = LoadVec3(vGravity);

		const auto ground_state = m_pCharVir->GetGroundState();
		const bool onGround =
			(ground_state == JPH::CharacterVirtual::EGroundState::OnGround);

		// === 1) m_vVelocity를 기준으로 중력 / 지면 보정 ===

		if (!onGround)
		{
			m_vVelocity += m_vGravity * fFixedDt;

			JPH::Vec3 horiz = m_vVelocity;
			horiz.SetY(0.0f);

			_float speed = horiz.Length();
			if (speed > 0.0f)
			{
				_float delta = m_fAirLoss * fFixedDt;

				_float newSpeed = max(0.0f, speed - delta);
				horiz *= (newSpeed / speed);
			}

			m_vVelocity.SetX(horiz.GetX());
			m_vVelocity.SetZ(horiz.GetZ());

			// 3) (선택) 터미널 속도 제한
			float maxFallSpeed = -50.0f;               // 원하는 값
			if (m_vVelocity.GetY() < maxFallSpeed)
				m_vVelocity.SetY(maxFallSpeed);
		}
		else
		{
			if (m_vVelocity.GetY() < 0.0f)
				m_vVelocity.SetY(0.0f);

			JPH::Vec3 horiz = m_vVelocity;
			horiz.SetY(0.0f);

			_float speed = horiz.Length();
			if (speed > 0.0f)
			{
				_float decel = m_fLoss * fFixedDt; // 초당 25씩 줄이기 (튜닝)
				_float newSpeed = max(0.0f, speed - decel);
				horiz *= (newSpeed / speed);
			}

			m_vVelocity.SetX(horiz.GetX());
			m_vVelocity.SetZ(horiz.GetZ());
		}

		JPH::Vec3 final_vel = m_vVelocity;

		if (onGround)
		{
			JPH::Vec3 ground_vel = m_pCharVir->GetGroundVelocity();
			ground_vel.SetY(0.0f);
			final_vel += ground_vel;
		}

		m_pCharVir->SetLinearVelocity(final_vel);

		// === 3) 충돌 / 계단 / 경사 처리 ===
		m_pGameInstance->CharVir_ExtendedUpdate(
			fFixedDt,
			m_pCharVir,
			m_vGravity,
			m_iNumObjectLayer,
			m_pBodyFilter,
			m_pShapeFilter,
			m_tEXUpdateSetting
		);

		m_tPrevPose = m_tCurrPose;
		m_tCurrPose.vPos = m_pCharVir->GetPosition();
		m_tCurrPose.vRot = m_pCharVir->GetRotation();
		m_tCurrPose.vLinvel = final_vel; 


		if (onGround && m_vVelocity.GetY() < 0.0f)
			m_vVelocity.SetY(0.0f);

		if (m_isFirstSync)
		{
			m_tPrevPose = m_tCurrPose;
			m_isFirstSync = false;
		}

		m_fAcc -= fFixedDt;
	}

	// === 5) 보간해서 Transform에 적용 ===
	const _float fAlpha = (fFixedDt > 0.f) ? (m_fAcc / fFixedDt) : 1.f;
	const _float fSmoothAlpha = Smoothstep(fAlpha);

	JPH::RVec3 ipos = LerpRVec3(m_tPrevPose.vPos, m_tCurrPose.vPos, fSmoothAlpha);
	JPH::Quat  irot = SlerpQuat(m_tPrevPose.vRot, m_tCurrPose.vRot, fSmoothAlpha);

	_vector vPos = XMVectorSet((float)ipos.GetX(), (float)ipos.GetY(), (float)ipos.GetZ(), 1.f);
	_vector vRot = XMVectorSet(irot.GetX(), irot.GetY(), irot.GetZ(), irot.GetW());

	pTransform->Set_State(STATE::POSITION, vPos);
	pTransform->Set_Quaternion(vRot);
}

void CCharacterVirtual::Sync_Update(_matrix WorldMatirx)
{
	_vector vScale, vRotation, vTranslation;

	XMMatrixDecompose(&vScale, &vRotation, &vTranslation, WorldMatirx);

	Set_PosRot(vTranslation, vRotation);
}

void CCharacterVirtual::Update(_float fTimeDelta, _vector& outQuatRotation, _vector& outPosition, _vector vGravity)
{
	m_fAcc += fTimeDelta;

	while (m_fAcc >= fFixedDt)
	{
		if (!m_pCharVir) return;

		m_vGravity = LoadVec3(vGravity);

		const auto ground_state = m_pCharVir->GetGroundState();
		const bool onGround =
			(ground_state == JPH::CharacterVirtual::EGroundState::OnGround);

		// === 1) m_vVelocity를 기준으로 중력 / 지면 보정 ===

		if (!onGround)
		{
			m_vVelocity += m_vGravity * fFixedDt;

			JPH::Vec3 horiz = m_vVelocity;
			horiz.SetY(0.0f);

			_float speed = horiz.Length();
			if (speed > 0.0f)
			{
				_float delta = m_fAirLoss * fFixedDt;

				_float newSpeed = max(0.0f, speed - delta);
				horiz *= (newSpeed / speed);
			}

			m_vVelocity.SetX(horiz.GetX());
			m_vVelocity.SetZ(horiz.GetZ());

			// 3) (선택) 터미널 속도 제한
			float maxFallSpeed = -50.0f;               // 원하는 값
			if (m_vVelocity.GetY() < maxFallSpeed)
				m_vVelocity.SetY(maxFallSpeed);
		}
		else
		{
			if (m_vVelocity.GetY() < 0.0f)
				m_vVelocity.SetY(0.0f);

			JPH::Vec3 horiz = m_vVelocity;
			horiz.SetY(0.0f);

			_float speed = horiz.Length();
			if (speed > 0.0f)
			{
				_float decel = m_fLoss * fFixedDt; // 초당 25씩 줄이기 (튜닝)
				_float newSpeed = max(0.0f, speed - decel);
				horiz *= (newSpeed / speed);
			}

			m_vVelocity.SetX(horiz.GetX());
			m_vVelocity.SetZ(horiz.GetZ());
		}

		JPH::Vec3 final_vel = m_vVelocity;

		if (onGround)
		{
			JPH::Vec3 ground_vel = m_pCharVir->GetGroundVelocity();
			ground_vel.SetY(0.0f);
			final_vel += ground_vel;
		}

		m_pCharVir->SetLinearVelocity(final_vel);

		// === 3) 충돌 / 계단 / 경사 처리 ===
		m_pGameInstance->CharVir_ExtendedUpdate(
			fFixedDt,
			m_pCharVir,
			m_vGravity,
			m_iNumObjectLayer,
			m_pBodyFilter,
			m_pShapeFilter,
			m_tEXUpdateSetting
		);

		m_tPrevPose = m_tCurrPose;
		m_tCurrPose.vPos = m_pCharVir->GetPosition();
		m_tCurrPose.vRot = m_pCharVir->GetRotation();
		m_tCurrPose.vLinvel = final_vel;


		if (onGround && m_vVelocity.GetY() < 0.0f)
			m_vVelocity.SetY(0.0f);

		if (m_isFirstSync)
		{
			m_tPrevPose = m_tCurrPose;
			m_isFirstSync = false;
		}

		m_fAcc -= fFixedDt;
	}

	// === 5) 보간해서 Transform에 적용 ===
	const _float fAlpha = (fFixedDt > 0.f) ? (m_fAcc / fFixedDt) : 1.f;
	const _float fSmoothAlpha = Smoothstep(fAlpha);

	JPH::RVec3 ipos = LerpRVec3(m_tPrevPose.vPos, m_tCurrPose.vPos, fSmoothAlpha);
	JPH::Quat  irot = SlerpQuat(m_tPrevPose.vRot, m_tCurrPose.vRot, fSmoothAlpha);

	_vector vPos = XMVectorSet((float)ipos.GetX(), (float)ipos.GetY(), (float)ipos.GetZ(), 1.f);
	_vector vRot = XMVectorSet(irot.GetX(), irot.GetY(), irot.GetZ(), irot.GetW());

	outPosition = vPos;
	outQuatRotation = vRot;
}

void CCharacterVirtual::Set_PosRot(_vector vPos, _vector vRot)
{
	m_pCharVir->SetPosition(LoadVec3(vPos));
	m_pCharVir->SetRotation(LoadQuat(vRot));
}

void CCharacterVirtual::Set_Position(_vector vPos)
{
	m_pCharVir->SetPosition(LoadVec3(vPos));
}

void CCharacterVirtual::Set_Velocity(_vector vVelocity)
{
	m_pCharVir->SetLinearVelocity(LoadVec3(vVelocity));
}

void CCharacterVirtual::Set_Rotation(_vector vRotation)
{
	m_pCharVir->SetRotation(LoadQuat(vRotation));
}

void CCharacterVirtual::Set_Gravity(_float fGravity)
{
	m_pBodyInterface->SetGravityFactor(m_pCharVir->GetInnerBodyID(), fGravity);
}

void CCharacterVirtual::Set_VelocityPower(_vector vDir, _float fPower, _float fLoss)
{
	m_vVelocity = Vec3(vDir.m128_f32[0], vDir.m128_f32[1], vDir.m128_f32[2]) * fPower;
	m_fLoss = fLoss;
}

void CCharacterVirtual::Collision_Active(_bool isActive)
{
	if (isActive)
	{
		if (!m_pBodyInterface->IsAdded(m_BodyId))
		{
			m_pBodyInterface->AddBody(m_BodyId, EActivation::Activate);
		}
	}
	else 
	{
		if (m_pBodyInterface->IsAdded(m_BodyId))
		{
			m_pBodyInterface->RemoveBody(m_BodyId);
		}
	}
}

_bool CCharacterVirtual::Get_isGround()
{
	return m_pCharVir->GetGroundState() == JPH::CharacterVirtual::EGroundState::OnGround;
}

CCharacterVirtual* CCharacterVirtual::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCharacterVirtual* pInstance = new CCharacterVirtual(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CCharacterVirtual"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CCharacterVirtual::Clone(void* pArg)
{
	CCharacterVirtual* pInstance = new CCharacterVirtual(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CCharacterVirtual"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CCharacterVirtual::Free()
{
	__super::Free();

	//Safe_Delete(m_pCharVir);
	Safe_Delete(m_pBodyFilter);
	Safe_Delete(m_pShapeFilter);
}
