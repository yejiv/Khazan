#include "Skill_Slot_Panel.h"
#include "GameInstance.h"
#include "ClientInstance.h"
#include "DB_Manager.h"

#include "Skill_Slot.h"

#include "UI_Atlas_Icon.h"
#include "UI_TextBox.h"

CSkill_Slot_Panel::CSkill_Slot_Panel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Panel{ pDevice, pContext }
{
}

CSkill_Slot_Panel::CSkill_Slot_Panel(const CSkill_Slot_Panel& Prototype)
	: CUI_Panel(Prototype)
{
}

HRESULT CSkill_Slot_Panel::Setting_Slot(_int iTapIndex, _int iPanelIndex)
{
	const SKILL_TABLE* pData = CClientInstance::GetInstance()->Get_DataTalbe<SKILL_TABLE>();
	_int iMaxIndexX = { -1 };
	for (const auto& [ID, Skill] : *pData)
	{
		if (Skill.iType == iTapIndex && Skill.iSubID == iPanelIndex)
			if(Skill.iSlotX > iMaxIndexX)
				iMaxIndexX = Skill.iSlotX;
	}

	for (const auto& [ID, Skill] : *pData)
	{
		if (Skill.iType == iTapIndex && Skill.iSubID == iPanelIndex)
		{
			CSkill_Slot::UISKILLSLOT_DESC SkillDesc = {};
			SkillDesc.vLocalSize = { 64.f, 64.f };
			if(iMaxIndexX < 3)
				SkillDesc.vLocalPos = { -70.f + (Skill.iSlotX * 70.f), -175.f + (Skill.iLevel * 80.f) };
			else //70 0 70 // 105 35 35 
				SkillDesc.vLocalPos = { -105.f + (Skill.iSlotX * 70.f), -175.f + (Skill.iLevel * 80.f) };
			SkillDesc.iUIType = ENUM_CLASS(UITYPE::TEXTURE);
			SkillDesc.szName = "Skill_Slot";
			SkillDesc.fDepth = 4.f;
			SkillDesc.iSkillIndex = ID;
			CSkill_Slot* pSkill = static_cast<CSkill_Slot*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, m_iLevel, TEXT("Prototype_GameObject_UI_Skill"), &SkillDesc));
			m_Children.push_back(pSkill);
			m_Skill.push_back(pSkill);
			Safe_AddRef(pSkill);
		}
	}

	Update_Transform(nullptr, m_vWorldPos);

	Panel_NameMapping(iTapIndex, iPanelIndex);
	return S_OK;
}

void CSkill_Slot_Panel::LocalPos_Set(_float2 vPos, CUIObject* pParent)
{
	m_vLocalPos = vPos;
	
	Update_Transform(nullptr, m_vLocalPos);
	
}

HRESULT CSkill_Slot_Panel::Initialize_Prototype(_int iLevel)
{
	m_iLevel = iLevel;
	CHECK_FAILED(Ready_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CSkill_Slot_Panel::Initialize_Clone(void* pArg)
{
	CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);
	return S_OK;
}

void CSkill_Slot_Panel::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CSkill_Slot_Panel::Update(_float fTimeDelta)
{
	if (IsPick(g_hWnd))
	{
		m_pTextBox->Set_Color({ 1.f, 1.f, 1.f, 1.f });
		m_pAtlasIcon->Update_Color_Child({ 1.f, 1.f, 1.f, 1.f });
	}
	else
	{
		m_pTextBox->Set_Color({ 1.f, 1.f, 1.f, 0.6f });
		m_pAtlasIcon->Update_Color_Child({ 1.f, 1.f, 1.f, 0.6f });
	}

	__super::Update(fTimeDelta);
}

void CSkill_Slot_Panel::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CSkill_Slot_Panel::Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg)
{
	__super::Load_UI(pInData, iPrototypeLevelID, pArg);

	for (auto pChild : m_Children)
	{
		string strName = pChild->Get_Name();
	
		if (strName == "Skill_Slot_Name")
		{
			m_pTextBox = static_cast<CUI_TextBox*>(pChild);
			Safe_AddRef(m_pTextBox);
		}
		else if (strName == "Skill_Slot_Icon")
		{
			m_pAtlasIcon = static_cast<CUI_Atlas_Icon*>(pChild);
			Safe_AddRef(m_pAtlasIcon);
		}
		
	}

	return S_OK;
}

HRESULT CSkill_Slot_Panel::Ready_Prototype()
{
	CHECK_FAILED(m_pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_UI_Skill"),
		CSkill_Slot::Create(m_pDevice, m_pContext, m_iLevel)), E_FAIL);

	return S_OK;
}

void CSkill_Slot_Panel::Panel_NameMapping(_int iTapIndex, _int iPanelIndex)
{
	if (iTapIndex == 0)
	{
		if (iPanelIndex == 0)
			m_pTextBox->Set_Text(TEXT("빠른 공격"));
		else if (iPanelIndex == 1)
			m_pTextBox->Set_Text(TEXT("강한 공격"));
		else if (iPanelIndex == 2)
			m_pTextBox->Set_Text(TEXT("연계"));
		else if (iPanelIndex == 3)
			m_pTextBox->Set_Text(TEXT("기교"));
		else if (iPanelIndex == 4)
			m_pTextBox->Set_Text(TEXT("가드"));
	}
	else if (iTapIndex == 1)
	{
		if (iPanelIndex == 0)
			m_pTextBox->Set_Text(TEXT("빠른 공격"));
		else if (iPanelIndex == 1)
			m_pTextBox->Set_Text(TEXT("강한 공격"));
		else if (iPanelIndex == 2)
			m_pTextBox->Set_Text(TEXT("불굴"));
		else if (iPanelIndex == 3)
			m_pTextBox->Set_Text(TEXT("약진"));
		else if (iPanelIndex == 4)
			m_pTextBox->Set_Text(TEXT("가드"));
	}
	else if (iTapIndex == 2)
	{
		if (iPanelIndex == 0)
			m_pTextBox->Set_Text(TEXT("자원"));
		else if (iPanelIndex == 1)
			m_pTextBox->Set_Text(TEXT("브루탈 어택"));
		else if (iPanelIndex == 2)
			m_pTextBox->Set_Text(TEXT("투창"));
		else if (iPanelIndex == 3)
			m_pTextBox->Set_Text(TEXT("카운트 어택"));
	}
}

CSkill_Slot_Panel* CSkill_Slot_Panel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _int iLevel)
{
	CSkill_Slot_Panel* pInstance = new CSkill_Slot_Panel(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX(TEXT("Failed Created : CSkill_Slot_Panel"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CSkill_Slot_Panel::Clone(void* pArg)
{
	CSkill_Slot_Panel* pInstance = new CSkill_Slot_Panel(*this);
	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CSkill_Slot_Panel"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSkill_Slot_Panel::Free()
{
	__super::Free();

	Safe_Release(m_pTextBox);
	Safe_Release(m_pAtlasIcon);
	for (auto pSkill : m_Skill)
		Safe_Release(pSkill);
	m_Skill.clear();
}
