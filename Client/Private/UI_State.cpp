#include "UI_State.h"
#include "GameInstance.h"
#include "ClientInstance.h"

#include "UI_TextBox.h"
#include "UI_BackGround.h"

#include "UI_State_Panel.h"
#include "UI_State_Button.h"
#include "UI_State_List.h"

CUI_State::CUI_State(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Panel{ pDevice, pContext }
{
}

CUI_State::CUI_State(const CUI_State& Prototype)
	: CUI_Panel(Prototype)
{
}

void CUI_State::On_Panel()
{
	if (m_IsUpdate)
		return;

	m_eAnimState = UIANIMSTATE::ON;
	m_fAccTime = 0.5f;
	m_IsUpdate = true;
}

void CUI_State::Off_Panel()
{
	if (!m_IsUpdate)
		return;

	m_eAnimState = UIANIMSTATE::OFF;
	m_fAccTime = 1.f;

}

HRESULT CUI_State::Initialize_Prototype(_uint iLevel)
{
	m_iLevel = iLevel;
	CHECK_FAILED(Ready_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CUI_State::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;
	return S_OK;
}

void CUI_State::Priority_Update(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down(DIK_8))
		Update_Switch(nullptr);

	if (!m_IsUpdate)
		return;

	UI_Animation(fTimeDelta);
	__super::Priority_Update(fTimeDelta);
}

void CUI_State::Update(_float fTimeDelta)
{
	if (!m_IsUpdate)
		return;

	__super::Update(fTimeDelta);

}

void CUI_State::Late_Update(_float fTimeDelta)
{
	if (!m_IsUpdate)
		return;

	__super::Late_Update(fTimeDelta);

}

HRESULT CUI_State::Render()
{
	return S_OK;
}

HRESULT CUI_State::Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg)
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
	
	CUIObject::UIOBJECT_DESC UIDesc{};
	UIDesc.szName = "";
	UIDesc.iUIType = 0;
	UIDesc.vLocalSize = { 1.f, 1.f };
	UIDesc.fDepth = 0;
	UIDesc.vLocalPos = { g_iWinSizeX >> 1 , g_iWinSizeY >> 1 };

	if (pInData.contains("Children"))
	{
		m_pPanel.resize(ENUM_CLASS(STATE_PANEL::END));
		for (auto& child : pInData["Children"])
		{
			string strName = child.value("name", "");
			if (strName == "State_List")
			{
				_float2 vPos = { child["LocalPos"].value("x", 0.f), child["LocalPos"].value("y", 0.f) };

				for (_int i = 0; i < ENUM_CLASS(STATE_LIST::END); ++i)
				{
					string strClass = child.value("class", "");
					CUI_State_List* pChild = static_cast<CUI_State_List*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, iPrototypeLevelID, AnsiToWString(strClass).c_str(), &UIDesc));

					CHECK_NULLPTR_MSG(pChild, TEXT("자식 클론 생성 실패"), E_FAIL);
					CHECK_FAILED(pChild->Load_UI(child, iPrototypeLevelID, pArg), E_FAIL);

					pChild->Insert_Bubble([this](BUBBLEEVENT* pArg) {this->Bubble_EventCall(pArg); });
					m_Children.push_back(pChild);

					m_pState.push_back(pChild);

					Safe_AddRef(pChild);
					pChild->Update_Pos(i, vPos, 65.f);
				}
			}
			else
			{
				string strClass = child.value("class", "");
				CUIObject* pChild = static_cast<CUIObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, iPrototypeLevelID, AnsiToWString(strClass).c_str(), &UIDesc));

				CHECK_NULLPTR_MSG(pChild, TEXT("자식 클론 생성 실패"), E_FAIL);
				CHECK_FAILED(pChild->Load_UI(child, iPrototypeLevelID, pArg), E_FAIL);

				pChild->Insert_Bubble([this](BUBBLEEVENT* pArg) {this->Bubble_EventCall(pArg); });
				m_Children.push_back(pChild);


				if (strName == "State_Name")
				{
					m_pTitle = static_cast<CUI_TextBox*>(pChild);
					Safe_AddRef(pChild);
				}
				else if (strName == "Level")
				{
					m_pPanel[ENUM_CLASS(STATE_PANEL::LEVEL)] = static_cast<CUI_State_Panel*>(pChild);
					Safe_AddRef(pChild);
				}
				else if (strName == "Lachryma")
				{
					m_pPanel[ENUM_CLASS(STATE_PANEL::LACHRYMA)] = static_cast<CUI_State_Panel*>(pChild);
					Safe_AddRef(pChild);
				}
				else if (strName == "DefaultState")
				{
					m_pPanel[ENUM_CLASS(STATE_PANEL::DEFAULT_STATE)] = static_cast<CUI_State_Panel*>(pChild);
					Safe_AddRef(pChild);
				}
				else if (strName == "AddState")
				{
					m_pPanel[ENUM_CLASS(STATE_PANEL::ADD_STATE)] = static_cast<CUI_State_Panel*>(pChild);
					Safe_AddRef(pChild);
				}
				else if (strName == "Elemental")
				{
					m_pPanel[ENUM_CLASS(STATE_PANEL::ELEMENTAL)] = static_cast<CUI_State_Panel*>(pChild);
					Safe_AddRef(pChild);
				}
				else if (strName == "State_Button")
				{
					m_pUpButton = static_cast<CUI_State_List*>(pChild);
					Safe_AddRef(pChild);
				}
			}
		}
	}

	__super::Update_Transform(nullptr, m_vLocalPos);

	CHECK_FAILED(Ready_Object(), E_FAIL);
	return S_OK;
}

HRESULT CUI_State::Update_Switch(void* pArg)
{
	m_IsUpdate ? Off_Panel() : On_Panel();
	return S_OK;
}


HRESULT CUI_State::Ready_Prototype()
{
	CHECK_FAILED(m_pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_UI_State_Panel"),
		CUI_State_Panel::Create(m_pDevice, m_pContext, m_iLevel)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_UI_State_List"),
		CUI_State_List::Create(m_pDevice, m_pContext, m_iLevel)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_UI_State_Button"),
		CUI_State_Button::Create(m_pDevice, m_pContext, m_iLevel)), E_FAIL);


	if (FAILED(m_pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_Component_UI_State_Arrow"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("..//Bin/Resources/UI/State/ArrowButton_UI_%d.png"), 2))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_Component_UI_State_Button"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("..//Bin/Resources/UI/State/NormalButton_%d.png"), 7))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_State::Ready_Object()
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
	m_pBackGround->Setting_BG(CUI_BackGround::UIBGTYPE::STATE);
	m_Children.push_back(m_pBackGround);
	Safe_AddRef(m_pBackGround);

	return S_OK;
}

void CUI_State::UI_Animation(_float fTimeDelta)
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

CUI_State* CUI_State::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CUI_State* pInstance = new CUI_State(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX(TEXT("Failed Created : CUI_State"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_State::Clone(void* pArg)
{
	CUI_State* pInstance = new CUI_State(*this);
	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CUI_State"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_State::Free()
{
	__super::Free();
	for (auto pList : m_pState)
		Safe_Release(pList);
	m_pState.clear();

	for (auto pPanel : m_pPanel)
		Safe_Release(pPanel);
	m_pPanel.clear();

	Safe_Release(m_pUpButton);
	Safe_Release(m_pBackGround);
	Safe_Release(m_pTitle);
}
