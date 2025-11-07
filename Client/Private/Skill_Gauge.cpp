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

	return S_OK;
}

void CSkill_Gauge::Priority_Update(_float fTimeDelta)
{
	if (!m_IsUpdate)
		return;

	__super::Priority_Update(fTimeDelta);
}

void CSkill_Gauge::Update(_float fTimeDelta)
{
	if (!m_IsUpdate)
		return;
	__super::Update(fTimeDelta);
}

void CSkill_Gauge::Late_Update(_float fTimeDelta)
{
	if (!m_IsUpdate)
		return;

	__super::Late_Update(fTimeDelta);
}

HRESULT CSkill_Gauge::Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg)
{
	__super::Load_UI(pInData, iPrototypeLevelID, pArg);

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
}
