#include "Light_Manager.h"
#include "Light.h"
#include "GameInstance.h"

CLight_Manager::CLight_Manager()
	: m_pGameInstance { CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CLight_Manager::Initialize(_uint iNumLevels)
{
	m_pLights = new unordered_map<_wstring, class CLight*>[iNumLevels];

	m_iNumLevels = iNumLevels;

	return S_OK;
}

void CLight_Manager::Update(_float fTimeDelta)
{
    if (false == m_isTransition)
        return;

    m_fTransTimeAcc += fTimeDelta;

    if (m_fTransTimeAcc >= m_TargetLightDesc.fDuration)
    {
        LIGHT_DESC FinalLightDesc = m_StartLightDesc;

        if (true == m_TargetLightDesc.isReturnToStart) 
            //  || (0.f < m_fBlinkPeriod && 0 < m_TargetLightDesc.iBlinkCount))
            m_pTransLight->Set_LightDesc(FinalLightDesc);
        else
        {
            FinalLightDesc.vDiffuse = m_TargetLightDesc.vDiffuse;
            FinalLightDesc.vAmbient = m_TargetLightDesc.vAmbient;
            FinalLightDesc.vSpecular = m_TargetLightDesc.vSpecular;
        }

        m_pTransLight->Set_LightDesc(FinalLightDesc);

        m_fBlinkPeriod = 0.f;
        m_isTransition = false;
        Safe_Release(m_pTransLight);
        m_pTransLight = nullptr;
        return;
    }

    // 보간 이전 정보 저장해놔야 함
    // 보간 시작 함수에서 태그, 트랜지션 디스크립션 받고 해당 태그의 Light의 광원 구성 정보 가져와서 저장
    
    if (m_TargetLightDesc.iBlinkCount > 0 && m_fBlinkPeriod > 0.f)
    {
        _float fBlinkTimeAcc = fmod(m_fTransTimeAcc, m_fBlinkPeriod);
        _float fBlinkPeriodRatio = fBlinkTimeAcc / m_fBlinkPeriod;

        _float fRatio = (1.f - cosf(XM_2PI * fBlinkPeriodRatio)) * 0.5f;

        LIGHT_DESC CurLightDesc = m_StartLightDesc;
        _vector vCurDiffuse, vCurAmbient, vCurSpecular;

        // D, A, S lerp
        _vector vStartDiffuse = XMLoadFloat4(&m_StartLightDesc.vDiffuse);
        _vector vStartAmbient = XMLoadFloat4(&m_StartLightDesc.vAmbient);
        _vector vStartSpecular = XMLoadFloat4(&m_StartLightDesc.vSpecular);

        _vector vTargetDiffuse = XMLoadFloat4(&m_TargetLightDesc.vDiffuse);
        _vector vTargetAmbient = XMLoadFloat4(&m_TargetLightDesc.vAmbient);
        _vector vTargetSpecular = XMLoadFloat4(&m_TargetLightDesc.vSpecular);

        vCurDiffuse = XMVectorLerp(vStartDiffuse, vTargetDiffuse, fRatio);
        vCurAmbient = XMVectorLerp(vStartAmbient, vTargetAmbient, fRatio);
        vCurSpecular = XMVectorLerp(vStartSpecular, vTargetSpecular, fRatio);

        XMStoreFloat4(&CurLightDesc.vDiffuse, vCurDiffuse);
        XMStoreFloat4(&CurLightDesc.vAmbient, vCurAmbient);
        XMStoreFloat4(&CurLightDesc.vSpecular, vCurSpecular);

        m_pTransLight->Set_LightDesc(CurLightDesc);
    }
    // Fade In
    else if (m_fTransTimeAcc <= m_TargetLightDesc.vFadeTime.x)
    {
        // D, A, S lerp
        _float fRatio = m_fTransTimeAcc / m_TargetLightDesc.vFadeTime.x;

        LIGHT_DESC CurLightDesc = m_StartLightDesc;
        _vector vCurDiffuse, vCurAmbient, vCurSpecular;

        // D, A, S lerp
        _vector vStartDiffuse = XMLoadFloat4(&m_StartLightDesc.vDiffuse);
        _vector vStartAmbient = XMLoadFloat4(&m_StartLightDesc.vAmbient);
        _vector vStartSpecular = XMLoadFloat4(&m_StartLightDesc.vSpecular);

        _vector vTargetDiffuse = XMLoadFloat4(&m_TargetLightDesc.vDiffuse);
        _vector vTargetAmbient = XMLoadFloat4(&m_TargetLightDesc.vAmbient);
        _vector vTargetSpecular = XMLoadFloat4(&m_TargetLightDesc.vSpecular);

        vCurDiffuse = XMVectorLerp(vStartDiffuse, vTargetDiffuse, fRatio);
        vCurAmbient = XMVectorLerp(vStartAmbient, vTargetAmbient, fRatio);
        vCurSpecular = XMVectorLerp(vStartSpecular, vTargetSpecular, fRatio);

        XMStoreFloat4(&CurLightDesc.vDiffuse, vCurDiffuse);
        XMStoreFloat4(&CurLightDesc.vAmbient, vCurAmbient);
        XMStoreFloat4(&CurLightDesc.vSpecular, vCurSpecular);

        m_pTransLight->Set_LightDesc(CurLightDesc);
    }
    // Fade Out
    else if (true == m_TargetLightDesc.isReturnToStart && m_fTransTimeAcc >= m_TargetLightDesc.vFadeTime.y)
    {
        _float fFadeDuration = m_TargetLightDesc.fDuration - m_TargetLightDesc.vFadeTime.y;
        _float fFadeTimeAcc = m_fTransTimeAcc - m_TargetLightDesc.vFadeTime.y;
        _float fRatio = (fFadeTimeAcc / fFadeDuration);

        LIGHT_DESC CurLightDesc = m_StartLightDesc;
        _vector vCurDiffuse, vCurAmbient, vCurSpecular;

        // D, A, S lerp
        _vector vStartDiffuse = XMLoadFloat4(&m_StartLightDesc.vDiffuse);
        _vector vStartAmbient = XMLoadFloat4(&m_StartLightDesc.vAmbient);
        _vector vStartSpecular = XMLoadFloat4(&m_StartLightDesc.vSpecular);

        _vector vTargetDiffuse = XMLoadFloat4(&m_TargetLightDesc.vDiffuse);
        _vector vTargetAmbient = XMLoadFloat4(&m_TargetLightDesc.vAmbient);
        _vector vTargetSpecular = XMLoadFloat4(&m_TargetLightDesc.vSpecular);

        vCurDiffuse = XMVectorLerp(vTargetDiffuse, vStartDiffuse, fRatio);
        vCurAmbient = XMVectorLerp(vTargetAmbient, vStartAmbient, fRatio);
        vCurSpecular = XMVectorLerp(vTargetSpecular, vStartSpecular, fRatio);

        XMStoreFloat4(&CurLightDesc.vDiffuse, vCurDiffuse);
        XMStoreFloat4(&CurLightDesc.vAmbient, vCurAmbient);
        XMStoreFloat4(&CurLightDesc.vSpecular, vCurSpecular);

        m_pTransLight->Set_LightDesc(CurLightDesc);
    }
}

HRESULT CLight_Manager::Add_Light(const _wstring& strLightTag, _uint iLevelIndex, const LIGHT_DESC& LightDesc, _bool isEnable)
{
	if (nullptr != Find_Light(strLightTag, iLevelIndex))
		return E_FAIL;

	CLight* pLight = CLight::Create(LightDesc);
	if (nullptr == pLight)
		return E_FAIL;

	pLight->Set_Enable(isEnable);
	m_pLights[iLevelIndex].emplace(strLightTag, pLight);

	return S_OK;
}

void CLight_Manager::Set_LightDesc(const _wstring& strLightTag, _uint iLevelIndex, const LIGHT_DESC& LightDesc)
{
	CLight* pLight = Find_Light(strLightTag, iLevelIndex);
	if (nullptr == pLight)
		return;

	pLight->Set_LightDesc(LightDesc);
}

void CLight_Manager::Set_LightPosition(const _wstring& strLightTag, _uint iLevelIndex, const _float4& vPosition)
{
	CLight* pLight = Find_Light(strLightTag, iLevelIndex);
	if (nullptr == pLight)
		return;

	pLight->Set_Position(vPosition);
}

void CLight_Manager::Set_LightEnable(const _wstring& strLightTag, _uint iLevelIndex, _bool isEnable)
{
	CLight* pLight = Find_Light(strLightTag, iLevelIndex);
	if (nullptr == pLight)
		return;

	pLight->Set_Enable(isEnable);
}

_bool CLight_Manager::Is_LightEnable(const _wstring& strLightTag, _uint iLevelIndex)
{
	CLight* pLight = Find_Light(strLightTag, iLevelIndex);
	CHECK_NULLPTR(pLight, false);

	return pLight->isEnable();
}

HRESULT CLight_Manager::Render(CShader* pShader, CVIBuffer_Rect* pVIBuffer, _uint iLevelIndex)
{
	for (auto& pLight : m_pLights[iLevelIndex])
	{
		if (true == pLight.second->isEnable())
		{
			pLight.second->Render(pShader, pVIBuffer);
		}
	}

	return S_OK;
}

void CLight_Manager::Clear(_uint iLevelIndex)
{
	if (iLevelIndex >= m_iNumLevels)
		return;

	for (auto& Pair : m_pLights[iLevelIndex])
		Safe_Release(Pair.second);

	m_pLights[iLevelIndex].clear();
}

void CLight_Manager::Start_LightTransition(const _wstring& strLightTag, _uint iLevelIndex, const LIGHT_TRANSITION_DESC& Desc, _bool isRestore)
{
    CLight* pLight = Find_Light(strLightTag, iLevelIndex);
    if (nullptr == pLight || nullptr != m_pTransLight)
        return;

    m_pTransLight = pLight;
    Safe_AddRef(m_pTransLight);
    
    m_StartLightDesc = *pLight->Get_LightDesc();

    m_isTransition = true;
    m_fTransTimeAcc = 0.f;
    m_TargetLightDesc = Desc;

    if (true == isRestore)
    {
        // 자신의 태그로 저장된 원본이 있는지 확인
        // 있다면 타겟 빛 정보 색에 오리지널 디스크립션을 넣어줌
        auto iter = m_OriginalLightDesc.find(strLightTag);

        if (iter != m_OriginalLightDesc.end())
        {
            LIGHT_DESC OriginalDesc = iter->second;

            m_TargetLightDesc.vDiffuse = OriginalDesc.vDiffuse;
            m_TargetLightDesc.vAmbient = OriginalDesc.vAmbient;
            m_TargetLightDesc.vSpecular = OriginalDesc.vSpecular;
        }
    }
    
    if (0 < m_TargetLightDesc.iBlinkCount)
        m_fBlinkPeriod = m_TargetLightDesc.fDuration / m_TargetLightDesc.iBlinkCount;

    // 페이드 아웃 시작 시간으로 변경
    m_TargetLightDesc.vFadeTime.y = m_TargetLightDesc.fDuration - m_TargetLightDesc.vFadeTime.y;
}

void CLight_Manager::Backup_LightDesc(const _wstring& strLightTag, _uint iLevelIndex)
{
    CLight* pLight = Find_Light(strLightTag, iLevelIndex);
    if (nullptr == pLight)
        return;
    
    LIGHT_DESC LightDesc = *pLight->Get_LightDesc();
    m_OriginalLightDesc[strLightTag] = LightDesc; // 덮어씌우기
}

const LIGHT_DESC* CLight_Manager::Get_LightDesc(const _wstring& strLightTag, _uint iLevelIndex)
{
	CLight* pLight = Find_Light(strLightTag, iLevelIndex);
	if (nullptr == pLight)
		return nullptr;

	return pLight->Get_LightDesc();
}

const vector<_wstring>& CLight_Manager::Get_LightTags(_uint iLevelIndex)
{
    m_strLightTags.clear();

    for (auto& Pair : m_pLights[iLevelIndex])
    {
        if (nullptr != Pair.second)
            m_strLightTags.push_back(Pair.first);
    }

    return m_strLightTags;
}

CLight* CLight_Manager::Find_Light(const _wstring& strLightTag, _uint iLevelIndex)
{
	auto iter = m_pLights[iLevelIndex].find(strLightTag);

	if (iter == m_pLights[iLevelIndex].end())
		return nullptr;

	return iter->second;
}

void CLight_Manager::Lerp_LightProperties(_fvector vStart, _fvector vTarget)
{
}

CLight_Manager* CLight_Manager::Create(_uint iNumLevels)
{
	CLight_Manager* pInstance = new CLight_Manager();

	if (FAILED(pInstance->Initialize(iNumLevels)))
	{
		MSG_BOX(TEXT("Failed to Create : CLight_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLight_Manager::Free()
{
	__super::Free();

    Safe_Release(m_pTransLight);

	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pLights[i])
			Safe_Release(Pair.second);
		m_pLights[i].clear();
	}

	Safe_Delete_Array(m_pLights);

	Safe_Release(m_pGameInstance);
}
