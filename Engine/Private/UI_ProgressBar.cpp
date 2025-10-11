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
	UIOBJECT_DESC* pDesc = static_cast<UIOBJECT_DESC*>(pArg);

	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	m_vOriginPos = _float3(pDesc->vLocalPos.x, pDesc->vLocalPos.y, 0.1f);
	m_vOriginSize = pDesc->vLocalSize;

	return S_OK;
}

void CUI_ProgressBar::Priority_Update(_float fTimeDelta)
{
}

void CUI_ProgressBar::Update(_float fTimeDelta)
{
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

void CUI_ProgressBar::Free()
{
	__super::Free();
}
