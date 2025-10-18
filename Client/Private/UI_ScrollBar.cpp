#include "UI_ScrollBar.h"
#include "GameInstance.h"

CUI_ScrollBar::CUI_ScrollBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject{ pDevice, pContext }
{
}

CUI_ScrollBar::CUI_ScrollBar(const CUI_ScrollBar& Prototype)
	: CUIObject(Prototype)
{
}

HRESULT CUI_ScrollBar::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_ScrollBar::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	return S_OK;
}

void CUI_ScrollBar::Priority_Update(_float fTimeDelta)
{
}

void CUI_ScrollBar::Update(_float fTimeDelta)
{
}

void CUI_ScrollBar::Late_Update(_float fTimeDelta)
{
}

HRESULT CUI_ScrollBar::Render()
{
	return S_OK;
}

void CUI_ScrollBar::Free()
{
	__super::Free();
}
