#include "UI_PlayerHP_Penal.h"
#include "GameInstance.h"

#include "UI_PlayerHP_BG.h"
#include "UI_PlayerHP_Gauge.h"
#include "UI_PlayerStamina_Gauge.h"
#include "UI_QuickSlot_Skill_Panel.h"
#include "UI_QuickSlot_Item_Panel.h"

CUI_PlayerHP_Penal::CUI_PlayerHP_Penal(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Panel{ pDevice, pContext }
{
}

CUI_PlayerHP_Penal::CUI_PlayerHP_Penal(const CUI_PlayerHP_Penal& Prototype)
	: CUI_Panel ( Prototype )
{
}

void CUI_PlayerHP_Penal::On_Penal()
{
    if (m_IsUpdate)
        return;

	m_eAnimState = UIANIMSTATE::ON;
	m_iCurrentKeyFrameIndex = 0;
    m_fAccTime = 0.f;
	m_IsUpdate = true;
}

void CUI_PlayerHP_Penal::Off_Penal()
{
    if (!m_IsUpdate)
        return;
    m_eAnimState = UIANIMSTATE::OFF;
    m_fAccTime = 1.f;
}

HRESULT CUI_PlayerHP_Penal::Initialize_Prototype(_uint iLevel)
{
	m_iLevel = iLevel;

	if (FAILED(Ready_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_PlayerHP_Penal::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	return S_OK;
}

void CUI_PlayerHP_Penal::Priority_Update(_float fTimeDelta)
{
	if (!m_IsUpdate)
		return;
	__super::Priority_Update(fTimeDelta);
}

void CUI_PlayerHP_Penal::Update(_float fTimeDelta)
{
	if (m_eAnimState == UIANIMSTATE::ON)
	{
		m_fAccTime += fTimeDelta;
		Update_Track(m_fAccTime);

		if (m_fAccTime >= m_Track.back().fTrackPosition)
		{
			m_fAccTime = m_Track.back().fTrackPosition;
			m_eAnimState = UIANIMSTATE::END;
		}
	}
	else if (m_eAnimState == UIANIMSTATE::OFF)
	{
		m_fAccTime -= fTimeDelta * 2.f;
		__super::Update_Alpha(m_fAccTime);

		if (m_fAccTime <= 0.f)
		{
			m_fAccTime = 0.f;
			m_eAnimState = UIANIMSTATE::END;
			m_IsUpdate = false;
		}
	}
	
	if (!m_IsUpdate)
		return;
	__super::Update(fTimeDelta);
}

void CUI_PlayerHP_Penal::Late_Update(_float fTimeDelta)
{
	if (!m_IsUpdate)
		return;
	__super::Late_Update(fTimeDelta);
}

HRESULT CUI_PlayerHP_Penal::Render()
{
	return S_OK;
}

void CUI_PlayerHP_Penal::Bubble_EventCall()
{
}

HRESULT CUI_PlayerHP_Penal::Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg)
{
	CHECK_FAILED(__super::Load_UI(pInData, iPrototypeLevelID, pArg),E_FAIL);
	return S_OK;
}

HRESULT CUI_PlayerHP_Penal::Ready_Prototype()
{
	CHECK_FAILED(m_pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_UI_PlayerHP_BG"),
		CUI_PlayerHP_BG::Create(m_pDevice, m_pContext, m_iLevel)), E_FAIL);
	
	CHECK_FAILED(m_pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_UI_PlayerHP_Gauge"),
		CUI_PlayerHP_Gauge::Create(m_pDevice, m_pContext, m_iLevel)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_UI_PlayerStamina_Gauge"),
		CUI_PlayerStamina_Gauge::Create(m_pDevice, m_pContext, m_iLevel)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_UI_Quick_Item_Panel"),
		CUI_QuickSlot_Item_Panel::Create(m_pDevice, m_pContext, m_iLevel)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_UI_Quick_Skill_Panel"),
		CUI_QuickSlot_Skill_Panel::Create(m_pDevice, m_pContext, m_iLevel)), E_FAIL);

	return S_OK;
}

CUI_PlayerHP_Penal* CUI_PlayerHP_Penal::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CUI_PlayerHP_Penal* pInstance = new CUI_PlayerHP_Penal(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX(TEXT("Failed Created : CUI_PlayerHP_Penal"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_PlayerHP_Penal::Clone(void* pArg)
{
	CUI_PlayerHP_Penal* pInstance = new CUI_PlayerHP_Penal(*this);
	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CUI_PlayerHP_Penal"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_PlayerHP_Penal::Free()
{
	__super::Free();
}
