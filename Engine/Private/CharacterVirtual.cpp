#include "CharacterVirtual.h"
#include "GameInstance.h"

// ===== 유틸 =====
static inline bool IsFiniteVec3(const JPH::Vec3& v) {
    return std::isfinite(v.GetX()) && std::isfinite(v.GetY()) && std::isfinite(v.GetZ());
}
static inline float Clamp01(float x) { return x < 0.f ? 0.f : (x > 1.f ? 1.f : x); }

// 필요 시 매끄러움 유지용 (원하면 생략해도 됨)
static inline float Smoothstep(float a) { a = Clamp01(a); return a * a * (3.f - 2.f * a); }

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
        m_pBodyInterface->SetMotionQuality(m_BodyId, EMotionQuality::LinearCast);
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
    if (!pTransform || !m_pCharVir)
        return;

    // dt 유효성 체크
    if (!std::isfinite(fTimeDelta) || fTimeDelta <= 0.f)
        return;

    // ===== 중력 세팅 =====
    {
        float gx = XMVectorGetX(vGravity);
        float gy = XMVectorGetY(vGravity);
        float gz = XMVectorGetZ(vGravity);

        if (std::isfinite(gx) && std::isfinite(gy) && std::isfinite(gz))
            m_vGravity = JPH::Vec3(gx, gy, gz);
        else
            m_vGravity = JPH::Vec3(0.f, -980.f, 0.f); // fallback
    }

    // ===== 한 번만 고정 스텝 처리 =====
    StepFixed(fTimeDelta);

    // ===== 물리 결과를 Transform에 바로 반영 =====
    const RVec3 pos = m_pCharVir->GetPosition();
    const JPH::Quat rot = m_pCharVir->GetRotation();

    pTransform->Set_State(
        STATE::POSITION,
        XMVectorSet((float)pos.GetX(), (float)pos.GetY(), (float)pos.GetZ(), 1.f)
    );

    pTransform->Set_Quaternion(
        XMVectorSet(rot.GetX(), rot.GetY(), rot.GetZ(), rot.GetW())
    );
}

void CCharacterVirtual::Sync_Update(_matrix WorldMatirx)
{
    _vector vScale, vRotation, vTranslation;

    XMMatrixDecompose(&vScale, &vRotation, &vTranslation, WorldMatirx);

    Set_PosRot(vTranslation, vRotation);
}

void CCharacterVirtual::Update(_float fTimeDelta, _vector& outQuatRotation, _vector& outPosition, _vector vGravity)
{
    if (!m_pCharVir)
        return;

    if (!std::isfinite(fTimeDelta) || fTimeDelta <= 0.f)
        return;

    // ===== 중력 세팅 =====
    {
        float gx = XMVectorGetX(vGravity);
        float gy = XMVectorGetY(vGravity);
        float gz = XMVectorGetZ(vGravity);

        if (std::isfinite(gx) && std::isfinite(gy) && std::isfinite(gz))
            m_vGravity = JPH::Vec3(gx, gy, gz);
        else
            m_vGravity = JPH::Vec3(0.f, -980.f, 0.f);
    }

    // ===== 한 번만 고정 스텝 처리 =====
    StepFixed(fTimeDelta);

    // ===== 결과를 바로 out 파라미터로 반환 =====
    const RVec3 pos = m_pCharVir->GetPosition();
    const JPH::Quat rot = m_pCharVir->GetRotation();

    outPosition = XMVectorSet((float)pos.GetX(), (float)pos.GetY(), (float)pos.GetZ(), 1.f);
    outQuatRotation = XMVectorSet(rot.GetX(), rot.GetY(), rot.GetZ(), rot.GetW());
}

void CCharacterVirtual::Set_PosRot(_vector vPos, _vector vRot)
{
	m_pCharVir->SetPosition(LoadVec3(vPos));

    //_vector vRotation = XMVector3Normalize(vRot);
	m_pCharVir->SetRotation(LoadQuat(vRot));
}

void CCharacterVirtual::StepFixed(_float fTimeDelta)
{
    if (!m_pCharVir)
        return;

    // 현재 접지 상태
    const auto ground_state = m_pCharVir->GetGroundState();
    const bool onGround = (ground_state == JPH::CharacterVirtual::EGroundState::OnGround);

    // ===== 1) 중력 적용 =====
    if (!onGround)
    {
        // v = v + g * dt
        m_vVelocity += m_vGravity * fTimeDelta;

        // 낙하 속도 제한 (원하면 값 조절)
        const _float maxFallSpeed = -50.0f;
        if (m_vVelocity.GetY() < maxFallSpeed)
            m_vVelocity.SetY(maxFallSpeed);
    }
    else
    {
        // 땅에 붙어있는데 아직 아래로 남아 있으면 0으로 정리
        if (m_vVelocity.GetY() < 0.0f)
            m_vVelocity.SetY(0.0f);
    }

    // ===== 2) 감쇠(마찰) =====
    // 땅에 있으면 m_fLoss, 공중이면 m_fAirLoss 사용
    const _float loss = onGround ? m_fLoss : m_fAirLoss;
    if (loss > 0.0f)
    {
        const _float k = expf(-loss * fTimeDelta); // 지수 감쇠
        m_vVelocity.SetX(m_vVelocity.GetX() * k);
        m_vVelocity.SetY(m_vVelocity.GetY() * k);
        m_vVelocity.SetZ(m_vVelocity.GetZ() * k);
    }

    // NaN / inf 보호
    if (!IsFiniteVec3(m_vVelocity))
        m_vVelocity = JPH::Vec3::sZero();

    // ===== 3) Jolt에 속도 적용 + ExtendedUpdate 호출 =====
    m_pCharVir->SetLinearVelocity(m_vVelocity);

    m_pGameInstance->CharVir_ExtendedUpdate(
        fTimeDelta,
        m_pCharVir,
        m_vGravity,
        m_iNumObjectLayer,
        m_pBodyFilter,
        m_pShapeFilter,
        m_tEXUpdateSetting
    );

    // Jolt가 충돌로 속도 조절했을 수 있으니, 다시 캐싱해도 됨
    m_vVelocity = m_pCharVir->GetLinearVelocity();
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

void CCharacterVirtual::Fake_Release()
{
    if (m_pBodyInterface->IsAdded(m_BodyId))
    {
        m_pBodyInterface->RemoveBody(m_BodyId);
    }
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
    if (m_pBodyInterface->IsAdded(m_BodyId))
    {
        m_pBodyInterface->RemoveBody(m_BodyId);
    }

	Safe_Delete(m_pBodyFilter);
	Safe_Delete(m_pShapeFilter);
}
