#include "UI_State.h"
#include "GameInstance.h"
#include "ClientInstance.h"

#include "UI_TextBox.h"
#include "UI_BackGround.h"

#include "UI_State_MainPanel.h"
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

void CUI_State::On_Panel(UI_TYPE eType)
{
	if (m_IsUpdate)
		return;

	m_eType = eType;
	m_eType == UI_TYPE::DEFAULT ? m_pTitle->Set_Text(TEXT("상태")) : m_pTitle->Set_Text(TEXT("능력 강화"));

	m_pPanel[ENUM_CLASS(STATE_PANEL::LACHRYMA)]->Setting_Type(m_eType, this);
	m_pPanel[ENUM_CLASS(STATE_PANEL::LEVEL)]->Setting_Type(m_eType, this);

	_int iLacryma = m_iLachryma;
	_int iPreUpPoint = m_Player_Data.iLevel;
	_int iUpPoint = 0;
	while (iLacryma > (300 + (iPreUpPoint - 1 + iUpPoint) * 280))
	{
		iLacryma = iLacryma - (300 + (iPreUpPoint - 1  + iUpPoint) * 280);
		iUpPoint++;
	}

	m_Player_Data.iUPPoint = iUpPoint;
	m_Player_Data.iLachryma = m_iLachryma;
	m_Player_Data.iUpLachryma = (300 + (m_Player_Data.iLevel - 1 + m_UpPlayer_Data.iLevel) * 280);

	for (auto pList : m_pState)
		pList->Setting_Type(m_eType);
	m_eAnimState = UIANIMSTATE::ON;
	m_fAccTime = 0.5f;
	m_IsUpdate = true;
}


void CUI_State::Off_Panel()
{
	if (!m_IsUpdate)
		return;

	m_IsUpdate = false;
	//m_UpPlayer_Data = {};
	//for (_int i = 0; i < ENUM_CLASS(STATE_LIST::END); ++i)
	//	m_UpStateLevel[i] = 0;
	//	
	//m_eAnimState = UIANIMSTATE::OFF;
	//m_fAccTime = 1.f;

	if(m_eType == UI_TYPE::DEFAULT)
		CClientInstance::GetInstance()->UI_UpdateSwitch(TEXT("MainMeun"));
	else if (m_eType == UI_TYPE::UPAGERD)
		CClientInstance::GetInstance()->UI_UpdateSwitch(TEXT("MainMeun"));
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

	Ready_PlayerData();

	return S_OK;
}

void CUI_State::Priority_Update(_float fTimeDelta)
{
	if (!m_IsUpdate)
		return;
	if (m_pGameInstance->Key_Down(DIK_ESCAPE))
		Off_Panel();

	UI_Animation(fTimeDelta);
	__super::Priority_Update(fTimeDelta);
}

void CUI_State::Update(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down(DIK_M))
		m_iLachryma += 10000;

	if (!m_IsUpdate)
		return;

	__super::Update(fTimeDelta);

}

void CUI_State::Late_Update(_float fTimeDelta)
{
	if (!m_IsUpdate)
		return;

	m_pBackGround->Late_Update(fTimeDelta);
	m_pTitle->Late_Update(fTimeDelta);

	for (auto pChild : m_pState)
		pChild->Late_Update(fTimeDelta);
	for (auto pChild : m_pPanel)
		pChild->Late_Update(fTimeDelta);

	if (UI_TYPE::UPAGERD == m_eType)
		m_pUpButton->Late_Update(fTimeDelta);
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
					pChild->Setting_List(i, vPos, 65.f, &m_CulStateLevel[i], &m_UpStateLevel[i], &m_Player_Data.iUPPoint);
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
					m_pPanel[ENUM_CLASS(STATE_PANEL::LEVEL)] = static_cast<CUI_State_MainPanel*>(pChild);
					Safe_AddRef(pChild);
					m_pPanel[ENUM_CLASS(STATE_PANEL::LEVEL)]->Setting_PanelLevel(ENUM_CLASS(STATE_PANEL::LEVEL), &m_Player_Data, &m_UpPlayer_Data);
				}
				else if (strName == "Lachryma")
				{
					m_pPanel[ENUM_CLASS(STATE_PANEL::LACHRYMA)] = static_cast<CUI_State_MainPanel*>(pChild);
					Safe_AddRef(pChild);
					m_pPanel[ENUM_CLASS(STATE_PANEL::LACHRYMA)]->Setting_PanelLevel(ENUM_CLASS(STATE_PANEL::LACHRYMA), &m_Player_Data, &m_UpPlayer_Data);
				}
				else if (strName == "DefaultState")
				{
					m_pPanel[ENUM_CLASS(STATE_PANEL::DEFAULT_STATE)] = static_cast<CUI_State_MainPanel*>(pChild);
					Safe_AddRef(pChild);
					m_pPanel[ENUM_CLASS(STATE_PANEL::DEFAULT_STATE)]->Setting_PanelLevel(ENUM_CLASS(STATE_PANEL::DEFAULT_STATE), &m_Player_Data, &m_UpPlayer_Data);
				}
				else if (strName == "AddState")
				{
					m_pPanel[ENUM_CLASS(STATE_PANEL::ADD_STATE)] = static_cast<CUI_State_MainPanel*>(pChild);
					Safe_AddRef(pChild);
					m_pPanel[ENUM_CLASS(STATE_PANEL::ADD_STATE)]->Setting_PanelLevel(ENUM_CLASS(STATE_PANEL::ADD_STATE), &m_Player_Data, &m_UpPlayer_Data);
				}
				else if (strName == "Elemental")
				{
					m_pPanel[ENUM_CLASS(STATE_PANEL::ELEMENTAL)] = static_cast<CUI_State_MainPanel*>(pChild);
					Safe_AddRef(pChild);
					m_pPanel[ENUM_CLASS(STATE_PANEL::ELEMENTAL)]->Setting_PanelLevel(ENUM_CLASS(STATE_PANEL::ELEMENTAL), &m_Player_Data, &m_UpPlayer_Data);
				}
				else if (strName == "State_Button")
				{
					m_pUpButton = static_cast<CUI_State_List*>(pChild);
					Safe_AddRef(pChild);
					m_pUpButton->Setting_Button(&m_UpPlayer_Data.iLevel);
				}
			}
		}
	}

	__super::Update_Transform(nullptr, m_vLocalPos);

	CHECK_FAILED(Ready_Object(), E_FAIL);
	CHECK_FAILED(Ready_UISetting(), E_FAIL);

	return S_OK;
}

void CUI_State::Bubble_EventCall(BUBBLEEVENT* pArg)
{
	UI_STATE_BUBLLE* pDesc = static_cast<UI_STATE_BUBLLE*>(pArg);

	if (pDesc->isList)
		List_Bubble_Event(pDesc);
	else
		Button_Bubble_Event(pDesc);
}

HRESULT CUI_State::Update_Switch(void* pArg)
{
	if (m_IsUpdate)
		Off_Panel();
	else
	{
		UI_StateONDesc* pDesc = static_cast<UI_StateONDesc*>(pArg);
		On_Panel(pDesc->eType);
	}
	return S_OK;
}

HRESULT CUI_State::Ready_Prototype()
{
	CHECK_FAILED(m_pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_UI_State_MainPanel"),
		CUI_State_MainPanel::Create(m_pDevice, m_pContext, m_iLevel)), E_FAIL);

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
		CTexture::Create(m_pDevice, m_pContext, TEXT("..//Bin/Resources/UI/State/NormalButton_%d.png"), 10))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_State::Ready_Object()
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
	m_pBackGround->Setting_BG(CUI_BackGround::UIBGTYPE::STATE);
	m_Children.push_back(m_pBackGround);
	Safe_AddRef(m_pBackGround);

	return S_OK;
}

void CUI_State::Ready_PlayerData()
{
	m_iLachryma									= 3000;
	//플레이어 기본 정보
	m_Player_Data.iLevel						= 3;
	m_Player_Data.iUPPoint						= 0;
	m_Player_Data.iUpLachryma					= 0;
	m_Player_Data.iLachryma						= 1;
	
	m_Player_Data.iMaxHp						= 100;
	m_Player_Data.iMaxStamina					= 100;
	m_Player_Data.iAtk							= 1111;
	m_Player_Data.iDef							= 934;
	m_Player_Data.fWeight						= 15.f;
	m_Player_Data.fMaxWeight					= 35.f;
	m_Player_Data.fAgile						= 100.f;
	
	m_Player_Data.fStaminaAttack				= 50.f;
	m_Player_Data.fStaminaRegen					= 17.1f;
	m_Player_Data.fEvasion_StaminaDown			= 10.7f;
	m_Player_Data.fDamage_StaminaDown			= 0.0f;
	m_Player_Data.fGuard_StaminaDown			= 15.0f;
	
	m_Player_Data.iFire							= 500;
	m_Player_Data.iWater						= 500;
	m_Player_Data.iLightning					= 500;
	m_Player_Data.iEarth						= 500;
	m_Player_Data.iChaos						= 500;
	m_Player_Data.iDisease						= 500;
	m_Player_Data.iPoison						= 500;

	//플레이어 스텟업 정보
	m_UpPlayer_Data.iMaxHp						= 0;
	m_UpPlayer_Data.iMaxStamina					= 0;
	m_UpPlayer_Data.iAtk						= 0;
	m_UpPlayer_Data.iDef						= 0;
	m_UpPlayer_Data.fWeight						= 0.f;
	m_UpPlayer_Data.fMaxWeight					= 0.f;
	m_UpPlayer_Data.fAgile						= 0.f;
												  
	m_UpPlayer_Data.fStaminaAttack				= 0.f;
	m_UpPlayer_Data.fStaminaRegen				= 0.f;
	m_UpPlayer_Data.fEvasion_StaminaDown		= 0.f;
	m_UpPlayer_Data.fDamage_StaminaDown			= 0.f;
	m_UpPlayer_Data.fGuard_StaminaDown			= 0.f;
												 
	m_UpPlayer_Data.iFire						= 0;
	m_UpPlayer_Data.iWater						= 0;
	m_UpPlayer_Data.iLightning					= 0;
	m_UpPlayer_Data.iEarth						= 0;
	m_UpPlayer_Data.iChaos						= 0;
	m_UpPlayer_Data.iDisease					= 0;
	m_UpPlayer_Data.iPoison						= 0;

	m_UpPlayer_Data.iLevel						= 0;
	m_UpPlayer_Data.iUPPoint					= 0;
	m_UpPlayer_Data.iUpLachryma					= 0;
	m_UpPlayer_Data.iLachryma					= 0;
	
	//스텟창 레벨
	m_CulStateLevel.resize(ENUM_CLASS(STATE_LIST::END));
	m_UpStateLevel.resize(ENUM_CLASS(STATE_LIST::END));

	m_CulStateLevel[ENUM_CLASS(STATE_LIST::VITALITY)] = 10;
	m_CulStateLevel[ENUM_CLASS(STATE_LIST::ENDURANCE)] = 10;
	m_CulStateLevel[ENUM_CLASS(STATE_LIST::POWER)] = 10;
	m_CulStateLevel[ENUM_CLASS(STATE_LIST::COMPETENCY)] = 10;
	m_CulStateLevel[ENUM_CLASS(STATE_LIST::WILL)] = 10;

	m_UpStateLevel[ENUM_CLASS(STATE_LIST::VITALITY)] = 0;
	m_UpStateLevel[ENUM_CLASS(STATE_LIST::ENDURANCE)] = 0;
	m_UpStateLevel[ENUM_CLASS(STATE_LIST::POWER)] = 0;
	m_UpStateLevel[ENUM_CLASS(STATE_LIST::COMPETENCY)] = 0;
	m_UpStateLevel[ENUM_CLASS(STATE_LIST::WILL)] = 0;

}

HRESULT CUI_State::Ready_UISetting()
{
	//m_pPanel[ENUM_CLASS(STATE_PANEL::LEVEL)]
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

void CUI_State::List_Bubble_Event(UI_STATE_BUBLLE* pDesc)
{
	if (pDesc->isClick)
	{
		_int iType = ENUM_CLASS(pDesc->eListType);

		pair<_int, _float> Index[4];
		Index[0].first = CClientInstance::GetInstance()->Get_Data<STATE_DATA>(iType)->iType_1;
		Index[1].first = CClientInstance::GetInstance()->Get_Data<STATE_DATA>(iType)->iType_2;
		Index[2].first = CClientInstance::GetInstance()->Get_Data<STATE_DATA>(iType)->iType_3;
		Index[3].first = CClientInstance::GetInstance()->Get_Data<STATE_DATA>(iType)->iType_4;

		Index[0].second = CClientInstance::GetInstance()->Get_Data<STATE_DATA>(iType)->iValue_1;
		Index[1].second = CClientInstance::GetInstance()->Get_Data<STATE_DATA>(iType)->iValue_2;
		Index[2].second = CClientInstance::GetInstance()->Get_Data<STATE_DATA>(iType)->fValue_3;
		Index[3].second = CClientInstance::GetInstance()->Get_Data<STATE_DATA>(iType)->fValue_4;

		for (_int i = 0; i < 4; ++i)
		{
			switch (static_cast<PLAYTER_STATE>(Index[i].first))
			{
			case PLAYTER_STATE::MAXHP:
				pDesc->isUp ? m_UpPlayer_Data.iMaxHp += Index[i].second : m_UpPlayer_Data.iMaxHp -= Index[i].second;
				break;
			case PLAYTER_STATE::MAXSTAMINA:
				pDesc->isUp ? m_UpPlayer_Data.iMaxStamina += Index[i].second : m_UpPlayer_Data.iMaxStamina -= Index[i].second;
				break;
			case PLAYTER_STATE::ATK:
				pDesc->isUp ? m_UpPlayer_Data.iAtk += Index[i].second : m_UpPlayer_Data.iAtk -= Index[i].second;
				break;
			case PLAYTER_STATE::DEF:
				pDesc->isUp ? m_UpPlayer_Data.iDef += Index[i].second : m_UpPlayer_Data.iDef -= Index[i].second;
				break;
			case PLAYTER_STATE::WEIGHT:
				pDesc->isUp ? m_UpPlayer_Data.fMaxWeight += Index[i].second : m_UpPlayer_Data.fWeight -= Index[i].second;
				break;
			case PLAYTER_STATE::AGILE:
				pDesc->isUp ? m_UpPlayer_Data.fAgile += Index[i].second : m_UpPlayer_Data.fAgile -= Index[i].second;
				break;
			case PLAYTER_STATE::STAMINAATK:
				pDesc->isUp ? m_UpPlayer_Data.fStaminaAttack += Index[i].second : m_UpPlayer_Data.fStaminaAttack -= Index[i].second;
				break;
			case PLAYTER_STATE::STAMINAREGEN:
				pDesc->isUp ? m_UpPlayer_Data.fStaminaRegen += Index[i].second : m_UpPlayer_Data.fStaminaRegen -= Index[i].second;
				break;
			case PLAYTER_STATE::EVASION_STAMINADOWN:
				pDesc->isUp ? m_UpPlayer_Data.fEvasion_StaminaDown += Index[i].second : m_UpPlayer_Data.fEvasion_StaminaDown -= Index[i].second;
				break;
			case PLAYTER_STATE::DAMAGE_STAMINADOWN:
				pDesc->isUp ? m_UpPlayer_Data.fDamage_StaminaDown += Index[i].second : m_UpPlayer_Data.fDamage_StaminaDown -= Index[i].second;
				break;
			case PLAYTER_STATE::GUARD_STAMINADOWN:
				pDesc->isUp ? m_UpPlayer_Data.fGuard_StaminaDown += Index[i].second : m_UpPlayer_Data.fGuard_StaminaDown -= Index[i].second;
				break;
			case PLAYTER_STATE::FIRE:
				pDesc->isUp ? m_UpPlayer_Data.iFire += Index[i].second : m_UpPlayer_Data.iFire -= Index[i].second;
				break;
			case PLAYTER_STATE::WATER:
				pDesc->isUp ? m_UpPlayer_Data.iWater += Index[i].second : m_UpPlayer_Data.iWater -= Index[i].second;
				break;
			case PLAYTER_STATE::LIGHTNING:
				pDesc->isUp ? m_UpPlayer_Data.iLightning += Index[i].second : m_UpPlayer_Data.iLightning -= Index[i].second;
				break;
			case PLAYTER_STATE::EARTH:
				pDesc->isUp ? m_UpPlayer_Data.iEarth += Index[i].second : m_UpPlayer_Data.iEarth -= Index[i].second;
				break;
			case PLAYTER_STATE::CHAOS:
				pDesc->isUp ? m_UpPlayer_Data.iChaos += Index[i].second : m_UpPlayer_Data.iChaos -= Index[i].second;
				break;
			case PLAYTER_STATE::DISEASE:
				pDesc->isUp ? m_UpPlayer_Data.iDisease += Index[i].second : m_UpPlayer_Data.iDisease -= Index[i].second;
				break;
			case PLAYTER_STATE::POISON:
				pDesc->isUp ? m_UpPlayer_Data.iPoison += Index[i].second : m_UpPlayer_Data.iPoison -= Index[i].second;
				break;
			}
		}

		if (pDesc->isUp)
		{
			m_UpPlayer_Data.iLevel += 1;
			m_Player_Data.iLachryma = m_iLachryma - m_Player_Data.iUpLachryma;
			m_Player_Data.iUpLachryma += (300 + ((m_Player_Data.iLevel) - 1 + (m_UpPlayer_Data.iLevel)) * 280);
		}
		else
		{
			m_Player_Data.iUpLachryma -= (300 + ((m_Player_Data.iLevel) - 1 + (m_UpPlayer_Data.iLevel)) * 280);
			m_UpPlayer_Data.iLevel -= 1;
			m_Player_Data.iLachryma = m_Player_Data.iLachryma + m_Player_Data.iUpLachryma;
		}
	}
	else
	{
		_int iType = ENUM_CLASS(pDesc->eListType);

		pair<_int, _float> Index[4];
		Index[0].first = CClientInstance::GetInstance()->Get_Data<STATE_DATA>(iType)->iType_1;
		Index[1].first = CClientInstance::GetInstance()->Get_Data<STATE_DATA>(iType)->iType_2;
		Index[2].first = CClientInstance::GetInstance()->Get_Data<STATE_DATA>(iType)->iType_3;
		Index[3].first = CClientInstance::GetInstance()->Get_Data<STATE_DATA>(iType)->iType_4;

		Index[0].second = CClientInstance::GetInstance()->Get_Data<STATE_DATA>(iType)->iValue_1;
		Index[1].second = CClientInstance::GetInstance()->Get_Data<STATE_DATA>(iType)->iValue_2;
		Index[2].second = CClientInstance::GetInstance()->Get_Data<STATE_DATA>(iType)->fValue_3;
		Index[3].second = CClientInstance::GetInstance()->Get_Data<STATE_DATA>(iType)->fValue_4;

		for (_int i = 0; i < 4; ++i)
		{
			if (Index[i].first <= ENUM_CLASS(PLAYTER_STATE::AGILE))
				m_pPanel[ENUM_CLASS(STATE_PANEL::DEFAULT_STATE)]->State_Hover(Index[i]);
			else if (Index[i].first <= ENUM_CLASS(PLAYTER_STATE::GUARD_STAMINADOWN))
				m_pPanel[ENUM_CLASS(STATE_PANEL::ADD_STATE)]->State_Hover(Index[i]);
			else if (Index[i].first < ENUM_CLASS(PLAYTER_STATE::END))
				m_pPanel[ENUM_CLASS(STATE_PANEL::ELEMENTAL)]->State_Hover(Index[i]);
		}
	}
}

void CUI_State::Button_Bubble_Event(UI_STATE_BUBLLE* pDesc)
{
	m_Player_Data.iLevel += m_UpPlayer_Data.iLevel;
	m_UpPlayer_Data.iLevel = 0;

	m_iLachryma = m_Player_Data.iLachryma;

	_int iLacryma = m_iLachryma;
	_int iPreUpPoint = m_Player_Data.iLevel;
	_int iUpPoint = 0;
	while (iLacryma > (300 + (iPreUpPoint - 1 + iUpPoint) * 280))
	{
		iLacryma = iLacryma - (300 + (iPreUpPoint - 1 + iUpPoint) * 280);
		iUpPoint++;
	}

	m_Player_Data.iUPPoint = iUpPoint;
	m_Player_Data.iLachryma = m_iLachryma;
	m_Player_Data.iUpLachryma = (300 + (m_Player_Data.iLevel - 1 + m_UpPlayer_Data.iLevel) * 280);

	for (_int i = 0; i < ENUM_CLASS(STATE_LIST::END); ++i)
	{
		m_CulStateLevel[i] += m_UpStateLevel[i];
		m_UpStateLevel[i] = 0;
	}

	m_Player_Data.iMaxHp				+=		m_UpPlayer_Data.iMaxHp					;
	m_Player_Data.iMaxStamina			+=		m_UpPlayer_Data.iMaxStamina				;
	m_Player_Data.iAtk					+=		m_UpPlayer_Data.iAtk					;
	m_Player_Data.iDef					+=		m_UpPlayer_Data.iDef					;
	m_Player_Data.fWeight				+=		m_UpPlayer_Data.fWeight					;
	m_Player_Data.fMaxWeight			+=		m_UpPlayer_Data.fMaxWeight				;
	m_Player_Data.fAgile				+=		m_UpPlayer_Data.fAgile					;
										
	m_Player_Data.fStaminaAttack		+=		m_UpPlayer_Data.fStaminaAttack			;
	m_Player_Data.fStaminaRegen			+=		m_UpPlayer_Data.fStaminaRegen			;
	m_Player_Data.fEvasion_StaminaDown	+=		m_UpPlayer_Data.fEvasion_StaminaDown	;
	m_Player_Data.fDamage_StaminaDown	+=		m_UpPlayer_Data.fDamage_StaminaDown		;
	m_Player_Data.fGuard_StaminaDown	+=		m_UpPlayer_Data.fGuard_StaminaDown		;
									
	m_Player_Data.iFire					+=		m_UpPlayer_Data.iFire					;
	m_Player_Data.iWater				+=		m_UpPlayer_Data.iWater					;
	m_Player_Data.iLightning			+=		m_UpPlayer_Data.iLightning				;
	m_Player_Data.iEarth				+=		m_UpPlayer_Data.iEarth					;
	m_Player_Data.iChaos				+=		m_UpPlayer_Data.iChaos					;
	m_Player_Data.iDisease				+=		m_UpPlayer_Data.iDisease				;
	m_Player_Data.iPoison				+=		m_UpPlayer_Data.iPoison					;

	m_UpPlayer_Data.iMaxHp				= 0;
	m_UpPlayer_Data.iMaxStamina			= 0;
	m_UpPlayer_Data.iAtk				= 0;
	m_UpPlayer_Data.iDef				= 0;
	m_UpPlayer_Data.fWeight				= 0;
	m_UpPlayer_Data.fMaxWeight			= 0;
	m_UpPlayer_Data.fAgile				= 0;

	m_UpPlayer_Data.fStaminaAttack		= 0;
	m_UpPlayer_Data.fStaminaRegen		= 0;
	m_UpPlayer_Data.fEvasion_StaminaDown= 0;
	m_UpPlayer_Data.fDamage_StaminaDown	= 0;
	m_UpPlayer_Data.fGuard_StaminaDown	= 0;

	m_UpPlayer_Data.iFire				= 0;
	m_UpPlayer_Data.iWater				= 0;
	m_UpPlayer_Data.iLightning			= 0;
	m_UpPlayer_Data.iEarth				= 0;
	m_UpPlayer_Data.iChaos				= 0;
	m_UpPlayer_Data.iDisease			= 0;
	m_UpPlayer_Data.iPoison				= 0;
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
