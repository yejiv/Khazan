#include "Light.h"

#include "Shader.h"
#include "VIBuffer_Rect.h"

CLight::CLight()
{
}

HRESULT CLight::Initialize(const LIGHT_DESC& LightDesc)
{
	m_LightDesc = LightDesc;

	return S_OK;
}

void CLight::Update(_float fTimeDelta)
{
    if (false == m_isTransition)
        return;

    m_fTransTimeAcc += fTimeDelta;

    if (m_fTransTimeAcc >= m_TargetLightDesc.fDuration)
    {
        LIGHT_DESC FinalLightDesc = m_StartLightDesc;

        if (true == m_TargetLightDesc.isReturnToStart)
            //  || (0.f < m_fBlinkPeriod && 0 < m_TargetLightDesc.iBlinkCount))
            m_LightDesc = FinalLightDesc;
        else
        {
            FinalLightDesc.vDiffuse = m_TargetLightDesc.vDiffuse;
            FinalLightDesc.vAmbient = m_TargetLightDesc.vAmbient;
            FinalLightDesc.vSpecular = m_TargetLightDesc.vSpecular;
        }

        m_LightDesc = FinalLightDesc;

        m_fBlinkPeriod = 0.f;
        
        m_isTransition = false;

        if (nullptr != m_Callback)
        {
            m_Callback();
            m_Callback = nullptr;
        }

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

        m_LightDesc = CurLightDesc;
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

        m_LightDesc = CurLightDesc;
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

        m_LightDesc = CurLightDesc;
    }
}

HRESULT CLight::Render(CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	_uint		iPassIndex = { 1 };

	if (LIGHT_DESC::DIRECTIONAL == m_LightDesc.eType)
	{
		iPassIndex = 1;

		if (FAILED(pShader->Bind_RawValue("g_vLightDir", &m_LightDesc.vDirection, sizeof(_float4))))
			return E_FAIL;
	}
	else if (LIGHT_DESC::POINT == m_LightDesc.eType)
	{
		iPassIndex = 2;

		if (FAILED(pShader->Bind_RawValue("g_vLightPos", &m_LightDesc.vPosition, sizeof(_float4))))
			return E_FAIL;
		if (FAILED(pShader->Bind_RawValue("g_fLightRange", &m_LightDesc.fRange, sizeof(_float))))
			return E_FAIL;

	}

	if (FAILED(pShader->Bind_RawValue("g_vLightDiffuse", &m_LightDesc.vDiffuse, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(pShader->Bind_RawValue("g_vLightAmbient", &m_LightDesc.vAmbient, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(pShader->Bind_RawValue("g_vLightSpecular", &m_LightDesc.vSpecular, sizeof(_float4))))
		return E_FAIL;

	pShader->Begin(iPassIndex);

	pVIBuffer->Bind_Resources();
	pVIBuffer->Render();

	return S_OK;
}

void CLight::Start_LightTransition(const LIGHT_TRANSITION_DESC& Desc, _bool isRestore)
{
    // 이미 보간 중이면 덮어쓰기 방지
    if (true == m_isTransition)
        return;

    m_StartLightDesc = m_LightDesc;

    m_isEnable = true;
    m_isTransition = true;
    m_fTransTimeAcc = 0.f;
    m_TargetLightDesc = Desc;
    m_Callback = Desc.Callback;

    if (true == isRestore)
    {
        m_TargetLightDesc.vDiffuse = m_OriginalLightDesc.vDiffuse;
        m_TargetLightDesc.vAmbient = m_OriginalLightDesc.vAmbient;
        m_TargetLightDesc.vSpecular = m_OriginalLightDesc.vSpecular;
    }

    if (0 < m_TargetLightDesc.iBlinkCount)
        m_fBlinkPeriod = m_TargetLightDesc.fDuration / m_TargetLightDesc.iBlinkCount;

    // 페이드 아웃 시작 시간으로 변경
    m_TargetLightDesc.vFadeTime.y = m_TargetLightDesc.fDuration - m_TargetLightDesc.vFadeTime.y;
}

CLight* CLight::Create(const LIGHT_DESC& LightDesc)
{
	CLight* pInstance = new CLight();

	if (FAILED(pInstance->Initialize(LightDesc)))
	{
		MSG_BOX(TEXT("Failed to Created : CLight"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLight::Free()
{
	__super::Free();

}
