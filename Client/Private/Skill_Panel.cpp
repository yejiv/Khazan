#include "Skill_Panel.h"
#include "GameInstance.h"
#include "ClientInstance.h"

#include "Skill_Gauge.h"
#include "UI_TextBox.h"


CSkill_Panel::CSkill_Panel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Panel{ pDevice, pContext }
{
}

CSkill_Panel::CSkill_Panel(const CSkill_Panel& Prototype)
	: CUI_Panel(Prototype)
{
}

HRESULT CSkill_Panel::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CSkill_Panel::Initialize_Clone(void* pArg)
{
	CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);

	m_iLevel = &CClientInstance::GetInstance()->Get_PlayerData().iSkillLevel;
	m_iPoint = &CClientInstance::GetInstance()->Get_PlayerData().iSkilPoint;
	m_fExp = &CClientInstance::GetInstance()->Get_PlayerData().fSkillLevel_EXP;

	return S_OK;
}

void CSkill_Panel::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CSkill_Panel::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	m_pPoint->Set_Text(to_wstring(*m_iPoint));
	m_pLevel->Set_Text(to_wstring(*m_iLevel));

	_float fValue = *m_fExp;
	_int iTextValue = {};
	if (fValue >= 100)
		iTextValue = 3;
	else if (fValue >= 10)
		iTextValue = 2;
	else
		iTextValue = 1;

	_wstring wstrTemp = TEXT("숙련 레벨 + ") + to_wstring(fValue).substr(0, iTextValue) + TEXT("%");
	m_pExp->Set_Text(wstrTemp);

}

void CSkill_Panel::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CSkill_Panel::Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg)
{
	__super::Load_UI(pInData, iPrototypeLevelID, pArg);

	for (auto pChild : m_Children)
	{
		string strName = pChild->Get_Name();

		if (strName == "Point_Value")
		{
			m_pPoint = static_cast<CUI_TextBox*>(pChild);
			Safe_AddRef(m_pPoint);
		}
		else if (strName == "Exp_Gauge")
		{
			m_pGauge = static_cast<CSkill_Gauge*>(pChild);
			Safe_AddRef(m_pGauge);
		}
		else if (strName == "SkillLevel")
		{
			m_pLevel = static_cast<CUI_TextBox*>(pChild);
			Safe_AddRef(m_pLevel);
		}
		else if (strName == "Level_Level_Per")
		{
			m_pExp = static_cast<CUI_TextBox*>(pChild);
			Safe_AddRef(m_pExp);
		}
	}


	return S_OK;
}

CSkill_Panel* CSkill_Panel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSkill_Panel* pInstance = new CSkill_Panel(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed Created : CSkill_Panel"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CSkill_Panel::Clone(void* pArg)
{
	CSkill_Panel* pInstance = new CSkill_Panel(*this);
	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CSkill_Panel"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSkill_Panel::Free()
{
	__super::Free();

	Safe_Release(m_pGauge);
	Safe_Release(m_pPoint);
	Safe_Release(m_pLevel);
	Safe_Release(m_pExp);

}
