#include "CharacterVirtual.h"
#include "GameInstance.h"

CCharacterVirtual::CCharacterVirtual(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent { pDevice, pContext }
    , m_pGameInstance { CGameInstance::GetInstance()}
{
    Safe_AddRef(m_pGameInstance);
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
	
	CharacterVirtualSettings SettingDesc{};

	SettingDesc.mInnerBodyShape;

	SettingDesc.mID = CharacterID::CharacterID();
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

	m_pCharacterVir = m_pGameInstance->CreateCharacterVirtual(&SettingDesc, RVec3Arg(LoadVec3(pDesc->vPos)), QuatArg(LoadQuat(pDesc->vQuat)), 0);
	
	return S_OK;
}
void CCharacterVirtual::Update()
{

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
    Safe_Release(m_pGameInstance);
}
