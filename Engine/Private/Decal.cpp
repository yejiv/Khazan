#include "Decal.h"

CDecal::CDecal(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject { pDevice, pContext }
{
}

CDecal::CDecal(const CDecal& Prototype)
	: CGameObject(Prototype)
{
}

HRESULT CDecal::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDecal::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	return S_OK;
}

void CDecal::Priority_Update(_float fTimeDelta)
{
}

void CDecal::Update(_float fTimeDelta)
{
	m_fTimeAcc += fTimeDelta;

	// 수명이 끝나면 사망 처리 -> 풀
	if (m_fTimeAcc >= m_fLifeTime)
	{
		//	Set_IsDead(true);
		m_isDead = true;
		m_fOpacity = 0.f;
		return;
	}

	// 페이드 아웃 계산
	if (m_fTimeAcc > m_fFadeStartTime)
	{
		_float fFadeDuration = m_fLifeTime - m_fFadeStartTime;
		_float fFadeTimeAcc = m_fTimeAcc - m_fFadeStartTime;
		m_fOpacity = 1.f - (fFadeTimeAcc / fFadeDuration);
		m_fOpacity = max(0.f, m_fOpacity);
	}
}

void CDecal::Late_Update(_float fTimeDelta)
{
}

HRESULT CDecal::Render()
{
	// Decal 전용 셰이더 만들어서 호출

	return S_OK;
}

void CDecal::Reset()
{
}

CDecal* CDecal::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDecal* pInstance = new CDecal(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CDecal"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDecal::Clone(void* pArg)
{
	CDecal* pInstance = new CDecal(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CDecal"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDecal::Free()
{
	__super::Free();
}