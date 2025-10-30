#include "UI_BladeNexus.h"
#include "GameInstance.h"
#include "ClientInstance.h"

#include "UI_TextBox.h"
#include "UI_BackGround.h"

#include "BladeNexus_List.h"
#include "MainMenu_Deco.h"

#include "UI_Inven.h"

CUI_BladeNexus::CUI_BladeNexus(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Panel{ pDevice, pContext }
{
}

CUI_BladeNexus::CUI_BladeNexus(const CUI_BladeNexus& Prototype)
	: CUI_Panel(Prototype)
{
}

void CUI_BladeNexus::On_Panel(ONTYPE eType, _wstring strMapName)
{
	if (m_IsUpdate)
		return;

	m_pText[1]->Set_Text(strMapName);

	m_iListeType = ENUM_CLASS(eType);
	m_iSeleteIndex = 0;
	for (_int i = 0; i < ENUM_CLASS(MENULIST::END); ++i)
		i == m_iSeleteIndex ? m_pList[i]->Set_Selete(true) : m_pList[i]->Set_Selete(false);

	m_eAnimState = UIANIMSTATE::ON;
	m_fAccTime = 0.5f;
	m_IsUpdate = true;
	m_eNextEvent = MENULIST::END;
}


void CUI_BladeNexus::Off_Panel()
{
	if (!m_IsUpdate)
		return;
	if (m_eNextEvent == MENULIST::END)
	{
		m_eNextEvent = MENULIST::END;
		m_eAnimState = UIANIMSTATE::OFF;
		m_fAccTime = 1.f;
	}
	else
	{
		m_IsUpdate = false;
		Next_Event();
	}
}

HRESULT CUI_BladeNexus::Initialize_Prototype(_uint iLevel)
{
	m_iLevel = iLevel;
	CHECK_FAILED(Ready_Prototype(), E_FAIL);
	return S_OK;
}

HRESULT CUI_BladeNexus::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;
	return S_OK;
}

void CUI_BladeNexus::Priority_Update(_float fTimeDelta)
{

	if (m_pGameInstance->Key_Down(DIK_7) && m_eNextEvent == MENULIST::END)
		m_IsUpdate ? Off_Panel() : On_Panel(ONTYPE::DEFAULT, TEXT("황야 벌판 어딘가"));

	if (m_pGameInstance->Key_Down(DIK_6) && m_eNextEvent == MENULIST::END)
		m_IsUpdate ? Off_Panel() : On_Panel(ONTYPE::CREVICE, TEXT("경계의 틈"));

	if (!m_IsUpdate)
		return;

	UI_Animation(fTimeDelta);
	m_pBackGround->Priority_Update(fTimeDelta);
	for (_int i = 0; i < m_iListeType; ++i)
		m_pList[i]->Priority_Update(fTimeDelta);

	for (auto pText : m_pText)
		pText->Priority_Update(fTimeDelta);
}

void CUI_BladeNexus::Update(_float fTimeDelta)
{
	if (!m_IsUpdate)
		return;

	m_pBackGround->Update(fTimeDelta);
	for (_int i = 0; i < m_iListeType; ++i)
		m_pList[i]->Update(fTimeDelta);

	for (auto pText : m_pText)
		pText->Update(fTimeDelta);
}

void CUI_BladeNexus::Late_Update(_float fTimeDelta)
{
	if (!m_IsUpdate)
		return;

	m_pBackGround->Late_Update(fTimeDelta);
	for (_int i = 0; i < m_iListeType; ++i)
		m_pList[i]->Late_Update(fTimeDelta);

	for (auto pText : m_pText)
		pText->Late_Update(fTimeDelta);
}

HRESULT CUI_BladeNexus::Render()
{
	return S_OK;
}

HRESULT CUI_BladeNexus::Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg)
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
	m_pText.resize(2);

	if (pInData.contains("Children"))
	{
		for (auto& child : pInData["Children"])
		{
			string strChildName = child.value("name", "");

			if (strChildName == "Munu_Name")
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

				m_pText[0] = static_cast<CUI_TextBox*>(pChild);
				Safe_AddRef(pChild);
			}
			else if (strChildName == "Map_Name")
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

				m_pText[1] = static_cast<CUI_TextBox*>(pChild);
				Safe_AddRef(pChild);
			}
			else
			{
				_float2 vPos = { child["LocalPos"].value("x", 0.f), child["LocalPos"].value("y", 0.f) };
				for (_int i = 0; i < ENUM_CLASS(MENULIST::END); ++i)
				{
					string strClass = child.value("class", "");
					_wstring wstrClass = AnsiToWString(strClass);

					CBladeNexus_List::MAINMENULIST_DESC UIDesc{};
					UIDesc.szName = "";
					UIDesc.iUIType = 0;
					UIDesc.vLocalSize = { 1.f, 1.f };
					UIDesc.fDepth = 0;
					UIDesc.vLocalPos = { g_iWinSizeX >> 1 , g_iWinSizeY >> 1 };
					UIDesc.eMenu = static_cast<MENULIST>(i);

					CBladeNexus_List* pChild = static_cast<CBladeNexus_List*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, iPrototypeLevelID, wstrClass.c_str(), &UIDesc));

					if (pChild == nullptr)
					{
						MSG_BOX(TEXT("자식 클론 생성 실패"));
						return E_FAIL;
					}
					if (pChild->Load_UI(child, iPrototypeLevelID, pArg))
						return E_FAIL;

					pChild->Insert_Bubble([this](BUBBLEEVENT* pArg) {this->Bubble_EventCall(pArg); });
					m_Children.push_back(pChild);

					pChild->Update_Pos(i, vPos, 85.f);
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

void CUI_BladeNexus::Bubble_EventCall(BUBBLEEVENT* pArg)
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

HRESULT CUI_BladeNexus::Ready_Prototype()
{
	CHECK_FAILED(m_pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_UI_BladeNexus_List"),
		CBladeNexus_List::Create(m_pDevice, m_pContext, m_iLevel)), E_FAIL);

	return S_OK;
}

HRESULT CUI_BladeNexus::Ready_Object()
{
	UIOBJECT_DESC Desc = {};
	Desc.fDepth = 4;
	Desc.iUIType = ENUM_CLASS(UITYPE::TEXTURE);
	Desc.szName = "BackGround";
	Desc.vLocalSize = { g_iWinSizeX, g_iWinSizeY };
	Desc.vLocalPos = { g_iWinSizeX >> 1, g_iWinSizeY >> 1 };

	m_pBackGround = static_cast<CUI_BackGround*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_BackGround"), &Desc));
	if (m_pBackGround == nullptr)
		return E_FAIL;
	m_pBackGround->Setting_BG(CUI_BackGround::UIBGTYPE::BLADENEXUS);
	m_Children.push_back(m_pBackGround);
	Safe_AddRef(m_pBackGround);

	return S_OK;
}

void CUI_BladeNexus::UI_Animation(_float fTimeDelta)
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

void CUI_BladeNexus::Next_Event()
{

	if (m_eNextEvent == MENULIST::STATE)
	{
		//CUI_Inven::INVEN_ONOFF_DESC Desc = {};
		//Desc.isOpen = true;
		//Desc.isEquip = true;
		//Desc.szName = m_szName;
		//CClientInstance::GetInstance()->UI_UpdateSwitch(TEXT("Inven"), &Desc);
	}
	else if (m_eNextEvent == MENULIST::WARP)
	{

	}
	else if (m_eNextEvent == MENULIST::CREVICE)
	{
	}

}

CUI_BladeNexus* CUI_BladeNexus::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CUI_BladeNexus* pInstance = new CUI_BladeNexus(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX(TEXT("Failed Created : CUI_BladeNexus"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_BladeNexus::Clone(void* pArg)
{
	CUI_BladeNexus* pInstance = new CUI_BladeNexus(*this);
	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CUI_BladeNexus"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_BladeNexus::Free()
{
	__super::Free();
	for (auto pList : m_pList)
		Safe_Release(pList);
	m_pList.clear();

	for (auto pText : m_pText)
		Safe_Release(pText);
	m_pText.clear();

	Safe_Release(m_pBackGround);
}
