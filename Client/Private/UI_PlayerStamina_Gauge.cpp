#include "UI_PlayerStamina_Gauge.h"
#include "GameInstance.h"
#include "ClientInstance.h"

CUI_PlayerStamina_Gauge::CUI_PlayerStamina_Gauge(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_ProgressBar{ pDevice, pContext }
{
}

CUI_PlayerStamina_Gauge::CUI_PlayerStamina_Gauge(const CUI_PlayerStamina_Gauge& Prototype)
	: CUI_ProgressBar(Prototype)
{
}

HRESULT CUI_PlayerStamina_Gauge::Initialize_Prototype(_uint iLevel)
{
	m_iLevel = iLevel;
	return S_OK;
}

HRESULT CUI_PlayerStamina_Gauge::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_fMaxValue = 100;
	m_fCurrentValue = 100;
	return S_OK;
}

void CUI_PlayerStamina_Gauge::Priority_Update(_float fTimeDelta)
{
}

void CUI_PlayerStamina_Gauge::Update(_float fTimeDelta)
{
	if (m_fCurrentValue < 0)
		m_fCurrentValue = 0;
	if (m_fCurrentValue > m_fMaxValue)
		m_fCurrentValue = m_fMaxValue;

	Progress_Update();
}

void CUI_PlayerStamina_Gauge::Late_Update(_float fTimeDelta)
{
	CClientInstance::GetInstance()->Add_UIRender(UI_RENDER_TYPE::DEFAULT, this);
}

HRESULT CUI_PlayerStamina_Gauge::Render()
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
	CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_fProgressValue", &m_fProgress_Value, sizeof(_float)), E_FAIL);

	m_pShaderCom->Begin(4);
	m_pVIBufferCom->Bind_Resources();
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CUI_PlayerStamina_Gauge::Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg)
{
	string strTexType = pInData.value("TexType", "");

	if ("Tex" == strTexType)
	{
		string strTexTag = pInData.value("TexTag", "");
		wstring wstrTexTag = AnsiToWString(strTexTag);

		if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), wstrTexTag.c_str(),
			TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
			return E_FAIL;
	}


	if (FAILED(__super::Load_UI(pInData, iPrototypeLevelID, pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_PlayerStamina_Gauge::Ready_Component()
{
	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex_UI"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	return S_OK;
}

CUI_PlayerStamina_Gauge* CUI_PlayerStamina_Gauge::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CUI_PlayerStamina_Gauge* pInstance = new CUI_PlayerStamina_Gauge(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX(TEXT("Failed Created : CUI_PlayerStamina_Gauge"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_PlayerStamina_Gauge::Clone(void* pArg)
{
	CUI_PlayerStamina_Gauge* pInstance = new CUI_PlayerStamina_Gauge(*this);
	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CUI_PlayerStamina_Gauge"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_PlayerStamina_Gauge::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
