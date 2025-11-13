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
    if (!pTransform) return;

    // ---- 고정 스텝 유효성 보장 ----
    if (!(m_fFixedDt > 0.f) || !std::isfinite(m_fFixedDt))
        m_fFixedDt = 1.f / 60.f;

    // 델타/누적 상한 (프레임 드랍 대비)
    if (!std::isfinite(fTimeDelta) || fTimeDelta < 0.f) fTimeDelta = 0.f;
    m_fAcc += fTimeDelta;
    if (m_fAcc > m_fMaxLagClamp) m_fAcc = m_fMaxLagClamp;

    // 캐릭터 핸들이 없으면 보간 생략 (포즈만 유지)
    if (!m_pCharVir) return;

    // 중력 세팅 (Vec3에 IsFinite 없음 → 구성요소 검사)
    JPH::Vec3 g;
    {
        float gx = XMVectorGetX(vGravity);
        float gy = XMVectorGetY(vGravity);
        float gz = XMVectorGetZ(vGravity);
        if (std::isfinite(gx) && std::isfinite(gy) && std::isfinite(gz))
            g = JPH::Vec3(gx, gy, gz);
        else
            g = JPH::Vec3(0, -980.f, 0);
    }
    m_vGravity = g;

    // ===== 고정 스텝 수행 (상한 적용) =====
    int numSteps = (int)floor(m_fAcc / m_fFixedDt);
    if (numSteps > m_iMaxSubsteps) numSteps = m_iMaxSubsteps;
    m_fAcc -= numSteps * m_fFixedDt;

    for (int i = 0; i < numSteps; ++i)
        StepFixed(m_fFixedDt);

    // ===== 보간 (렌더로 못 옮긴다고 했으니 여기서 1회만) =====
    float alpha = (m_fFixedDt > 0.f) ? (m_fAcc / m_fFixedDt) : 1.f;
    // alpha = Smoothstep(alpha); // 부드럽게 하고 싶으면 주석 해제
    ApplyInterpolatedPose(alpha, pTransform);
}

void CCharacterVirtual::Sync_Update(_matrix WorldMatirx)
{
    _vector vScale, vRotation, vTranslation;

    XMMatrixDecompose(&vScale, &vRotation, &vTranslation, WorldMatirx);

    Set_PosRot(vTranslation, vRotation);
}

void CCharacterVirtual::Update(_float fTimeDelta, _vector& outQuatRotation, _vector& outPosition, _vector vGravity)
{
    // ---- 고정 스텝 유효성 보장 ----
    if (!(m_fFixedDt > 0.f) || !std::isfinite(m_fFixedDt))
        m_fFixedDt = 1.f / 60.f;

    // 델타/누적 상한 (프레임 드랍 대비)
    if (!std::isfinite(fTimeDelta) || fTimeDelta < 0.f) fTimeDelta = 0.f;
    m_fAcc += fTimeDelta;
    if (m_fAcc > m_fMaxLagClamp) m_fAcc = m_fMaxLagClamp;

    // 캐릭터 핸들이 없으면 보간 생략 (포즈만 유지)
    if (!m_pCharVir) return;

    // 중력 세팅 (Vec3에 IsFinite 없음 → 구성요소 검사)
    JPH::Vec3 g;
    {
        float gx = XMVectorGetX(vGravity);
        float gy = XMVectorGetY(vGravity);
        float gz = XMVectorGetZ(vGravity);
        if (std::isfinite(gx) && std::isfinite(gy) && std::isfinite(gz))
            g = JPH::Vec3(gx, gy, gz);
        else
            g = JPH::Vec3(0, -980.f, 0);
    }
    m_vGravity = g;

    // ===== 고정 스텝 수행 (상한 적용) =====
    int numSteps = (int)floor(m_fAcc / m_fFixedDt);
    if (numSteps > m_iMaxSubsteps) numSteps = m_iMaxSubsteps;
    m_fAcc -= numSteps * m_fFixedDt;

    for (int i = 0; i < numSteps; ++i)
        StepFixed(m_fFixedDt);

    // ===== 보간 (렌더로 못 옮긴다고 했으니 여기서 1회만) =====
    float alpha = (m_fFixedDt > 0.f) ? (m_fAcc / m_fFixedDt) : 1.f;
    // alpha = Smoothstep(alpha); // 부드럽게 하고 싶으면 주석 해제
    ApplyInterpolatedPose(alpha, outQuatRotation, outPosition);
}

void CCharacterVirtual::Set_PosRot(_vector vPos, _vector vRot)
{
	m_pCharVir->SetPosition(LoadVec3(vPos));
	m_pCharVir->SetRotation(LoadQuat(vRot));
}

void CCharacterVirtual::StepFixed(_float fTimeDelta)
{
    // 이전 포즈 백업
    m_tPrevPose = m_tCurrPose;

    // 접지 상태 1회만 쿼리
    const auto ground_state = m_pCharVir->GetGroundState();
    const _bool onGround = (ground_state == JPH::CharacterVirtual::EGroundState::OnGround);

    // ===== 속도 적분 & 감쇠(가벼운 지수 감쇠) =====
    if (!onGround)
    {
        // 중력
        m_vVelocity += m_vGravity * fTimeDelta;

        // XZ 지수 감쇠
        const _float damp = expf(-m_fAirLoss * fTimeDelta);
        m_vVelocity.SetX(m_vVelocity.GetX() * damp);
        m_vVelocity.SetZ(m_vVelocity.GetZ() * damp);

        // 낙하 속도 제한
        const _float maxFallSpeed = -50.0f;
        if (m_vVelocity.GetY() < maxFallSpeed) m_vVelocity.SetY(maxFallSpeed);
    }
    else
    {
        // 접지 시 Y- 음수 정리
        if (m_vVelocity.GetY() < 0.0f) m_vVelocity.SetY(0.0f);

        // XZ 지수 감쇠
        const _float damp = expf(-m_fLoss * fTimeDelta);
        m_vVelocity.SetX(m_vVelocity.GetX() * damp);
        m_vVelocity.SetZ(m_vVelocity.GetZ() * damp);
    }

    // ===== 지면 속도 더하기 (캐시/지연 갱신) =====
    JPH::Vec3 final_vel = m_vVelocity;

    if (onGround)
    {
        if (!m_prevOnGround || ++m_groundVelQueryDefer >= m_groundVelQueryInterval)
        {
            JPH::Vec3 gv = m_pCharVir->GetGroundVelocity();
            gv.SetY(0.0f);
            m_cachedGroundVel = IsFiniteVec3(gv) ? gv : JPH::Vec3::sZero();
            m_groundVelQueryDefer = 0;
        }
        final_vel += m_cachedGroundVel;
    }
    m_prevOnGround = onGround;

    if (!IsFiniteVec3(final_vel))
        final_vel = JPH::Vec3::sZero();


    m_pCharVir->SetLinearVelocity(final_vel);

    m_pGameInstance->CharVir_ExtendedUpdate(
        fTimeDelta,
        m_pCharVir,
        m_vGravity,
        m_iNumObjectLayer,
        m_pBodyFilter,
        m_pShapeFilter,
        m_tEXUpdateSetting
    );

    // ===== Jolt 업데이트 (1회) =====
    if (m_pCharVir->GetGroundState() == JPH::CharacterVirtual::EGroundState::OnGround) {
        const JPH::Vec3 n = m_pCharVir->GetGroundNormal();          // 단위 법선
        JPH::Vec3 v = m_pCharVir->GetLinearVelocity();

        v -= n * v.Dot(n);


        m_pCharVir->SetLinearVelocity(v);
    }

    // ===== 스텝 이후 현재 포즈 갱신 =====
    m_tCurrPose.vPos = m_pCharVir->GetPosition();
    m_tCurrPose.vRot = m_pCharVir->GetRotation();
    m_tCurrPose.vLinvel = final_vel;

    // 안정화: 접지 & 낙하 중이면 Y속도 0
    if (onGround && m_vVelocity.GetY() < 0.0f)
        m_vVelocity.SetY(0.0f);

    if (m_isFirstSync) { m_tPrevPose = m_tCurrPose; m_isFirstSync = false; }
}

void CCharacterVirtual::ApplyInterpolatedPose(float alpha, CTransform* pTransform)
{
    alpha = Clamp01(alpha);

    // RVec3 → Vec3 다운캐스트 후 선형보간 (float로 가볍게)
    const JPH::Vec3 a((float)m_tPrevPose.vPos.GetX(), (float)m_tPrevPose.vPos.GetY(), (float)m_tPrevPose.vPos.GetZ());
    const JPH::Vec3 b((float)m_tCurrPose.vPos.GetX(), (float)m_tCurrPose.vPos.GetY(), (float)m_tCurrPose.vPos.GetZ());
    const JPH::Vec3 ip = a + (b - a) * alpha;

    // 회전: nlerp (slerp보다 훨씬 가벼움)
    JPH::Quat qa = m_tPrevPose.vRot;
    JPH::Quat qb = m_tCurrPose.vRot;
    if (qa.Dot(qb) < 0.0f) qb = JPH::Quat(-qb.GetX(), -qb.GetY(), -qb.GetZ(), -qb.GetW());
    JPH::Quat qi(
        qa.GetX() + (qb.GetX() - qa.GetX()) * alpha,
        qa.GetY() + (qb.GetY() - qa.GetY()) * alpha,
        qa.GetZ() + (qb.GetZ() - qa.GetZ()) * alpha,
        qa.GetW() + (qb.GetW() - qa.GetW()) * alpha
    );
    qi = qi.Normalized();

    // Transform 한 번만 세팅
    pTransform->Set_State(STATE::POSITION, XMVectorSet(ip.GetX(), ip.GetY(), ip.GetZ(), 1.f));
    pTransform->Set_Quaternion(XMVectorSet(qi.GetX(), qi.GetY(), qi.GetZ(), qi.GetW()));
}

void CCharacterVirtual::ApplyInterpolatedPose(_float alpha, _vector& outQuatRotation, _vector& outPosition)
{
    alpha = Clamp01(alpha);

    // RVec3 → Vec3 다운캐스트 후 선형보간 (float로 가볍게)
    const JPH::Vec3 a((_float)m_tPrevPose.vPos.GetX(), (_float)m_tPrevPose.vPos.GetY(), (_float)m_tPrevPose.vPos.GetZ());
    const JPH::Vec3 b((_float)m_tCurrPose.vPos.GetX(), (_float)m_tCurrPose.vPos.GetY(), (_float)m_tCurrPose.vPos.GetZ());
    const JPH::Vec3 ip = a + (b - a) * alpha;

    // 회전: nlerp (slerp보다 훨씬 가벼움)
    JPH::Quat qa = m_tPrevPose.vRot;
    JPH::Quat qb = m_tCurrPose.vRot;
    if (qa.Dot(qb) < 0.0f) qb = JPH::Quat(-qb.GetX(), -qb.GetY(), -qb.GetZ(), -qb.GetW());
    JPH::Quat qi(
        qa.GetX() + (qb.GetX() - qa.GetX()) * alpha,
        qa.GetY() + (qb.GetY() - qa.GetY()) * alpha,
        qa.GetZ() + (qb.GetZ() - qa.GetZ()) * alpha,
        qa.GetW() + (qb.GetW() - qa.GetW()) * alpha
    );
    qi = qi.Normalized();

    outQuatRotation = XMVectorSet(qi.GetX(), qi.GetY(), qi.GetZ(), qi.GetW());
    outPosition = XMVectorSet(ip.GetX(), ip.GetY(), ip.GetZ(), 1.f);
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
