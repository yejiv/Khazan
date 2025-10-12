#include "UI_Panel.h"

CUI_Panel::CUI_Panel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUIObject{ pDevice, pContext }
{
}

CUI_Panel::CUI_Panel(const CUI_Panel& Prototype)
	:CUIObject{ Prototype }
{
}

HRESULT CUI_Panel::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CUI_Panel::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	return S_OK;
}

void CUI_Panel::Priority_Update(_float fTimeDelta)
{
}

void CUI_Panel::Update(_float fTimeDelta)
{
}

void CUI_Panel::Late_Update(_float fTimeDelta)
{
}

HRESULT CUI_Panel::Render()
{
	return S_OK;
}

void CUI_Panel::Free()
{
	__super::Free();
}
