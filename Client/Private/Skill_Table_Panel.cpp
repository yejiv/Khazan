#include "Skill_Table_Panel.h"
#include "GameInstance.h"
#include "ClientInstance.h"

CSkill_Table_Panel::CSkill_Table_Panel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Panel{ pDevice, pContext }
{
}

CSkill_Table_Panel::CSkill_Table_Panel(const CSkill_Table_Panel& Prototype)
	: CUI_Panel(Prototype)
{
}

HRESULT CSkill_Table_Panel::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSkill_Table_Panel::Initialize_Clone(void* pArg)
{
	CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);

	return S_OK;
}

void CSkill_Table_Panel::Priority_Update(_float fTimeDelta)
{
	if (!m_IsUpdate)
		return;

	__super::Priority_Update(fTimeDelta);
}

void CSkill_Table_Panel::Update(_float fTimeDelta)
{
	if (!m_IsUpdate)
		return;
	__super::Update(fTimeDelta);
}

void CSkill_Table_Panel::Late_Update(_float fTimeDelta)
{
	if (!m_IsUpdate)
		return;

	__super::Late_Update(fTimeDelta);
}

HRESULT CSkill_Table_Panel::Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg)
{
	__super::Load_UI(pInData, iPrototypeLevelID, pArg);

	return S_OK;
}

CSkill_Table_Panel* CSkill_Table_Panel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSkill_Table_Panel* pInstance = new CSkill_Table_Panel(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed Created : CSkill_Table_Panel"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CSkill_Table_Panel::Clone(void* pArg)
{
	CSkill_Table_Panel* pInstance = new CSkill_Table_Panel(*this);
	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CSkill_Table_Panel"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSkill_Table_Panel::Free()
{
	__super::Free();
}
