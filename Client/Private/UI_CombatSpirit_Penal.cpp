#include "UI_CombatSpirit_Penal.h"
#include "GameInstance.h"

#include "UI_ComBatSpirit_Gauge.h"
#include "UI_ComBatSpirit_Slot.h"

CUI_CombatSpirit_Penal::CUI_CombatSpirit_Penal(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Panel{ pDevice, pContext }
{
}

CUI_CombatSpirit_Penal::CUI_CombatSpirit_Penal(const CUI_CombatSpirit_Penal& Prototype)
	: CUI_Panel(Prototype)
{
}

HRESULT CUI_CombatSpirit_Penal::Initialize_Prototype(_uint iLevel)
{
	m_iLevel = iLevel;
	
	if(FAILED(Ready_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_CombatSpirit_Penal::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	return S_OK;
}

void CUI_CombatSpirit_Penal::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUI_CombatSpirit_Penal::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CUI_CombatSpirit_Penal::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CUI_CombatSpirit_Penal::Render()
{
	return S_OK;
}

void CUI_CombatSpirit_Penal::Bubble_EventCall()
{
}

HRESULT CUI_CombatSpirit_Penal::Ready_Prototype()
{
	if(FAILED(m_pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_UI_CombatSpirit_Slot"),
		CUI_ComBatSpirit_Slot::Create(m_pDevice, m_pContext, m_iLevel))))
		return E_FAIL;

	if(FAILED(m_pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_UI_CombatSpirit_Gauge"),
		CUI_CombatSpirit_Gauge::Create(m_pDevice, m_pContext, m_iLevel))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_Component_Hud_ComBatSpirit"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Hud/State/T_Hud_Gauge_CombatSpirit_0%d.png"), 4))))
		return E_FAIL;

	return S_OK;
}

CUI_CombatSpirit_Penal* CUI_CombatSpirit_Penal::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CUI_CombatSpirit_Penal* pInstance = new CUI_CombatSpirit_Penal(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX(TEXT("Failed Created : CUI_CombatSpirit_Penal"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_CombatSpirit_Penal::Clone(void* pArg)
{
	CUI_CombatSpirit_Penal* pInstance = new CUI_CombatSpirit_Penal(*this);
	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CUI_CombatSpirit_Penal"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_CombatSpirit_Penal::Free()
{
	__super::Free();
}
