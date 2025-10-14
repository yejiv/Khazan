#include "Level_UI.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "Camera_UI.h"
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
	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera_UI"))))
		return E_FAIL;

	m_pGameInstance->AddWidget(TEXT("UI"), [&]() {

		ImGui::Begin("UI TOOL", nullptr, ImGuiWindowFlags_MenuBar);

		static _char szDefaultName[MAX_PATH] = "DefaultName.dat";
		ImGui::InputText("FilePath", szDefaultName, IM_ARRAYSIZE(szDefaultName));

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
				Show_Hierarchy_Menu(szDefaultName);
				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Inspector"))
			{
				Show_Inspector_Menu();
				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Create UI"))
			{
				Show_CreateUI_Menu(szDefaultName);
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

HRESULT CLevel_UI::Ready_Layer_Camera(const _wstring& strLayerTag)
{
	CCamera_UI::CAMERA_FREE_DESC		CameraDesc{};

	CameraDesc.vEye = _float4(0.f, 20.f, -15.f, 1.f);
	CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	CameraDesc.fFovy = XMConvertToRadians(60.0f);
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 500.f;
	CameraDesc.fSpeedPerSec = 10.f;
	CameraDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	CameraDesc.fMouseSensor = 0.2f;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::UI), strLayerTag,
		ENUM_CLASS(LEVEL::UI), TEXT("Prototype_GameObject_Camera_UI"), &CameraDesc)))
		return E_FAIL;

	return S_OK;
}

void CLevel_UI::Show_Hierarchy_Menu(const _char* szDefaultName)
{
	ImGui::Text("UI Hierarchy");
	ImGui::Separator();

	if (m_EditorUIObjects.empty())
		return;

	
	//// Test 계층 리스트 출력
	//for (auto& pUI : m_EditorUIObjects)
	//{
	//	string strName = pUI->Get_Name();
	//	_bool isSelected = (pUI == m_SelectedObject);
	//	if (ImGui::Selectable(strName.c_str(), isSelected))
	//		m_SelectedObject = pUI;
	//}
	

	// 계층 구조를 보여주는 창
	for (auto& pRootUI : m_EditorUIObjects)
	{
		Show_Hierachy(pRootUI);
	}
		
	ImGui::Separator();

	// 자식 추가 하는 기능

	if (m_SelectedObject)
	{
		if (ImGui::Button("Add_Child"))
		{
			Add_Child(m_szSelectedName);
		}
		
		if (ImGui::Button("Remove_Child"))
		{
			Remove_Child(m_szSelectedName);
		}
	}

}

void CLevel_UI::Show_Hierachy(CUIObject* pRootUIObject)
{

	ImGuiTreeNodeFlags Flags = ImGuiTreeNodeFlags_OpenOnArrow; // ImGui 에서 트리 노드를 화살표로 클릭으로만 열리게 하는 플래그

	if (m_SelectedObject == pRootUIObject)
		Flags |= ImGuiTreeNodeFlags_Selected; // 해당 트리 (아이템) 을 선택 상태로 렌더시켜준다.

	_bool isOpen = ImGui::TreeNodeEx(pRootUIObject->Get_Name(), Flags); // 트리 노드 접힙, 펼침을 만들고, 트리 스택을 푸쉬하는 기능
	// 반환 값을 bool 값으로 받아서 펼쳐진 상태라면 true 접힌 상태라면 false
	if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
	{
		m_SelectedObject = pRootUIObject;
	}
	if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
	{
		m_SelectedParrentObject = pRootUIObject;
		strcpy_s(m_szSelectedName, MAX_PATH, m_SelectedParrentObject->Get_Name());
	}
	
	if (isOpen)
	{
		// 계층구조를 쭉 렌더시킨다.
		for (auto& pChild : pRootUIObject->Get_Children())
		{
			Show_Hierachy(pChild);
		}
		ImGui::TreePop(); // TreeNodeEx()로 열었던 트리 노드의 스택을 정리하는 함수이다.트리의 끝을 정의?
	}

}

void CLevel_UI::Add_Child(const _char* szDefaultName)
{
	if (nullptr == m_SelectedObject)
		return;

	CUIObject* pParent = Find_UIObject(szDefaultName);
	// 선택된 객체가 부모인 객체와 같아면 리턴
	if (nullptr == pParent && m_SelectedObject == pParent)
		return;

	pParent->Add_Child(m_SelectedObject);
	
	// 자식을 추가했으면 에디터 UIObjects에서 선택된 오브젝트를 빼주도록한다.
	auto iter = find(m_EditorUIObjects.begin(), m_EditorUIObjects.end(), m_SelectedObject);
	if (iter != m_EditorUIObjects.end())
		m_EditorUIObjects.erase(iter);

}

void CLevel_UI::Remove_Child(const _char* szDefaultName)
{
	if (nullptr == m_SelectedObject)
		return;

	CUIObject* pParent = Find_UIObject(szDefaultName);

	if (nullptr == pParent && m_SelectedObject == pParent)
		return;

	pParent->Remove_Child(m_SelectedObject);

	// EditorUIObject에 없는경우에만 추가
	auto iter = std::find(m_EditorUIObjects.begin(), m_EditorUIObjects.end(), m_SelectedObject);
	if (iter == m_EditorUIObjects.end())
		m_EditorUIObjects.push_back(m_SelectedObject);

}

void CLevel_UI::Show_Inspector_Menu()
{
	if (m_EditorUIObjects.empty())
		return;

	if (!m_SelectedObject)
	{
		ImGui::Text("No Object Selected.");
		return;
	}

	ImGui::Text("Inspector : %s", m_SelectedObject->Get_Name());
	ImGui::Separator();

	// 공용
	
	// 위치
	_float3 vPos = m_SelectedObject->Get_LocalPos();
	if (ImGui::DragFloat3("Local Position", (_float*)&vPos, 0.1f))
		m_SelectedObject->Set_LocalPos(vPos);

	// 크기
	_float3 vSize = m_SelectedObject->Get_LocalSize();
	if (ImGui::DragFloat3("Local Size", (_float*)&vSize, 0.1f))
		m_SelectedObject->Set_LocalSize(vSize);
	
	// 색상
	static _float Colors[4] = { 1.f,1.f,1.f,1.f };
	static CUIObject* pPrevSelected = { nullptr };

	if (m_SelectedObject != pPrevSelected)
	{
		_float4 vColor = m_SelectedObject->Get_UI_Color();
		Colors[0] = vColor.x;
		Colors[1] = vColor.y;
		Colors[2] = vColor.z;
		Colors[3] = vColor.w;

		pPrevSelected = m_SelectedObject;
	}


	if (m_SelectedObject)
	{
		ImGui::ColorEdit4("Edit_Color", Colors);
		m_SelectedObject->Set_UI_Color(_float4(Colors[0], Colors[1], Colors[2], Colors[3]));

	}

	ImGui::Separator();

	// 각 타입에 맞게 고유 기능 수정 가능하도록 해보자
	if (CEdit_Panel* pPanel = dynamic_cast<CEdit_Panel*>(m_SelectedObject))
	{
		static _int iType = 0;
		if (ImGui::Combo("Panel Type", &iType, "Virtical\0Horizontal\0None\0"))
		{
			pPanel->Set_PanelType(static_cast<CEdit_Panel::PANEL_TYPE>(iType));
			pPanel->Set_Allign(true);
		}
	}
	else if (CEdit_Button* pButton = dynamic_cast<CEdit_Button*>(m_SelectedObject))
	{
		ImGui::Text("Test_Button");
	}
	else if (CEdit_ProgressBar* pProgressBar = dynamic_cast<CEdit_ProgressBar*>(m_SelectedObject))
	{
		ImGui::Text("Test_ProgressBar");
	}
	else if (CEdit_TextBox* pTextBox = dynamic_cast<CEdit_TextBox*>(m_SelectedObject))
	{
		ImGui::Text("Test_TextBox");
	}

}

void CLevel_UI::Show_CreateUI_Menu(const _char* szDefaultName)
{
	ImGui::Text("Create UIObject");
	ImGui::Separator();

	CUI_Panel::UIOBJECT_DESC Desc = {};
	strcpy_s(Desc.szName, sizeof(Desc.szName), szDefaultName);
	Desc.vLocalPos = _float3(g_iWinSizeX * 0.5f, g_iWinSizeY * 0.5f, 0.3f);
	Desc.vLocalSize = _float3(g_iWinSizeX, g_iWinSizeY, 1.f);

	static _bool isWorld_UIObject = { false };
	ImGui::Checkbox("UI_Space", &isWorld_UIObject);
	ImGui::Text("UI Space: %s", isWorld_UIObject ? "WORLD" : "SCREEN");

	ImGui::Separator();

	if (ImGui::Button("Create Panel"))
	{
		Desc.eSpaceTeype = isWorld_UIObject ? UISPACETYPE::WORLD : UISPACETYPE::SCREEN;

		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::UI), TEXT("Layer_UI"), 
			ENUM_CLASS(LEVEL::UI), TEXT("Prototype_UIObject_Edit_Panel"), &Desc)))
			return;

		CUIObject* pPanel = dynamic_cast<CUIObject*>(m_pGameInstance->Find_GameObject(ENUM_CLASS(LEVEL::UI),TEXT("Layer_UI"), m_iCurrentCount));
		if (nullptr == pPanel)
		{
			MSG_BOX(TEXT("Failed Create : Edit_Panel"));
		}
		m_EditorUIObjects.push_back(pPanel);
		Safe_AddRef(pPanel);
		//m_SelectedObject = pPanel; // 자동 선택
		//Safe_AddRef(pPanel);
		m_iCurrentCount++;
	}

	ImGui::SameLine();

	if (ImGui::Button("Create Button"))
	{
		Desc.eSpaceTeype = isWorld_UIObject ? UISPACETYPE::WORLD : UISPACETYPE::SCREEN;

		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::UI), TEXT("Layer_UI"),
			ENUM_CLASS(LEVEL::UI), TEXT("Prototype_UIObject_Edit_Button"), &Desc)))
			return;

		CUIObject* pButton = dynamic_cast<CUIObject*>(m_pGameInstance->Find_GameObject(ENUM_CLASS(LEVEL::UI), TEXT("Layer_UI"), m_iCurrentCount));
		if (nullptr == pButton)
		{
			MSG_BOX(TEXT("Failed Create : Edit_Button"));
		}
		m_EditorUIObjects.push_back(pButton);
		Safe_AddRef(pButton);
		//m_SelectedObject = pPanel; // 자동 선택
		//Safe_AddRef(pPanel);
		m_iCurrentCount++;
	}

	ImGui::SameLine();

	if (ImGui::Button("Create ProgressBar"))
	{
		Desc.eSpaceTeype = isWorld_UIObject ? UISPACETYPE::WORLD : UISPACETYPE::SCREEN;

		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::UI), TEXT("Layer_UI"),
			ENUM_CLASS(LEVEL::UI), TEXT("Prototype_UIObject_Edit_ProgressBar"), &Desc)))
			return;

		CUIObject* pProgressBar = dynamic_cast<CUIObject*>(m_pGameInstance->Find_GameObject(ENUM_CLASS(LEVEL::UI), TEXT("Layer_UI"), m_iCurrentCount));
		if (nullptr == pProgressBar)
		{
			MSG_BOX(TEXT("Failed Create : Edit_ProgressBar"));
		}
		m_EditorUIObjects.push_back(pProgressBar);
		Safe_AddRef(pProgressBar);

		//m_SelectedObject = pPanel; // 자동 선택
		//Safe_AddRef(pPanel);
		m_iCurrentCount++;
	}

	ImGui::SameLine();


	if (ImGui::Button("Create TextBox"))
	{
		Desc.eSpaceTeype = isWorld_UIObject ? UISPACETYPE::WORLD : UISPACETYPE::SCREEN;

		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::UI), TEXT("Layer_UI"),
			ENUM_CLASS(LEVEL::UI), TEXT("Prototype_UIObject_Edit_TextBox"), &Desc)))
			return;

		CUIObject* pTextBox = dynamic_cast<CUIObject*>(m_pGameInstance->Find_GameObject(ENUM_CLASS(LEVEL::UI), TEXT("Layer_UI"), m_iCurrentCount));
		if (nullptr == pTextBox)
		{
			MSG_BOX(TEXT("Failed Create : Edit_TextBox"));
		}
		m_EditorUIObjects.push_back(pTextBox);
		Safe_AddRef(pTextBox);

		//m_SelectedObject = pPanel; // 자동 선택
		//Safe_AddRef(pPanel);
		m_iCurrentCount++;
	}

	

	ImGui::Separator();

	ImGui::Text("Currently Created: %d", (int)m_EditorUIObjects.size());

}

CUIObject* CLevel_UI::Find_UIObject(const _char* szUIObjectName)
{
	for (auto pUIObject : m_EditorUIObjects)
	{
		if (0 == strcmp(pUIObject->Get_Name(), szUIObjectName))
			return pUIObject;
	}

	return nullptr;
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

	Safe_Release(m_SelectedObject);
	Safe_Release(m_SelectedParrentObject);
	for (auto& UIObject : m_EditorUIObjects)
	{
		Safe_Release(UIObject);
	}
	m_EditorUIObjects.clear();
}
