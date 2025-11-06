#include "UI_Default_Button.h"
#include "GameInstance.h"
#include "ClientInstance.h"

CUI_Default_Button::CUI_Default_Button(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Button{ pDevice, pContext }
{
}

CUI_Default_Button::CUI_Default_Button(const CUI_Default_Button& Prototype)
	: CUI_Button(Prototype)
{
}

void CUI_Default_Button::Update_Color(_float4 vColor)
{
	m_vColor = vColor;
}

void CUI_Default_Button::Set_Texture(_float4 vUV, _uint iTexPass)
{
	m_vUV[0] = vUV;
	m_iTexPass = iTexPass;
}

void CUI_Default_Button::Set_Shader(_uint iShaderPass)
{
	m_iShaderPass = iShaderPass;
}

HRESULT CUI_Default_Button::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Default_Button::Initialize_Clone(void* pArg)
{
	UIATLASICON_DESC* pDesc = static_cast<UIATLASICON_DESC*>(pArg);

	m_vUV.push_back(pDesc->vUV);
	m_iShaderPass = pDesc->iShaderPass;
	m_iTexPass = pDesc->iTexPass;
	m_vColor = pDesc->vColor;

	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	return S_OK;
}
void CUI_Default_Button::Priority_Update(_float fTimeDelta)
{
}

void CUI_Default_Button::Update(_float fTimeDelta)
{
}

void CUI_Default_Button::Late_Update(_float fTimeDelta)
{
	if (m_isVisible)
		CClientInstance::GetInstance()->Add_UIRender(UI_RENDER_TYPE::ATLAS, this);
}

HRESULT CUI_Default_Button::Render()
{
	return S_OK;
}

CUI_Default_Button* CUI_Default_Button::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Default_Button* pInstance = new CUI_Default_Button(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed Created : CUI_Default_Button"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_Default_Button::Clone(void* pArg)
{
	CUI_Default_Button* pInstance = new CUI_Default_Button(*this);
	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CUI_Default_Button"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_Default_Button::Free()
{
	__super::Free();
}
