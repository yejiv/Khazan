#include "UI_Inven.h"
#include "GameInstance.h"

#include "Inven_Panel.h"
#include "Inven_Tap.h"

#include "Inven_OtherItem.h"
#include "UI_BackGround.h"

CUI_Inven::CUI_Inven(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Panel{ pDevice, pContext }
{
}

CUI_Inven::CUI_Inven(const CUI_Inven& Prototype)
	: CUI_Panel(Prototype)
{
}

HRESULT CUI_Inven::Initialize_Prototype(_uint iLevel)
{
	m_iLevel = iLevel;

	FAILED_CHECK(Ready_Prototype(), E_FAIL);
	
	return S_OK;
}

HRESULT CUI_Inven::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	FAILED_CHECK(Ready_Object(), E_FAIL);
	return S_OK;
}

void CUI_Inven::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUI_Inven::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CUI_Inven::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
	m_pBackGround->Late_Update(fTimeDelta);
}

HRESULT CUI_Inven::Render()
{
	return S_OK;
}

void CUI_Inven::Bubble_EventCall()
{
}

HRESULT CUI_Inven::Ready_Prototype()
{
	CHECK_FAILED(m_pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_UI_Inven_Panel"),
		CInven_Panel::Create(m_pDevice, m_pContext, m_iLevel)), E_FAIL);
	CHECK_FAILED(m_pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_UI_Inven_Tap"),
		CInven_Tap::Create(m_pDevice, m_pContext, m_iLevel)), E_FAIL);
	CHECK_FAILED(m_pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_UI_Inven_OtherItem"),
		CInven_OtherItem::Create(m_pDevice, m_pContext, m_iLevel)), E_FAIL);

	return S_OK;
}

HRESULT CUI_Inven::Ready_Object()
{
	UIOBJECT_DESC Desc = {};
	Desc.fDepth = 3;
	Desc.iUIType = ENUM_CLASS(UITYPE::TEXTURE);
	Desc.szName = "BackGround";
	Desc.vLocalSize = { g_iWinSizeX, g_iWinSizeY };
	Desc.vLocalPos = { g_iWinSizeX >> 1, g_iWinSizeY >> 1 };

	m_pBackGround = static_cast<CUI_BackGround*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC),TEXT("Prototype_GameObject_UI_BackGround"), &Desc));
	if (m_pBackGround == nullptr)
		return E_FAIL;
	m_pBackGround->Setting_BG(CUI_BackGround::UIBGTYPE::ITEM);
	return S_OK;
}

CUI_Inven* CUI_Inven::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CUI_Inven* pInstance = new CUI_Inven(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX(TEXT("Failed Created : CUI_Inven"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_Inven::Clone(void* pArg)
{
	CUI_Inven* pInstance = new CUI_Inven(*this);
	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CUI_Inven"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_Inven::Free()
{
	__super::Free();
	Safe_Release(m_pBackGround);
}
