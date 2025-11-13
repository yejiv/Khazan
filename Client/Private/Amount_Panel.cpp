#include "Amount_Panel.h"
#include "GameInstance.h"
#include "ClientInstance.h"

#include "UI_TextBox.h"
#include "UI_Atlas_Icon.h"

CAmount_Panel::CAmount_Panel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Panel{ pDevice, pContext }
{
}

CAmount_Panel::CAmount_Panel(const CAmount_Panel& Prototype)
	: CUI_Panel(Prototype)
{
}

void CAmount_Panel::Add_Value(_int IValue)
{
	if (m_iAddValue <= 0)
	{
		m_iAccValue = 1;
		m_isAddValue = true;
		m_fAccTime = 0.f;
		m_pAddValueText->Update_Visible(true);
	}
	m_iAddValue += IValue;
}

void CAmount_Panel::Setting_Index(CAmount::AMOUNT_TYPE eType, _uint* iCulValue)
{
	m_eType = eType;
	m_iCulValue = iCulValue;

	for (auto child : m_Children)
	{
		string strName = child->Get_Name();

		if (eType == CAmount::AMOUNT_TYPE::GOLD)
		{
			if (strName == "Gold_Icon")
			{
				static_cast<CUI_Atlas_Icon*>(child)->Set_Texture(CClientInstance::GetInstance()->Get_AtlasUV("T_Item_Material_GraveStuff_UI.png", 2), 2);
			}
			else if (strName == "Gold_Line")
			{
				static_cast<CUI_Atlas_Icon*>(child)->Set_Color({ 1.f,1.f,1.f,0.8f });
			}
			else if (strName == "Gold_Text")
			{
				m_pCulValueText = static_cast<CUI_TextBox*>(child);
				Safe_AddRef(m_pCulValueText);
			}
			else if (strName == "Gold_Text_Get")
			{
				m_pAddValueText = static_cast<CUI_TextBox*>(child);
				Safe_AddRef(m_pAddValueText);
			}
		}
		else if (eType == CAmount::AMOUNT_TYPE::LACHRYMA)
		{
			if (strName == "Lachryma_Icon")
			{
				static_cast<CUI_Atlas_Icon*>(child)->Set_Texture(CClientInstance::GetInstance()->Get_AtlasUV("T_Item_Potion_HP_1.png", 2), 2);
			}
			else if (strName == "Lachryma_Line")
			{
				static_cast<CUI_Atlas_Icon*>(child)->Set_Color({ 1.f,1.f,1.f,0.8f });
			}
			else if (strName == "Lachryma_Text")
			{
				m_pCulValueText = static_cast<CUI_TextBox*>(child);
				Safe_AddRef(m_pCulValueText);
			}
			else if (strName == "Lachryma_Text_Get")
			{
				m_pAddValueText = static_cast<CUI_TextBox*>(child);
				Safe_AddRef(m_pAddValueText);
			}
		}
		else if (eType == CAmount::AMOUNT_TYPE::STONE)
		{
			if (strName == "Stone_Icon")
			{
				static_cast<CUI_Atlas_Icon*>(child)->Set_Texture(CClientInstance::GetInstance()->Get_AtlasUV("T_Img_StoneLevelUp_Deco_01.png", 2), 2);
			}
			else if (strName == "Stone_Line")
			{
				static_cast<CUI_Atlas_Icon*>(child)->Set_Color({ 1.f,1.f,1.f,0.8f });
			}
			else if (strName == "Stone_Text")
			{
				m_pCulValueText = static_cast<CUI_TextBox*>(child);
				Safe_AddRef(m_pCulValueText);
			}
			else if (strName == "Stone_Text_Get")
			{
				m_pAddValueText = static_cast<CUI_TextBox*>(child);
				Safe_AddRef(m_pAddValueText);
			}
		}
	}
}

HRESULT CAmount_Panel::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAmount_Panel::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	CHECK_FAILED(Ready_Component(), E_FAIL);
	return S_OK;
}

void CAmount_Panel::Priority_Update(_float fTimeDelta)
{
}

void CAmount_Panel::Update(_float fTimeDelta)
{
	AddValue_Check(fTimeDelta);

	m_pCulValueText->Set_Text(IntToWstring(*m_iCulValue));
}

void CAmount_Panel::Late_Update(_float fTimeDelta)
{
	CClientInstance::GetInstance()->Add_UIRender(UI_RENDER_TYPE::DEFAULT, this);
	__super::Late_Update(fTimeDelta);
}

HRESULT CAmount_Panel::Render()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_Texture", m_iTexPass)))
		return E_FAIL;

	CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float)), E_FAIL);
	CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_float4)), E_FAIL);

	m_pShaderCom->Begin(m_iShaderPass);
	m_pVIBufferCom->Bind_Resources();
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CAmount_Panel::Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg)
{
	__super::Load_UI(pInData, iPrototypeLevelID, pArg);

	return S_OK;
}

HRESULT CAmount_Panel::Ready_Component()
{
	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex_UI"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_UI_Common_MenuList"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
		return E_FAIL;

	return S_OK;
}

_wstring CAmount_Panel::IntToWstring(_int iValue)
{
	_wstring wstrValue = {};
	if (iValue > 999)
	{
		_int iTemp = iValue * 0.001;
		wstrValue = to_wstring(iTemp);
		wstrValue += TEXT(",");
		iTemp = iValue - (1000 * iTemp);
		if (iTemp <= 0)
			wstrValue += TEXT("000");
		else
			wstrValue += to_wstring(iTemp);
	}
	else
	{
		wstrValue = to_wstring(iValue);
	}
	return wstrValue;
}

void CAmount_Panel::AddValue_Check(_float fTimeDelta)
{
	if (m_isAddValue)
	{
		if (m_fAccTime < 1.f)
		{
			m_fAccTime += fTimeDelta;
			if (m_fAccTime > 1.f)
				m_fAccTime = 1.f;

			_int iPivotY = -17 * m_fAccTime;
			m_pAddValueText->Update_Alpha(m_fAccTime);
			m_pAddValueText->Setting_Pivot(70, iPivotY);

			_wstring strAddCount = TEXT("+");
			strAddCount += IntToWstring(m_iAddValue);
			m_pAddValueText->Set_Text(strAddCount);
		}
		else if (m_iAddValue > 0)
		{
			m_iAccValue += 1;

			if (m_iAddValue < m_iAccValue)
			{
				*m_iCulValue += m_iAddValue;
				m_iAddValue -= m_iAddValue;
			}
			else
			{
				*m_iCulValue += m_iAccValue;
				m_iAddValue -= m_iAccValue;
			}

			_wstring strAddCount = TEXT("+");
			strAddCount += IntToWstring(m_iAddValue);
			m_pAddValueText->Set_Text(strAddCount);
		}
		else
		{
			m_isAddValue = false;
			m_pAddValueText->Update_Visible(false);
			__super::Bubble_EventCall(nullptr);
		}
	}
	else
		m_pAddValueText->Update_Visible(false);
}

CAmount_Panel* CAmount_Panel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CAmount_Panel* pInstance = new CAmount_Panel(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed Created : CAmount_Panel"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CAmount_Panel::Clone(void* pArg)
{
	CAmount_Panel* pInstance = new CAmount_Panel(*this);
	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CAmount_Panel"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CAmount_Panel::Free()
{
	__super::Free();

	Safe_Release(m_pCulValueText);
	Safe_Release(m_pAddValueText);

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
