#include "UI_TextBox.h"
#include "GameInstance.h"
#include "ClientInstance.h"

CUI_TextBox::CUI_TextBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Text(pDevice, pContext)
{
}

CUI_TextBox::CUI_TextBox(const CUI_TextBox& Prototype)
	: CUI_Text(Prototype)
{
}

HRESULT CUI_TextBox::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_TextBox::Initialize_Clone(void* pArg)
{
	__super::Initialize_Clone(pArg);
	CHECK_FAILED(Ready_Component(), E_FAIL);
	return S_OK;
}

void CUI_TextBox::Priority_Update(_float fTimeDelta)
{
}

void CUI_TextBox::Update(_float fTimeDelta)
{
}

void CUI_TextBox::Late_Update(_float fTimeDelta)
{
	CClientInstance::GetInstance()->Add_UIRender(UI_RENDER_TYPE::DEFAULT, this);
}

HRESULT CUI_TextBox::Render()
{
	CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float)), E_FAIL);
	m_pShaderCom->Begin(m_iShaderPass);

	if (m_bIsTextBox)
	{
		m_pGameInstance->Draw_TextBox(m_wstrTexttag, m_wstrText, m_vWorldPos.x + m_iPivotX, m_vWorldPos.y + m_iPivotY, m_fMaxWidth, m_fOffsetHeight, m_vColor, m_eTextAlign);
	}
	else
	{
		m_pGameInstance->Draw_Text(m_wstrTexttag, m_wstrText, m_vWorldPos.x + m_iPivotX, m_vWorldPos.y + m_iPivotY, m_vColor, m_eTextAlign);
	}
	return S_OK;
}

HRESULT CUI_TextBox::Ready_Component()
{
	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex_Font"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
		return E_FAIL;

	return S_OK;
}

CUI_TextBox* CUI_TextBox::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_TextBox* pInstance = new CUI_TextBox(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed Created : CUI_TextBox"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_TextBox::Clone(void* pArg)
{
	CUI_TextBox* pInstance = new CUI_TextBox(*this);
	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CUI_TextBox"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_TextBox::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
}
