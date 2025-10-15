#include "CharacterVirtual.h"
#include "GameInstance.h"

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
	SettingDesc.mMaxSlopeAngle = DegreesToRadians(pDesc->fMaxSlopeAngle);
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

	m_pCharVir->SetUserData(static_cast<uint64>(reinterpret_cast<uintptr_t>(pDesc->pGameObject)));
	if (!m_BodyId.IsInvalid())
	{
		//m_pBodyInterface->SetObjectLayer(m_BodyId, m_iNumObjectLayer);
		m_pBodyInterface->SetIsSensor(m_BodyId, false);

	}

	m_vVelocity = Vec3::sZero();
	m_vUp = Vec3::sAxisY();
	m_vGravity = Vec3(0, -9.81f, 0);

	m_pBodyFilter = new BodyFilter();
	m_pShapeFilter = new ShapeFilter();

	m_tEXUpdateSetting.mStickToFloorStepDown = LoadVec3(pDesc->vStickToFloorStepDown);
	m_tEXUpdateSetting.mWalkStairsStepUp = LoadVec3(pDesc->vWalkStairsStepUp);
	m_tEXUpdateSetting.mWalkStairsMinStepForward = pDesc->fWalkStairsMinStepForward;
	m_tEXUpdateSetting.mWalkStairsStepForwardTest = pDesc->fWalkStairsStepForwardTest;
	m_tEXUpdateSetting.mWalkStairsCosAngleForwardContact = Cos(DegreesToRadians(pDesc->fWalkStairsCosAngleForwardContact));
	m_tEXUpdateSetting.mWalkStairsStepDownExtra = LoadVec3(pDesc->vWalkStairsStepDownExtra);

	return S_OK;
}
void CCharacterVirtual::Update(_float fTimeDelta, CTransform* pTransform)
{

	if (!m_pCharVir) return;

	JPH::Vec3 vHorizontal = JPH::Vec3::sZero();

	const bool onGround = (m_pCharVir->GetGroundState() == JPH::CharacterVirtual::EGroundState::OnGround);
	if (onGround)
	{
		if (m_vVelocity.GetY() < 0.0f)
			m_vVelocity.SetY(0.0f);

		vHorizontal += m_pCharVir->GetGroundVelocity();
	}
	else
	{
		m_vVelocity += m_vGravity * fTimeDelta;
	}

	const JPH::Vec3 desired(vHorizontal.GetX(), m_vVelocity.GetY(), vHorizontal.GetZ());
	m_pCharVir->SetLinearVelocity(desired);

	//m_pGameInstance->CharVir_Update(fTimeDelta, m_pCharVir, m_vGravity, m_iNumObjectLayer, m_pBodyFilter, m_pShapeFilter);
	m_pGameInstance->CharVir_ExtendedUpdate(fTimeDelta, m_pCharVir, m_vGravity, m_iNumObjectLayer, m_pBodyFilter, m_pShapeFilter, m_tEXUpdateSetting);

	const JPH::RVec3 pos = m_pCharVir->GetPosition();
	const JPH::Quat  rot = m_pCharVir->GetRotation();

	_vector vPos = XMVectorSet(pos.GetX(), pos.GetY(), pos.GetZ(), 1.f);
	_vector vRot = XMVectorSet(rot.GetX(), rot.GetY(), rot.GetZ(), rot.GetW());
	pTransform->Set_State(STATE::POSITION, vPos);
	pTransform->Set_Quaternion(vRot);

}

void CCharacterVirtual::Set_Position(_vector vPos)
{
	m_pCharVir->SetPosition(LoadVec3(vPos));
}

void CCharacterVirtual::Set_Velocity(_vector vVelocity)
{
	m_vVelocity = LoadVec3(vVelocity);
}

void CCharacterVirtual::Set_Rotation(_vector vRotation)
{
	m_pCharVir->SetRotation(LoadQuat(vRotation));
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

	Safe_Delete(m_pCharVir);
	Safe_Delete(m_pBodyFilter);
	Safe_Delete(m_pShapeFilter);
}
