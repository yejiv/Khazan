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
	if (m_fTimeAcc >= m_Desc.fLifeTime)
	{
		m_isDead = true;
		m_fTimeAcc = 0.f;
		m_fOpacity = 0.f;
		return;
	}

	// 페이드 아웃 계산
	if (m_fTimeAcc > m_Desc.vFadeTime.y)
	{
		_float fFadeDuration = m_Desc.fLifeTime - m_Desc.vFadeTime.y;	// 페이드 아웃 총 시간
		_float fFadeTimeAcc = m_fTimeAcc - m_Desc.vFadeTime.y;			// 페이드 아웃 시작 후 누적 시간
		_float fRatio = (fFadeTimeAcc / fFadeDuration);					// 페이드 아웃 비율
		m_fOpacity = 1.f - fRatio;		
		m_fOpacity = max(0.f, m_fOpacity);								// 비율 0 -> 불투명, 비율 1 -> 투명
	}
	else if (m_fTimeAcc < m_Desc.vFadeTime.x)
	{
		m_fOpacity = m_fTimeAcc / m_Desc.vFadeTime.x;		// 페이드 인 총 시간
		m_fOpacity = min(1.f, m_fOpacity);
	}
}

void CDecal::Late_Update(_float fTimeDelta)
{
}

HRESULT CDecal::Render()
{
	return S_OK;
}

void CDecal::Reset()
{
	m_fOpacity = 1.f;
}

void CDecal::Set_Desc(DECAL_DESC Desc)
{
	m_Desc = Desc;

	m_pTransformCom->Scale(Desc.vScale);
	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(Desc.vPosition.x, Desc.vPosition.y, Desc.vPosition.z, 1.f));
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