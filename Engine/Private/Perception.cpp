#include "Perception.h"
#include "GameObject.h"
#include "GameInstance.h"
#include "BlackBoard.h"

CPerception::CPerception()
	:m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}


HRESULT CPerception::Initialize(const AIPERCEPTION_DATA& Desc, _uint iTeamID)
{
	m_strName = Desc.strMonsterType;
	m_tSightDesc = Desc;
	m_iTeamID = iTeamID;
	m_fCurrnetTime = 0.f;
	m_fCheckAcc = 0.f;
	m_fSenseRadiusCache = m_tSightDesc.fRadius * m_tSightDesc.fRadius;
	m_Perceived.clear();

	return S_OK;
}

void CPerception::Update(CGameObject* pOwner, _float fTimeDelta)
{
	m_fCheckAcc += fTimeDelta;
	m_fCurrnetTime += fTimeDelta;

	if (m_fCheckAcc >= m_tSightDesc.fCheckInterval)
	{
		m_fCheckAcc = 0.f;
		Check_Sight(pOwner);
	}


	Forget();

}

void CPerception::Check_Sight(CGameObject* pOwner)
{
	CGameObject* pTarget = m_pGameInstance->Get_BlackBoard()->Get_Value<CGameObject*>(m_strName, "Target");
	CTransform* pOwnerTransform = static_cast<CTransform*>(pOwner->Get_Component(TEXT("Com_Transform")));
	CTransform* pTargetTransform = static_cast<CTransform*>(pTarget->Get_Component(TEXT("Com_Transform")));

	_vector vEyePos = pOwnerTransform->Get_State(STATE::POSITION);
	vEyePos.m128_f32[1] += m_tSightDesc.fHeightOffset;
	_vector vLook = pOwnerTransform->Get_State(STATE::LOOK);
	_vector vTargetPos = pTargetTransform->Get_State(STATE::POSITION);

	if (pOwner->Get_Team() == m_iTeamID)
		return;

	_vector vDist = vTargetPos - vEyePos;
	_float fDistsq = XMVectorGetX(vDist) * XMVectorGetX(vDist) + XMVectorGetY(vDist) * XMVectorGetY(vDist) + XMVectorGetZ(vDist) * XMVectorGetZ(vDist);

	if (fDistsq > m_fSenseRadiusCache)
		return;

	_vector vDirToTarget = XMVector3Normalize(vDist);
	_float fDot = XMVectorGetX(XMVector3Dot(XMVector3Normalize(vLook), vDirToTarget));
	if (fDot < m_tSightDesc.fFovCos)
		return;

	// 시야 감지 사이클에 들어왔을때만 갱신
	m_pGameInstance->Get_BlackBoard()->Set_Value<_float>(m_strName, "TargetDist", fDistsq);
	
	//_vector vSide = XMVector3Cross(vLook,vDist);
	_vector vSide = XMVector3Cross(vDist,vLook);
	_float fSide = XMVectorGetY(vSide);

	m_tDirInfo.Clear_Flag();

	// Front
	if (fDot > 0.5f)
	{
		m_tDirInfo.Add_Flag(m_tDirInfo.F);

		if (fSide > 0.5f)
		{
			m_tDirInfo.Add_Flag(m_tDirInfo.R);
		}
		else if (fSide < -0.5f)
		{

			m_tDirInfo.Add_Flag(m_tDirInfo.L);
		}
	}
	// Back
	else if (fDot < -0.5f)
	{
		m_tDirInfo.Add_Flag(m_tDirInfo.B);

		if (fSide > 0.5)
			m_tDirInfo.Add_Flag(m_tDirInfo.R);
		

		else if (fSide < -0.5f)
			m_tDirInfo.Add_Flag(m_tDirInfo.L);
		
	}
	// Side
	else
	{
		if (fSide > 0.f)
			m_tDirInfo.Add_Flag(m_tDirInfo.R);
		else
			m_tDirInfo.Add_Flag(m_tDirInfo.L);
	}
	_float3 vTargetDir;
	XMStoreFloat3(&vTargetDir, vDirToTarget);
	m_pGameInstance->Get_BlackBoard()->Set_Value<_float3>(m_strName, "TargetDir", vTargetDir);
	m_pGameInstance->Get_BlackBoard()->Set_Value<_float>(m_strName, "fDot", fDot);
	m_pGameInstance->Get_BlackBoard()->Set_Value<_uint>(m_strName,"TargetDirection",m_tDirInfo.iDirFlag);


	STIMULUS Stim;
	Stim.eType = SENSETYPE::SIGHT;
	XMStoreFloat3(&Stim.vLocation, vTargetPos);
	Stim.dTimeStamp = m_fCurrnetTime;
	Stim.fStrength = 1.f;
	Stim.bSensed = true;
	Stim.fVaildTime = m_tSightDesc.fLoseSightTime;

	// PERCEIVED_DESC 갱신
	PERCEIVED_DESC& perceived = m_Perceived[pTarget];
	perceived.LastStimulus[SENSETYPE::SIGHT] = Stim;
	perceived.fLastUpdated = m_fCurrnetTime;
	perceived.fLastSeenTime = m_fCurrnetTime;
	perceived.isCurrentlySensed = true;
	perceived.fVisibilityScore = 1.f;
	perceived.fForgetTime = m_tSightDesc.fLoseSightTime;

	// 블랙보드 콜백
	if (m_PerceptionCallBack)
		m_PerceptionCallBack(pTarget, Stim);
}

void CPerception::Notify_Damage(CGameObject* pAttacker, const STIMULUS& Stim)
{
	PERCEIVED_DESC& Perceived = m_Perceived[pAttacker];
	Perceived.LastStimulus[SENSETYPE::DAMAGE] = Stim;
	Perceived.fLastUpdated = m_fCurrnetTime;
	Perceived.isCurrentlySensed = Stim.bSensed;

	if (m_PerceptionCallBack)
		m_PerceptionCallBack(pAttacker, Stim);
}

void CPerception::Forget()
{
	// 망각
	for (auto iter = m_Perceived.begin(); iter != m_Perceived.end(); )
	{
		bool bHasValidStim = false;
		for (auto& Pair : iter->second.LastStimulus)
		{
			if (m_fCurrnetTime - Pair.second.dTimeStamp < Pair.second.fVaildTime)
				bHasValidStim = true;
		}

		if (!bHasValidStim)
		{
			// 타겟 망각 콜백
			if (m_PerceptionCallBack)
			{
				STIMULUS loseStim;
				loseStim.eType = SENSETYPE::SIGHT;
				loseStim.bSensed = false;
				loseStim.dTimeStamp = m_fCurrnetTime;
				m_PerceptionCallBack(iter->first, loseStim);
			}

			iter = m_Perceived.erase(iter);
		}
		else
			++iter;
	}


}


CPerception* CPerception::Create(const AIPERCEPTION_DATA& Desc, _uint iTeamID)
{
	CPerception* pInstance = new CPerception();
	if (FAILED(pInstance->Initialize(Desc, iTeamID)))
	{
		Safe_Release(pInstance);
		MSG_BOX(TEXT("Failed Create : Perception"));
	}
	return pInstance;
}

void CPerception::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
}
