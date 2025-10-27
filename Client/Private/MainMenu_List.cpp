#include "MainMenu_List.h"
#include "ClientInstance.h"
#include "GameInstance.h"

#include "UI_TextBox.h"
#include "MainMenu_Deco.h"

CMainMenu_List::CMainMenu_List(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Tap{ pDevice, pContext }
{
}

CMainMenu_List::CMainMenu_List(const CMainMenu_List& Prototype)
	: CUI_Tap(Prototype)
{
}

void CMainMenu_List::Update_Pos(_int iIndex, _float2 vPos, _float fOffSetY)
{
	m_vLocalPos.x = vPos.x;
	m_vLocalPos.y = vPos.y + iIndex * fOffSetY;

	__super::Update_Transform(nullptr, m_vWorldPos);
}

HRESULT CMainMenu_List::Initialize_Prototype(_uint iLevel)
{
	m_iLevel = iLevel;

	return S_OK;
}

HRESULT CMainMenu_List::Initialize_Clone(void* pArg)
{
	MAINMENULIST_DESC* pDesc = static_cast<MAINMENULIST_DESC*>(pArg);
	m_eMenuType = pDesc->eMenu;

	CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);
	CHECK_FAILED(Ready_Componet(), E_FAIL);

	return S_OK;
}

void CMainMenu_List::Priority_Update(_float fTimeDelta)
{
}

void CMainMenu_List::Update(_float fTimeDelta)
{
	m_isVisible = false;

	if ( ButtonOver(g_hWnd))
	{
		m_isVisible = true;
		m_pTextBox->Set_Color({ 0.145f, 0.141f, 0.149f, 1.0f });
	}
	else
	{
		m_pTextBox->Set_Color({ 0.749f, 0.749f, 0.749f, 1.0f });
	}
	if (ButtonClick(g_hWnd, false, true))
	{
		CUI_MainMenu::MAINMENUBUBBLE_DESC Desc{};
		Desc.eListType = m_eMenuType;

		Bubble_EventCall(&Desc);
	}


}

void CMainMenu_List::Late_Update(_float fTimeDelta)
{
	if (m_isVisible)
	{
		CClientInstance::GetInstance()->Add_UIRender(UI_RENDER_TYPE::DEFAULT, this);
		m_pDeco->Late_Update(fTimeDelta);
	}
	m_pTextBox->Late_Update(fTimeDelta);
}

HRESULT CMainMenu_List::Render()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_Texture", m_iTexPass)))
		return E_FAIL;

	CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float)), E_FAIL);

	m_pShaderCom->Begin(1);
	m_pVIBufferCom->Bind_Resources();
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CMainMenu_List::Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg)
{
	__super::Load_UI(pInData, iPrototypeLevelID, pArg);
	m_iShaderPass = 2;
	m_iState = ENUM_CLASS(UISTATE::ENABLE);

	for (auto pChild : m_Children)
	{
		string strName = pChild->Get_Name();
		if (strName == "Menu_Name")
		{
			m_pTextBox = static_cast<CUI_TextBox*>(pChild);
			Safe_AddRef(m_pTextBox);
		}
		else if (strName == "Menu_List_Deco")
		{
			m_pDeco = static_cast<CMainMune_Deco*>(pChild);
			Safe_AddRef(m_pDeco);
		}
	}

	_wstring szText = {};
	switch (m_eMenuType)
	{
	case CUI_MainMenu::MENULIST::WEAPON:
		szText = TEXT("장비");
		break;
	case CUI_MainMenu::MENULIST::OTHER:
		szText = TEXT("소지품");
		break;
	case CUI_MainMenu::MENULIST::SKILL:
		szText = TEXT("스킬 트리");
		break;
	case CUI_MainMenu::MENULIST::STATE:
		szText = TEXT("상태");
		break;
	case CUI_MainMenu::MENULIST::OPTION:
		szText = TEXT("설정");
		break;
	case CUI_MainMenu::MENULIST::TITLE:
		szText = TEXT("타이틀로 이동");
		break;
	case CUI_MainMenu::MENULIST::EXIT:
		szText = TEXT("게임 종료");
		break;
	}

	m_pTextBox->Set_Text(szText);
	return S_OK;
}

HRESULT CMainMenu_List::Ready_Componet()
{
	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex_UI"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_UI_Common_MenuList"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
		return E_FAIL;

	return S_OK;
}

CMainMenu_List* CMainMenu_List::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CMainMenu_List* pInstance = new CMainMenu_List(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX(TEXT("Failed Created : CMainMenu_List"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CMainMenu_List::Clone(void* pArg)
{
	CMainMenu_List* pInstance = new CMainMenu_List(*this);
	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CMainMenu_List"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMainMenu_List::Free()
{
	__super::Free();

	Safe_Release(m_pTextBox);
	Safe_Release(m_pDeco);

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
