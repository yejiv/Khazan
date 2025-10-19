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

	m_fFovy = pDesc->fFovy;
	m_fAspect = ViewportDesc.Width / ViewportDesc.Height;
	m_fNear = pDesc->fNear;
	m_fFar = pDesc->fFar;

	m_fMouseSensor = pDesc->fMouseSensor;

	m_iCameraType = pDesc->iCameraType;
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

void CCamera::Add_Animation(_wstring strAnimationTag, CAMERA_ANIMATION_DATA tAnimation)
{
	auto iter = m_Animations.find(strAnimationTag);

	if (iter == m_Animations.end())
	{
		vector<CAMERA_ANIMATION_DATA> Animations;
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
	vector<CAMERA_ANIMATION_DATA>* Animations = Get_Animations(strAnimationTag);

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

vector<CAMERA_ANIMATION_DATA>* CCamera::Get_Animations(_wstring strAnimationTag)
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
