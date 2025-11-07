#include "UI_Tutorial.h"
#include "GameInstance.h"
#include "ClientInstance.h"

#include "Tutorial_Panel.h"
#include "Tutorial_Tex.h"

#include "UI_BackGround.h"
#include "UI_Default_Button.h"
#include "UI_TextBox.h"
#include "UI_Guide_Icon.h"
#include "UI_Default_Tex.h"

CUI_Tutorial::CUI_Tutorial(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Panel{ pDevice, pContext }
{
}

CUI_Tutorial::CUI_Tutorial(const CUI_Tutorial& Prototype)
	: CUI_Panel(Prototype)
{
}

void CUI_Tutorial::NextPage()
{
	if (m_iSeletePage >= m_iMaxPage)
		return;
	++m_iSeletePage;
	if (m_iSeletePage >= m_iMaxPage)
	{
		m_iSeletePage = m_iMaxPage;
		m_pButtonUp->Update_Visible(false);
		m_pButtonDown->Update_Visible(true);


		m_pIconPageExit->Update_Visible(true);
		m_pIconPageMove->Set_Pos({ 890.f, 760.f });
	}

	for (auto Icon : m_pPageIcon)
		Icon->Set_Color({ 1.f, 1.f, 1.f, 0.6f });
	m_pPageIcon[m_iSeletePage]->Set_Color({ 1.f, 1.f, 1.f, 1.f });
	m_pTutorialTex->Setting_Tex(m_eGuideType, m_iSeletePage);
	m_pTutorialPanel->Setting_Panel(m_eGuideType, m_iSeletePage);
}

void CUI_Tutorial::ReturnPage()
{
	if (m_iSeletePage <= 0)
		return;
	--m_iSeletePage;
	if (m_iSeletePage <= 0)
	{
		m_iSeletePage = 0;
		m_pButtonUp->Update_Visible(true);
		m_pButtonDown->Update_Visible(false);

		m_pIconPageExit->Update_Visible(false);
		m_pIconPageMove->Set_Pos({ 960.f, 760.f });
	}
	for (auto Icon : m_pPageIcon)
		Icon->Set_Color({ 1.f, 1.f, 1.f, 0.6f });
	m_pPageIcon[m_iSeletePage]->Set_Color({ 1.f, 1.f, 1.f, 1.f });
	m_pTutorialTex->Setting_Tex(m_eGuideType, m_iSeletePage);
	m_pTutorialPanel->Setting_Panel(m_eGuideType, m_iSeletePage);
}

void CUI_Tutorial::On_Panel(GUIDE_TYPE eType)
{
	if (m_IsUpdate)
		return;
	m_iSeletePage = 0;

	m_eAnimState = UIANIMSTATE::ON;
	m_fAccTime = 0.5f;
	m_IsUpdate = true;

	m_eGuideType = eType;
	Setting_GuidePagae();
	m_ePreInputType = m_pGameInstance->Get_InputType();
	m_pGameInstance->Change_InputType(INPUT_TYPE::POPUP);
}

void CUI_Tutorial::Off_Panel()
{
	if (!m_IsUpdate)
		return;

	m_eAnimState = UIANIMSTATE::OFF;
	m_fAccTime = 1.f;
	m_pGameInstance->Change_InputType(m_ePreInputType);
}

HRESULT CUI_Tutorial::Initialize_Prototype(_uint iLevel)
{
	m_iLevel = iLevel;
	CHECK_FAILED(Ready_Prototype(), E_FAIL);
	return S_OK;
}

HRESULT CUI_Tutorial::Initialize_Clone(void* pArg)
{
	CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);

	CHECK_FAILED(Ready_Component(), E_FAIL);
	return S_OK;
}

void CUI_Tutorial::Priority_Update(_float fTimeDelta)
{
	if (!m_IsUpdate)
		return;
	if (m_pGameInstance->Key_Down(DIK_ESCAPE, INPUT_TYPE::POPUP))
		Off_Panel();

	UI_Animation(fTimeDelta);
	__super::Priority_Update(fTimeDelta);
}

void CUI_Tutorial::Update(_float fTimeDelta)
{
	if (!m_IsUpdate)
		return;
	
	if (m_iMaxPage >= 1 && m_pGameInstance->Key_Down(DIK_D, INPUT_TYPE::POPUP))
		NextPage();
	else if (m_iMaxPage >= 1 && m_pGameInstance->Key_Down(DIK_A, INPUT_TYPE::POPUP))
		ReturnPage();

	__super::Update(fTimeDelta);
}

void CUI_Tutorial::Late_Update(_float fTimeDelta)
{
	if (!m_IsUpdate)
		return;
	CClientInstance::GetInstance()->Add_UIRender(UI_RENDER_TYPE::DEFAULT, this);
	__super::Late_Update(fTimeDelta);
}

HRESULT CUI_Tutorial::Render()
{
	CHECK_FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix), E_FAIL);
	CHECK_FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix), E_FAIL);
	CHECK_FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);
	CHECK_FAILED(m_pTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_Texture", m_iTexPass), E_FAIL);

	CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float)), E_FAIL);
	CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_float4)), E_FAIL);

	m_pShaderCom->Begin(m_iShaderPass);
	m_pVIBufferCom->Bind_Resources();
	m_pVIBufferCom->Render();
	return S_OK;
}

HRESULT CUI_Tutorial::Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg)
{
	m_szName = pInData.value("name", "");
	CClientInstance::GetInstance()->Add_UIEvent(AnsiToWString(m_szName), TEXT("NextPage"), [this]() {NextPage(); });
	CClientInstance::GetInstance()->Add_UIEvent(AnsiToWString(m_szName), TEXT("ReturnPage"), [this]() {ReturnPage(); });
	CClientInstance::GetInstance()->Add_UIRender(UI_RENDER_TYPE::DEFAULT, this);
	__super::Load_UI(pInData, iPrototypeLevelID, pArg);

	m_iShaderPass = 15;

	for (auto pChild : m_Children)
	{
		string strName = pChild->Get_Name();

		if(strName == "Guide_Button_Down")
		{
			m_pButtonDown = static_cast<CUI_Default_Button*>(pChild);
			Safe_AddRef(m_pButtonDown);
			m_pButtonDown->Set_State(CUI_Button::STATE::SELETE);
		}
		else if (strName == "Guide_Button_Up")
		{
			m_pButtonUp = static_cast<CUI_Default_Button*>(pChild);
			Safe_AddRef(m_pButtonUp);
			m_pButtonUp->Set_State(CUI_Button::STATE::SELETE);
		}
		else if (strName == "Guide_Page_0" || strName == "Guide_Page_1")
		{
			m_pPageIcon.push_back(static_cast<CUI_Default_Tex*>(pChild));
			Safe_AddRef(pChild);
		}
		else if (strName == "Guide_Name" )
		{
			m_pTitleName = static_cast<CUI_TextBox*>(pChild);
			Safe_AddRef(m_pTitleName);
		}
		else if (strName == "Guide_Tex")
		{
			m_pTutorialTex = static_cast<CTutorial_Tex*>(pChild);
			Safe_AddRef(m_pTutorialTex);
		}
		else if (strName == "Guide_Panel")
		{
			m_pTutorialPanel = static_cast<CTutorial_Panel*>(pChild);
			Safe_AddRef(m_pTutorialPanel);
		}
		else if (strName == "Page_Guide_Icon")
		{
			m_pIconPageMove = static_cast<CUI_Guide_Icon*>(pChild);
			Safe_AddRef(m_pIconPageMove);
			CUI_Guide_Icon::TEXTBOX_DESC Desc = {};
			Desc.bIsTextBox = false;
			Desc.eTextAlign = TEXT_ALIGN::LEFT_CENTER;
			Desc.iPivotX = 40.f;
			Desc.iPivotY = 15.f;
			Desc.vColor = { 1.f, 1.f, 1.f, 1.f };
			Desc.wstrTexttag = TEXT("Blade_Medium_20");
			Desc.wstrText = TEXT("페이지 이동");

			m_pIconPageMove->Setting_Text(Desc);
		}
		else if (strName == "Exit_Guide_Icon")
		{
			m_pIconPageExit = static_cast<CUI_Guide_Icon*>(pChild);
			Safe_AddRef(m_pIconPageExit);
			CUI_Guide_Icon::TEXTBOX_DESC Desc = {};
			Desc.bIsTextBox = false;
			Desc.eTextAlign = TEXT_ALIGN::LEFT_CENTER;
			Desc.iPivotX = 20.f;
			Desc.iPivotY = 15.f;
			Desc.wstrTexttag = TEXT("Blade_Medium_20");
			Desc.wstrText = TEXT("닫기");
			Desc.vColor = { 1.f, 1.f, 1.f, 1.f };
			m_pIconPageExit->Setting_Text(Desc);
		}

	}

	CHECK_FAILED(Ready_Object(), E_FAIL);
	return S_OK;
}

void CUI_Tutorial::Bubble_EventCall(BUBBLEEVENT* pArg)
{

}

HRESULT CUI_Tutorial::Update_Switch(void* pArg)
{
	return S_OK;
}

HRESULT CUI_Tutorial::Ready_Prototype()
{
	CHECK_FAILED(m_pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_UI_Tutorial_Panel"),
		CTutorial_Panel::Create(m_pDevice, m_pContext)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_UI_Tutorial_Tex"),
		CTutorial_Tex::Create(m_pDevice, m_pContext)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_Component_UI_GuidePage"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/TutorialGuide/GuidePage_%d.dds"), 6)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_Component_UI_GuideTex"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/TutorialGuide/GuideTex_%d.dds"), 3)), E_FAIL);

	//CHECK_FAILED(m_pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_Component_Movie_CountAttack"),
	//	CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/TutorialGuide/CountAttack/CountAttack_%d.dds"), 152)), E_FAIL);

	return S_OK;
}

HRESULT CUI_Tutorial::Ready_Object()
{
	UIOBJECT_DESC Desc = {};
	Desc.fDepth = 2.2f;
	Desc.iUIType = ENUM_CLASS(UITYPE::TEXTURE);
	Desc.szName = "BackGround";
	Desc.vLocalSize = { g_iWinSizeX, g_iWinSizeY };
	Desc.vLocalPos = { g_iWinSizeX >> 1, g_iWinSizeY >> 1 };

	m_pBackGround = static_cast<CUI_BackGround*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_BackGround"), &Desc));
	if (m_pBackGround == nullptr)
		return E_FAIL;
	m_pBackGround->Setting_BG(CUI_BackGround::UIBGTYPE::END);
	m_pBackGround->Set_Color({ 0.0f, 0.0f, 0.0f, 0.8f });
	m_Children.push_back(m_pBackGround);
	Safe_AddRef(m_pBackGround);

	return S_OK;

}

void CUI_Tutorial::UI_Animation(_float fTimeDelta)
{
	if (m_eAnimState == UIANIMSTATE::ON)
	{
		m_fAccTime += fTimeDelta * 3.f;
		__super::Update_Alpha(m_fAccTime);

		if (m_fAccTime >= 1.f)
		{
			m_fAccTime = 1.f;
			m_eAnimState = UIANIMSTATE::END;
		}
	}
	else if (m_eAnimState == UIANIMSTATE::OFF)
	{
		m_fAccTime -= fTimeDelta * 3.f;
		__super::Update_Alpha(m_fAccTime);

		if (m_fAccTime <= 0.f)
		{
			m_fAccTime = 0.f;
			m_eAnimState = UIANIMSTATE::END;
			m_IsUpdate = false;
		}
	}
}

void CUI_Tutorial::Setting_GuidePagae()
{
	if (m_eGuideType == GUIDE_TYPE::LOCKON)
	{
		m_iMaxPage = 0;
		m_pButtonUp->Update_Visible(false);
		m_pButtonDown->Update_Visible(false);
		m_pTitleName->Set_Text(TEXT("락온"));
	}
	else if (m_eGuideType == GUIDE_TYPE::GUARD)
	{
		m_iMaxPage = 1;
		m_pButtonUp->Update_Visible(true);
		m_pButtonDown->Update_Visible(false);
		m_pTitleName->Set_Text(TEXT("가드"));
	}
	else if (m_eGuideType == GUIDE_TYPE::UNDERWORLD)
	{
		m_iMaxPage = 0;
		m_pButtonUp->Update_Visible(false);
		m_pButtonDown->Update_Visible(false);
		m_pTitleName->Set_Text(TEXT("명계의 기운"));
	}
	else if (m_eGuideType == GUIDE_TYPE::DODGE)
	{
		m_iMaxPage = 1;
		m_pButtonUp->Update_Visible(true);
		m_pButtonDown->Update_Visible(false);
		m_pTitleName->Set_Text(TEXT("회피"));
	}
	else if (m_eGuideType == GUIDE_TYPE::BURTALATTACK)
	{
		m_iMaxPage = 1;
		m_pButtonUp->Update_Visible(true);
		m_pButtonDown->Update_Visible(false);
		m_pTitleName->Set_Text(TEXT("브루탈 어택"));
	}
	else if (m_eGuideType == GUIDE_TYPE::FALLATTACK)
	{
		m_iMaxPage = 0;
		m_pButtonUp->Update_Visible(false);
		m_pButtonDown->Update_Visible(false);
		m_pTitleName->Set_Text(TEXT("낙하 공격"));
	}
	else if (m_eGuideType == GUIDE_TYPE::IMPULSE)
	{
		m_iMaxPage = 0;
		m_pButtonUp->Update_Visible(false);
		m_pButtonDown->Update_Visible(false);
		m_pTitleName->Set_Text(TEXT("충격량"));
	}

	if (m_iMaxPage == 0)
	{
		for (auto Icon : m_pPageIcon)
			Icon->Update_Visible(false);

		m_pIconPageMove->Update_Visible(false);
		m_pIconPageExit->Update_Visible(true);
		m_pIconPageExit->Set_Pos({ 960.f, 760.f });
	}
	else
	{
		for (auto Icon : m_pPageIcon)
		{
			Icon->Update_Visible(true);
			Icon->Set_Color({ 1.f, 1.f, 1.f, 0.6f });
		}
		m_pPageIcon[m_iSeletePage]->Set_Color({ 1.f, 1.f, 1.f, 1.f });

		m_pIconPageMove->Update_Visible(true);
		m_pIconPageExit->Update_Visible(false);
		m_pIconPageMove->Set_Pos({ 960.f, 760.f });
		m_pIconPageExit->Set_Pos({ 1060.f, 760.f });
	}

	m_pTutorialTex->Setting_Tex(m_eGuideType, m_iSeletePage);
	m_pTutorialPanel->Setting_Panel(m_eGuideType, m_iSeletePage);
}

HRESULT CUI_Tutorial::Ready_Component()
{
	CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex_UI"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr), E_FAIL);

	CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr), E_FAIL);

	CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_UI_GuideTex"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr), E_FAIL);

	return S_OK;
}

CUI_Tutorial* CUI_Tutorial::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CUI_Tutorial* pInstance = new CUI_Tutorial(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX(TEXT("Failed Created : CUI_Tutorial"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_Tutorial::Clone(void* pArg)
{
	CUI_Tutorial* pInstance = new CUI_Tutorial(*this);
	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CUI_Tutorial"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_Tutorial::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);

	Safe_Release(m_pBackGround);
	Safe_Release(m_pButtonUp);
	Safe_Release(m_pButtonDown);

	Safe_Release(m_pTitleName);
	Safe_Release(m_pTutorialTex);
	Safe_Release(m_pTutorialPanel);

	Safe_Release(m_pIconPageMove);
	Safe_Release(m_pIconPageExit);

	for (auto Icon : m_pPageIcon)
		Safe_Release(Icon);
	m_pPageIcon.clear();
}
