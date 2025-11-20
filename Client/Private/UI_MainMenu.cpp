#include "UI_MainMenu.h"
#include "GameInstance.h"
#include "ClientInstance.h"

#include "UI_TextBox.h"
#include "UI_BackGround.h"

#include "MainMenu_List.h"

#include "UI_Inven.h"
#include "UI_State.h"
#include "UI_SkillTree.h"
#include "Amount.h"

CUI_MainMenu::CUI_MainMenu(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Panel{ pDevice, pContext }
{
}

CUI_MainMenu::CUI_MainMenu(const CUI_MainMenu& Prototype)
	: CUI_Panel(Prototype)
{
}

void CUI_MainMenu::On_Panel()
{
	if (m_IsUpdate)
		return;
	m_iSeleteIndex = 0;
	for (_int i = 0; i < ENUM_CLASS(MENULIST::END); ++i)
		i == m_iSeleteIndex ? m_pList[i]->Set_Selete(true) : m_pList[i]->Set_Selete(false);

	m_eAnimState = UIANIMSTATE::ON;
	m_fAccTime = 0.5f;
	m_IsUpdate = true;
	m_eNextEvent = MENULIST::END;

	m_pGameInstance->Change_InputType(INPUT_TYPE::UI);

    static_cast<CAmount*>(CClientInstance::GetInstance()->Get_RootUI(TEXT("Amount")))->On_Panel();
}

void CUI_MainMenu::Off_Panel()
{
	if (!m_IsUpdate)
		return;

    static_cast<CAmount*>(CClientInstance::GetInstance()->Get_RootUI(TEXT("Amount")))->Off_Panel();
	if (m_eNextEvent == MENULIST::END)
	{
		m_eNextEvent = MENULIST::END;
		m_eAnimState = UIANIMSTATE::OFF;
		m_fAccTime = 1.f;
		m_pGameInstance->Change_InputType(INPUT_TYPE::GAMEPLAY);
	}
	else
	{
		m_IsUpdate = false;
		Next_Event();
	}
}

HRESULT CUI_MainMenu::Initialize_Prototype(_uint iLevel)
{
	m_iLevel = iLevel;
	CHECK_FAILED(Ready_Prototype(), E_FAIL);
	return S_OK;
}

HRESULT CUI_MainMenu::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;
	return S_OK;
}

void CUI_MainMenu::Priority_Update(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down(DIK_ESCAPE))
		On_Panel();
	else if (m_pGameInstance->Key_Down(DIK_ESCAPE, INPUT_TYPE::UI))
		Off_Panel();
	if (!m_IsUpdate)
		return;

	UI_Animation(fTimeDelta);
	__super::Priority_Update(fTimeDelta);
}

void CUI_MainMenu::Update(_float fTimeDelta)
{
	if (!m_IsUpdate)
		return;

	_bool isKeyInput = false;
	if (m_pGameInstance->Key_Down(DIK_W, INPUT_TYPE::UI))
	{
		m_iSeleteIndex -= 1;
		isKeyInput = true;

		if (m_iSeleteIndex < 0)
			m_iSeleteIndex = ENUM_CLASS(MENULIST::END) - 1;
	}
	else if (m_pGameInstance->Key_Down(DIK_S, INPUT_TYPE::UI))
	{
		m_iSeleteIndex += 1;
		isKeyInput = true;

		if (m_iSeleteIndex >= ENUM_CLASS(MENULIST::END))
			m_iSeleteIndex =0;
	}

	if(isKeyInput)
		for (_int i = 0; i < ENUM_CLASS(MENULIST::END); ++i)
			i == m_iSeleteIndex ? m_pList[i]->Set_Selete(true) : m_pList[i]->Set_Selete(false);


	__super::Update(fTimeDelta);

}

void CUI_MainMenu::Late_Update(_float fTimeDelta)
{
	if (!m_IsUpdate)
		return;

	__super::Late_Update(fTimeDelta);

}

HRESULT CUI_MainMenu::Render()
{
	return S_OK;
}

HRESULT CUI_MainMenu::Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg)
{
	m_szName = pInData.value("name", "");
	string strTexType = pInData.value("TexType", "");

	string szType = pInData.value("type", "");
	m_iUIType = CClientInstance::GetInstance()->UIType_StringToEnum(szType);

	m_iTexPass = pInData.value("TexIndex", 0);
	m_iShaderPass = pInData.value("shaderPass", 0);

	m_fDepth = pInData.value("depth", 0.f);

	if (pInData.contains("LocalPos"))
	{
		m_vLocalPos.x = pInData["LocalPos"].value("x", 0.f);
		m_vLocalPos.y = pInData["LocalPos"].value("y", 0.f);
	}

	if (pInData.contains("LocalSize"))
	{
		m_vLocalSize.x = pInData["LocalSize"].value("x", 0.f);
		m_vLocalSize.y = pInData["LocalSize"].value("y", 0.f);
	}

	if (pInData.contains("Angle"))
	{
		m_vAngle.x = pInData["Angle"].value("x", 0.f);
		m_vAngle.y = pInData["Angle"].value("y", 0.f);
		m_vAngle.z = pInData["Angle"].value("z", 0.f);
	}

	if (pInData.contains("Color"))
	{
		m_vColor.x = pInData["Color"].value("x", 0.f);
		m_vColor.y = pInData["Color"].value("y", 0.f);
		m_vColor.z = pInData["Color"].value("z", 0.f);
		m_vColor.w = pInData["Color"].value("w", 0.f);
	}

	if (pInData.contains("UV"))
	{
		m_vUV.clear();
		for (auto& uv : pInData["UV"])
		{
			_float4 uvData;
			uvData.x = uv.value("MinX", 0.f);
			uvData.y = uv.value("MinY", 0.f);
			uvData.z = uv.value("MaxX", 0.f);
			uvData.w = uv.value("MaxY", 0.f);
			m_vUV.push_back(uvData);
		}
	}
	if (pInData.contains("Anime"))
	{
		m_Track.clear();
		for (auto& t : pInData["Anime"])
		{
			UIKEYFRAME track;
			track.fTrackPosition = t.value("TrackPosition", 0.f);
			track.fAlpha = t.value("Alpha", 1.f);
			track.fAngle = t.value("Angle", 0.f);
			track.fSize = t.value("Size", 1.f);
			track.szEvent = t.value("Event", "");

			if (t.contains("Transloation"))
			{
				track.vTransloation.x = t["Transloation"].value("x", 0.f);
				track.vTransloation.y = t["Transloation"].value("y", 0.f);
			}

			m_Track.push_back(track);
		}
	}

	m_pTransformCom->Scale(_float3{ m_vLocalSize.x, m_vLocalSize.y, 1.f });
	__super::Update_Rotation(0.f);

	if (pInData.contains("Children"))
	{
		for (auto& child : pInData["Children"])
		{
			string strChildName = child.value("name", "");

			if (strChildName != "Menu_List")
			{
				string strClass = child.value("class", "");
				_wstring wstrClass = AnsiToWString(strClass);

				CUIObject::UIOBJECT_DESC UIDesc{};
				UIDesc.szName = "";
				UIDesc.iUIType = 0;
				UIDesc.vLocalSize = { 1.f, 1.f };
				UIDesc.fDepth = 0;
				UIDesc.vLocalPos = { g_iWinSizeX >> 1 , g_iWinSizeY >> 1 };

				CUIObject* pChild = static_cast<CUIObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, iPrototypeLevelID, wstrClass.c_str(), &UIDesc));

				if (pChild == nullptr)
				{
					MSG_BOX(TEXT("자식 클론 생성 실패"));
					return E_FAIL;
				}
				if (pChild->Load_UI(child, iPrototypeLevelID, pArg))
					return E_FAIL;

				pChild->Insert_Bubble([this](BUBBLEEVENT* pArg) {this->Bubble_EventCall(pArg); });
				m_Children.push_back(pChild);
			}
			else
			{

				_float2 vPos = { child["LocalPos"].value("x", 0.f), child["LocalPos"].value("y", 0.f) };
				for (_int i = 0; i < ENUM_CLASS(MENULIST::END); ++i)
				{
					string strClass = child.value("class", "");
					_wstring wstrClass = AnsiToWString(strClass);

					CMainMenu_List::MAINMENULIST_DESC UIDesc{};
					UIDesc.szName = "";
					UIDesc.iUIType = 0;
					UIDesc.vLocalSize = { 1.f, 1.f };
					UIDesc.fDepth = 0;
					UIDesc.vLocalPos = { g_iWinSizeX >> 1 , g_iWinSizeY >> 1 };
					UIDesc.eMenu = static_cast<MENULIST>(i);

					CMainMenu_List* pChild = static_cast<CMainMenu_List*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, iPrototypeLevelID, wstrClass.c_str(), &UIDesc));

					if (pChild == nullptr)
					{
						MSG_BOX(TEXT("자식 클론 생성 실패"));
						return E_FAIL;
					}
					if (pChild->Load_UI(child, iPrototypeLevelID, pArg))
						return E_FAIL;

					pChild->Insert_Bubble([this](BUBBLEEVENT* pArg) {this->Bubble_EventCall(pArg); });
					m_Children.push_back(pChild);

					pChild->Update_Pos(i, vPos, 70.f);
					m_pList.push_back(pChild);
					Safe_AddRef(pChild);
				}
			}
		}
	}

	__super::Update_Transform(nullptr, m_vLocalPos);

	CHECK_FAILED(Ready_Object(), E_FAIL);
	return S_OK;
}

void CUI_MainMenu::Bubble_EventCall(BUBBLEEVENT* pArg)
{
	MAINMENUBUBBLE_DESC* Desc = static_cast<MAINMENUBUBBLE_DESC*>(pArg);
	if (Desc->isClick)
	{
		m_eNextEvent = Desc->eListType;

		Off_Panel();
	}
	else
	{
		m_iSeleteIndex = ENUM_CLASS(Desc->eListType);
		for (_int i = 0; i < ENUM_CLASS(MENULIST::END); ++i)
			i == m_iSeleteIndex ? m_pList[i]->Set_Selete(true) : m_pList[i]->Set_Selete(false);
		
	}
}

HRESULT CUI_MainMenu::Update_Switch(void* pArg)
{
	m_IsUpdate ? Off_Panel() :On_Panel();
	return S_OK;
}


HRESULT CUI_MainMenu::Ready_Prototype()
{
	CHECK_FAILED(m_pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_UI_MainMenu_List"),
		CMainMenu_List::Create(m_pDevice, m_pContext, m_iLevel)), E_FAIL);

	return S_OK;
}

HRESULT CUI_MainMenu::Ready_Object()
{
	UIOBJECT_DESC Desc = {};
	Desc.fDepth = 5.5f;
	Desc.iUIType = ENUM_CLASS(UITYPE::TEXTURE);
	Desc.szName = "BackGround";
	Desc.vLocalSize = { g_iWinSizeX, g_iWinSizeY };
	Desc.vLocalPos = { g_iWinSizeX >> 1, g_iWinSizeY >> 1 };

	m_pBackGround = static_cast<CUI_BackGround*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_BackGround"), &Desc));
	if (m_pBackGround == nullptr)
		return E_FAIL;
	m_pBackGround->Setting_BG(CUI_BackGround::UIBGTYPE::MAIN);
	m_Children.push_back(m_pBackGround);
	Safe_AddRef(m_pBackGround);

	return S_OK;
}

void CUI_MainMenu::UI_Animation(_float fTimeDelta)
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
		for (auto pList : m_pList)
			pList->OnAnime(m_fAccTime, 100.f, this);
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

void CUI_MainMenu::Next_Event()
{

	if (m_eNextEvent == MENULIST::WEAPON)
	{
		CUI_Inven::INVEN_ONOFF_DESC Desc = {};
		Desc.isOpen = true;
        Desc.eState = CUI_Inven::INVEN_STATE::EQUIP;
		Desc.szName = m_szName;
		CClientInstance::GetInstance()->UI_UpdateSwitch(TEXT("Inven"), &Desc);
	}
	else if (m_eNextEvent == MENULIST::OTHER)
	{
		CUI_Inven::INVEN_ONOFF_DESC Desc = {};
		Desc.isOpen = true;
        Desc.eState = CUI_Inven::INVEN_STATE::DEFAULT;
        Desc.szName = m_szName;
		CClientInstance::GetInstance()->UI_UpdateSwitch(TEXT("Inven"), &Desc);
	}
	else if (m_eNextEvent == MENULIST::SKILL)
	{
        static_cast<CUI_SkillTree*>(CClientInstance::GetInstance()->Get_RootUI(TEXT("Skill")))->On_Panel();
	}
	else if (m_eNextEvent == MENULIST::STATE)
	{
		CUI_State::UI_STATE_ON Desc = {};
		Desc.eType = CUI_State::UI_TYPE::DEFAULT;

		CClientInstance::GetInstance()->UI_UpdateSwitch(TEXT("State"), &Desc);
	}
	else if (m_eNextEvent == MENULIST::TITLE)
	{
        m_eNextEvent = MENULIST::END;
        m_eAnimState = UIANIMSTATE::OFF;
        m_fAccTime = 1.f;

        m_pGameInstance->Change_InputType(INPUT_TYPE::GAMEPLAY);
	}
	else if (m_eNextEvent == MENULIST::EXIT)
	{
        m_eNextEvent = MENULIST::END;
        m_eAnimState = UIANIMSTATE::OFF;
        m_fAccTime = 1.f;

        m_pGameInstance->Change_InputType(INPUT_TYPE::GAMEPLAY);
	}
}

CUI_MainMenu* CUI_MainMenu::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CUI_MainMenu* pInstance = new CUI_MainMenu(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX(TEXT("Failed Created : CUI_MainMenu"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_MainMenu::Clone(void* pArg)
{
	CUI_MainMenu* pInstance = new CUI_MainMenu(*this);
	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CUI_MainMenu"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_MainMenu::Free()
{
	__super::Free();
	for (auto pList : m_pList)
		Safe_Release(pList);
	m_pList.clear();

	Safe_Release(m_pBackGround);
}
