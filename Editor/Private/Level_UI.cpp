#include "Level_UI.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "Edit_Panel.h"
#include "Edit_Button.h"
#include "Edit_ProgressBar.h"
#include "Edit_TextBox.h"

CLevel_UI::CLevel_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_UI::Initialize()
{

	/* 현재 레벨을 구성해주기 위한 객체들을 생성한다. */
	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	m_pGameInstance->AddWidget(TEXT("UI"), [&]() {

		ImGui::Begin("UI TOOL", nullptr, ImGuiWindowFlags_MenuBar);

		static char szDefaultFileName[MAX_PATH] = "DefaultModelName.dat";
		ImGui::InputText("FilePath", szDefaultFileName, IM_ARRAYSIZE(szDefaultFileName));

		ImGui::Separator();

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Save Layout")) {}
				if (ImGui::MenuItem("Load Layout")) {}
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		if (ImGui::BeginTabBar("UITabs"))
		{
			if (ImGui::BeginTabItem("Hierarchy"))
			{
				Show_Hierarchy_Menu();
				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Inspector"))
			{
				Show_Inspector_Menu();
				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Create UI"))
			{
				Show_CreateUI_Menu(szDefaultFileName);
				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}

		ImGui::End();
		});

	return S_OK;
}

void CLevel_UI::Update(_float fTimeDelta)
{

	return;
}

HRESULT CLevel_UI::Render()
{
	SetWindowText(g_hWnd, TEXT("UI툴"));

	return S_OK;
}

HRESULT CLevel_UI::Ready_Layer_BackGround(const _wstring& strLayerTag)
{
	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::TITLE), strLayerTag,
	//	ENUM_CLASS(LEVEL::TITLE), TEXT("Prototype_GameObject_BackGround"))))
	//	return E_FAIL;

	return S_OK;
}

void CLevel_UI::Show_Hierarchy_Menu()
{
	ImGui::Text("UI Hierarchy");
	ImGui::Separator();

	if (m_EditorUIObjects.empty())
		return;

	// 리스트 출력
	for (auto& pUI : m_EditorUIObjects)
	{
		string strName = pUI->Get_Name();
		_bool isSelected = (pUI == m_SelectedObject);

		if (ImGui::Selectable(strName.c_str(), isSelected))
			m_SelectedObject = pUI;
	}

	ImGui::Separator();
}

void CLevel_UI::Show_Inspector_Menu()
{
	if (!m_SelectedObject)
	{
		ImGui::Text("No Object Selected.");
		return;
	}

	ImGui::Text("Inspector : %s", m_SelectedObject->Get_Name());
	ImGui::Separator();

	// 위치
	_float3 vPos = m_SelectedObject->Get_LocalPos();
	if (ImGui::DragFloat3("Local Position", (_float*)&vPos, 0.1f))
		m_SelectedObject->Set_LocalPos(vPos);

	// 크기
	_float3 vSize = m_SelectedObject->Get_LocalSize();
	if (ImGui::DragFloat3("Local Size", (_float*)&vSize, 0.1f))
		m_SelectedObject->Set_LocalSize(vSize);

}

void CLevel_UI::Show_CreateUI_Menu(const char* szDefaultFileName)
{
	ImGui::Text("Create UIObject");
	ImGui::Separator();

	if (ImGui::Button("Create Panel"))
	{
		CUI_Panel::UIOBJECT_DESC Desc = {};
		strcpy_s(Desc.szName, sizeof(Desc.szName), szDefaultFileName);

		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::UI), TEXT("Layer_UI"), 
			ENUM_CLASS(LEVEL::UI), TEXT("Prototype_UIObject_Edit_Panel"), &Desc)))
			return;

		CUIObject* pPanel = dynamic_cast<CUIObject*>(m_pGameInstance->Find_GameObject(ENUM_CLASS(LEVEL::UI),TEXT("Layer_UI"),m_EditorUIObjects.size()));
		if (nullptr == pPanel)
		{
			MSG_BOX(TEXT("Failed Create : Edit_Panel"));
		}
		m_EditorUIObjects.push_back(pPanel);
		Safe_AddRef(pPanel);
		//m_SelectedObject = pPanel; // 자동 선택
		//Safe_AddRef(pPanel);
	}

	ImGui::SameLine();
	if (ImGui::Button("Create Button"))
	{
		// 버튼 생성 시 같은 방식
	}

	ImGui::Separator();
	ImGui::Text("Currently Created: %d", (int)m_EditorUIObjects.size());

}


CLevel_UI* CLevel_UI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_UI* pInstance = new CLevel_UI(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_UI"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CLevel_UI::Free()
{
	__super::Free();

}
