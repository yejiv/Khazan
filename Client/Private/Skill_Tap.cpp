#include "Skill_Tap.h"
#include "ClientInstance.h"
#include "GameInstance.h"

#include "UI_TextBox.h"
#include "Skill_Slot_Panel.h"

CSkill_Tap::CSkill_Tap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Tap{ pDevice, pContext }
{
}

CSkill_Tap::CSkill_Tap(const CSkill_Tap& Prototype)
	: CUI_Tap(Prototype)
{
}

HRESULT CSkill_Tap::Setting_Panel(_int iTapIndex)
{
	m_iTapIndex = iTapIndex;

	for (_int i = 0; i < m_pPanel.size(); ++i)
	{
		m_pPanel[i]->Setting_Slot(iTapIndex, i);
	}
	return S_OK;
}

HRESULT CSkill_Tap::Initialize_Prototype(_int iLevel)
{
	m_iLevel = iLevel;
	return S_OK;
}

HRESULT CSkill_Tap::Initialize_Clone(void* pArg)
{
	CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);
	return S_OK;
}

void CSkill_Tap::Priority_Update(_float fTimeDelta)
{
	if (m_iState == ENUM_CLASS(UISTATE::DISABLE))
		return;

	for (auto pPanel : m_pPanel)
		pPanel->Priority_Update(fTimeDelta);
	__super::Priority_Update(fTimeDelta);

}

void CSkill_Tap::Update(_float fTimeDelta)
{
	if (m_iState == ENUM_CLASS(UISTATE::DISABLE))
		return;

	for (auto pPanel : m_pPanel)
		pPanel->Update(fTimeDelta);
	__super::Update(fTimeDelta);
}

void CSkill_Tap::Late_Update(_float fTimeDelta)
{
	if (m_iState == ENUM_CLASS(UISTATE::DISABLE))
		return;

	for (auto pPanel : m_pPanel)
		pPanel->Late_Update(fTimeDelta);
	__super::Late_Update(fTimeDelta);
}

HRESULT CSkill_Tap::Render()
{
	return S_OK;
}

HRESULT CSkill_Tap::Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg)
{
	__super::Load_UI(pInData, iPrototypeLevelID, pArg);
	if(m_szName == "Spear")
		m_iState = ENUM_CLASS(UISTATE::ENABLE);

	CHECK_FAILED(Ready_Children(), E_FAIL);

	return S_OK;
}

HRESULT CSkill_Tap::Ready_Children()
{
	_float2 vPos = { 350, 540 };

	for (_int i = 0; i < 5; ++i)
	{
		CSkill_Slot_Panel* pPanel = static_cast<CSkill_Slot_Panel*>(CClientInstance::GetInstance()->Load_UIObject(m_iLevel, TEXT("../Bin/Resources/UI/UIData/Skill_Slot_Panel.json")));
		if (pPanel == nullptr)
			return E_FAIL;

		m_pPanel.push_back(pPanel);
		pPanel->LocalPos_Set({ vPos.x + i * 310.f, vPos.y }, this);
	}
	return S_OK;
}

CSkill_Tap* CSkill_Tap::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _int iLevel)
{
	CSkill_Tap* pInstance = new CSkill_Tap(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX(TEXT("Failed Created : CSkill_Tap"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CSkill_Tap::Clone(void* pArg)
{
	CSkill_Tap* pInstance = new CSkill_Tap(*this);
	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CSkill_Tap"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSkill_Tap::Free()
{
	__super::Free();
	for (auto pPanel : m_pPanel)
		Safe_Release(pPanel);
	m_pPanel.clear();
}
