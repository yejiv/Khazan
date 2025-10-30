#include "Camera.h"
#include "GameInstance.h"

CCamera::CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject { pDevice, pContext }
{

}

CCamera::CCamera(const CCamera& Prototype)
	: CGameObject{ Prototype }
{

}

HRESULT CCamera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera::Initialize_Clone(void* pArg)
{
	CAMERA_DESC* pDesc = static_cast<CAMERA_DESC*>(pArg);

	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat4(&pDesc->vEye));
	m_pTransformCom->LookAt(XMLoadFloat4(&pDesc->vAt));

	_uint			iNumViewports = { 1 };
	D3D11_VIEWPORT	ViewportDesc{};

	m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	m_vEye = pDesc->vEye;
	m_vAt = pDesc->vAt;
	m_fFovy = pDesc->fFovy;
	m_fAspect = ViewportDesc.Width / ViewportDesc.Height;
	m_fNear = pDesc->fNear;
	m_fFar = pDesc->fFar;

	m_fMouseSensor = pDesc->fMouseSensor;

	m_iCameraType = pDesc->iCameraType;
	if (pDesc->strCameraTag != TEXT(""))
		m_strCameraTag = pDesc->strCameraTag;

	return S_OK;
}

void CCamera::Priority_Update(_float fTimeDelta)
{
}

void CCamera::Update(_float fTimeDelta)
{
}

void CCamera::Late_Update(_float fTimeDelta)
{
}

HRESULT CCamera::Render()
{
	return S_OK;
}

void CCamera::Set_Animation(_wstring strAnimationTag)
{
	m_pCurrentAnimation = Get_Animations(strAnimationTag);
	if ((*m_pCurrentAnimation).size() < 3)
	{
		m_pCurrentAnimation = nullptr;
		return;
	}
		

	m_isAnimation = true;
	m_iAnimationIndex = 0;
	m_vOldLook = m_pTransformCom->Get_State(STATE::LOOK);

	m_tPosCatmullrom.v1 = XMVectorSet(
		(*m_pCurrentAnimation)[m_iAnimationIndex].vTranslation.x,
		(*m_pCurrentAnimation)[m_iAnimationIndex].vTranslation.y,
		(*m_pCurrentAnimation)[m_iAnimationIndex].vTranslation.z,
		1.f
	);
	m_tPosCatmullrom.v2 = XMVectorSet(
		(*m_pCurrentAnimation)[m_iAnimationIndex].vTranslation.x,
		(*m_pCurrentAnimation)[m_iAnimationIndex].vTranslation.y,
		(*m_pCurrentAnimation)[m_iAnimationIndex].vTranslation.z,
		1.f
	);
	m_tPosCatmullrom.v3 = XMVectorSet(
		(*m_pCurrentAnimation)[m_iAnimationIndex + 1].vTranslation.x,
		(*m_pCurrentAnimation)[m_iAnimationIndex + 1].vTranslation.y,
		(*m_pCurrentAnimation)[m_iAnimationIndex + 1].vTranslation.z,
		1.f
	);
	m_tPosCatmullrom.v4 = XMVectorSet(
		(*m_pCurrentAnimation)[m_iAnimationIndex + 2].vTranslation.x,
		(*m_pCurrentAnimation)[m_iAnimationIndex + 2].vTranslation.y,
		(*m_pCurrentAnimation)[m_iAnimationIndex + 2].vTranslation.z,
		1.f
	);
}

void CCamera::Play_Animation(_float fTimeDelta)
{
	if (m_isAnimation && m_pCurrentAnimation)
	{
		m_fCurrentTrackPosition = fTimeDelta * (*m_pCurrentAnimation)[m_iAnimationIndex].fSpeed;
		m_fAnimationRatio += m_fCurrentTrackPosition / (*m_pCurrentAnimation)[m_iAnimationIndex].fTrackPosition;

		XMVECTOR vPos = XMVectorCatmullRom(
			m_tPosCatmullrom.v1,
			m_tPosCatmullrom.v2,
			m_tPosCatmullrom.v3,
			m_tPosCatmullrom.v4,
			m_fAnimationRatio
		);

		_vector vLook = XMVector3Normalize(XMVectorLerp(m_vOldLook, XMLoadFloat4(&(*m_pCurrentAnimation)[m_iAnimationIndex].vLookAt), m_fAnimationRatio));

		_vector vUp = XMVectorSet(0, 1, 0, 0);
		_vector vRight = XMVector3Normalize(XMVector3Cross(vUp, vLook));
		vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));

		m_pTransformCom->Set_State(STATE::RIGHT, vRight);
		m_pTransformCom->Set_State(STATE::UP, vUp);
		m_pTransformCom->Set_State(STATE::LOOK, vLook);
		m_pTransformCom->Set_State(STATE::POSITION, vPos);

		if (m_fAnimationRatio >= 1.f)
		{
			m_iAnimationIndex++;
			if ((*m_pCurrentAnimation).size() <= m_iAnimationIndex + 2)
			{
				m_iAnimationIndex = 0;
				m_fCurrentTrackPosition = 0.f;
				m_pCurrentAnimation = nullptr;
				m_isAnimation = false;
			}
			else {
				m_vOldLook = m_pTransformCom->Get_State(STATE::LOOK);
				m_tPosCatmullrom.v1 = XMVectorSet(
					(*m_pCurrentAnimation)[m_iAnimationIndex - 1].vTranslation.x,
					(*m_pCurrentAnimation)[m_iAnimationIndex - 1].vTranslation.y,
					(*m_pCurrentAnimation)[m_iAnimationIndex - 1].vTranslation.z,
					1.f
				);
				m_tPosCatmullrom.v2 = XMVectorSet(
					(*m_pCurrentAnimation)[m_iAnimationIndex].vTranslation.x,
					(*m_pCurrentAnimation)[m_iAnimationIndex].vTranslation.y,
					(*m_pCurrentAnimation)[m_iAnimationIndex].vTranslation.z,
					1.f
				);
				m_tPosCatmullrom.v3 = XMVectorSet(
					(*m_pCurrentAnimation)[m_iAnimationIndex + 1].vTranslation.x,
					(*m_pCurrentAnimation)[m_iAnimationIndex + 1].vTranslation.y,
					(*m_pCurrentAnimation)[m_iAnimationIndex + 1].vTranslation.z,
					1.f
				);
				m_tPosCatmullrom.v4 = XMVectorSet(
					(*m_pCurrentAnimation)[m_iAnimationIndex + 2].vTranslation.x,
					(*m_pCurrentAnimation)[m_iAnimationIndex + 2].vTranslation.y,
					(*m_pCurrentAnimation)[m_iAnimationIndex + 2].vTranslation.z,
					1.f
				);
			}
			m_fAnimationRatio = 0.f;
		}
	}
}

void CCamera::Create_Animation(_wstring strAnimationTag)
{
	auto iter = m_Animations.find(strAnimationTag);

	if (iter == m_Animations.end())
	{
		vector<CAMERA_KEYFRAME> Animations;
		m_Animations.emplace(strAnimationTag, Animations);
	}
}

void CCamera::Create_Event(_wstring strAnimationTag)
{
	auto iter = m_Events.find(strAnimationTag);

	if (iter == m_Events.end())
	{
		vector<CAMERA_EVENT_DATA> Events;
		m_Events.emplace(strAnimationTag, Events);
	}
}

void CCamera::Create_Animation_Item(_wstring strAnimationTag)
{
	auto iter = m_Animations.find(strAnimationTag);

	if (iter == m_Animations.end())
		return;

	CAMERA_KEYFRAME Desc{};
	Desc.fTrackPosition = 0.f;
	Desc.vTranslation = _float3(0.f, 0.f, 0.f);
	Desc.vLookAt = _float4(0.f, 0.f, 0.f, 0.f);

	iter->second.push_back(Desc);
}

void CCamera::Create_Event_Item(_wstring strAnimationTag)
{
	auto iter = m_Events.find(strAnimationTag);

	if (iter == m_Events.end())
		return;

	CAMERA_EVENT_DATA Desc{};
	Desc.fTrackPosition = 0.f;
	Desc.isComplete = false;
	Desc.strEventKey = TEXT("");
	Desc.iEventType = 0;

	iter->second.push_back(Desc);
}

void CCamera::Set_Animation_Item(_wstring strAnimationTag, CAMERA_KEYFRAME tAnimation, _uint iIndex)
{
	auto iter = m_Animations.find(strAnimationTag);

	if (iter == m_Animations.end())
		return;

	if (iter->second.size() < iIndex)
		return;

	iter->second[iIndex] = tAnimation;
}

void CCamera::Set_Event_Item(_wstring strAnimationTag, CAMERA_EVENT_DATA tEvent, _uint iIndex)
{
	auto iter = m_Events.find(strAnimationTag);

	if (iter == m_Events.end())
		return;

	if (iter->second.size() < iIndex)
		return;

	iter->second[iIndex] = tEvent;
}

void CCamera::Add_Animation(_wstring strAnimationTag, CAMERA_KEYFRAME tAnimation)
{
	auto iter = m_Animations.find(strAnimationTag);

	if (iter == m_Animations.end())
	{
		vector<CAMERA_KEYFRAME> Animations;
		Animations.push_back(tAnimation);
		m_Animations.emplace(strAnimationTag, Animations);
	}
	else
	{
		iter->second.push_back(tAnimation);
	}
}

void CCamera::Add_Event(_wstring strAnimationTag, CAMERA_EVENT_DATA tEvent)
{
	auto iter = m_Events.find(strAnimationTag);

	if (iter == m_Events.end())
	{
		vector<CAMERA_EVENT_DATA> Events;
		Events.push_back(tEvent);
		m_Events.emplace(strAnimationTag, Events);
	}
	else
	{
		iter->second.push_back(tEvent);
	}
}

HRESULT CCamera::Remove_Animation(_wstring strAnimationTag, _uint iIndex)
{
	vector<CAMERA_KEYFRAME>* Animations = Get_Animations(strAnimationTag);

	if (Animations == nullptr || iIndex > Animations->size())
		return E_FAIL;

	if(iIndex >= 0 && iIndex < Animations->size())
		Animations->erase(Animations->begin() + iIndex);

	if (Animations->size() == 0)
	{
		m_Animations.erase(strAnimationTag);
	}

	return S_OK;
}

HRESULT CCamera::Remove_AllAnimation(_wstring strAnimationTag)
{
	m_Animations.erase(strAnimationTag);

	return S_OK;
}

HRESULT CCamera::Remove_Event(_wstring strAnimationTag, _uint iIndex)
{
	vector<CAMERA_EVENT_DATA>* Events = Get_Events(strAnimationTag);

	if (Events == nullptr || iIndex > Events->size())
		return E_FAIL;

	if (iIndex >= 0 && iIndex < Events->size())
		Events->erase(Events->begin() + iIndex);

	if (Events->size() == 0)
	{
		m_Events.erase(strAnimationTag);
	}

	return S_OK;
}

HRESULT CCamera::Remove_AllEvent(_wstring strAnimationTag)
{
	m_Events.erase(strAnimationTag);

	return S_OK;
}

CCamera::CAMERA_DESC CCamera::Get_CameraDesc()
{
	CAMERA_DESC Desc{};
	Desc.fFar = m_fFar;
	Desc.fFovy = m_fFovy;
	Desc.fMouseSensor = m_fMouseSensor;
	Desc.fNear = m_fNear;
	Desc.fRotationPerSec = m_pTransformCom->Get_RotationPerSec();
	Desc.fSpeedPerSec = m_pTransformCom->Get_SpeedPerSec();
	Desc.iCameraType = m_iCameraType;
	Desc.strCameraTag = m_strCameraTag;
	Desc.vAt = m_vAt;
	Desc.vEye = m_vEye;

	return Desc;
}

vector<CAMERA_KEYFRAME>* CCamera::Get_Animations(_wstring strAnimationTag)
{
	auto iter = m_Animations.find(strAnimationTag);

	if (iter == m_Animations.end())
	{
		return nullptr;
	}
	else
	{
		return &iter->second;
	}

	
}

vector<CAMERA_EVENT_DATA>* CCamera::Get_Events(_wstring strAnimationTag)
{
	auto iter = m_Events.find(strAnimationTag);

	if (iter == m_Events.end())
	{
		return nullptr;
	}
	else
	{
		return &iter->second;
	}
}

HRESULT CCamera::Set_DefaultData(CAMERA_DESC tDesc)
{
	m_fFovy = tDesc.fFovy;
	m_fNear = tDesc.fNear;
	m_fFar = tDesc.fFar;
	m_fMouseSensor = tDesc.fMouseSensor;
	m_iCameraType = tDesc.iCameraType;
	m_strCameraTag = tDesc.strCameraTag;	

	return S_OK;
}

HRESULT CCamera::Load(map<_wstring, vector<CAMERA_KEYFRAME>> Animations, map<_wstring, vector<CAMERA_EVENT_DATA>> Events)
{
	m_Animations = Animations;
	m_Events = Events;

	return S_OK;
}

void CCamera::Update_PipeLines()
{
	if (!m_isActive)
		return;

	m_pGameInstance->Set_Transform(D3DTS::VIEW, m_pTransformCom->Get_WorldMatrix_Inverse());
	m_pGameInstance->Set_Transform(D3DTS::PROJ, XMMatrixPerspectiveFovLH(m_fFovy, m_fAspect, m_fNear, m_fFar));
}

void CCamera::Free()
{
	__super::Free();
	m_pObjMatrix = nullptr;
	m_pSocketMatrix = nullptr;

}
