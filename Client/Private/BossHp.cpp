#include "BossHp.h"
#include "GameInstance.h"
#include "ClientInstance.h"

#include "UI_Default_Tex.h"
#include "BossHp_Gauge.h"
#include "UI_Announce_MapName.h"
CBossHp::CBossHp(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Panel{ pDevice, pContext }
{
}

CBossHp::CBossHp(const CBossHp& Prototype)
	: CUI_Panel(Prototype)
{
}

HRESULT CBossHp::Initialize_Prototype(_uint iLevel)
{
	m_iLevel = iLevel;

	if (FAILED(Ready_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBossHp::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	return S_OK;
}

void CBossHp::Priority_Update(_float fTimeDelta)
{
	UI_Animation(fTimeDelta);
}

void CBossHp::Update(_float fTimeDelta)
{
	if (!m_IsUpdate)
		return;
	__super::Update(fTimeDelta);
}

void CBossHp::Late_Update(_float fTimeDelta)
{
	if (!m_IsUpdate)
		return;
	__super::Late_Update(fTimeDelta);
}

HRESULT CBossHp::Render()
{
	return S_OK;
}

void CBossHp::Bubble_EventCall()
{
}

HRESULT CBossHp::Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg)
{
	__super::Load_UI(pInData, iPrototypeLevelID, pArg);

	for (auto child : m_Children)
	{
		string strName = child->Get_Name();

		if (strName == "BossHp_BG_Top" || strName == "BossHp_BG_Bottom")
		{
			static_cast<CUI_Default_Tex*>(child)->Set_Color({ 1.f, 1.f, 1.f, 0.6f });
		}
		if (strName == "BossHp_BG_Top_Gauge")
		{
			m_pHPGauge = static_cast<CBossHp_Gauge*>(child);
			Safe_AddRef(m_pHPGauge);
			m_pHPGauge->Set_ShaderPass(11);
		}
		if (strName == "BossHp_BG_Bottom_Gauge")
		{
			m_pStaminaGauge = static_cast<CBossHp_Gauge*>(child);
			Safe_AddRef(m_pStaminaGauge);
			m_pStaminaGauge->Set_ShaderPass(7);
		}
	}
	return S_OK;
}

HRESULT CBossHp::Update_Switch(void* pArg)
{
	BOSSMON_UPDATE_DESC* pDesc = static_cast<BOSSMON_UPDATE_DESC*>(pArg);
	if (!pDesc->isOpen)
	{
		m_eAnimState = UIANIMSTATE::OFF;
		return S_OK;
	}

	m_eAnimState = UIANIMSTATE::ON;
	m_IsUpdate = true;
	m_pHPGauge->Setting_Progress(pDesc->pHpValue, pDesc->pHpMaxValue);
	m_pStaminaGauge->Setting_Progress(pDesc->pStaminaCulValue, pDesc->pStaminaMaxValue);
	return S_OK;
}

HRESULT CBossHp::Ready_Prototype()
{
	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_BossHp_Gauge"),
		CBossHp_Gauge::Create(m_pDevice, m_pContext)), E_FAIL);

	return S_OK;
}

void CBossHp::UI_Animation(_float fTimeDelta)
{
	if (m_eAnimState == UIANIMSTATE::ON)
	{
		m_fAccTime += fTimeDelta * 3.f;
		__super::Update_Alpha(m_fAccTime);

		if (m_fAccTime >= 1.f)
		{
			m_fAccTime = 1.f;
			m_eAnimState = UIANIMSTATE::END;
		}
	}
	else if (m_eAnimState == UIANIMSTATE::OFF)
	{
		m_fAccTime -= fTimeDelta * 3.f;
		__super::Update_Alpha(m_fAccTime);

		if (m_fAccTime <= 0.f)
		{
			m_fAccTime = 0.f;
			m_eAnimState = UIANIMSTATE::END;
			m_IsUpdate = false;
			m_pHPGauge->Setting_Progress(nullptr, nullptr);
			m_pStaminaGauge->Setting_Progress(nullptr, nullptr);
		}
	}
}

CBossHp* CBossHp::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CBossHp* pInstance = new CBossHp(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX(TEXT("Failed Created : CBossHp"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CBossHp::Clone(void* pArg)
{
	CBossHp* pInstance = new CBossHp(*this);
	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CBossHp"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBossHp::Free()
{
	__super::Free();

	Safe_Release(m_pHPGauge);
	Safe_Release(m_pStaminaGauge);
}
