#include "Skill_Gauge.h"
#include "GameInstance.h"
#include "ClientInstance.h"

CSkill_Gauge::CSkill_Gauge(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Panel{ pDevice, pContext }
{
}

CSkill_Gauge::CSkill_Gauge(const CSkill_Gauge& Prototype)
	: CUI_Panel(Prototype)
{
}

HRESULT CSkill_Gauge::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSkill_Gauge::Initialize_Clone(void* pArg)
{
	CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);
	CHECK_FAILED(Ready_Component(), E_FAIL);
	return S_OK;
}

void CSkill_Gauge::Priority_Update(_float fTimeDelta)
{
}

void CSkill_Gauge::Update(_float fTimeDelta)
{
}

void CSkill_Gauge::Late_Update(_float fTimeDelta)
{
	if (m_isVisible)
		CClientInstance::GetInstance()->Add_UIRender(UI_RENDER_TYPE::DEFAULT, this);
}

HRESULT CSkill_Gauge::Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg)
{
	string strTexTag = pInData.value("TexTag", "");
	wstring wstrTexTag = AnsiToWString(strTexTag);

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), wstrTexTag.c_str(),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
		return E_FAIL;

	if (FAILED(__super::Load_UI(pInData, iPrototypeLevelID, pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CSkill_Gauge::Render()
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

HRESULT CSkill_Gauge::Ready_Component()
{
	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex_UI"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	return S_OK;
}

CSkill_Gauge* CSkill_Gauge::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSkill_Gauge* pInstance = new CSkill_Gauge(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed Created : CSkill_Gauge"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CSkill_Gauge::Clone(void* pArg)
{
	CSkill_Gauge* pInstance = new CSkill_Gauge(*this);
	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CSkill_Gauge"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSkill_Gauge::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
