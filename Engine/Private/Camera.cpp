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
	//m_pCurrentAnimation = Get_Animations(strAnimationTag);
	//if ((*m_pCurrentAnimation).size() < 3)
	//{
	//	m_pCurrentAnimation = nullptr;
	//	return;
	//}
	//	

	//m_isAnimation = true;
	//m_iAnimationIndex = 0;
	//m_vOldLook = m_pTransformCom->Get_State(STATE::LOOK);

	//m_tPosCatmullrom.v1 = XMVectorSet(
	//	(*m_pCurrentAnimation)[m_iAnimationIndex].vTranslation.x,
	//	(*m_pCurrentAnimation)[m_iAnimationIndex].vTranslation.y,
	//	(*m_pCurrentAnimation)[m_iAnimationIndex].vTranslation.z,
	//	1.f
	//);
	//m_tPosCatmullrom.v2 = XMVectorSet(
	//	(*m_pCurrentAnimation)[m_iAnimationIndex].vTranslation.x,
	//	(*m_pCurrentAnimation)[m_iAnimationIndex].vTranslation.y,
	//	(*m_pCurrentAnimation)[m_iAnimationIndex].vTranslation.z,
	//	1.f
	//);
	//m_tPosCatmullrom.v3 = XMVectorSet(
	//	(*m_pCurrentAnimation)[m_iAnimationIndex + 1].vTranslation.x,
	//	(*m_pCurrentAnimation)[m_iAnimationIndex + 1].vTranslation.y,
	//	(*m_pCurrentAnimation)[m_iAnimationIndex + 1].vTranslation.z,
	//	1.f
	//);
	//m_tPosCatmullrom.v4 = XMVectorSet(
	//	(*m_pCurrentAnimation)[m_iAnimationIndex + 2].vTranslation.x,
	//	(*m_pCurrentAnimation)[m_iAnimationIndex + 2].vTranslation.y,
	//	(*m_pCurrentAnimation)[m_iAnimationIndex + 2].vTranslation.z,
	//	1.f
	//);
	auto it = m_Animations.find(strAnimationTag);
	if (it == m_Animations.end() || it->second.size() < 2) {
		m_pCurrentAnimation = nullptr; m_isAnimation = false; return;
	}

	m_RuntimeKeys = it->second;
	m_pCurrentAnimation = &m_RuntimeKeys;

	_vector vEntryPos = m_pTransformCom->Get_State(STATE::POSITION);
	_vector vEntryLook = XMVector3Normalize(m_pTransformCom->Get_State(STATE::LOOK));
	vEntryLook = XMVectorSetW(vEntryLook, 0.f);

	auto makeLookF4 = [](_vector look) {
		_float4 f; XMStoreFloat4(&f, XMVectorSetW(XMVector3Normalize(look), 0.f)); return f;
		};

	const int N = (int)m_RuntimeKeys.size();

	
	for (size_t i = 0; i < m_RuntimeKeys.size(); i++)
	{
		if (m_RuntimeKeys[i].isCurPos)
		{
			_float3 p;
			XMStoreFloat3(&p, vEntryPos);
			m_RuntimeKeys[i].vTranslation = p;
			m_RuntimeKeys[i].vLookAt = makeLookF4(vEntryLook);

			if (i == 0)
			{
				m_vCurPos = vEntryPos;
				m_vCurQ = vEntryLook;
			}
		}
		else {
			_vector v0Pos = XMLoadFloat3(&m_RuntimeKeys[i].vTranslation);
			_vector v0Look = XMLoadFloat4(&m_RuntimeKeys[i].vLookAt);


			if (i == 0)
			{
				m_vCurPos = XMVectorSetW(v0Pos, 1.f);
				m_vCurQ = XMVectorSetW(v0Look, 0.f);
			}

		}
	}

	m_isAnimation = true;
	m_isLoop = false;  // 컷씬이면 보통 false
	m_iSeg = 0;
	m_fSegTime = 0.f;
	m_isStarted = true;
}

void CCamera::Play_Animation(_float fTimeDelta)
{
	if (!m_isAnimation || !m_pCurrentAnimation || m_pCurrentAnimation->size() < 2) return;

	const _int iCurrAniSize = (_int)m_pCurrentAnimation->size();


	_float fSpeed = max(1e-4f, (*m_pCurrentAnimation)[m_iSeg].fSpeed);
	_float fTrack = max(1e-4f, (*m_pCurrentAnimation)[m_iSeg].fTrackPosition);
	_float fDuration = fTrack / fSpeed;

	m_fSegTime += fTimeDelta;
	while (m_fSegTime >= fDuration - 1e-6f) {
		m_fSegTime -= fDuration;
		m_iSeg++;
		if (m_iSeg >= iCurrAniSize - 1) {
			if (m_isLoop) 
                m_iSeg = 0;
			else 
            { 
                m_iSeg = iCurrAniSize - 2; 
                m_isAnimation = false; 
                m_fSegTime = fDuration;

                OnCameraAniEnd();

                break;
            }
		}
		// 다음 세그 duration 갱신
		fSpeed = max(1e-4f, (*m_pCurrentAnimation)[m_iSeg].fSpeed);
		fTrack = max(1e-4f, (*m_pCurrentAnimation)[m_iSeg].fTrackPosition);
		fDuration = fTrack / fSpeed;
	}


	_float fSeg = std::clamp(m_fSegTime / fDuration, 0.f, 1.f);
	auto idx = [&](int i)->int { return m_isLoop ? Wrap(i, iCurrAniSize) : std::clamp(i, 0, iCurrAniSize - 1); };

	int iSeg0 = m_iSeg - 1;
	int iSeg1 = m_iSeg;
	int iSeg2 = m_iSeg + 1;
	int iSeg3 = m_iSeg + 2;


	_vector P0, P1, P2, P3;

	if (m_isLoop)
	{
		P0 = XMLoadFloat3(&(*m_pCurrentAnimation)[idx(iSeg0)].vTranslation);
		P1 = XMLoadFloat3(&(*m_pCurrentAnimation)[idx(iSeg1)].vTranslation);
		P2 = XMLoadFloat3(&(*m_pCurrentAnimation)[idx(iSeg2)].vTranslation);
		P3 = XMLoadFloat3(&(*m_pCurrentAnimation)[idx(iSeg3)].vTranslation);
	}
	else
	{

		_vector Key0 = XMLoadFloat3(&(*m_pCurrentAnimation)[idx(iSeg1)].vTranslation);
		_vector Key1 = XMLoadFloat3(&(*m_pCurrentAnimation)[idx(iSeg2)].vTranslation);

		P1 = Key0;
		P2 = Key1;


		if (iSeg0 >= 0)
			P0 = XMLoadFloat3(&(*m_pCurrentAnimation)[iSeg0].vTranslation);
		else
			P0 = P1 * 2.f - P2;


		if (iSeg3 < iCurrAniSize)
			P3 = XMLoadFloat3(&(*m_pCurrentAnimation)[iSeg3].vTranslation);
		else
			P3 = P2 * 2.f - P1;
	}


	float s01 = ChordLenAlpha(P0, P1);
	float s12 = ChordLenAlpha(P1, P2);
	float s23 = ChordLenAlpha(P2, P3);


	_vector vPosTarget;
	if (s12 < 1e-6f)
		vPosTarget = XMVectorLerp(P1, P2, fSeg);
	else
		vPosTarget = CatmullCR(P0, P1, P2, P3, s01, s12, s23, fSeg);

	vPosTarget = XMVectorSetW(vPosTarget, 1.f);


	_vector qFrom = QuatFromLook((*m_pCurrentAnimation)[idx(iSeg1)].vLookAt);
	_vector qTo = QuatFromLook((*m_pCurrentAnimation)[idx(iSeg2)].vLookAt);
	if (XMVectorGetX(XMVector4Dot(qFrom, qTo)) < 0.f) qTo = XMVectorNegate(qTo);
	_vector vQuatTarget = XMQuaternionNormalize(XMQuaternionSlerp(qFrom, qTo, fSeg));


	if (m_isStarted) {
		m_vCurPos = vPosTarget;
		m_vCurQ = vQuatTarget;
		m_isStarted = false;
	}


	float fAlphaPos = DampAlpha(m_fPosSmooth, fTimeDelta);
	float fAlphaRot = DampAlpha(m_fRotSmooth, fTimeDelta);
	m_vCurPos = XMVectorLerp(m_vCurPos, vPosTarget, fAlphaPos);
	m_vCurQ = XMQuaternionNormalize(XMQuaternionSlerp(m_vCurQ, vQuatTarget, fAlphaRot));


	XMMATRIX R = XMMatrixRotationQuaternion(m_vCurQ);
	_vector right = XMVectorSetW(XMVector3Normalize(R.r[0]), 0.f);
	_vector up = XMVectorSetW(XMVector3Normalize(R.r[1]), 0.f);
	_vector look = XMVectorSetW(XMVector3Normalize(R.r[2]), 0.f);

	m_pTransformCom->Set_State(STATE::RIGHT, right);
	m_pTransformCom->Set_State(STATE::UP, up);
	m_pTransformCom->Set_State(STATE::LOOK, look);
	m_pTransformCom->Set_State(STATE::POSITION, m_vCurPos);
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
	Desc.fTrackPosition = static_cast<_float>(iter->second.size());
	_vector vPos = m_pTransformCom->Get_State(STATE::POSITION);
	_vector vLook = m_pTransformCom->Get_State(STATE::LOOK);
	Desc.vTranslation = _float3(vPos.m128_f32[0] , vPos.m128_f32[1], vPos.m128_f32[2]);
	Desc.vLookAt = _float4(vLook.m128_f32[0], vLook.m128_f32[1], vLook.m128_f32[2], vLook.m128_f32[3]);
	Desc.fSpeed = 3.f;

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

HRESULT CCamera::Load_Animation(map<_wstring, vector<CAMERA_KEYFRAME>> Animations)
{
	m_Animations = Animations;

	return S_OK;
}

void CCamera::Update_PipeLines(_float fTimeDelta)
{
	if (!m_isActive)
		return;

	Update_FOVChannel(fTimeDelta);

	m_pGameInstance->Set_Transform(D3DTS::VIEW, m_pTransformCom->Get_WorldMatrix_Inverse());
	m_pGameInstance->Set_Transform(D3DTS::PROJ, XMMatrixPerspectiveFovLH(m_fFovy, m_fAspect, m_fNear, m_fFar));
}

void CCamera::Shaking_Start(_float fPower, _float fDuration)
{
	m_fShaking_Power = fPower;
	m_fShaking_Duration = fDuration;
	m_fShaking_Time = 0.f;

	auto fRnd = [&]{ 
		m_fShaking_Seed = m_fShaking_Seed * 1664525u + 1013904223u;
		return (m_fShaking_Seed & 0xFFFF) / 65535.f; 
	};
	const _float fTAU = 6.2831853f;
	m_vShaking_Phase = _float3(fRnd() * fTAU, fRnd() * fTAU, fRnd() * fTAU);
}

void CCamera::Shaking(_float fTimeDelta)
{
	if (!Shaking_Active()) return;

	// 진행
	m_fShaking_Time = min(m_fShaking_Time + fTimeDelta, m_fShaking_Duration);
	_float fS = m_fShaking_Time / m_fShaking_Duration;

	// 감쇠 (끝으로 갈수록 0)
	_float fEnv = m_fShaking_Power * (1.f - fS);
	fEnv *= fEnv; // (1-s)^2

	// duration 동안 최소 3회 왕복 보장
	const _float fCyclesMin = 3.f;
	_float fFreqEff = max(m_fShaking_Freq, fCyclesMin / max(1e-4f, m_fShaking_Duration));

	// 혼합 파형(편향 방지): sin + 0.5*cos(2x)
	const _float w = 6.2831853f * fFreqEff;     // 2π f
	_float t = m_fShaking_Time;
	_float sx = sinf(w * t + m_vShaking_Phase.x) + 0.7f * cosf(2.f * w * t + m_vShaking_Phase.x * 1.7f);
	_float sy = sinf(w * t * 1.1f + m_vShaking_Phase.y) + 0.5f * cosf(2.1f * w * t + m_vShaking_Phase.y * 1.9f);
	_float sz = sinf(w * t * 0.9f + m_vShaking_Phase.z) + 0.5f * cosf(1.8f * w * t + m_vShaking_Phase.z * 1.3f);

	// 축별 진폭(미터) 사용
	_float fOX = fEnv * m_vShaking_Amplitude.x * sx;
	_float fOY = fEnv * m_vShaking_Amplitude.y * sy;
	_float fOZ = fEnv * m_vShaking_Amplitude.z * sz;

	// ★★ “베이스 축/위치” 기준으로만 오프셋 적용 (누적 금지!)
	_vector vPosOff =
		XMVectorMultiplyAdd(XMVectorReplicate(fOX), m_vShaking_BaseRight,
			XMVectorMultiplyAdd(XMVectorReplicate(fOY), m_vShaking_BaseUp,
				XMVectorMultiplyAdd(XMVectorReplicate(fOZ), m_vShaking_BaseLook, XMVectorZero())));

	_vector vShakenPos = XMVectorAdd(m_vShaking_BasePos, vPosOff);

	// 위치만 흔들기 (회전 쉐이크 원하면 축도 회전해서 Set_State)
	m_pTransformCom->Set_State(STATE::POSITION, vShakenPos);
}

void CCamera::Push_FOVModifier(const FOVModifier& tMod)
{
	_int idx = FindModIndexByID(tMod.strID);
	if (idx >= 0)
	{
		auto& Mod = m_vFOVMods[idx];
		Mod.eMode = tMod.eMode;
		Mod.fFrom = tMod.fFrom;
		Mod.fTo = tMod.fTo;
		Mod.fTime = 0.f;
		Mod.fDuration = tMod.fDuration;
		Mod.iPriority = tMod.iPriority;
		Mod.isAlive = true;
		Mod.Ease = tMod.Ease;
	}
	else
	{
		FOVModifier Mod = tMod;
		Mod.fTime = 0.f;
		Mod.isAlive = true;
		m_vFOVMods.emplace_back(std::move(Mod));
	}

}

void CCamera::Kill_FOVModifier(const _wstring& strID)
{
	_int idx = FindModIndexByID(strID);
	if (idx >= 0)
		m_vFOVMods[idx].isAlive = false;
}

void CCamera::Update_FOVChannel(_float fTimeDelta)
{
	// 타임라인 진행 & 완료 판전
	for (auto& Mod : m_vFOVMods)
	{
		if (Mod.isAlive == false)
			continue;

		Mod.fTime += fTimeDelta;
		const _float fTime = (Mod.fDuration > 0.f) ? min(Mod.fTime / Mod.fDuration, 1.f) : 1.f;

		// 유지형 아니면 즉시 삭제
		if (fTime >= 1.f && Mod.fDuration > 0.f)
		{
			if (Mod.eMode == FOVModifier::FOV_MODE::ADD ||
				Mod.eMode == FOVModifier::FOV_MODE::MULTIPLY)
			{
				Mod.isAlive = false;
			}
		}
	}

	// 죽은 항목 삭제
	m_vFOVMods.erase(
		remove_if(m_vFOVMods.begin(), m_vFOVMods.end(),
			[](const FOVModifier& Mod)
			{
				return Mod.isAlive <= 0.f;
			}), m_vFOVMods.end());

	// 합성 (Base * Multiply + Add) 우선순위 포함하여
	_float fMulAcc = 1.f;
	_float fAddAcc = 0.f;
	const FOVModifier* pPri = nullptr;

	for (auto& Mod : m_vFOVMods)
	{
		const _float fProgress = Mod.fDuration > 0.f ? min(Mod.fTime / Mod.fDuration, 1.f) : 1.f;
		const _float fEaseRatio = Mod.Ease ? Mod.Ease(fProgress) : fProgress;
		const _float fModulated = Mod.fFrom + (Mod.fTo - Mod.fFrom) * fEaseRatio;

		switch (Mod.eMode)
		{
		case FOVModifier::FOV_MODE::MULTIPLY:
			fMulAcc *= fModulated;
			break;

		case FOVModifier::FOV_MODE::ADD:
			fAddAcc += fModulated;
			break;

		case FOVModifier::FOV_MODE::PRIORITY:
			if (!pPri || pPri->iPriority < Mod.iPriority)
				pPri = &Mod;
			break;
		}
	}

	_float fTargetFov = m_fBaseFOV * fMulAcc + fAddAcc;

	if (pPri)
	{
		const _float fProgress =
			(pPri->fDuration > 0.f) ? min(pPri->fTime / pPri->fDuration, 1.f) : 1.f;

		const _float fEaseRatio =
			(pPri->Ease ? pPri->Ease(fProgress) : fProgress);

		fTargetFov = pPri->fFrom + (pPri->fTo - pPri->fFrom) * fEaseRatio;
	}

	if (m_vFOVMods.empty())
		fTargetFov = m_fBaseFOV;

	fTargetFov = max(m_fFOVMin, min(fTargetFov, m_fFOVMax));

	const _float fSmoothingWeight = std::clamp(m_fFOVSmooth * fTimeDelta, 0.f, 1.f);
	const _float fPrevFovy = m_fFovy;
	m_fFovy = fPrevFovy + (fTargetFov - fPrevFovy) * fSmoothingWeight;
}

_int CCamera::FindModIndexByID(const _wstring& strID) const
{
	for (_int i = 0; i < static_cast<_int>(m_vFOVMods.size()); i++)
	{
		if (m_vFOVMods[i].strID == strID)
			return i;
	}

	return -1;
}

void CCamera::Free()
{
	__super::Free();
	m_pObjMatrix = nullptr;
	m_pSocketMatrix = nullptr;

}
