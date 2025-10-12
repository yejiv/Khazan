#include "UI_ProgressBar.h"

CUI_ProgressBar::CUI_ProgressBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUIObject{pDevice,pContext}
{
}

CUI_ProgressBar::CUI_ProgressBar(const CUI_ProgressBar& Prototype)
	:CUIObject{Prototype}
{
}

HRESULT CUI_ProgressBar::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_ProgressBar::Initialize_Clone(void* pArg)
{
	PROGRESSBAR_DESC* pDesc = static_cast<PROGRESSBAR_DESC*>(pArg);

	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	m_vOriginPos = _float3(pDesc->vLocalPos.x, pDesc->vLocalPos.y, 0.1f);
	m_vOriginSize = pDesc->vLocalSize;
	m_eDirection = pDesc->eDirection;
	m_eMode = pDesc->eMode;
	m_fLerpSpeed = 1.f;
	m_fDisplayRate = 1.f;

	return S_OK;
}

void CUI_ProgressBar::Priority_Update(_float fTimeDelta)
{
}

void CUI_ProgressBar::Update(_float fTimeDelta)
{
	if (!m_isChange)
		return;

	_float fTargetRate = Make_Rate(m_fCurrentValue, m_fMaxValue);

	m_fDisplayRate = Lerp(m_fDisplayRate, fTargetRate, fTimeDelta * m_fLerpSpeed);

	switch (m_eMode)
	{
	case BAR_MODE::REDUCE:
		switch (m_eDirection)
		{
		case BAR_DIRECTION::LEFT_TO_RIGHT:
			Reduce_LeftToRight(m_fDisplayRate);
			break;
		case BAR_DIRECTION::RIGHT_TO_LEFT:
			Reduce_RightToLeft(m_fDisplayRate);
			break;
		case BAR_DIRECTION::TOP_TO_BOTTOM:
			// 필요시 세로 버전 함수 추가
			break;
		case BAR_DIRECTION::BOTTOM_TO_TOP:
			// 필요시 세로 버전 함수 추가
			break;
		}
		break;

	case BAR_MODE::EXPAND:
		switch (m_eDirection)
		{
		case BAR_DIRECTION::LEFT_TO_RIGHT:
			Expand_LeftToRight(m_fDisplayRate);
			break;
		case BAR_DIRECTION::RIGHT_TO_LEFT:
			Expand_RightToLeft(m_fDisplayRate);
			break;
		case BAR_DIRECTION::TOP_TO_BOTTOM:
			// 필요시 세로 버전 함수 추가
			break;
		case BAR_DIRECTION::BOTTOM_TO_TOP:
			// 필요시 세로 버전 함수 추가
			break;
		}
		break;
	}

	// Lerp가 끝나면 Update를 막는다.
	if (fabs(m_fDisplayRate - fTargetRate) < 0.001f)
		m_isChange = false;

}

void CUI_ProgressBar::Late_Update(_float fTimeDelta)
{
}

HRESULT CUI_ProgressBar::Render()
{
	return S_OK;
}

_float CUI_ProgressBar::Make_Rate(_float fSrc, _float fDst)
{

	if (fDst <= 0.f)
		return 0.f;

	_float fRate = fSrc / fDst;

	if (fRate <= 0.001f)
		fRate = 0.001f;
	if (fRate > 1.f)
		fRate = 1.f;

	return fRate;
}

void CUI_ProgressBar::Reduce_RightToLeft(_float fRate)
{
	m_vLocalSize.x = m_vOriginSize.x * fRate;

	if (m_vLocalSize.x <= 0.001f)
		m_vLocalSize.x = 0.001f;

	m_pTransformCom->Scaling(_float3(m_vLocalSize.x, m_vLocalSize.y, 1.f));

	m_vLocalPos.x = m_vOriginPos.x + (m_vOriginPos.x- m_vLocalSize.x) * 0.5f;
}

void CUI_ProgressBar::Reduce_LeftToRight(_float fRate)
{
	m_vLocalSize.x = m_vOriginSize.x * fRate;

	if (m_vLocalSize.x <= 0.001f)
		m_vLocalSize.x = 0.001f;

	m_pTransformCom->Scaling(_float3(m_vLocalSize.x, m_vLocalSize.y, 1.f));

	m_vLocalPos.x = m_vOriginPos.x - (m_vOriginPos.x - m_vLocalSize.x) * 0.5f;

}

void CUI_ProgressBar::Expand_RightToLeft(_float fRate)
{
	m_vLocalSize.x = m_vOriginSize.x * fRate;

	if (m_vLocalSize.x < m_vOriginSize.x)
		m_vLocalSize.x = m_vOriginSize.x;

	m_pTransformCom->Scaling(_float3(m_vLocalSize.x, m_vLocalSize.y, 1.f));

	m_vLocalPos.x = m_vOriginPos.x + (m_vOriginPos.x - m_vLocalSize.x) * 0.5f;
}

void CUI_ProgressBar::Expand_LeftToRight(_float fRate)
{
	m_vLocalSize.x = m_vOriginSize.x * fRate;

	if (m_vLocalSize.x < m_vOriginSize.x)
		m_vLocalSize.x = m_vOriginSize.x;

	m_pTransformCom->Scaling(_float3(m_vLocalSize.x, m_vLocalSize.y, 1.f));

	m_vLocalPos.x = m_vOriginPos.x - (m_vOriginPos.x - m_vLocalSize.x) * 0.5f;
}

void CUI_ProgressBar::Free()
{
	__super::Free();
}
