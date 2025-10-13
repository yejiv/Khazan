#include "CharacterVirtual.h"
#include "GameInstance.h"
#include "CharacterContactListener.h"

CCharacterVirtual::CCharacterVirtual(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent { pDevice, pContext }
{

}

CCharacterVirtual::CCharacterVirtual(const CCharacterVirtual& Prototype)
	: CComponent{ Prototype }
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
	SettingDesc.mHitReductionCosMaxAngle = Cos(DegreesToRadians(pDesc->fHitReductionCosMaxAngle));
	SettingDesc.mPenetrationRecoverySpeed = pDesc->fPenetrationRecoverySpeed;
	SettingDesc.mEnhancedInternalEdgeRemoval = pDesc->bEnhancedInternalEdgeRemoval;
	SettingDesc.mShapeOffset = LoadVec3(pDesc->vShapeOffset);
	SettingDesc.mSupportingVolume = pDesc->fSupportingVolume;

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

	m_pCharacterVir = m_pGameInstance->CreateCharacterVirtual(&SettingDesc, RVec3Arg(LoadVec3(pDesc->vPos)), QuatArg(LoadQuat(pDesc->vQuat)), 0, &m_pBodyInterface);
	m_BodyId = m_pCharacterVir->GetInnerBodyID();

	if (!m_BodyId.IsInvalid())
	{
		//m_pBodyInterface->SetObjectLayer(m_BodyId, m_iNumObjectLayer);
		m_pBodyInterface->SetIsSensor(m_BodyId, false);                     

	}

	m_vVelocity = Vec3::sZero();
	m_vUp = Vec3::sAxisY();
	m_vGravity = Vec3(0, -9.81f, 0);

	CCharacterContactListener::CONFIG_DESC ConfigDesc{};

	m_pContactListener = new CCharacterContactListener(ConfigDesc);
	m_pCharacterVir->SetListener(m_pContactListener);

	m_pBodyFilter = new BodyFilter();
	m_pShapeFilter = new ShapeFilter();

	return S_OK;
}
void CCharacterVirtual::Update(_float fTimeDelta, CTransform* pTransform)
{
	
	if (!m_pCharacterVir) return;

	// 1) 수평 입력 속도 (지금은 0; 필요 시 여기서 만들어 더해주면 됨)
	JPH::Vec3 vHorizontal = JPH::Vec3::sZero();

	// 2) 접지 상태 확인 + 중력 처리
	const bool onGround = (m_pCharacterVir->GetGroundState() == JPH::CharacterVirtual::EGroundState::OnGround);
	if (onGround)
	{
		// 바닥에 있을 때는 하강 속도 제거해 '붙이기'
		if (m_vVelocity.GetY() < 0.0f)
			m_vVelocity.SetY(0.0f);

		// (선택) 이동 플랫폼 위라면 플랫폼 속도 보정
		vHorizontal += m_pCharacterVir->GetGroundVelocity();
	}
	else
	{
		// 공중일 때 중력 적용
		m_vVelocity += m_vGravity * fTimeDelta;
	}

	// 3) 최종 의도 속도 구성 후 캐릭터에 세팅
	const JPH::Vec3 desired(vHorizontal.GetX(), m_vVelocity.GetY(), vHorizontal.GetZ());
	m_pCharacterVir->SetLinearVelocity(desired);

	// 4) 캐릭터 업데이트 (네 버전 시그니처)
	m_pGameInstance->CharVir_Update(fTimeDelta, m_pCharacterVir, m_vGravity, m_iNumObjectLayer, m_pBodyFilter, m_pShapeFilter);

	// 5) 결과 Transform 반영
	const JPH::RVec3 pos = m_pCharacterVir->GetPosition();
	const JPH::Quat  rot = m_pCharacterVir->GetRotation();

	// TODO: 너희 엔진 API에 맞게 치환
	_vector vPos = XMVectorSet(pos.GetX(), pos.GetY(), pos.GetZ(), 1.f);
	_vector vRot = XMVectorSet(rot.GetX(), rot.GetY(), rot.GetZ(), rot.GetW());
	pTransform->Set_State(STATE::POSITION, vPos);
	pTransform->Set_Quaternion(vRot);

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

	Safe_Delete(m_pCharacterVir);
	Safe_Delete(m_pBodyFilter);
	Safe_Delete(m_pShapeFilter);
	Safe_Delete(m_pContactListener);
}
