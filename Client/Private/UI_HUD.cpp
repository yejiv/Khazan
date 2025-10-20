#include "UI_HUD.h"
#include "GameInstance.h"

#include "UI_PlayerHP_Penal.h"
#include "UI_CombatSpirit_Penal.h"

CUI_HUD::CUI_HUD(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Panel{ pDevice, pContext }
{
}

CUI_HUD::CUI_HUD(const CUI_HUD& Prototype)
	: CUI_Panel(Prototype)
{
}

HRESULT CUI_HUD::Initialize_Prototype(_uint iLevel)
{
	m_iLevel = iLevel;

	if (FAILED(Ready_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_HUD::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	return S_OK;
}

void CUI_HUD::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUI_HUD::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CUI_HUD::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CUI_HUD::Render()
{
	return S_OK;
}

void CUI_HUD::Bubble_EventCall()
{
}

HRESULT CUI_HUD::Ready_Prototype()
{
	CHECK_FAILED(m_pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_UI_PlayerHP_Penal"),
		CUI_PlayerHP_Penal::Create(m_pDevice, m_pContext, m_iLevel)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_UI_CombatSpirit_Penal"),
		CUI_CombatSpirit_Penal::Create(m_pDevice, m_pContext, m_iLevel)), E_FAIL);

	return S_OK;
}

CUI_HUD* CUI_HUD::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CUI_HUD* pInstance = new CUI_HUD(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX(TEXT("Failed Created : CUI_HUD"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_HUD::Clone(void* pArg)
{
	CUI_HUD* pInstance = new CUI_HUD(*this);
	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CUI_HUD"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_HUD::Free()
{
	__super::Free();
}
