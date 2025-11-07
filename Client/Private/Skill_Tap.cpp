#include "Skill_Tap.h"
#include "ClientInstance.h"
#include "GameInstance.h"

#include "UI_TextBox.h"

CSkill_Tap::CSkill_Tap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Tap{ pDevice, pContext }
{
}

CSkill_Tap::CSkill_Tap(const CSkill_Tap& Prototype)
	: CUI_Tap(Prototype)
{
}

HRESULT CSkill_Tap::Initialize_Prototype(_uint iLevel)
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
}

void CSkill_Tap::Update(_float fTimeDelta)
{

}

void CSkill_Tap::Late_Update(_float fTimeDelta)
{

}

HRESULT CSkill_Tap::Render()
{
	return S_OK;
}

HRESULT CSkill_Tap::Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg)
{
	__super::Load_UI(pInData, iPrototypeLevelID, pArg);
	return S_OK;
}

CSkill_Tap* CSkill_Tap::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
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

}
