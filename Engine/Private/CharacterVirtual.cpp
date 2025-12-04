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
    SettingDesc.mMass = pDesc->fMass;
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
    if (!std::isfinite(fTimeDelta) || fTimeDelta <= 0.f)
        return;

    // 1) 회전 동기화
    _vector tRotVec = pTransform->Get_Rotation_Quat();
    JPH::Quat tRot = LoadQuat(tRotVec);
    m_pCharVir->SetRotation(tRot);

    // 2) 현재 지면 상태 체크
    const auto ground_state = m_pCharVir->GetGroundState();
    const bool onGround =
        (ground_state == JPH::CharacterVirtual::EGroundState::OnGround) ||
        (ground_state == JPH::CharacterVirtual::EGroundState::OnSteepGround);

    //const bool ignoreRootMotion =
    //    m_isJump ||   // 점프 중
    //    m_isDive ||   // 다이브 중
    //    m_isPower;         // 넉백이나 힘으로 미는 중 (원하면 빼도 됨)

    const bool ignoreRootMotion =
        m_isJump ||
        m_isDive;

    const bool useRootForMotion =
        !ignoreRootMotion && (onGround || m_isOnLadder);

    m_isUseRootMotion = false;
    
    if (useRootForMotion)
    {
        m_isUseRootMotion = true;

        const RVec3 curCharPos = m_pCharVir->GetPosition();
        JPH::Vec3   curCharPosF(
            (float)curCharPos.GetX(),
            (float)curCharPos.GetY(),
            (float)curCharPos.GetZ()
        );

        _vector tPosVec = pTransform->Get_State(STATE::POSITION);
        JPH::Vec3 tPos = LoadVec3(tPosVec);

        JPH::Vec3 rootDelta = tPos - curCharPosF;

        if (!m_isOnLadder)
        {
            rootDelta.SetY(0.0f);
        }        
        if (fTimeDelta > 0.f && rootDelta.LengthSq() > 0.0f)
        {
            JPH::Vec3 rootVel = rootDelta / fTimeDelta;

            float vy = m_vVelocity.GetY();

            if (!m_isOnLadder)
                rootVel = m_pCharVir->CancelVelocityTowardsSteepSlopes(rootVel);

            m_vVelocity = rootVel;

            if (!m_isOnLadder)
                m_vVelocity.SetY(vy);
        }
        else if (m_isOnLadder)
        {
            m_vVelocity = JPH::Vec3::sZero();
        }
        else 
        {
            m_vVelocity = JPH::Vec3::sZero();
        }
    }

    // 4) Transform 위치를 캐릭터에 맞추기 (이건 그대로)
    {
        const RVec3 curCharPos = m_pCharVir->GetPosition();
        JPH::Vec3   curCharPosF(
            (float)curCharPos.GetX(),
            (float)curCharPos.GetY(),
            (float)curCharPos.GetZ()
        );

        pTransform->Set_State(
            STATE::POSITION,
            XMVectorSet(curCharPosF.GetX(), curCharPosF.GetY(), curCharPosF.GetZ(), 1.f)
        );
    }
    
    // 5) 중력 세팅
    {
        float gx = XMVectorGetX(vGravity);
        float gy = XMVectorGetY(vGravity);
        float gz = XMVectorGetZ(vGravity);

        if (std::isfinite(gx) && std::isfinite(gy) && std::isfinite(gz))
            m_vGravity = JPH::Vec3(gx, gy, gz);
        else
            m_vGravity = JPH::Vec3(0.f, g_fGravity, 0.f);
    }

    // 6) 스텝 업데이트
    StepFixed(fTimeDelta);

    // 7) 최종 위치/회전 다시 Transform에 반영
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

void CCharacterVirtual::Update_Dive(_float fTimeDelta)
{
    if (!m_pCharVir)
        return;

    const RVec3 curPosR = m_pCharVir->GetPosition();
    JPH::Vec3   vCurPos(
        (float)curPosR.GetX(),
        (float)curPosR.GetY(),
        (float)curPosR.GetZ()
    );

    JPH::Vec3 vToTarget = m_vDivePos - vCurPos;
    _float     fDist = vToTarget.Length();

    // 1) 거의 도착했다고 판단되면 강제 스냅
    if (fDist < 0.3f || m_pCharVir->IsSupported())
    {
        // 상태 플래그 리셋
        m_isDive = false;
        m_isJump = false;
        m_isPower = false;
        m_fLoss = m_fBaseLoss;

        // 위치/속도 리셋
        RVec3 snapPos(
            (double)m_vDivePos.GetX(),
            (double)m_vDivePos.GetY(),
            (double)m_vDivePos.GetZ()
        );
        m_pCharVir->SetPosition(snapPos);

        m_vVelocity = JPH::Vec3::sZero();
        m_pCharVir->SetLinearVelocity(m_vVelocity);

        // 접촉정보 다시 계산
        m_pGameInstance->CharVir_RefreshContact(
            m_pCharVir,
            m_iNumObjectLayer,
            m_pBodyFilter,
            m_pShapeFilter
        );
    }

    // 2) 다이브 스피드 보정
    _float fDiveSpeed = m_fDiveSpeed;
    if (!std::isfinite(fDiveSpeed) || fDiveSpeed <= 0.0f)
        fDiveSpeed = 25.0f;

    if (fDist > 1e-4f)
    {
        vToTarget /= fDist; // 방향 벡터

        // 필요하다면 Y를 어느 정도 아래로만 제한
        // if (vToTarget.GetY() > -0.2f)
        //     vToTarget.SetY(-0.2f);
    }
    else
    {
        vToTarget = JPH::Vec3(0.0f, -1.0f, 0.0f);
        fDist = 0.0f;
    }

    // 3) 이번 프레임에 이동 가능한 최대 거리
    _float maxStep = fDiveSpeed * fTimeDelta;
    if (maxStep <= 0.0f)
        return;

    // 4) 남은 거리보다 더 많이 가지 않도록 클램프
    _float stepDist = (fDist > maxStep) ? maxStep : fDist;

    // 이 속도로 가면 정확히 stepDist만큼 이동함
    JPH::Vec3 vStepVel = vToTarget * (stepDist / fTimeDelta);

    if (!IsFiniteVec3(vStepVel))
        vStepVel = JPH::Vec3::sZero();

    m_vVelocity = vStepVel;
    m_pCharVir->SetLinearVelocity(m_vVelocity);

    // 5) 다이브 동안은 중력 0으로, "목표로 끌려가는" 느낌으로
    m_pGameInstance->CharVir_ExtendedUpdate(
        fTimeDelta,
        m_pCharVir,
        JPH::Vec3::sZero(), // 중력 끔
        m_iNumObjectLayer,
        m_pBodyFilter,
        m_pShapeFilter,
        m_tEXUpdateSetting
    );

    m_vVelocity = m_pCharVir->GetLinearVelocity();

}


void CCharacterVirtual::Set_PosRot(_vector vPos, _vector vRot)
{
	m_pCharVir->SetPosition(LoadVec3(vPos));
	m_pCharVir->SetRotation(LoadQuat(vRot));
    m_vVelocity = { 0.f, 0.f, 0.f };
    m_pCharVir->SetLinearVelocity({ 0.f, 0.f, 0.f });
}

void CCharacterVirtual::StepFixed(_float fTimeDelta)
{
    if (!m_pCharVir)
        return;

    if (m_isDive)
    {
        Update_Dive(fTimeDelta);
        return;
    }

    if (m_isOnLadder)
    {
        if (!IsFiniteVec3(m_vVelocity))
            m_vVelocity = JPH::Vec3::sZero();

        m_pCharVir->SetLinearVelocity(m_vVelocity);

        m_pGameInstance->CharVir_ExtendedUpdate(
            fTimeDelta,
            m_pCharVir,
            JPH::Vec3::sZero(),
            m_iNumObjectLayer,
            m_pBodyFilter,
            m_pShapeFilter,
            m_tEXUpdateSetting
        );

        m_vVelocity = m_pCharVir->GetLinearVelocity();

        return;
    }

    if (!m_pCharVir->IsSupported())
    {

        if (m_isJump)
        {
            m_vVelocity += m_vGravity * fTimeDelta;
        }
        else
        {
            m_vVelocity += m_vGravity * fTimeDelta * 1.3f;
        }

        const _float maxFallSpeed = -50.0f;
        if (m_vVelocity.GetY() < maxFallSpeed)
            m_vVelocity.SetY(maxFallSpeed);
    }
    else
    {
        if (m_vVelocity.GetY() < 0.0f)
            m_vVelocity.SetY(0.0f);
    }


    _float loss = 0.0f;

    if (!m_isJump)
    {
        if (m_isPower)
        {
            loss = m_fLoss;
        }
        else if (!m_isUseRootMotion)
        {
            loss = m_pCharVir->IsSupported() ? m_fBaseLoss : m_fBaseAirLoss;
        }
            
    }
    else
    {
        loss = m_pCharVir->IsSupported() ? m_fBaseLoss : m_fBaseAirLoss;
    }
        

    if (loss > 0.0f)
    {
        const _float k = expf(-loss * fTimeDelta);
        m_vVelocity.SetX(m_vVelocity.GetX() * k);
        m_vVelocity.SetY(m_vVelocity.GetY() * k);
        m_vVelocity.SetZ(m_vVelocity.GetZ() * k);
    }

    if (m_isPower)
    {
        const float minSpeedSq = 0.01f; 
        if (m_vVelocity.LengthSq() < minSpeedSq)
        {
            m_isPower = false;
            m_vVelocity = JPH::Vec3::sZero();

            // 감쇠값도 기본으로 되돌리기
            m_fLoss = m_fBaseLoss;
            if (m_pCharVir)
                m_pCharVir->SetLinearVelocity(m_vVelocity);
        }
    }


    if (!IsFiniteVec3(m_vVelocity))
        m_vVelocity = JPH::Vec3::sZero();

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

    m_vVelocity = m_pCharVir->GetLinearVelocity();

    if (m_isJump && m_pCharVir->IsSupported())
    {
        m_isJump = false;

        // 착지 후 Y속도는 0으로 클램프(바운스 방지)
        if (m_vVelocity.GetY() < 0.0f)
            m_vVelocity.SetY(0.0f);

        m_pCharVir->SetLinearVelocity(m_vVelocity);
    }

    if (!m_isJump && !m_isPower && !m_isOnLadder && !m_isDive && m_pCharVir->IsSupported())
    {

        JPH::Vec3 vHoriz(m_vVelocity.GetX(), 0.0f, m_vVelocity.GetZ());
        float lenSq = vHoriz.LengthSq();


        const float stopThreshold = 0.05f;
        if (lenSq < stopThreshold * stopThreshold)
        {
            m_vVelocity.SetX(0.0f);
            m_vVelocity.SetZ(0.0f);
            m_pCharVir->SetLinearVelocity(m_vVelocity);
        }
    }
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
	//m_pBodyInterface->SetGravityFactor(m_pCharVir->GetInnerBodyID(), fGravity);
    m_vGravity = Vec3(0.f, fGravity, 0.f);
}



void CCharacterVirtual::Set_VelocityPower(_vector vDir, _float fPower, _float fLoss)
{
	m_vVelocity = Vec3(vDir.m128_f32[0], vDir.m128_f32[1], vDir.m128_f32[2]) * fPower;
	m_fLoss = fLoss;
    m_isPower = true;

    if (m_pCharVir)
        m_pCharVir->SetLinearVelocity(m_vVelocity);
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

void CCharacterVirtual::Jump(_float fHeightUp, _float fHorizontalSpeed)
{
    if (!m_pCharVir)
        return;

    _float fGravity = m_vGravity.GetY();
    if (!isfinite(fGravity) || fGravity >= 0.f)
        fGravity = g_fGravity;

    _float fHeight = fHeightUp;
    if (!isfinite(fHeight) || fHeight <= 0.f)
        fHeight = 0.5f;

    _float v0Y = sqrtf(-2.f * fGravity * fHeight);

    Vec3 vHoriz(m_vVelocity.GetX(), 0.f, m_vVelocity.GetZ());
    if (vHoriz.LengthSq() > 0.0001f && fHorizontalSpeed > 0.f)
    {
        vHoriz = vHoriz.Normalized() * fHorizontalSpeed;
    }
    else
    {
        vHoriz = Vec3::sZero();
    }

    Vec3 v0 = vHoriz;
    v0.SetY(v0Y);

    if (!IsFiniteVec3(v0))
        return;

    m_isJump = true;
    m_vVelocity = v0;
    m_pCharVir->SetLinearVelocity(m_vVelocity);

    m_vGravity = Vec3(0.f, fGravity, 0.f);
}

void CCharacterVirtual::Jump_ToTarget(_vector vTargetWorldPos,
    _float  fJumpApexHeight,
    _float  fDesiredHorizontalSpeed)
{
    if (!m_pCharVir)
        return;

    // --- 시작 / 목표 위치 ---
    const RVec3 vCurrentPosR = m_pCharVir->GetPosition();
    JPH::Vec3   vStartPos(
        (float)vCurrentPosR.GetX(),
        (float)vCurrentPosR.GetY(),
        (float)vCurrentPosR.GetZ()
    );

    JPH::Vec3 vTargetPos = LoadVec3(vTargetWorldPos);

    _float fStartY = vStartPos.GetY();
    _float fTargetY = vTargetPos.GetY();

    // --- 중력 ---
    _float fGravityY = m_vGravity.GetY();
    if (!std::isfinite(fGravityY) || fGravityY >= 0.0f)
        fGravityY = -9.81f;

    // --- 원하는 최고 높이 (현재 위치 기준) ---
    _float fClampedApexHeight = fJumpApexHeight;
    if (!std::isfinite(fClampedApexHeight) || fClampedApexHeight <= 0.0f)
        fClampedApexHeight = 0.5f;

    // 수직 초기 속도: apex = fStartY + fClampedApexHeight
    _float fInitialVerticalVelocity = sqrtf(-2.0f * fGravityY * fClampedApexHeight);

    // 이 수직 속도로 targetY에 도달하는 시간 T 해 구하기:
    // 0.5 * g * T^2 + v0y * T + (startY - targetY) = 0
    _float fQuadA = 0.5f * fGravityY;
    _float fQuadB = fInitialVerticalVelocity;
    _float fQuadC = fStartY - fTargetY;

    _float fDiscriminant = fQuadB * fQuadB - 4.0f * fQuadA * fQuadC;
    if (fDiscriminant < 0.0f)
    {
        // 이 높이/중력 조합으로는 targetY에 도달 불가 → 자기 기준 점프로 대체
        Jump(fJumpApexHeight, fDesiredHorizontalSpeed);
        return;
    }

    _float fSqrtDiscriminant = sqrtf(fDiscriminant);
    _float fTravelTimeCandidate1 = (-fQuadB + fSqrtDiscriminant) / (2.0f * fQuadA);
    _float fTravelTimeCandidate2 = (-fQuadB - fSqrtDiscriminant) / (2.0f * fQuadA);
    _float fTimeToApex = -fInitialVerticalVelocity / fGravityY; // 최고점까지 시간

    // --- 수평 거리 ---
    JPH::Vec3 vToTarget = vTargetPos - vStartPos;
    JPH::Vec3 vToTargetXZ = JPH::Vec3(vToTarget.GetX(), 0.0f, vToTarget.GetZ());
    _float    fHorizontalDistance = vToTargetXZ.Length();

    if (fHorizontalDistance < 0.0001f)
    {
        // 거의 같은 자리 → 수평 이동 의미 없음 → 제자리 점프
        Jump(fJumpApexHeight, 0.0f);
        return;
    }

    auto IsValidTravelTime = [&](float fTravelTime) -> bool
        {
            return fTravelTime > fTimeToApex &&
                fTravelTime > 0.0f &&
                std::isfinite(fTravelTime);
        };

    _float fSelectedTravelTime = -1.0f;
    _float fBestSpeedDiff = FLT_MAX;

    auto EvaluateCandidateTime = [&](float fCandidateTime)
        {
            if (!IsValidTravelTime(fCandidateTime))
                return;

            _float fRequiredHorizontalSpeed = fHorizontalDistance / fCandidateTime;
            _float fSpeedDiff = fabsf(fRequiredHorizontalSpeed - fDesiredHorizontalSpeed);

            if (fSpeedDiff < fBestSpeedDiff)
            {
                fBestSpeedDiff = fSpeedDiff;
                fSelectedTravelTime = fCandidateTime;
            }
        };

    // 두 후보 시간 중에서 "원하는 수평 속도"에 가장 가까운 것을 고른다.
    EvaluateCandidateTime(fTravelTimeCandidate1);
    EvaluateCandidateTime(fTravelTimeCandidate2);

    // 쓸만한 시간 못 찾으면 fallback
    if (!(fSelectedTravelTime > 0.0f && std::isfinite(fSelectedTravelTime)))
    {
        Jump(fJumpApexHeight, fDesiredHorizontalSpeed);
        return;
    }

    // --- 최종 초기 속도 계산 ---
    JPH::Vec3 vInitialVelocity = JPH::Vec3::sZero();
    if (fHorizontalDistance > 0.0001f)
    {
        vInitialVelocity.SetX(vToTargetXZ.GetX() / fSelectedTravelTime);
        vInitialVelocity.SetZ(vToTargetXZ.GetZ() / fSelectedTravelTime);
    }

    vInitialVelocity.SetY(fInitialVerticalVelocity);

    if (!IsFiniteVec3(vInitialVelocity))
    {
        Jump(fJumpApexHeight, fDesiredHorizontalSpeed);
        return;
    }

    m_isJump = true;
    m_vVelocity = vInitialVelocity;
    m_pCharVir->SetLinearVelocity(m_vVelocity);

    m_vGravity = JPH::Vec3(0.0f, fGravityY, 0.0f);

}

void CCharacterVirtual::Jump_Direction(_vector vDir, _float fHeight, _float fSpeed)
{
    if (!m_pCharVir)
        return;

    _float gy = m_vGravity.GetY();
    if (!std::isfinite(gy) || gy >= 0.0f)
        gy = g_fGravity;        

    _float height = fHeight;
    if (!std::isfinite(height) || height <= 0.0f)
        height = 0.5f;   

    _float v0Y = sqrtf(-2.0f * gy * height);

    JPH::Vec3 horizDir = LoadVec3(vDir);
    horizDir.SetY(0.0f);

    JPH::Vec3 vHoriz = JPH::Vec3::sZero();

    if (horizDir.LengthSq() > 1e-6f && fSpeed > 0.0f)
    {
        horizDir = horizDir.Normalized();
        vHoriz = horizDir * fSpeed;
    }

    JPH::Vec3 v0 = vHoriz;
    v0.SetY(v0Y);

    if (!IsFiniteVec3(v0))
        return;

    m_isJump = true;
    m_isDive = false;

    m_vVelocity = v0;
    m_pCharVir->SetLinearVelocity(m_vVelocity);

    m_vGravity = JPH::Vec3(0.0f, gy, 0.0f);
}

void CCharacterVirtual::Start_Dive(_vector vDivePos, _float fDiveSpeed)
{
    if (!m_pCharVir)
        return;

    const RVec3 curPosR = m_pCharVir->GetPosition();
    JPH::Vec3   vStartPos(
        (float)curPosR.GetX(),
        (float)curPosR.GetY(),
        (float)curPosR.GetZ()
    );

    m_vDivePos = LoadVec3(vDivePos);

    if (!std::isfinite(fDiveSpeed) || fDiveSpeed <= 0.0f)
        m_fDiveSpeed = 25.0f; 
    else
        m_fDiveSpeed = fDiveSpeed;

    JPH::Vec3 vToTarget = m_vDivePos - vStartPos;

    if (vToTarget.LengthSq() < 0.0001f)
        return;

    if (vToTarget.GetY() > -0.2f)
        vToTarget.SetY(-0.2f);

    vToTarget = vToTarget.Normalized();

    JPH::Vec3 vInitialVel = vToTarget * m_fDiveSpeed;

    if (!IsFiniteVec3(vInitialVel))
        return;

    m_isJump = true;
    m_isDive = true;

    m_vVelocity = vInitialVel;
    m_pCharVir->SetLinearVelocity(m_vVelocity);

    m_vGravity = JPH::Vec3(0.0f, m_vGravity.GetY(), 0.0f);
}

_bool CCharacterVirtual::Get_isGround()
{
    if (m_pCharVir->IsSupported())
        return true;

	return false;
}

_vector CCharacterVirtual::Get_GroundPos()
{
    return _vector();
}

void CCharacterVirtual::Teleport(_vector vPos, _vector qRot, CTransform* pTransform)
{
    if (m_pCharVir == nullptr)
        return;

    m_isDive = false;
    m_isJump = false;
    m_isOnLadder = false;
    m_isPower = false;

    // 1) 위치 / 회전 즉시 세팅
    JPH::RVec3 jPos = RVec3(
        XMVectorGetX(vPos),
        XMVectorGetY(vPos),
        XMVectorGetZ(vPos)
    );

    m_pCharVir->SetPosition(jPos);
    m_pCharVir->SetRotation(LoadQuat(qRot));

    // 2) 속도 완전 0으로
    m_pCharVir->SetLinearVelocity(JPH::Vec3::sZero());

    // 3) 내가 따로 들고 있던 속도 / 점프 상태도 0으로
    m_vVelocity = JPH::Vec3::sZero();   // 네가 쓰는 필드 이름에 맞게    

    // 4) 새 위치 기준으로 접촉 정보 다시 계산
    //    (Update에서 쓰는 필터/알로케이터 그대로 사용)
    m_pGameInstance->CharVir_RefreshContact(
        m_pCharVir,
        m_iNumObjectLayer,
        m_pBodyFilter,
        m_pShapeFilter
    );

    pTransform->Set_State(STATE::POSITION, vPos);
}

void CCharacterVirtual::Begin_Ladder()
{
    m_isOnLadder = true;
    
    m_vVelocity = JPH::Vec3::sZero();
    if (m_pCharVir)
        m_pCharVir->SetLinearVelocity(Vec3::sZero());

    m_isJump = false;
    m_isDive = false;
}

void CCharacterVirtual::End_Ladder()
{
    m_isOnLadder = false;
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

    if (m_pCharVir)
    {
        m_pGameInstance->Remove_CharacterVirtual(m_pCharVir->GetID());


        m_pCharVir = nullptr;
        m_BodyId = BodyID();
    }


	Safe_Delete(m_pBodyFilter);
	Safe_Delete(m_pShapeFilter);
}
