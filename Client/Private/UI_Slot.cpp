#include "UI_Slot.h"
#include "GameInstance.h"

CUI_Slot::CUI_Slot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject{ pDevice, pContext }
{
}

CUI_Slot::CUI_Slot(const CUI_Slot& Prototype)
	: CUIObject( Prototype )
{
}

HRESULT CUI_Slot::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Slot::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	return S_OK;
}

void CUI_Slot::Priority_Update(_float fTimeDelta)
{
}

void CUI_Slot::Update(_float fTimeDelta)
{
}

void CUI_Slot::Late_Update(_float fTimeDelta)
{
}

HRESULT CUI_Slot::Render()
{
	return S_OK;
}

void CUI_Slot::Free()
{
	__super::Free();
}
