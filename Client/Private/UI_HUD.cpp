#include "UI_HUD.h"
#include "GameInstance.h"
#include "ClientInstance.h"

#include "UI_PlayerHP_Penal.h"
#include "UI_CombatSpirit_Penal.h"
#include "ItemInfo_Hud.h"

CUI_HUD::CUI_HUD(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Panel{ pDevice, pContext }
{
}

CUI_HUD::CUI_HUD(const CUI_HUD& Prototype)
	: CUI_Panel(Prototype)
{
}

void CUI_HUD::Add_Item(_int iIndex)
{
	if (m_pItemInfo.empty())
	{
		CItemInfo_Hud* pInfo = m_pRender_ItemInfo.front();
		m_pRender_ItemInfo.pop_front();
		pInfo->Add_Item(iIndex);
		m_pRender_ItemInfo.push_back(pInfo);
	}
	else
	{
		m_pItemInfo.front()->Add_Item(iIndex);
		m_pRender_ItemInfo.push_back(m_pItemInfo.front());
		m_pItemInfo.pop();
	}
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

	CHECK_FAILED(Ready_Childeren(), E_FAIL);
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

	for (_int i = (_int)m_pRender_ItemInfo.size() - 1; i >= 0; --i)
	{
		m_pRender_ItemInfo[i]->Update_Pos(i, (_int)m_pRender_ItemInfo.size() - 1, -70.f);
		m_pRender_ItemInfo[i]->Late_Update(fTimeDelta);
		if (m_pRender_ItemInfo[i]->Get_IsDead())
		{
			m_pItemInfo.push(m_pRender_ItemInfo[i]);
			m_pRender_ItemInfo.erase(m_pRender_ItemInfo.begin() + i);
		}
	}
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

	CHECK_FAILED(m_pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_UI_ItemInfo_Hud"),
		CItemInfo_Hud::Create(m_pDevice, m_pContext, m_iLevel)), E_FAIL);

	return S_OK;
}

HRESULT CUI_HUD::Ready_Childeren()
{
	for (_int i = 0; i < 5; ++i)
	{
		CUIObject* pItemInfo = CClientInstance::GetInstance()->Load_UIObject(m_iLevel, TEXT("../Bin/Resources/UI/UIData/Hud_ItemInfo.json"));

		if (pItemInfo == nullptr)
			return E_FAIL;

		m_pItemInfo.push(static_cast<CItemInfo_Hud*>(pItemInfo));
	}
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
	while (!m_pRender_ItemInfo.empty())
	{
		Safe_Release(m_pRender_ItemInfo.front());
		m_pRender_ItemInfo.pop_front();
	}

	while (!m_pItemInfo.empty())
	{
		Safe_Release(m_pItemInfo.front());
		m_pItemInfo.pop();
	}

	__super::Free();
	
}
