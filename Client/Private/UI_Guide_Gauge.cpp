#include "UI_Guide_Gauge.h"
#include "GameInstance.h"
#include "ClientInstance.h"

CUI_Guide_Gauge::CUI_Guide_Gauge(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_ProgressBar{ pDevice, pContext }
{
}

CUI_Guide_Gauge::CUI_Guide_Gauge(const CUI_Guide_Gauge& Prototype)
	: CUI_ProgressBar(Prototype)
{
}

HRESULT CUI_Guide_Gauge::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Guide_Gauge::Initialize_Clone(void* pArg)
{
	GUIDEGAUGE_DESC* pDesc = static_cast<GUIDEGAUGE_DESC*>(pArg);

	m_pMaxValue = pDesc->pMaxValue;
	m_pCulValue = pDesc->pCulValue;
	m_iShaderPass = 9;

	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Component()))
		return E_FAIL;

	return S_OK;
}

void CUI_Guide_Gauge::Priority_Update(_float fTimeDelta)
{
}

void CUI_Guide_Gauge::Update(_float fTimeDelta)
{
	if (m_pCulValue == nullptr || m_pMaxValue == nullptr)
		return;
	m_fMaxValue = *m_pMaxValue;
	m_fCurrentValue = *m_pCulValue;

	if (m_fCurrentValue < 0)
		m_fCurrentValue = 0;
	if (m_fCurrentValue > m_fMaxValue)
		m_fCurrentValue = m_fMaxValue;

	Progress_Update();
}

void CUI_Guide_Gauge::Late_Update(_float fTimeDelta)
{
	if (m_pCulValue == nullptr || m_pMaxValue == nullptr)
		return;

	CClientInstance::GetInstance()->Add_UIRender(UI_RENDER_TYPE::DEFAULT, this);
}

HRESULT CUI_Guide_Gauge::Render()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float)), E_FAIL);
	CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_fProgressValue", &m_fProgress_Value, sizeof(_float4)), E_FAIL);

	m_pShaderCom->Begin(m_iShaderPass);
	m_pVIBufferCom->Bind_Resources();
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CUI_Guide_Gauge::Ready_Component()
{
	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex_UI"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Tex_Guide_Circle"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
		return E_FAIL;

	return S_OK;
}

CUI_Guide_Gauge* CUI_Guide_Gauge::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Guide_Gauge* pInstance = new CUI_Guide_Gauge(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed Created : CUI_Guide_Gauge"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_Guide_Gauge::Clone(void* pArg)
{
	CUI_Guide_Gauge* pInstance = new CUI_Guide_Gauge(*this);
	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CUI_Guide_Gauge"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_Guide_Gauge::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
