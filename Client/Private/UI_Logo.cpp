#include "UI_Logo.h"
#include "GameInstance.h"
#include "ClientInstance.h"

#include "UI_Logo_List.h"
#include "UI_TextBox.h"

CUI_Logo::CUI_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Panel{ pDevice, pContext }
{
}

CUI_Logo::CUI_Logo(const CUI_Logo& Prototype)
	: CUI_Panel(Prototype)
{
}

HRESULT CUI_Logo::Initialize_Prototype(_uint iLevel)
{
	m_iLevel = iLevel;

	CHECK_FAILED(Ready_Prototype(), E_FAIL);
	return S_OK;
}

HRESULT CUI_Logo::Initialize_Clone(void* pArg)
{
	__super::Initialize_Clone(pArg);

	return S_OK;
}

void CUI_Logo::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUI_Logo::Update(_float fTimeDelta)
{
	Update_StartState(fTimeDelta);
	Update_MenuState(fTimeDelta);
	__super::Update(fTimeDelta);
}

void CUI_Logo::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CUI_Logo::Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg)
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
			string strName = child.value("name", "");
			_int iLevelID = ENUM_CLASS(LEVEL::STATIC);
			
			_int iObjectNum = 1;
			_float2 vPos = {};

			if (strName == "Logo_List")
			{

				if (child.contains("LocalPos"))
				{
					vPos.x = child["LocalPos"].value("x", 0.f);
					vPos.y = child["LocalPos"].value("y", 0.f);
				}
				iLevelID = ENUM_CLASS(LEVEL::TITLE);
				iObjectNum = ENUM_CLASS(LISTTYPE::END);
			}
			for (_int i = 0; i < iObjectNum; ++i)
			{
				string strClass = child.value("class", "");
				_wstring wstrClass = AnsiToWString(strClass);

				CUIObject::UIOBJECT_DESC UIDesc{};
				UIDesc.szName = "";
				UIDesc.iUIType = 0;
				UIDesc.vLocalSize = { 1.f, 1.f };
				UIDesc.fDepth = 0;
				UIDesc.vLocalPos = { g_iWinSizeX >> 1 , g_iWinSizeY >> 1 };

				CUIObject* pChild = static_cast<CUIObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, iLevelID, wstrClass.c_str(), &UIDesc));

				if (pChild == nullptr)
				{
					MSG_BOX(TEXT("자식 클론 생성 실패"));
					return E_FAIL;
				}
				if (pChild->Load_UI(child, iPrototypeLevelID, pArg))
					return E_FAIL;

				pChild->Insert_Bubble([this](BUBBLEEVENT* pArg) {this->Bubble_EventCall(pArg); });
				m_Children.push_back(pChild);

				if (strName == "Logo_Start_Text")
				{
					m_pStartText = static_cast<CUI_TextBox*>(pChild);
					Safe_AddRef(pChild);
				}
				if (strName == "Logo_List")
				{
					m_pList.push_back(static_cast<CUI_Logo_List*>(pChild));
					Safe_AddRef(pChild);
					pChild->Update_Visible(false);
					static_cast<CUI_Logo_List*>(pChild)->Update_Pos(static_cast<LISTTYPE>(i), vPos, 45.f);
				}
			}

		}
	}

	__super::Update_Transform(nullptr, m_vLocalPos);


	m_eState = UISTATE::START;


	return S_OK;
}

void CUI_Logo::Bubble_EventCall(BUBBLEEVENT* pArg)
{
	LOGOBUBBLE_DESC* Desc = static_cast<LOGOBUBBLE_DESC*>(pArg);

	if (m_isClick)
		return;
	if (Desc->isClick)
	{
		m_eNextEvent = Desc->eListType;

		if (m_eNextEvent == LISTTYPE::NEWGAME)
		{
			CClientInstance::GetInstance()->Fade_Out([this]() {this->Event_NewGame(); });
			m_isClick = true;
		}
		else if (m_eNextEvent == LISTTYPE::OPTION)
		{

		}
		else if (m_eNextEvent == LISTTYPE::EXIT)
		{
		}
	}
	else
	{
		m_iSeleteIndex = ENUM_CLASS(Desc->eListType);
		for (_int i = 0; i < ENUM_CLASS(LISTTYPE::END); ++i)
			i == m_iSeleteIndex ? m_pList[i]->Set_Selete(true) : m_pList[i]->Set_Selete(false);

	}
}

HRESULT CUI_Logo::Update_Switch(void* pArg)
{
	return S_OK;
}

HRESULT CUI_Logo::Ready_Prototype()
{
	CHECK_FAILED(m_pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_UI_Logo_List"),
		CUI_Logo_List::Create(m_pDevice, m_pContext, m_iLevel)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_UI_Log_Tex"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Logo/Logo_Tex_%d.png"), 5)), E_FAIL);

	return S_OK;
}

void CUI_Logo::UI_Animation(_float fTimeDelta)
{
}

void CUI_Logo::Update_StartState(_float fTimeDelta)
{
	if (m_eState == UISTATE::START)
	{
		if (m_fAccTime < 0.6f)
		{
			m_fAccTime = 0.6f;
			m_isUpDelta = true;
		}
		else if (m_fAccTime > 0.9f)
		{
			m_fAccTime = 0.9f;
			m_isUpDelta = false;
		}

		m_isUpDelta ? m_fAccTime += fTimeDelta * 0.25f : m_fAccTime -= fTimeDelta * 0.25f;
		if (m_pGameInstance->Key_Down(DIK_F, INPUT_TYPE::UI))
		{
			m_eState = UISTATE::START_END;
			m_fAccTime = 0.9f;
			m_isUpDelta = true;

			m_iSeleteIndex = 0;
			for (_int i = 0; i < ENUM_CLASS(LISTTYPE::END); ++i)
				i == m_iSeleteIndex ? m_pList[i]->Set_Selete(true) : m_pList[i]->Set_Selete(false);
		}
		m_pStartText->Set_Color({ 1.f, 1.f, 1.f, m_fAccTime });
	}
	else if (m_eState == UISTATE::START_END)
	{
		if (m_isUpDelta)
		{
			m_fAccTime += fTimeDelta;
			if (m_fAccTime >= 1.f)
				m_isUpDelta = false;
		}
		else
		{
			m_fAccTime -= fTimeDelta;
		}
		m_pStartText->Set_Color({ 1.f, 1.f, 1.f, m_fAccTime });
		if (m_fAccTime < 0.f)
		{
			m_pStartText->Update_Visible(false);
			m_eState = UISTATE::MENU;
			for (auto pList : m_pList)
				pList->Update_Visible(true);
		}

	}
}

void CUI_Logo::Update_MenuState(_float fTimeDelta)
{
	if (m_eState == UISTATE::MENU)
	{
		_bool isKeyInput = false;
		if (m_pGameInstance->Key_Down(DIK_W, INPUT_TYPE::UI))
		{
			m_iSeleteIndex -= 1;
			isKeyInput = true;

			if (m_iSeleteIndex < 0)
				m_iSeleteIndex = ENUM_CLASS(LISTTYPE::END) - 1;
		}
		else if (m_pGameInstance->Key_Down(DIK_S, INPUT_TYPE::UI))
		{
			m_iSeleteIndex += 1;
			isKeyInput = true;

			if (m_iSeleteIndex >= ENUM_CLASS(LISTTYPE::END))
				m_iSeleteIndex = 0;
		}

		if (isKeyInput)
			for (_int i = 0; i < ENUM_CLASS(LISTTYPE::END); ++i)
				i == m_iSeleteIndex ? m_pList[i]->Set_Selete(true) : m_pList[i]->Set_Selete(false);
	}
}

void CUI_Logo::Event_NewGame()
{
	m_pGameInstance->Emit_Event<EVENT_LEVEL_CHANGE>(ENUM_CLASS(EVENT_TYPE::LEVEL_CHANGE), { ENUM_CLASS(LEVEL::HEINMACH) });
}

CUI_Logo* CUI_Logo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CUI_Logo* pInstance = new CUI_Logo(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX(TEXT("Failed Created : CUI_Logo"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_Logo::Clone(void* pArg)
{
	CUI_Logo* pInstance = new CUI_Logo(*this);
	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CUI_Logo"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_Logo::Free()
{
	__super::Free();
	Safe_Release(m_pStartText);

	for (auto pList : m_pList)
		Safe_Release(pList);
	m_pList.clear();

}
