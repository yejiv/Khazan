#include "UI_Button.h"
#include "GameInstance.h"

CUI_Button::CUI_Button(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CUIObject{ pDevice,pContext }
{
}

CUI_Button::CUI_Button(const CUI_Button& Prototype)
    :CUIObject( Prototype )
{
}

HRESULT CUI_Button::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_Button::Initialize_Clone(void* pArg)
{
    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    return S_OK;
}

void CUI_Button::Priority_Update(_float fTimeDelta)
{
}

void CUI_Button::Update(_float fTimeDelta)
{

}

void CUI_Button::Late_Update(_float fTimeDelta)
{
}

HRESULT CUI_Button::Render()
{
    return S_OK;
}

void CUI_Button::Free()
{
    __super::Free();
}
