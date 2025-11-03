#include "BossHp_Gauge.h"
#include "GameInstance.h"
#include "ClientInstance.h"

CBossHp_Gauge::CBossHp_Gauge(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_ProgressBar{ pDevice, pContext }
{
}

CBossHp_Gauge::CBossHp_Gauge(const CBossHp_Gauge& Prototype)
	: CUI_ProgressBar(Prototype)
{
}

void CBossHp_Gauge::Setting_Progress(const _float* pCulValue, const _float* pMaxValue)
{
	m_pMaxValue = pMaxValue;
	m_pCulValue = pCulValue;
}

void CBossHp_Gauge::Reset_Progress()
{
	m_pMaxValue = nullptr;
	m_pCulValue = nullptr;
}

HRESULT CBossHp_Gauge::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBossHp_Gauge::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_fLerpSpeed = 0.5f;
	m_fLerpValue = 1.f;
	m_fDeleyTime = 0.5f;

	return S_OK;
}

void CBossHp_Gauge::Priority_Update(_float fTimeDelta)
{
}

void CBossHp_Gauge::Update(_float fTimeDelta)
{
	if (m_pCulValue == nullptr || m_pMaxValue == nullptr)
		return;
	m_fMaxValue = *m_pMaxValue;
	m_fCurrentValue = *m_pCulValue;

	if (m_fCurrentValue < 0)
		m_fCurrentValue = 0;
	if (m_fCurrentValue > m_fMaxValue)
		m_fCurrentValue = m_fMaxValue;

	Update_Deley(fTimeDelta);
}

void CBossHp_Gauge::Late_Update(_float fTimeDelta)
{
	if (m_pCulValue == nullptr || m_pMaxValue == nullptr)
		return;

	CClientInstance::GetInstance()->Add_UIRender(UI_RENDER_TYPE::DEFAULT, this);
}

HRESULT CBossHp_Gauge::Render()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_Texture", m_iTexPass)))
		return E_FAIL;

	_float2 vValue = { m_fProgress_Value , m_fLerpValue };
	CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float)), E_FAIL);
	CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_fProgressValue", &vValue, sizeof(_float2)), E_FAIL);

	m_pShaderCom->Begin(m_iShaderPass);
	m_pVIBufferCom->Bind_Resources();
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CBossHp_Gauge::Ready_Component()
{
	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex_UI"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Hud_HPGauge"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
		return E_FAIL;

	return S_OK;
}

void CBossHp_Gauge::Update_Deley(_float fTimeDelta)
{
	if (m_fCurrentValue < m_fPreValue && !m_isDelay)
	{
		m_isDelay = true;
		m_fAccTime = m_fDeleyTime;
	}
	Progress_Update();
	if (m_fAccTime > 0.f)
	{
		m_fAccTime -= fTimeDelta;
	}
	else
	{
		__super::Update(fTimeDelta);
		if (m_fLerpValue <= m_fProgress_Value)
			m_isDelay = false;
	}
}

CBossHp_Gauge* CBossHp_Gauge::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBossHp_Gauge* pInstance = new CBossHp_Gauge(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed Created : CBossHp_Gauge"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CBossHp_Gauge::Clone(void* pArg)
{
	CBossHp_Gauge* pInstance = new CBossHp_Gauge(*this);
	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CBossHp_Gauge"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBossHp_Gauge::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
