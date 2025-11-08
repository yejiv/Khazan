#include "Skill_Table.h"
#include "GameInstance.h"
#include "ClientInstance.h"

#include "UI_BackGround.h"
#include "UI_Default_Button.h"
#include "UI_TextBox.h"
#include "UI_Guide_Icon.h"
#include "UI_Default_Tex.h"

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

	return S_OK;
}

void CSkill_Table::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CSkill_Table::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CSkill_Table::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CSkill_Table::Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg)
{
	//m_szName = pInData.value("name", "");
	//CClientInstance::GetInstance()->Add_UIEvent(AnsiToWString(m_szName), TEXT("NextPage"), [this]() {NextPage(); });
	//CClientInstance::GetInstance()->Add_UIEvent(AnsiToWString(m_szName), TEXT("ReturnPage"), [this]() {ReturnPage(); });
	//CClientInstance::GetInstance()->Add_UIRender(UI_RENDER_TYPE::DEFAULT, this);
	__super::Load_UI(pInData, iPrototypeLevelID, pArg);

	//for (auto pChild : m_Children)
	//{
	//	string strName = pChild->Get_Name();

	//	if (strName == "Guide_Button_Down")
	//	{
	//		m_pButtonDown = static_cast<CUI_Default_Button*>(pChild);
	//		Safe_AddRef(m_pButtonDown);
	//		m_pButtonDown->Set_State(CUI_Button::STATE::SELETE);
	//	}
	//}


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

}
