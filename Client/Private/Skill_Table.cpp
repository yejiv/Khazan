#include "Skill_Table.h"
#include "GameInstance.h"
#include "ClientInstance.h"

#include "UI_Atlas_Icon.h"

CSkill_Table::CSkill_Table(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Panel{ pDevice, pContext }
{
}

CSkill_Table::CSkill_Table(const CSkill_Table& Prototype)
	: CUI_Panel(Prototype)
{
}

HRESULT CSkill_Table::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CSkill_Table::Initialize_Clone(void* pArg)
{
	CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);
	m_iSkillLevel = &CClientInstance::GetInstance()->Get_PlayerData().iSkillLevel;
	return S_OK;
}

void CSkill_Table::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CSkill_Table::Update(_float fTimeDelta)
{
	if (m_iPreSkillLevel != *m_iSkillLevel)
	{
		m_iPreSkillLevel = *m_iSkillLevel;

		if (m_iPreSkillLevel >= 2)
			m_pLine[0]->Update_Color_Child({ 1.f, 1.f, 1.f, 1.f });
		if (m_iPreSkillLevel >= 10)
			m_pLine[1]->Update_Color_Child({ 1.f, 1.f, 1.f, 1.f });
		if (m_iPreSkillLevel >= 18)
			m_pLine[2]->Update_Color_Child({ 1.f, 1.f, 1.f, 1.f });
		if (m_iPreSkillLevel >= 26)
			m_pLine[3]->Update_Color_Child({ 1.f, 1.f, 1.f, 1.f });
		if (m_iPreSkillLevel >= 34)
			m_pLine[4]->Update_Color_Child({ 1.f, 1.f, 1.f, 1.f });
	}
	__super::Update(fTimeDelta);
}

void CSkill_Table::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CSkill_Table::Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg)
{
	__super::Load_UI(pInData, iPrototypeLevelID, pArg);

	for (auto pChild : m_Children)
	{
		m_pLine.push_back(static_cast<CUI_Atlas_Icon*>(pChild));
		Safe_AddRef(pChild);
	}

	for (auto pLine : m_pLine)
		pLine->Update_Color_Child({ 1.f, 1.f, 1.f, 0.6f });

	return S_OK;
}

CSkill_Table* CSkill_Table::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSkill_Table* pInstance = new CSkill_Table(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed Created : CSkill_Table"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CSkill_Table::Clone(void* pArg)
{
	CSkill_Table* pInstance = new CSkill_Table(*this);
	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CSkill_Table"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSkill_Table::Free()
{
	__super::Free();

	for (auto pLine : m_pLine)
		Safe_Release(pLine);
	m_pLine.clear();

}
